#include "ApplicationsManager.h"
#include "../MainWindow.h"
#include "../translations/translations.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

ApplicationsManager::ApplicationsManager(MainWindow* mainWindow, GtkWidget* parentContainer, GtkWidget* parentFixed)
    : mainWindow(mainWindow), parentContainer(parentContainer), parentFixed(parentFixed),
      applicationsContainer(nullptr), backButton(nullptr), titleLabel(nullptr),
      headerWidget(nullptr), configLabel(nullptr), leftArrow(nullptr), rightArrow(nullptr),
      refreshButton(nullptr), mainBox(nullptr), scrolledWindow(nullptr), linesBox(nullptr), currentConfigIdx(0), refreshTimerId(0) {
    
    // Initialize config files and names - EXACTLY like reference
    std::string homeDir = g_get_home_dir();
    configFiles = {
        homeDir + "/.config/hypr/applications.conf",
        homeDir + "/.config/hypr/app_keybinds.conf", 
        homeDir + "/.config/hypr/auto_start.conf",
        homeDir + "/.config/hypr/window_rules.conf"
    };
    
    configNames = {
        TR(TranslationKeys::APPLICATIONS_TITLE),
        TR(TranslationKeys::KEYBINDS), 
        TR(TranslationKeys::AUTORUNS),
        TR(TranslationKeys::WINDOW_RULES)
    };
    
    setupUI();
}

ApplicationsManager::~ApplicationsManager() {
    if (refreshTimerId > 0) {
        g_source_remove(refreshTimerId);
        refreshTimerId = 0;
    }
    
    // Clean up line edit data
    for (auto* data : lineEditDataList) {
        delete data;
    }
    lineEditDataList.clear();
    lineWidgets.clear();
    lineEntries.clear();
}

