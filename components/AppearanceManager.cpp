#include "AppearanceManager.h"
#include "../MainWindow.h"
#include "../translations/translations.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <unistd.h>
#include <algorithm>
#include <filesystem>

AppearanceManager::AppearanceManager(MainWindow* mainWindow, GtkWindow* parentWindow, GtkWidget* overlay)
    : mainWindow(mainWindow), parentWindow(parentWindow), overlay(overlay),
      appearanceContainer(nullptr), backButton(nullptr), titleLabel(nullptr),
      themeContainer(nullptr), wallpaperPreview(nullptr), wallpaperLabel(nullptr),
      previousButton(nullptr), nextButton(nullptr), applyButton(nullptr), browseButton(nullptr),
      currentWallpaperIndex(0), themeDetectionTimer(0) {
    
    setupUI();
}

AppearanceManager::~AppearanceManager() {
    // Stop theme detection timer
    if (themeDetectionTimer > 0) {
        g_source_remove(themeDetectionTimer);
        themeDetectionTimer = 0;
    }
    
    // Clear widget vectors for cleanup
    appearanceWidgets.clear();
    themeWidgets.clear();
}

void AppearanceManager::setupUI() {
    if (!overlay) return;
    
    // Create main container like other managers
    appearanceContainer = gtk_fixed_new();
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), appearanceContainer);
    gtk_widget_set_visible(appearanceContainer, FALSE);
    
    initAppearanceUI();
}

