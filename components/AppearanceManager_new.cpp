#include "AppearanceManager.h"
#include "../MainWindow.h"
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
    
    // Create main container with proper centering
    appearanceContainer = gtk_fixed_new();
    gtk_widget_set_size_request(appearanceContainer, 1000, 600);
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), appearanceContainer);
    gtk_widget_set_visible(appearanceContainer, FALSE);
    
    // Center the container in the overlay
    gtk_widget_set_halign(appearanceContainer, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(appearanceContainer, GTK_ALIGN_CENTER);
    
    // Add CSS class for styling
    gtk_widget_add_css_class(appearanceContainer, "appearance-container");
    
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
        "  backdrop-filter: blur(25px);"
        "}"
        ".appearance-title {"
        "  color: #ffffff;"
        "  font-size: 32px;"
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
        "  border-radius: 24px;"
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
        "  border-radius: 20px;"
        "  transition: all 0.5s ease;"
        "  box-shadow: 0 8px 25px rgba(0, 0, 0, 0.2);"
        "}"
        ".theme-tick {"
        "  color: #e5a7c6;"
        "  font-size: 24px;"
        "  font-weight: 900;"
        "  text-shadow: 0 3px 15px rgba(0, 0, 0, 0.8), 0 0 30px rgba(229, 167, 198, 0.6);"
        "  background: rgba(0, 0, 0, 0.4);"
        "  border-radius: 50%;"
        "  border: 2px solid rgba(229, 167, 198, 0.6);"
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
        "  transform: scale(1.15) translateY(-4px);"
        "  box-shadow: 0 15px 45px rgba(229, 167, 198, 0.5), inset 0 3px 0 rgba(255, 255, 255, 0.3);"
        "  text-shadow: 0 3px 12px rgba(0, 0, 0, 0.4);"
        "}"
        ".action-button {"
        "  background: linear-gradient(135deg, #e5a7c6 0%, #edcee3 30%, #f0d4e8 70%, #f5dced 100%);"
        "  color: #ffffff;"
        "  font-weight: 800;"
        "  font-size: 18px;"
        "  border: none;"
        "  padding: 16px 32px;"
        "  letter-spacing: 2px;"
        "  border-radius: 20px;"
        "  transition: all 0.5s cubic-bezier(0.34, 1.56, 0.64, 1);"
        "  box-shadow: 0 12px 35px rgba(229, 167, 198, 0.5), inset 0 2px 0 rgba(255, 255, 255, 0.25);"
        "  text-shadow: 0 3px 8px rgba(0, 0, 0, 0.25);"
        "  backdrop-filter: blur(15px);"
        "}"
        ".action-button:hover {"
        "  background: linear-gradient(135deg, #edcee3 0%, #f0d4e8 30%, #f5dced 70%, #fae6f1 100%);"
        "  transform: translateY(-6px) scale(1.08);"
        "  box-shadow: 0 18px 50px rgba(229, 167, 198, 0.6), inset 0 3px 0 rgba(255, 255, 255, 0.35);"
        "  text-shadow: 0 3px 12px rgba(0, 0, 0, 0.35);"
        "}";
    
    gtk_css_provider_load_from_string(cssProvider, css);
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(cssProvider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    
    setupBackButton();
    
    // Main title - perfectly centered
    titleLabel = gtk_label_new("APPEARANCE");
    gtk_widget_add_css_class(titleLabel, "appearance-title");
    gtk_widget_set_size_request(titleLabel, 400, 50);
    gtk_label_set_xalign(GTK_LABEL(titleLabel), 0.5);
    gtk_fixed_put(GTK_FIXED(appearanceContainer), titleLabel, 300, 30);
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
    GtkWidget* label = gtk_label_new("BACK");
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
    GtkWidget* themeSectionLabel = gtk_label_new("THEME SELECTION");
    gtk_widget_add_css_class(themeSectionLabel, "section-title");
    gtk_widget_set_size_request(themeSectionLabel, 400, 35);
    gtk_label_set_xalign(GTK_LABEL(themeSectionLabel), 0.5);
    gtk_fixed_put(GTK_FIXED(appearanceContainer), themeSectionLabel, 300, 100);
    appearanceWidgets.push_back(themeSectionLabel);
    
    // Theme selection container
    themeContainer = gtk_fixed_new();
    gtk_widget_set_size_request(themeContainer, 400, 140);
    gtk_fixed_put(GTK_FIXED(appearanceContainer), themeContainer, 300, 150);
    appearanceWidgets.push_back(themeContainer);
    
    // Initialize theme options
    themes = {
        {"Light", "Light", "light.png", false},
        {"Dark", "Dark", "dark.png", false}
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
        GtkWidget* themeTick = gtk_label_new("✓");
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
    gtk_fixed_put(GTK_FIXED(appearanceContainer), wallpaperSection, 150, 310);
    appearanceWidgets.push_back(wallpaperSection);
    
    // Wallpaper section title
    wallpaperLabel = gtk_label_new("WALLPAPER SELECTION");
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
    previousButton = gtk_button_new_with_label("‹");
    gtk_widget_set_size_request(previousButton, 50, 50);
    gtk_widget_add_css_class(previousButton, "nav-button");
    gtk_widget_set_tooltip_text(previousButton, "Previous wallpaper");
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
    nextButton = gtk_button_new_with_label("›");
    gtk_widget_set_size_request(nextButton, 50, 50);
    gtk_widget_add_css_class(nextButton, "nav-button");
    gtk_widget_set_tooltip_text(nextButton, "Next wallpaper");
    gtk_fixed_put(GTK_FIXED(previewContainer), nextButton, 580, 75);
    g_signal_connect(nextButton, "clicked", G_CALLBACK(onNextWallpaperClicked), this);
    appearanceWidgets.push_back(nextButton);
}

void AppearanceManager::setupButtons() {
    if (!appearanceContainer) return;
    
    // Button container - better positioning
    GtkWidget* buttonContainer = gtk_fixed_new();
    gtk_widget_set_size_request(buttonContainer, 400, 60);
    gtk_fixed_put(GTK_FIXED(appearanceContainer), buttonContainer, 300, 520);
    appearanceWidgets.push_back(buttonContainer);
    
    // Browse button - larger and better styled
    browseButton = gtk_button_new_with_label("BROWSE");
    gtk_widget_set_size_request(browseButton, 180, 50);
    gtk_widget_add_css_class(browseButton, "action-button");
    gtk_widget_set_tooltip_text(browseButton, "Open wallpaper folder");
    gtk_fixed_put(GTK_FIXED(buttonContainer), browseButton, 10, 5);
    g_signal_connect(browseButton, "clicked", G_CALLBACK(onBrowseWallpapersClicked), this);
    appearanceWidgets.push_back(browseButton);
    
    // Apply button - larger and better styled
    applyButton = gtk_button_new_with_label("APPLY");
    gtk_widget_set_size_request(applyButton, 180, 50);
    gtk_widget_add_css_class(applyButton, "action-button");
    gtk_widget_set_tooltip_text(applyButton, "Set this wallpaper");
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
    
    selectedTheme = themeName;
    updateThemeVisuals();
    loadWallpapers();
    updateWallpaperPreview();
    runThemeScript(themeName);
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
    if (fileExists(scriptPath)) {
        std::string command = "bash \"" + scriptPath + "\" &";
        executeCommand(command);
        std::cout << "Running theme script: " << scriptPath << std::endl;
    } else {
        std::cout << "Theme script not found: " << scriptPath << std::endl;
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