void ApplicationsManager::setupUI() {
    std::cout << "=== SETTING UP APPLICATIONS UI ===" << std::endl;
    
    // Create main container - a bit bigger than before
    applicationsContainer = gtk_fixed_new();
    gtk_widget_set_size_request(applicationsContainer, 1100, 550);
    
    // Use overlay_add_overlay like other managers!
    if (parentFixed && applicationsContainer) {
        gtk_overlay_add_overlay(GTK_OVERLAY(parentFixed), applicationsContainer);
        std::cout << "Added applications container to overlay" << std::endl;
    } else {
        std::cout << "ERROR: parentFixed or applicationsContainer is NULL!" << std::endl;
    }
    
    // Center the container properly on screen (assuming 1920x1080 screen)
    gtk_widget_set_halign(applicationsContainer, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(applicationsContainer, GTK_ALIGN_CENTER);
    
    std::cout << "Created applications container size 1100x550 and centered it" << std::endl;
    
    // Setup back button first
    setupBackButton();
    
    // Add title OUTSIDE and on top of the container
    titleLabel = gtk_label_new(TR(TranslationKeys::APPLICATIONS_TITLE));
    gtk_widget_add_css_class(titleLabel, "applications-title");
    gtk_widget_set_size_request(titleLabel, 500, 50);
    gtk_label_set_xalign(GTK_LABEL(titleLabel), 0.5);
    
    // Add title directly to parent overlay, positioned above the container
    gtk_overlay_add_overlay(GTK_OVERLAY(parentFixed), titleLabel);
    gtk_widget_set_halign(titleLabel, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(titleLabel, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(titleLabel, -610); // Position above the centered container
    gtk_widget_set_visible(titleLabel, FALSE); // Initially hidden
    
    // Main vertical box layout - bigger to fit the larger container
    mainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_size_request(mainBox, 1000, 500);  // Bigger for 1100x550 container
    gtk_fixed_put(GTK_FIXED(applicationsContainer), mainBox, 50, 25);  // Start from top since no title inside
    gtk_widget_set_visible(mainBox, TRUE);
    
    std::cout << "Created title and main box" << std::endl;
    
    // Header with navigation arrows and config name - adjusted for bigger container
    headerWidget = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_size_request(headerWidget, 1000, 50);
    
    // Left arrow
    leftArrow = gtk_button_new_with_label(TR(TranslationKeys::LEFT_ARROW));
    gtk_widget_set_size_request(leftArrow, 50, 50);
    g_signal_connect(leftArrow, "clicked", G_CALLBACK(onLeftArrowClicked), this);
    
    // Config label - centered
    configLabel = gtk_label_new(configNames[currentConfigIdx].c_str());
    gtk_widget_set_hexpand(configLabel, TRUE);
    gtk_widget_set_halign(configLabel, GTK_ALIGN_CENTER);
    
    // Right arrow
    rightArrow = gtk_button_new_with_label(TR(TranslationKeys::RIGHT_ARROW));
    gtk_widget_set_size_request(rightArrow, 50, 50);
    g_signal_connect(rightArrow, "clicked", G_CALLBACK(onRightArrowClicked), this);
    
    // Refresh button - like NetworkManager
    refreshButton = gtk_button_new();
    GtkWidget* refreshIcon = gtk_image_new_from_icon_name("view-refresh-symbolic");
    gtk_image_set_pixel_size(GTK_IMAGE(refreshIcon), 18);
    gtk_button_set_child(GTK_BUTTON(refreshButton), refreshIcon);
    gtk_widget_add_css_class(refreshButton, "refresh-button");
    gtk_widget_add_css_class(refreshButton, "flat");
    gtk_widget_add_css_class(refreshButton, "circular");
    gtk_widget_set_tooltip_text(refreshButton, "Refresh Config Files");
    gtk_widget_set_size_request(refreshButton, 40, 40);
    g_signal_connect(refreshButton, "clicked", G_CALLBACK(onRefreshButtonClicked), this);
    
    // Pack header - add refresh button after right arrow
    gtk_box_append(GTK_BOX(headerWidget), leftArrow);
    gtk_box_append(GTK_BOX(headerWidget), configLabel);
    gtk_box_append(GTK_BOX(headerWidget), rightArrow);
    gtk_box_append(GTK_BOX(headerWidget), refreshButton);
    gtk_box_append(GTK_BOX(mainBox), headerWidget);
    
    // Make everything visible
    gtk_widget_set_visible(headerWidget, TRUE);
    gtk_widget_set_visible(leftArrow, TRUE);
    gtk_widget_set_visible(configLabel, TRUE);
    gtk_widget_set_visible(rightArrow, TRUE);
    gtk_widget_set_visible(refreshButton, TRUE);
    
    std::cout << "Created header with arrows and label: " << configNames[currentConfigIdx] << std::endl;
    
    // Scrolled window for configuration lines - bigger for larger container
    scrolledWindow = gtk_scrolled_window_new();
    gtk_widget_set_size_request(scrolledWindow, 1000, 420);  // Bigger to fit
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    
    // Lines container - VBox with 20px spacing like reference
    linesBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_margin_top(linesBox, 20);
    gtk_widget_set_margin_bottom(linesBox, 20);
    gtk_widget_set_margin_start(linesBox, 20);
    gtk_widget_set_margin_end(linesBox, 20);
    
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolledWindow), linesBox);
    gtk_box_append(GTK_BOX(mainBox), scrolledWindow);
    
    // Make scroll area visible
    gtk_widget_set_visible(scrolledWindow, TRUE);
    gtk_widget_set_visible(linesBox, TRUE);
    
    std::cout << "Created scrolled window and lines box" << std::endl;
    
    // Apply beautiful CSS styling like DisplayManager
    const char* css = R"(
        .applications-container {
            background: linear-gradient(145deg, rgba(255, 255, 255, 0.12), rgba(255, 166, 218, 0.06));
            border: 2px solid rgba(255, 166, 218, 0.25);
            border-radius: 32px;
            box-shadow: 0 20px 60px rgba(0, 0, 0, 0.4), inset 0 2px 0 rgba(255, 255, 255, 0.15);
            backdrop-filter: blur(25px);
        }
        
        .applications-title {
            font-family: ElysiaOSNew12;
            color: #ffffff;
            font-size: 28px;
            font-weight: 900;
            letter-spacing: 3px;
            text-shadow: 0 4px 20px rgba(0, 0, 0, 0.6), 0 2px 4px rgba(255, 166, 218, 0.4);
        }
        
        .config-title {
            font-family: ElysiaOSNew12;
            font-size: 22px;
            font-weight: bold;
            color: rgba(255, 255, 255, 0.95);
            text-shadow: 0 2px 8px rgba(0, 0, 0, 0.5);
            background: transparent;
            border: none;
        }
        
        .nav-arrow {
            background: linear-gradient(145deg, rgba(255, 255, 255, 0.15), rgba(255, 166, 218, 0.08));
            border: 2px solid rgba(255, 166, 218, 0.3);
            border-radius: 16px;
            color: rgba(255, 255, 255, 0.9);
            font-size: 20px;
            font-weight: bold;
            text-shadow: 0 2px 4px rgba(0, 0, 0, 0.4);
            box-shadow: 0 8px 20px rgba(0, 0, 0, 0.3);
            backdrop-filter: blur(15px);
            transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
        }
        
        .nav-arrow:hover {
            background: linear-gradient(145deg, rgba(255, 166, 218, 0.25), rgba(255, 166, 218, 0.15));
            border-color: rgba(255, 166, 218, 0.5);
            color: #ffa6da;
            transform: scale(1.05) translateY(-2px);
            box-shadow: 0 12px 30px rgba(0, 0, 0, 0.4);
        }
        
        .config-scroll-area {
            background: linear-gradient(145deg, rgba(255, 255, 255, 0.08), rgba(255, 166, 218, 0.03));
            border-radius: 24px;
            border: 2px solid rgba(255, 166, 218, 0.15);
            box-shadow: 0 12px 40px rgba(0, 0, 0, 0.2), inset 0 2px 0 rgba(255, 255, 255, 0.1);
            backdrop-filter: blur(20px);
        }
        
        .config-line-row {
            background: linear-gradient(145deg, rgba(255, 255, 255, 0.1), rgba(255, 166, 218, 0.04));
            border: 1px solid rgba(255, 166, 218, 0.2);
            border-radius: 16px;
            margin: 8px 0;
            padding: 12px 16px;
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.15);
            backdrop-filter: blur(10px);
            transition: all 0.3s ease;
        }
        
        .config-line-row:hover {
            background: linear-gradient(145deg, rgba(255, 255, 255, 0.15), rgba(255, 166, 218, 0.08));
            border-color: rgba(255, 166, 218, 0.4);
            transform: translateY(-2px);
            box-shadow: 0 8px 25px rgba(0, 0, 0, 0.25);
        }
        
        .config-line-entry {
            background: rgba(0, 0, 0, 0.2);
            border: 2px solid rgba(255, 166, 218, 0.2);
            border-radius: 12px;
            color: white;
            font-family: 'JetBrains Mono Nerd Font', monospace;
            font-size: 13px;
            font-weight: 500;
            padding: 8px 12px;
            text-shadow: 0 1px 2px rgba(0, 0, 0, 0.3);
            box-shadow: inset 0 2px 4px rgba(0, 0, 0, 0.2);
            transition: all 0.3s ease;
        }
        
        .config-line-entry:focus {
            background: rgba(0, 0, 0, 0.3);
            border-color: #ffa6da;
            color: #ffffff;
            outline: none;
            box-shadow: inset 0 2px 4px rgba(0, 0, 0, 0.3), 0 0 0 3px rgba(255, 166, 218, 0.3);
        }
        
        .line-button {
            background: linear-gradient(145deg, rgba(255, 255, 255, 0.15), rgba(255, 166, 218, 0.08));
            border: 2px solid rgba(255, 166, 218, 0.3);
            border-radius: 12px;
            font-size: 16px;
            font-weight: bold;
            text-shadow: 0 1px 2px rgba(0, 0, 0, 0.4);
            box-shadow: 0 4px 12px rgba(0, 0, 0, 0.2);
            backdrop-filter: blur(10px);
            transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
            min-width: 45px;
            min-height: 45px;
        }
        
        .add-button {
            color: #ffa6da;
        }
        
        .add-button:hover {
            background: linear-gradient(145deg, rgba(255, 166, 218, 0.25), rgba(255, 166, 218, 0.15));
            border-color: rgba(255, 166, 218, 0.6);
            color: white;
            transform: scale(1.1) translateY(-2px);
            box-shadow: 0 8px 20px rgba(0, 0, 0, 0.3);
        }
        
        .remove-button {
            color: #ff6b9d;
        }
        
        .remove-button:hover {
            background: linear-gradient(145deg, rgba(255, 107, 157, 0.25), rgba(255, 107, 157, 0.15));
            border-color: rgba(255, 107, 157, 0.6);
            color: white;
            transform: scale(1.1) translateY(-2px);
            box-shadow: 0 8px 20px rgba(0, 0, 0, 0.3);
        }
        
        .refresh-button {
            background: linear-gradient(145deg, rgba(255, 255, 255, 0.15), rgba(255, 166, 218, 0.08));
            border: 2px solid rgba(255, 166, 218, 0.3);
            border-radius: 12px;
            color: rgba(255, 255, 255, 0.9);
            text-shadow: 0 1px 2px rgba(0, 0, 0, 0.4);
            box-shadow: 0 4px 12px rgba(0, 0, 0, 0.2);
            backdrop-filter: blur(10px);
            transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
        }
        
        .refresh-button:hover {
            background: linear-gradient(145deg, rgba(255, 166, 218, 0.25), rgba(255, 166, 218, 0.15));
            border-color: rgba(255, 166, 218, 0.6);
            color: #ffa6da;
            transform: scale(1.1) rotate(180deg);
            box-shadow: 0 8px 20px rgba(0, 0, 0, 0.3);
        }
    )";
    
    GtkCssProvider* provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(provider, css);
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    g_object_unref(provider);
    
    // Add CSS classes
    gtk_widget_add_css_class(applicationsContainer, "applications-container");
    gtk_widget_add_css_class(configLabel, "config-title");
    gtk_widget_add_css_class(leftArrow, "nav-arrow");
    gtk_widget_add_css_class(rightArrow, "nav-arrow");
    gtk_widget_add_css_class(scrolledWindow, "config-scroll-area");
    
    std::cout << "Applied CSS and classes" << std::endl;
    
    // Load initial configuration - EXACTLY like reference
    loadConfig();
    
    std::cout << "=== UI SETUP COMPLETE ===" << std::endl;
    
    // Initially hidden like other managers
    gtk_widget_set_visible(applicationsContainer, FALSE);
}