void AppearanceManager::initAppearanceUI() {
    if (!appearanceContainer) return;
    
    // Apply sophisticated CSS styling
    GtkCssProvider* cssProvider = gtk_css_provider_new();
    const char* css =
        ".appearance-container {"
        "  background: linear-gradient(145deg, rgba(255, 255, 255, 0.12), rgba(229, 167, 198, 0.06));"
        "  border: 2px solid rgba(229, 167, 198, 0.25);"
        "  border-radius: 32px;"
        "  box-shadow: 0 20px 60px rgba(0, 0, 0, 0.4), inset 0 2px 0 rgba(255, 255, 255, 0.15);"
        "}"
        ".appearance-title {"
        "  color: #ffffff;"
        "  font-size: 32px;"
        "  font-family: ElysiaOSNew12;"
        "  font-weight: 900;"
        "  letter-spacing: 4px;"
        "  text-shadow: 0 4px 20px rgba(0, 0, 0, 0.6), 0 2px 4px rgba(229, 167, 198, 0.4);"
        "}"
        ".section-title {"
        "  color: #e5a7c6;"
        "  font-size: 22px;"
        "  font-weight: 800;"
        "  letter-spacing: 3px;"
        "  text-shadow: 0 3px 12px rgba(0, 0, 0, 0.5);"
        "}"
        ".theme-frame {"
        "  background: linear-gradient(145deg, rgba(255, 255, 255, 0.12), rgba(229, 167, 198, 0.04));"
        "  border: 3px solid rgba(255, 255, 255, 0.2);"
        "  border-radius: 32px;"
        "  transition: all 0.5s cubic-bezier(0.34, 1.56, 0.64, 1);"
        "  box-shadow: 0 12px 35px rgba(0, 0, 0, 0.25), inset 0 2px 0 rgba(255, 255, 255, 0.15);"
        "  backdrop-filter: blur(15px);"
        "}"
        ".theme-frame:hover {"
        "  background: linear-gradient(145deg, rgba(229, 167, 198, 0.2), rgba(237, 206, 227, 0.12));"
        "  border-color: rgba(229, 167, 198, 0.9);"
        "  transform: translateY(-8px) scale(1.05);"
        "  box-shadow: 0 25px 50px rgba(229, 167, 198, 0.35), inset 0 3px 0 rgba(255, 255, 255, 0.25);"
        "}"
        ".theme-frame.selected {"
        "  background: linear-gradient(145deg, rgba(229, 167, 198, 0.25), rgba(237, 206, 227, 0.15));"
        "  border-color: #e5a7c6;"
        "  box-shadow: 0 18px 45px rgba(229, 167, 198, 0.5), inset 0 3px 0 rgba(255, 255, 255, 0.25);"
        "  transform: translateY(-4px) scale(1.02);"
        "}"
        ".theme-image {"
        "  border-radius: 28px;"
        "  transition: all 0.5s ease;"
        "  box-shadow: 0 8px 25px rgba(0, 0, 0, 0.2);"
        "}"
        ".wallpaper-section {"
        "  background: linear-gradient(145deg, rgba(255, 255, 255, 0.08), rgba(229, 167, 198, 0.03));"
        "  border-radius: 24px;"
        "  border: 2px solid rgba(229, 167, 198, 0.15);"
        "  box-shadow: 0 12px 40px rgba(0, 0, 0, 0.2), inset 0 2px 0 rgba(255, 255, 255, 0.1);"
        "  backdrop-filter: blur(20px);"
        "  padding: 24px;"
        "}"
        ".wallpaper-preview {"
        "  background: linear-gradient(145deg, rgba(255, 255, 255, 0.1), rgba(229, 167, 198, 0.04));"
        "  border-radius: 20px;"
        "  border: 3px solid rgba(229, 167, 198, 0.25);"
        "  box-shadow: 0 15px 45px rgba(0, 0, 0, 0.3), inset 0 2px 0 rgba(255, 255, 255, 0.15);"
        "  transition: all 0.4s ease;"
        "  backdrop-filter: blur(20px);"
        "}"
        ".wallpaper-preview:hover {"
        "  transform: scale(1.03);"
        "  box-shadow: 0 20px 60px rgba(0, 0, 0, 0.4), inset 0 3px 0 rgba(255, 255, 255, 0.2);"
        "  border-color: rgba(229, 167, 198, 0.4);"
        "}"
        ".nav-button {"
        "  color: #ffffff;"
        "  font-size: 32px;"
        "  font-weight: 900;"
        "  background: linear-gradient(145deg, rgba(229, 167, 198, 0.2), rgba(237, 206, 227, 0.08));"
        "  border: 3px solid rgba(229, 167, 198, 0.4);"
        "  border-radius: 30px;"
        "  transition: all 0.4s cubic-bezier(0.34, 1.56, 0.64, 1);"
        "  box-shadow: 0 10px 30px rgba(0, 0, 0, 0.25), inset 0 2px 0 rgba(255, 255, 255, 0.2);"
        "  backdrop-filter: blur(15px);"
        "}"
        ".nav-button:hover {"
        "  background: linear-gradient(145deg, #e5a7c6, #edcee3);"
        "  border-color: #e5a7c6;"
        "  transform: scale(1.05) translateY(-2px);"
        "  box-shadow: 0 10px 25px rgba(229, 167, 198, 0.5);"
        "  text-shadow: 0 2px 8px rgba(0, 0, 0, 0.4);"
        "}"
        ".browse-button {"
        "  background: linear-gradient(135deg, #ffa6da 0%, #fed0f4 30%, #ffeef8 70%, #ffffff 100%);"
        "  color: #8b5a6b;"
        "  font-family: ElysiaOSNew12;"
        "  font-weight: 800;"
        "  font-size: 18px;"
        "  border: none;"
        "  outline: none;"
        "  box-shadow: 0 8px 20px rgba(255, 166, 218, 0.4);"
        "  padding: 16px 32px;"
        "  letter-spacing: 2px;"
        "  border-radius: 20px;"
        "  transition: all 0.3s ease;"
        "  text-shadow: 0 1px 3px rgba(0, 0, 0, 0.2);"
        "}"
        ".browse-button:hover {"
        "  background: linear-gradient(135deg, #fed0f4 0%, #ffeef8 30%, #ffffff 70%, #f8f9fa 100%);"
        "  transform: translateY(-2px);"
        "  box-shadow: 0 12px 25px rgba(255, 166, 218, 0.5);"
        "  text-shadow: 0 2px 4px rgba(0, 0, 0, 0.3);"
        "}"
        ".apply-button {"
        "  background: linear-gradient(135deg, #ffa6da 0%, #fed0f4 30%, #ffeef8 70%, #ffffff 100%);"
        "  color: #8b5a6b;"
        "  font-family: ElysiaOSNew12;"
        "  font-weight: 800;"
        "  font-size: 18px;"
        "  border: none;"
        "  outline: none;"
        "  box-shadow: 0 8px 20px rgba(255, 166, 218, 0.4);"
        "  padding: 16px 32px;"
        "  letter-spacing: 2px;"
        "  border-radius: 20px;"
        "  transition: all 0.3s ease;"
        "  text-shadow: 0 1px 3px rgba(0, 0, 0, 0.2);"
        "}"
        ".apply-button:hover {"
        "  background: linear-gradient(135deg, #fed0f4 0%, #ffeef8 30%, #ffffff 70%, #f8f9fa 100%);"
        "  transform: translateY(-2px);"
        "  box-shadow: 0 12px 25px rgba(255, 166, 218, 0.5);"
        "  text-shadow: 0 2px 4px rgba(0, 0, 0, 0.3);"
        "}";
    
    gtk_css_provider_load_from_string(cssProvider, css);
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(cssProvider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    
    setupBackButton();
    
    // Main title - centered like other managers
    titleLabel = gtk_label_new(TR(TranslationKeys::APPEARANCE_TITLE));
    gtk_widget_add_css_class(titleLabel, "appearance-title");
    gtk_widget_set_size_request(titleLabel, 400, 50);
    gtk_label_set_xalign(GTK_LABEL(titleLabel), 0.5);
    gtk_fixed_put(GTK_FIXED(appearanceContainer), titleLabel, 600, 80);
    appearanceWidgets.push_back(titleLabel);
    
    createThemeOptions();
    setupWallpaperPreview();
    setupButtons();
    
    // Initialize themes and detect current theme
    selectedTheme = detectSystemTheme();
    updateThemeVisuals();
    loadWallpapers();
    updateWallpaperPreview();
}

void AppearanceManager::setupBackButton() {
    if (!appearanceContainer) return;
    
    std::string backPath = getAssetPath("back.png");
    
    // Create a button - EXACTLY like NetworkManager
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
    
    // Style the button to remove borders and background
    gtk_widget_set_size_request(backButton, 120, 60);
    gtk_widget_add_css_class(backButton, "flat");
    
    // Position the back button at top left of the appearance container
    gtk_fixed_put(GTK_FIXED(appearanceContainer), backButton, 0, 0);
    
    // Connect the callback
    g_signal_connect(backButton, "clicked", G_CALLBACK(onBackButtonClicked), this);
    
    appearanceWidgets.push_back(backButton);
    appearanceWidgets.push_back(fixedContainer);
    if (backImage) appearanceWidgets.push_back(backImage);
    if (label) appearanceWidgets.push_back(label);
}

void AppearanceManager::createThemeOptions() {
    if (!appearanceContainer) return;
    
    // Theme section title
    GtkWidget* themeSectionLabel = gtk_label_new(TR(TranslationKeys::THEME_SELECTION));
    gtk_widget_add_css_class(themeSectionLabel, "section-title");
    gtk_widget_set_size_request(themeSectionLabel, 400, 35);
    gtk_label_set_xalign(GTK_LABEL(themeSectionLabel), 0.5);
    gtk_fixed_put(GTK_FIXED(appearanceContainer), themeSectionLabel, 600, 150);
    appearanceWidgets.push_back(themeSectionLabel);
    
    // Theme selection container
    themeContainer = gtk_fixed_new();
    gtk_widget_set_size_request(themeContainer, 400, 140);
    gtk_fixed_put(GTK_FIXED(appearanceContainer), themeContainer, 600, 200);
    appearanceWidgets.push_back(themeContainer);
    
    // Initialize theme options
    themes = {
        {"Light", TR(TranslationKeys::LIGHT), "light.png", false},
        {"Dark", TR(TranslationKeys::DARK), "dark.png", false}
    };
    
    int xOffset = 60; // Better centering
    for (auto& theme : themes) {
        // Theme frame with larger size
        GtkWidget* themeFrame = gtk_fixed_new();
        gtk_widget_set_size_request(themeFrame, 130, 130);
        gtk_widget_add_css_class(themeFrame, "theme-frame");
        gtk_fixed_put(GTK_FIXED(themeContainer), themeFrame, xOffset, 5);
        
        // Theme image - larger
        std::string imagePath = getAssetPath(theme.imagePath);
        GtkWidget* themeImage = nullptr;
        if (fileExists(imagePath)) {
            themeImage = gtk_picture_new_for_filename(imagePath.c_str());
            gtk_widget_set_size_request(themeImage, 100, 100);
            gtk_widget_add_css_class(themeImage, "theme-image");
            gtk_fixed_put(GTK_FIXED(themeFrame), themeImage, 15, 15);
        } else {
            // Fallback text if image doesn't exist
            GtkWidget* fallbackLabel = gtk_label_new(theme.displayName.c_str());
            gtk_widget_set_size_request(fallbackLabel, 100, 100);
            gtk_widget_add_css_class(fallbackLabel, "theme-image");
            gtk_fixed_put(GTK_FIXED(themeFrame), fallbackLabel, 15, 15);
        }
        
        // Theme tick (checkmark) - better positioning
        GtkWidget* themeTick = gtk_label_new("");
        gtk_widget_add_css_class(themeTick, "theme-tick");
        gtk_widget_set_size_request(themeTick, 32, 32);
        gtk_widget_set_visible(themeTick, FALSE);
        gtk_fixed_put(GTK_FIXED(themeFrame), themeTick, 95, 5);
        
        // Make frame clickable
        GtkGesture* themeGesture = gtk_gesture_click_new();
        gtk_widget_add_controller(themeFrame, GTK_EVENT_CONTROLLER(themeGesture));
        g_object_set_data_full(G_OBJECT(themeGesture), "theme-name", 
                              g_strdup(theme.name.c_str()), g_free);
        g_signal_connect(themeGesture, "pressed", G_CALLBACK(onThemeClicked), this);
        
        themeWidgets[theme.name] = themeFrame;
        
        appearanceWidgets.push_back(themeFrame);
        if (themeImage) appearanceWidgets.push_back(themeImage);
        appearanceWidgets.push_back(themeTick);
        
        // Store tick widget reference for later access
        g_object_set_data(G_OBJECT(themeFrame), "tick-widget", themeTick);
        
        xOffset += 180;
    }
}

void AppearanceManager::setupWallpaperPreview() {
    if (!appearanceContainer) return;
    
    // Wallpaper section
    GtkWidget* wallpaperSection = gtk_fixed_new();
    gtk_widget_set_size_request(wallpaperSection, 700, 280);
    gtk_widget_add_css_class(wallpaperSection, "wallpaper-section");
    gtk_fixed_put(GTK_FIXED(appearanceContainer), wallpaperSection, 450, 360);
    appearanceWidgets.push_back(wallpaperSection);
    
    // Wallpaper section title
    wallpaperLabel = gtk_label_new(TR(TranslationKeys::WALLPAPER_SELECTION));
    gtk_widget_add_css_class(wallpaperLabel, "section-title");
    gtk_widget_set_size_request(wallpaperLabel, 400, 35);
    gtk_label_set_xalign(GTK_LABEL(wallpaperLabel), 0.5);
    gtk_fixed_put(GTK_FIXED(wallpaperSection), wallpaperLabel, 150, 20);
    appearanceWidgets.push_back(wallpaperLabel);
    
    // Preview container with navigation
    GtkWidget* previewContainer = gtk_fixed_new();
    gtk_widget_set_size_request(previewContainer, 650, 200);
    gtk_fixed_put(GTK_FIXED(wallpaperSection), previewContainer, 25, 70);
    appearanceWidgets.push_back(previewContainer);
    
    // Previous button - larger and better positioned
    previousButton = gtk_button_new_with_label(TR(TranslationKeys::PREVIOUS_ARROW));
    gtk_widget_set_size_request(previousButton, 50, 50);
    gtk_widget_add_css_class(previousButton, "nav-button");
    gtk_widget_set_tooltip_text(previousButton, TR(TranslationKeys::PREVIOUS_WALLPAPER));
    gtk_fixed_put(GTK_FIXED(previewContainer), previousButton, 20, 75);
    g_signal_connect(previousButton, "clicked", G_CALLBACK(onPreviousWallpaperClicked), this);
    appearanceWidgets.push_back(previousButton);
    
    // Wallpaper preview - larger and better centered
    wallpaperPreview = gtk_picture_new();
    gtk_widget_set_size_request(wallpaperPreview, 480, 160);
    gtk_widget_add_css_class(wallpaperPreview, "wallpaper-preview");
    gtk_picture_set_content_fit(GTK_PICTURE(wallpaperPreview), GTK_CONTENT_FIT_COVER);
    gtk_fixed_put(GTK_FIXED(previewContainer), wallpaperPreview, 85, 20);
    appearanceWidgets.push_back(wallpaperPreview);
    
    // Next button - larger and better positioned
    nextButton = gtk_button_new_with_label(TR(TranslationKeys::NEXT_ARROW));
    gtk_widget_set_size_request(nextButton, 50, 50);
    gtk_widget_add_css_class(nextButton, "nav-button");
    gtk_widget_set_tooltip_text(nextButton, TR(TranslationKeys::NEXT_WALLPAPER));
    gtk_fixed_put(GTK_FIXED(previewContainer), nextButton, 560, 75);
    g_signal_connect(nextButton, "clicked", G_CALLBACK(onNextWallpaperClicked), this);
    appearanceWidgets.push_back(nextButton);
}

void AppearanceManager::setupButtons() {
    if (!appearanceContainer) return;
    
    // Button container - moved down to avoid wallpaper section
    GtkWidget* buttonContainer = gtk_fixed_new();
    gtk_widget_set_size_request(buttonContainer, 400, 60);
    gtk_fixed_put(GTK_FIXED(appearanceContainer), buttonContainer, 600, 720);
    appearanceWidgets.push_back(buttonContainer);
    
    // Browse button - purple gradient styling
    browseButton = gtk_button_new_with_label(TR(TranslationKeys::BROWSE));
    gtk_widget_set_size_request(browseButton, 180, 50);
    gtk_widget_add_css_class(browseButton, "browse-button");
    gtk_widget_remove_css_class(browseButton, "button"); // Remove default GTK button class
    gtk_widget_set_tooltip_text(browseButton, TR(TranslationKeys::OPEN_WALLPAPER_FOLDER));
    gtk_widget_set_can_focus(browseButton, FALSE); // Disable focus to prevent focus boxes
    gtk_fixed_put(GTK_FIXED(buttonContainer), browseButton, 10, 5);
    g_signal_connect(browseButton, "clicked", G_CALLBACK(onBrowseWallpapersClicked), this);
    appearanceWidgets.push_back(browseButton);
    
    // Apply button - green gradient styling
    applyButton = gtk_button_new_with_label(TR(TranslationKeys::APPLY_CHANGES));
    gtk_widget_set_size_request(applyButton, 180, 50);
    gtk_widget_add_css_class(applyButton, "apply-button");
    gtk_widget_remove_css_class(applyButton, "button"); // Remove default GTK button class
    gtk_widget_set_tooltip_text(applyButton, TR(TranslationKeys::SET_THIS_WALLPAPER));
    gtk_widget_set_can_focus(applyButton, FALSE); // Disable focus to prevent focus boxes
    gtk_fixed_put(GTK_FIXED(buttonContainer), applyButton, 210, 5);
    g_signal_connect(applyButton, "clicked", G_CALLBACK(onApplyWallpaperClicked), this);
    appearanceWidgets.push_back(applyButton);
}

void AppearanceManager::show() {
    if (appearanceContainer) {
        gtk_widget_set_visible(appearanceContainer, TRUE);
        
        // Switch background
        if (mainWindow) {
            mainWindow->switchToBackground("background4.png");
        }
        
        // Start theme detection timer
        themeDetectionTimer = g_timeout_add(1000, onThemeDetectionTimer, this);
        
        // Refresh theme and wallpapers
        std::string currentTheme = detectSystemTheme();
        if (currentTheme != selectedTheme) {
            selectedTheme = currentTheme;
            updateThemeVisuals();
            loadWallpapers();
            updateWallpaperPreview();
        }
    }
}

void AppearanceManager::hide() {
    if (appearanceContainer) {
        gtk_widget_set_visible(appearanceContainer, FALSE);
        
        // Stop theme detection timer
        if (themeDetectionTimer > 0) {
            g_source_remove(themeDetectionTimer);
            themeDetectionTimer = 0;
        }
    }
}

std::string AppearanceManager::detectSystemTheme() {
    std::string result = executeCommand("gsettings get org.gnome.desktop.interface color-scheme");
    if (result.find("light") != std::string::npos) {
        return "Light";
    }
    return "Dark";
}

void AppearanceManager::selectTheme(const std::string& themeName) {
    if (selectedTheme == themeName) return;
    
    std::cout << "Selecting theme: " << themeName << std::endl;
    selectedTheme = themeName;
    updateThemeVisuals();
    runThemeScript(themeName);
    
    // Delay wallpaper loading slightly to allow theme script to complete
    g_timeout_add(500, [](gpointer user_data) -> gboolean {
        AppearanceManager* manager = static_cast<AppearanceManager*>(user_data);
        if (manager) {
            std::cout << "Refreshing wallpapers for theme: " << manager->selectedTheme << std::endl;
            manager->loadWallpapers();
            manager->updateWallpaperPreview();
        }
        return G_SOURCE_REMOVE;
    }, this);
}

void AppearanceManager::updateThemeVisuals() {
    for (auto& [themeName, widget] : themeWidgets) {
        bool isSelected = (themeName == selectedTheme);
        
        if (isSelected) {
            gtk_widget_add_css_class(widget, "selected");
        } else {
            gtk_widget_remove_css_class(widget, "selected");
        }
        
        // Update tick visibility
        GtkWidget* tick = static_cast<GtkWidget*>(g_object_get_data(G_OBJECT(widget), "tick-widget"));
        if (tick) {
            gtk_widget_set_visible(tick, isSelected);
        }
    }
}

void AppearanceManager::loadWallpapers() {
    wallpapers.clear();
    std::string wallpaperDir = getWallpaperPath(selectedTheme);
    
    if (!std::filesystem::exists(wallpaperDir)) {
        std::cout << "Wallpaper directory doesn't exist: " << wallpaperDir << std::endl;
        return;
    }
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(wallpaperDir)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                
                if (ext == ".jpg" || ext == ".jpeg" || ext == ".png") {
                    wallpapers.push_back(entry.path().filename().string());
                }
            }
        }
        
        // Sort wallpapers
        std::sort(wallpapers.begin(), wallpapers.end());
    } catch (const std::exception& e) {
        std::cerr << "Error loading wallpapers: " << e.what() << std::endl;
    }
    
    currentWallpaperIndex = 0;
}