void ApplicationsManager::setupBackButton() {
    if (!applicationsContainer) return;
    
    std::string homeDir = g_get_home_dir();
    std::string backPath = homeDir + "/.config/Elysia/assets/assets/back.png";
    
    // Create a button - EXACTLY like DisplayManager
    backButton = gtk_button_new();
    if (!backButton) return;
    
    // Create a fixed container to position the label on top of the image
    GtkWidget* fixedContainer = gtk_fixed_new();
    if (!fixedContainer) return;
    
    gtk_widget_set_size_request(fixedContainer, 120, 60);
    
    // Load the back image
    GtkWidget* backImage = gtk_picture_new_for_filename(backPath.c_str());
    if (!backImage) {
        backImage = gtk_image_new_from_icon_name("go-previous");
    } else {
        gtk_widget_set_size_request(backImage, 159, 66);
        gtk_picture_set_can_shrink(GTK_PICTURE(backImage), TRUE);
        gtk_picture_set_content_fit(GTK_PICTURE(backImage), GTK_CONTENT_FIT_CONTAIN);
    }
    
    // Create label
    GtkWidget* label = gtk_label_new(TR(TranslationKeys::BACK));
    if (label) {
        gtk_widget_add_css_class(label, "back-button-label");
    }
    
    // Position image and label in the fixed container
    if (backImage) {
        gtk_fixed_put(GTK_FIXED(fixedContainer), backImage, 0, 0);
    }
    if (label) {
        gtk_fixed_put(GTK_FIXED(fixedContainer), label, 40, 20);
    }
    
    // Add fixed container to button
    gtk_button_set_child(GTK_BUTTON(backButton), fixedContainer);
    
    // Position button OUTSIDE the applicationsContainer - directly on the background
    gtk_widget_set_size_request(backButton, 120, 60);
    gtk_widget_add_css_class(backButton, "flat");
    
    // Add to parent overlay SEPARATELY from applicationsContainer
    gtk_overlay_add_overlay(GTK_OVERLAY(parentFixed), backButton);
    
    // Position at absolute top-left corner (0,0)
    gtk_widget_set_halign(backButton, GTK_ALIGN_START);
    gtk_widget_set_valign(backButton, GTK_ALIGN_START);
    gtk_widget_set_margin_start(backButton, 0);
    gtk_widget_set_margin_top(backButton, 0);
    
    // Initially hidden - only show when manager is active
    gtk_widget_set_visible(backButton, FALSE);
    
    // Connect signal
    g_signal_connect(backButton, "clicked", G_CALLBACK(onBackButtonClicked), this);
}

void ApplicationsManager::show() {
    std::cout << "=== SHOWING APPLICATIONS MANAGER ===" << std::endl;
    mainWindow->switchToBackground("background4.png");
    
    // FORCE EVERYTHING VISIBLE
    if (applicationsContainer) {
        gtk_widget_set_visible(applicationsContainer, TRUE);
        std::cout << "Made applications container visible" << std::endl;
    } else {
        std::cout << "ERROR: applicationsContainer is NULL!" << std::endl;
    }
    
    // Show back button and title when manager is active
    if (backButton) {
        gtk_widget_set_visible(backButton, TRUE);
        std::cout << "Made back button visible" << std::endl;
    }
    if (titleLabel) {
        gtk_widget_set_visible(titleLabel, TRUE);
        std::cout << "Made title visible" << std::endl;
    }
    
    // Load config when shown - like reference
    loadConfig();
    
    // No auto-refresh timer - only refresh when manually switching configs or on show
    // This prevents interrupting user editing like the QT6 reference does
    std::cout << "No auto-refresh timer - only refreshes on config switch or show" << std::endl;
    
    std::cout << "=== APPLICATIONS MANAGER SHOWN ===" << std::endl;
}

void ApplicationsManager::hide() {
    std::cout << "=== HIDING APPLICATIONS MANAGER ===" << std::endl;
    if (applicationsContainer) {
        gtk_widget_set_visible(applicationsContainer, FALSE);
        std::cout << "Applications Manager hidden successfully" << std::endl;
    } else {
        std::cout << "ERROR: applicationsContainer is NULL in hide()" << std::endl;
    }
    
    // Hide back button and title when manager is not active
    if (backButton) {
        gtk_widget_set_visible(backButton, FALSE);
        std::cout << "Back button hidden successfully" << std::endl;
    }
    if (titleLabel) {
        gtk_widget_set_visible(titleLabel, FALSE);
        std::cout << "Title hidden successfully" << std::endl;
    }
}