void AppearanceManager::updateWallpaperPreview() {
    if (wallpapers.empty()) {
        gtk_picture_set_filename(GTK_PICTURE(wallpaperPreview), nullptr);
        selectedWallpaper.clear();
        return;
    }
    
    selectedWallpaper = wallpapers[currentWallpaperIndex];
    std::string wallpaperPath = getWallpaperPath(selectedTheme, selectedWallpaper);
    
    if (fileExists(wallpaperPath)) {
        gtk_picture_set_filename(GTK_PICTURE(wallpaperPreview), wallpaperPath.c_str());
    } else {
        gtk_picture_set_filename(GTK_PICTURE(wallpaperPreview), nullptr);
    }
}

void AppearanceManager::previousWallpaper() {
    if (!wallpapers.empty()) {
        currentWallpaperIndex = (currentWallpaperIndex - 1 + wallpapers.size()) % wallpapers.size();
        updateWallpaperPreview();
    }
}

void AppearanceManager::nextWallpaper() {
    if (!wallpapers.empty()) {
        currentWallpaperIndex = (currentWallpaperIndex + 1) % wallpapers.size();
        updateWallpaperPreview();
    }
}

void AppearanceManager::applyWallpaper() {
    if (selectedWallpaper.empty()) return;
    
    std::string wallpaperPath = getWallpaperPath(selectedTheme, selectedWallpaper);
    if (fileExists(wallpaperPath)) {
        std::string command = "swww img \"" + wallpaperPath + "\" --transition-type grow --transition-step 10 --transition-fps 60";
        executeCommand(command);
    }
}