void ApplicationsManager::loadConfig() {
    std::cout << "Loading config: " << configFiles[currentConfigIdx] << std::endl;
    
    // Read all lines - EXACTLY like reference
    std::vector<std::string> allLines;
    std::ifstream file(configFiles[currentConfigIdx]);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            allLines.push_back(line);
        }
        file.close();
    } else {
        std::cout << "Could not open file: " << configFiles[currentConfigIdx] << std::endl;
    }
    
    // Separate header based on current config type - EXACTLY like reference
    int firstConfigIdx = 0;
    if (currentConfigIdx == 0) {
        // Applications config - ignore ########, ##APPS##, ########
        for (size_t i = 0; i < allLines.size(); ++i) {
            std::string line = trim(allLines[i]);
            if (line.find("########") == 0 || line.find("##APPS##") == 0) {
                firstConfigIdx = static_cast<int>(i) + 1;
            }
        }
    } else if (currentConfigIdx == 1) {
        // Keybinds config - ignore ########################, ########KEYBINDS########, ########################, and the comment line
        for (size_t i = 0; i < allLines.size(); ++i) {
            std::string line = trim(allLines[i]);
            if (line.find("########################") == 0 || line.find("########KEYBINDS########") == 0) {
                firstConfigIdx = static_cast<int>(i) + 1;
            }
        }
        // Skip the comment line after header
        if (static_cast<size_t>(firstConfigIdx) < allLines.size() && 
            trim(allLines[firstConfigIdx]).find("# See https://wiki.hyprland.org/Configuring/Keywords/ for more") == 0) {
            firstConfigIdx++;
        }
    } else if (currentConfigIdx == 2) {
        // Autoruns config - ignore ########################, ########AUTORUNS########, ########################, and the comment line
        for (size_t i = 0; i < allLines.size(); ++i) {
            std::string line = trim(allLines[i]);
            if (line.find("########################") == 0 || line.find("########AUTORUNS########") == 0) {
                firstConfigIdx = static_cast<int>(i) + 1;
            }
        }
        // Skip the comment line after header
        if (static_cast<size_t>(firstConfigIdx) < allLines.size() && 
            trim(allLines[firstConfigIdx]).find("# Execute your favorite apps at launch") == 0) {
            firstConfigIdx++;
        }
    } else {
        // Window Rules config - ignore ################################, ##TRANSPARENT BACKGROUND APP####, ################################, and all the example/comment lines
        for (size_t i = 0; i < allLines.size(); ++i) {
            std::string line = trim(allLines[i]);
            if (line.find("################################") == 0 || line.find("##TRANSPARENT BACKGROUND APP####") == 0) {
                firstConfigIdx = static_cast<int>(i) + 1;
            }
        }
        // Skip all the example and comment lines after header
        while (static_cast<size_t>(firstConfigIdx) < allLines.size()) {
            std::string line = trim(allLines[firstConfigIdx]);
            if (line.empty() || line[0] == '#') {
                firstConfigIdx++;
            } else {
                break;
            }
        }
    }
    
    // Preserve header - EXACTLY like reference
    preservedHeader.clear();
    for (int i = 0; i < firstConfigIdx; ++i) {
        if (static_cast<size_t>(i) < allLines.size()) {
            preservedHeader.push_back(allLines[i]);
        }
    }
    
    // Extract current lines - EXACTLY like reference
    currentLines.clear();
    for (size_t i = firstConfigIdx; i < allLines.size(); ++i) {
        currentLines.push_back(allLines[i]);
    }
    
    std::cout << "Loaded " << currentLines.size() << " config lines after header of " << preservedHeader.size() << " lines" << std::endl;
    rebuildLines();
}

void ApplicationsManager::rebuildLines() {
    std::cout << "=== REBUILDING LINES UI ===" << std::endl;
    std::cout << "Current lines count: " << currentLines.size() << std::endl;
    
    // Debug: print all current lines
    for (size_t i = 0; i < currentLines.size(); ++i) {
        std::cout << "Line " << i << ": '" << currentLines[i] << "'" << std::endl;
    }
    
    // Clear old widgets - EXACTLY like reference
    GtkWidget* child = gtk_widget_get_first_child(linesBox);
    while (child) {
        GtkWidget* next = gtk_widget_get_next_sibling(child);
        gtk_box_remove(GTK_BOX(linesBox), child);
        child = next;
    }
    
    // Clear data
    for (auto* data : lineEditDataList) {
        delete data;
    }
    lineEditDataList.clear();
    lineWidgets.clear();
    lineEntries.clear();
    
    // Create new line widgets with beautiful styling
    for (size_t i = 0; i < currentLines.size(); ++i) {
        // Row container with beautiful styling
        GtkWidget* row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 15);
        gtk_widget_add_css_class(row, "config-line-row");
        
        // Line entry - EXACTLY like reference
        GtkWidget* entry = gtk_entry_new();
        GtkEntryBuffer* buffer = gtk_entry_get_buffer(GTK_ENTRY(entry));
        gtk_entry_buffer_set_text(buffer, currentLines[i].c_str(), -1);
        gtk_widget_add_css_class(entry, "config-line-entry");
        gtk_widget_set_hexpand(entry, TRUE);  // Expanding like reference
        
        // Add button with beautiful styling
        GtkWidget* addBtn = gtk_button_new_with_label(TR(TranslationKeys::ADD_BUTTON));
        gtk_widget_add_css_class(addBtn, "line-button");
        gtk_widget_add_css_class(addBtn, "add-button");
        gtk_widget_set_size_request(addBtn, 45, 45);
        
        // Remove button with beautiful styling
        GtkWidget* removeBtn = gtk_button_new_with_label(TR(TranslationKeys::REMOVE_BUTTON));
        gtk_widget_add_css_class(removeBtn, "line-button");
        gtk_widget_add_css_class(removeBtn, "remove-button");
        gtk_widget_set_size_request(removeBtn, 45, 45);
        
        // Create callback data
        LineEditData* editData = new LineEditData{this, static_cast<int>(i), entry};
        LineEditData* addData = new LineEditData{this, static_cast<int>(i), entry};
        LineEditData* removeData = new LineEditData{this, static_cast<int>(i), entry};
        
        lineEditDataList.push_back(editData);
        lineEditDataList.push_back(addData);
        lineEditDataList.push_back(removeData);
        
        // Connect signals
        g_signal_connect(entry, "activate", G_CALLBACK(onLineEditChanged), editData);
        g_signal_connect(addBtn, "clicked", G_CALLBACK(onAddButtonClicked), addData);
        g_signal_connect(removeBtn, "clicked", G_CALLBACK(onRemoveButtonClicked), removeData);
        
        // Pack widgets - EXACTLY like reference: edit expanding, then buttons
        gtk_box_append(GTK_BOX(row), entry);
        gtk_box_append(GTK_BOX(row), addBtn);
        gtk_box_append(GTK_BOX(row), removeBtn);
        
        // Make everything visible
        gtk_widget_set_visible(row, TRUE);
        gtk_widget_set_visible(entry, TRUE);
        gtk_widget_set_visible(addBtn, TRUE);
        gtk_widget_set_visible(removeBtn, TRUE);
        
        gtk_box_append(GTK_BOX(linesBox), row);
        
        // Store references
        lineWidgets.push_back(row);
        lineEntries.push_back(entry);
        
        std::cout << "Created line widget " << i << " with text: '" << currentLines[i] << "'" << std::endl;
    }
    
    // Add stretch at the end - like reference
    GtkWidget* spacer = gtk_label_new("");
    gtk_widget_set_vexpand(spacer, TRUE);
    gtk_box_append(GTK_BOX(linesBox), spacer);
    
    std::cout << "Rebuilt " << lineWidgets.size() << " line widgets" << std::endl;
}