void AppearanceManager::browseWallpapers() {
    std::string wallpaperDir = getWallpaperPath("");
    if (fileExists(wallpaperDir)) {
        std::string command = "xdg-open \"" + wallpaperDir + "\"";
        executeCommand(command);
    }
}

void AppearanceManager::runThemeScript(const std::string& themeName) {
    const char* home = getenv("HOME");
    if (!home) return;
    
    std::string scriptPath = std::string(home) + "/.config/Elysia/" + themeName + "Theme.sh";
    std::cout << "Looking for theme script at: " << scriptPath << std::endl;
    
    if (fileExists(scriptPath)) {
        std::cout << "Theme script found, executing: " << scriptPath << std::endl;
        
        // Try multiple methods to ensure script execution
        
        // Method 1: Direct bash execution
        std::string command1 = "bash \"" + scriptPath + "\"";
        std::cout << "Executing command: " << command1 << std::endl;
        int result1 = system(command1.c_str());
        std::cout << "Direct execution result: " << result1 << std::endl;
        
        // Method 2: Background execution
        std::string command2 = "bash \"" + scriptPath + "\" &";
        std::cout << "Executing command in background: " << command2 << std::endl;
        int result2 = system(command2.c_str());
        std::cout << "Background execution result: " << result2 << std::endl;
        
        // Method 3: Using executeCommand function
        std::string result3 = executeCommand(command1);
        std::cout << "executeCommand output: " << result3 << std::endl;
        
    } else {
        std::cout << "Theme script not found: " << scriptPath << std::endl;
        
        // List directory contents to debug
        std::string homeDir = std::string(home) + "/.config/Elysia/";
        std::string lsCommand = "ls -la \"" + homeDir + "\"";
        std::cout << "Directory listing:" << std::endl;
        std::cout << executeCommand(lsCommand) << std::endl;
    }
}