void ApplicationsManager::saveConfig() {
    std::cout << "Saving config: " << configFiles[currentConfigIdx] << std::endl;
    
    std::ofstream file(configFiles[currentConfigIdx]);
    if (!file.is_open()) {
        std::cout << "Could not write config file: " << configFiles[currentConfigIdx] << std::endl;
        return;
    }
    
    // Write preserved header - EXACTLY like reference
    for (const auto& line : preservedHeader) {
        file << line << "\n";
    }
    
    // Write current config lines - EXACTLY like reference
    for (const auto& line : currentLines) {
        file << line << "\n";
    }
    
    file.close();
}

void ApplicationsManager::refreshData() {
    loadConfig();
}

void ApplicationsManager::updateConfigDisplay() {
    gtk_label_set_text(GTK_LABEL(configLabel), configNames[currentConfigIdx].c_str());
    loadConfig();
}

void ApplicationsManager::handleLeftArrow() {
    currentConfigIdx = (currentConfigIdx - 1 + configFiles.size()) % configFiles.size();
    std::cout << "Switched to config: " << configNames[currentConfigIdx] << std::endl;
    updateConfigDisplay();
}

void ApplicationsManager::handleRightArrow() {
    currentConfigIdx = (currentConfigIdx + 1) % configFiles.size();
    std::cout << "Switched to config: " << configNames[currentConfigIdx] << std::endl;
    updateConfigDisplay();
}