std::string AppearanceManager::getAssetPath(const std::string& filename) {
    const char* home = getenv("HOME");
    if (!home) return filename;
    
    return std::string(home) + "/.config/Elysia/assets/assets/" + filename;
}

std::string AppearanceManager::getWallpaperPath(const std::string& theme, const std::string& wallpaper) {
    const char* home = getenv("HOME");
    if (!home) return "";
    
    std::string basePath = std::string(home) + "/.config/Elysia/wallpaper/";
    if (theme.empty()) {
        return basePath;
    }
    
    basePath += theme + "/";
    if (wallpaper.empty()) {
        return basePath;
    }
    
    return basePath + wallpaper;
}

std::string AppearanceManager::executeCommand(const std::string& command) {
    std::string result;
    FILE* pipe = popen(command.c_str(), "r");
    if (pipe) {
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
        pclose(pipe);
    }
    return result;
}

bool AppearanceManager::fileExists(const std::string& path) {
    return access(path.c_str(), F_OK) == 0;
}

// Static callback implementations
void AppearanceManager::onBackButtonClicked(GtkButton*, gpointer user_data) {
    AppearanceManager* manager = static_cast<AppearanceManager*>(user_data);
    if (manager && manager->mainWindow) {
        manager->hide();
        manager->mainWindow->showMainMenu();
    }
}

void AppearanceManager::onThemeClicked(GtkGestureClick* gesture, int, double, double, gpointer user_data) {
    AppearanceManager* manager = static_cast<AppearanceManager*>(user_data);
    if (!manager) return;
    
    const char* themeName = static_cast<const char*>(g_object_get_data(G_OBJECT(gesture), "theme-name"));
    if (themeName) {
        manager->selectTheme(std::string(themeName));
    }
}

void AppearanceManager::onPreviousWallpaperClicked(GtkButton*, gpointer user_data) {
    AppearanceManager* manager = static_cast<AppearanceManager*>(user_data);
    if (manager) {
        manager->previousWallpaper();
    }
}

void AppearanceManager::onNextWallpaperClicked(GtkButton*, gpointer user_data) {
    AppearanceManager* manager = static_cast<AppearanceManager*>(user_data);
    if (manager) {
        manager->nextWallpaper();
    }
}

void AppearanceManager::onApplyWallpaperClicked(GtkButton*, gpointer user_data) {
    AppearanceManager* manager = static_cast<AppearanceManager*>(user_data);
    if (manager) {
        manager->applyWallpaper();
    }
}

void AppearanceManager::onBrowseWallpapersClicked(GtkButton*, gpointer user_data) {
    AppearanceManager* manager = static_cast<AppearanceManager*>(user_data);
    if (manager) {
        manager->browseWallpapers();
    }
}

gboolean AppearanceManager::onThemeDetectionTimer(gpointer user_data) {
    AppearanceManager* manager = static_cast<AppearanceManager*>(user_data);
    if (!manager) return G_SOURCE_REMOVE;
    
    std::string currentTheme = manager->detectSystemTheme();
    if (currentTheme != manager->lastDetectedTheme) {
        manager->lastDetectedTheme = currentTheme;
        if (currentTheme != manager->selectedTheme) {
            manager->selectTheme(currentTheme);
        }
    }
    
    return G_SOURCE_CONTINUE;
}