void ApplicationsManager::handleLineEdited(int idx, const std::string& newText) {
    if (idx >= 0 && static_cast<size_t>(idx) < currentLines.size()) {
        currentLines[idx] = newText;
        saveConfig();
        std::cout << "Line " << idx << " edited: " << newText << std::endl;
    }
}

void ApplicationsManager::handleAddLine(int idx) {
    if (idx >= 0 && static_cast<size_t>(idx) <= currentLines.size()) {
        currentLines.insert(currentLines.begin() + idx + 1, "");
        saveConfig();
        rebuildLines();
        std::cout << "Added line after index: " << idx << std::endl;
    }
}

void ApplicationsManager::handleRemoveLine(int idx) {
    if (idx >= 0 && static_cast<size_t>(idx) < currentLines.size()) {
        currentLines.erase(currentLines.begin() + idx);
        saveConfig();
        rebuildLines();
        std::cout << "Removed line at index: " << idx << std::endl;
    }
}

std::string ApplicationsManager::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(start, end - start + 1);
}

// Static callback implementations
void ApplicationsManager::onBackButtonClicked(GtkButton* /*button*/, gpointer user_data) {
    ApplicationsManager* manager = static_cast<ApplicationsManager*>(user_data);
    std::cout << "=== BACK BUTTON CLICKED ===" << std::endl;
    
    // Hide this manager first
    manager->hide();
    
    // Then show main menu
    manager->mainWindow->showMainMenu();
    
    std::cout << "=== RETURNED TO MAIN MENU ===" << std::endl;
}

void ApplicationsManager::onLeftArrowClicked(GtkButton* /*button*/, gpointer user_data) {
    ApplicationsManager* manager = static_cast<ApplicationsManager*>(user_data);
    manager->handleLeftArrow();
}

void ApplicationsManager::onRightArrowClicked(GtkButton* /*button*/, gpointer user_data) {
    ApplicationsManager* manager = static_cast<ApplicationsManager*>(user_data);
    manager->handleRightArrow();
}

void ApplicationsManager::onRefreshButtonClicked(GtkButton* /*button*/, gpointer user_data) {
    ApplicationsManager* manager = static_cast<ApplicationsManager*>(user_data);
    std::cout << "=== REFRESH BUTTON CLICKED ===" << std::endl;
    manager->refreshData();
    std::cout << "Config files refreshed successfully" << std::endl;
}

void ApplicationsManager::onLineEditChanged(GtkEditable* editable, gpointer user_data) {
    LineEditData* data = static_cast<LineEditData*>(user_data);
    const char* text = gtk_editable_get_text(editable);
    data->manager->handleLineEdited(data->index, std::string(text ? text : ""));
}

void ApplicationsManager::onAddButtonClicked(GtkButton* /*button*/, gpointer user_data) {
    LineEditData* data = static_cast<LineEditData*>(user_data);
    data->manager->handleAddLine(data->index);
}

void ApplicationsManager::onRemoveButtonClicked(GtkButton* /*button*/, gpointer user_data) {
    LineEditData* data = static_cast<LineEditData*>(user_data);
    data->manager->handleRemoveLine(data->index);
}

gboolean ApplicationsManager::onRefreshTimer(gpointer user_data) {
    ApplicationsManager* manager = static_cast<ApplicationsManager*>(user_data);
    manager->refreshData();
    return G_SOURCE_CONTINUE;
}