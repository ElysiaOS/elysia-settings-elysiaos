#include "DisplayManager.h"
#include "../MainWindow.h"
#include "../translations/translations.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <algorithm>
#include <regex>

DisplayManager::DisplayManager(MainWindow* mainWindow, GtkWindow* parentWindow, GtkWidget* overlay)
    : mainWindow(mainWindow), parentWindow(parentWindow), overlay(overlay),
      displayContainer(nullptr), backButton(nullptr), titleLabel(nullptr),
      themeImageLabel(nullptr), resolutionLabel(nullptr), resolutionCombo(nullptr),
      brightnessLabel(nullptr), brightnessScale(nullptr), brightnessValueLabel(nullptr),
      applyButton(nullptr), infoContainer(nullptr), refreshTimer(0), userHasMadeSelection(false) {
    
    setupUI();
}

DisplayManager::~DisplayManager() {
    // Stop refresh timer
    if (refreshTimer > 0) {
        g_source_remove(refreshTimer);
        refreshTimer = 0;
    }
    
    // Clear widget vectors for cleanup
    displayWidgets.clear();
}

void DisplayManager::setupUI() {
    if (!overlay) return;
    
    // Create main container like other managers
    displayContainer = gtk_fixed_new();
    gtk_widget_add_css_class(displayContainer, "display-container");
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), displayContainer);
    gtk_widget_set_visible(displayContainer, FALSE);
    
    initDisplayUI();
}

void DisplayManager::initDisplayUI() {
    if (!displayContainer) return;
    
    // Apply sophisticated CSS styling
    GtkCssProvider* cssProvider = gtk_css_provider_new();
    const char* css =
        ".display-container {"
        "  background: linear-gradient(145deg, rgba(255, 255, 255, 0.12), rgba(255, 166, 218, 0.06));"
        "  border: 2px solid rgba(255, 166, 218, 0.25);"
        "  border-radius: 32px;"
        "  box-shadow: 0 20px 60px rgba(0, 0, 0, 0.4), inset 0 2px 0 rgba(255, 255, 255, 0.15);"
        "}"
        ".display-title {"
        "  font-family: ElysiaOSNew12;"
        "  color: #ffffff;"
        "  font-size: 32px;"
        "  font-weight: 900;"
        "  letter-spacing: 4px;"
        "  text-shadow: 0 4px 20px rgba(0, 0, 0, 0.6), 0 2px 4px rgba(255, 166, 218, 0.4);"
        "}"
        ".theme-preview {"
        "  border-radius: 20px;"
        "  border: 3px solid rgba(255, 166, 218, 0.4);"
        "  box-shadow: 0 12px 30px rgba(0, 0, 0, 0.3);"
        "}"
        ".display-section {"
        "  background: linear-gradient(145deg, rgba(255, 255, 255, 0.08), rgba(255, 166, 218, 0.03));"
        "  border-radius: 24px;"
        "  border: 2px solid rgba(255, 166, 218, 0.15);"
        "  box-shadow: 0 12px 40px rgba(0, 0, 0, 0.2), inset 0 2px 0 rgba(255, 255, 255, 0.1);"
        "  padding: 24px;"
        "}"
        ".section-label {"
        "  font-family: ElysiaOSNew12;"
        "  color: #ffa6da;"
        "  font-size: 18px;"
        "  font-weight: 800;"
        "  letter-spacing: 2px;"
        "  text-shadow: 0 2px 8px rgba(0, 0, 0, 0.4);"
        "}"
        ".display-combo {"
        "  background: linear-gradient(145deg, rgba(255, 255, 255, 0.15), rgba(255, 166, 218, 0.08));"
        "  color: #ffffff;"
        "  border: 2px solid rgba(255, 166, 218, 0.3);"
        "  border-radius: 20px;"
        "  padding: 12px 16px;"
        "  font-size: 16px;"
        "  font-weight: 600;"
        "  transition: all 0.3s ease;"
        "}"
        ".display-combo:hover {"
        "  background: linear-gradient(145deg, rgba(255, 255, 255, 0.25), rgba(255, 166, 218, 0.12));"
        "  border-color: rgba(255, 166, 218, 0.5);"
        "  box-shadow: 0 6px 20px rgba(255, 166, 218, 0.2);"
        "}"
        ".brightness-scale {"
        "  font-family: ElysiaOSNew12;"
        "}"
        ".brightness-scale trough {"
        "  background: rgba(255, 255, 255, 0.15);"
        "  border-radius: 12px;"
        "  min-height: 24px;"
        "  border: 2px solid rgba(255, 166, 218, 0.2);"
        "}"
        ".brightness-scale slider {"
        "  background: linear-gradient(145deg, #ffa6da, #fed0f4);"
        "  border: 2px solid rgba(255, 255, 255, 0.8);"
        "  border-radius: 14px;"
        "  min-width: 28px;"
        "  min-height: 28px;"
        "  box-shadow: 0 4px 12px rgba(255, 166, 218, 0.4);"
        "  transition: all 0.3s ease;"
        "}"
        ".brightness-scale slider:hover {"
        "  background: linear-gradient(145deg, #fed0f4, #ffeef8);"
        "  box-shadow: 0 6px 16px rgba(255, 166, 218, 0.6);"
        "  transform: scale(1.1);"
        "}"
        ".brightness-scale highlight {"
        "  background: linear-gradient(90deg, #ffa6da, #fed0f4);"
        "  border-radius: 10px;"
        "}"
        ".brightness-value {"
        "  font-family: ElysiaOSNew12;"
        "  color: #ffa6da;"
        "  font-size: 18px;"
        "  font-weight: 800;"
        "  text-shadow: 0 2px 6px rgba(0, 0, 0, 0.4);"
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
    
    // Main title - positioned like AppearanceManager
    titleLabel = gtk_label_new(TR(TranslationKeys::DISPLAY_TITLE));
    gtk_widget_add_css_class(titleLabel, "display-title");
    gtk_widget_set_size_request(titleLabel, 400, 50);
    gtk_label_set_xalign(GTK_LABEL(titleLabel), 0.5);
    gtk_fixed_put(GTK_FIXED(displayContainer), titleLabel, 600, 80); // Same as AppearanceManager
    displayWidgets.push_back(titleLabel);
    
    createDisplayControls();
    createInfoSection();
    
    // Initial updates
    updateDisplayModes();
    updateBrightnessSlider();
    updateThemeImage();
}

void DisplayManager::setupBackButton() {
    if (!displayContainer) return;
    
    std::string backPath = getAssetPath("back.png");
    
    // Create a button - EXACTLY like other managers
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
    
    // Position the back button at top left
    gtk_fixed_put(GTK_FIXED(displayContainer), backButton, 0, 0);
    
    // Connect the callback
    g_signal_connect(backButton, "clicked", G_CALLBACK(onBackButtonClicked), this);
    
    displayWidgets.push_back(backButton);
    displayWidgets.push_back(fixedContainer);
    if (backImage) displayWidgets.push_back(backImage);
    if (label) displayWidgets.push_back(label);
}

void DisplayManager::createDisplayControls() {
    if (!displayContainer) return;
    
    // Theme preview image - positioned like AppearanceManager theme section
    themeImageLabel = gtk_picture_new();
    gtk_widget_set_size_request(themeImageLabel, 200, 370);
    gtk_widget_add_css_class(themeImageLabel, "theme-preview");
    gtk_fixed_put(GTK_FIXED(displayContainer), themeImageLabel, 350, 150); // Follow AppearanceManager pattern
    displayWidgets.push_back(themeImageLabel);
    
    // Display controls section - positioned like AppearanceManager wallpaper section
    GtkWidget* controlsSection = gtk_fixed_new();
    gtk_widget_set_size_request(controlsSection, 400, 350);
    gtk_widget_add_css_class(controlsSection, "display-section");
    gtk_fixed_put(GTK_FIXED(displayContainer), controlsSection, 600, 150); // Follow AppearanceManager pattern
    displayWidgets.push_back(controlsSection);
    
    // Resolution label and combo
    resolutionLabel = gtk_label_new(TR(TranslationKeys::DISPLAY_RESOLUTION));
    gtk_widget_add_css_class(resolutionLabel, "section-label");
    gtk_widget_set_size_request(resolutionLabel, 300, 25);
    gtk_label_set_xalign(GTK_LABEL(resolutionLabel), 0.0);
    gtk_fixed_put(GTK_FIXED(controlsSection), resolutionLabel, 30, 30);
    displayWidgets.push_back(resolutionLabel);
    
    resolutionCombo = gtk_combo_box_text_new();
    gtk_widget_set_size_request(resolutionCombo, 340, 50);
    gtk_widget_add_css_class(resolutionCombo, "display-combo");
    gtk_fixed_put(GTK_FIXED(controlsSection), resolutionCombo, 30, 70);
    g_signal_connect(resolutionCombo, "changed", G_CALLBACK(onResolutionChanged), this);
    displayWidgets.push_back(resolutionCombo);
    
    // Brightness label and scale
    brightnessLabel = gtk_label_new(TR(TranslationKeys::SCREEN_BRIGHTNESS));
    gtk_widget_add_css_class(brightnessLabel, "section-label");
    gtk_widget_set_size_request(brightnessLabel, 200, 25);
    gtk_label_set_xalign(GTK_LABEL(brightnessLabel), 0.0);
    gtk_fixed_put(GTK_FIXED(controlsSection), brightnessLabel, 30, 160);
    displayWidgets.push_back(brightnessLabel);
    
    brightnessValueLabel = gtk_label_new(TR(TranslationKeys::BRIGHTNESS_VALUE));
    gtk_widget_add_css_class(brightnessValueLabel, "brightness-value");
    gtk_widget_set_size_request(brightnessValueLabel, 80, 25);
    gtk_label_set_xalign(GTK_LABEL(brightnessValueLabel), 1.0);
    gtk_fixed_put(GTK_FIXED(controlsSection), brightnessValueLabel, 290, 160);
    displayWidgets.push_back(brightnessValueLabel);
    
    brightnessScale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 1, 100, 1);
    gtk_widget_set_size_request(brightnessScale, 340, 40);
    gtk_widget_add_css_class(brightnessScale, "brightness-scale");
    gtk_scale_set_draw_value(GTK_SCALE(brightnessScale), FALSE);
    gtk_range_set_value(GTK_RANGE(brightnessScale), 50);
    gtk_fixed_put(GTK_FIXED(controlsSection), brightnessScale, 30, 200);
    g_signal_connect(brightnessScale, "value-changed", G_CALLBACK(onBrightnessChanged), this);
    displayWidgets.push_back(brightnessScale);
    
    // Apply button
    applyButton = gtk_button_new_with_label(TR(TranslationKeys::APPLY_CHANGES));
    gtk_widget_set_size_request(applyButton, 200, 50);
    gtk_widget_add_css_class(applyButton, "apply-button");
    gtk_widget_remove_css_class(applyButton, "button"); // Remove default GTK button class
    gtk_widget_set_can_focus(applyButton, FALSE);
    gtk_fixed_put(GTK_FIXED(controlsSection), applyButton, 100, 270);
    g_signal_connect(applyButton, "clicked", G_CALLBACK(onApplyButtonClicked), this);
    displayWidgets.push_back(applyButton);
}

void DisplayManager::createInfoSection() {
    if (!displayContainer) return;
    
    // Info section for additional display information - positioned like AppearanceManager buttons
    infoContainer = gtk_fixed_new();
    gtk_widget_set_size_request(infoContainer, 650, 100);
    gtk_widget_add_css_class(infoContainer, "display-section");
    gtk_fixed_put(GTK_FIXED(displayContainer), infoContainer, 380, 550); // Follow AppearanceManager pattern
    displayWidgets.push_back(infoContainer);
    
    GtkWidget* infoTitle = gtk_label_new(TR(TranslationKeys::DISPLAY_INFORMATION));
    gtk_widget_add_css_class(infoTitle, "section-label");
    gtk_widget_set_size_request(infoTitle, 300, 25);
    gtk_label_set_xalign(GTK_LABEL(infoTitle), 0.5);
    gtk_fixed_put(GTK_FIXED(infoContainer), infoTitle, 145, 20);
    displayWidgets.push_back(infoTitle);
    
    GtkWidget* infoText = gtk_label_new(TR(TranslationKeys::DISPLAY_INFO_TEXT));
    gtk_widget_add_css_class(infoText, "info-label");
    gtk_widget_set_size_request(infoText, 500, 10);
    gtk_label_set_xalign(GTK_LABEL(infoText), 0.5);
    gtk_label_set_justify(GTK_LABEL(infoText), GTK_JUSTIFY_CENTER);
    gtk_fixed_put(GTK_FIXED(infoContainer), infoText, 50, 60);
    displayWidgets.push_back(infoText);
}

void DisplayManager::show() {
    if (displayContainer) {
        gtk_widget_set_visible(displayContainer, TRUE);
        
        // Switch background
        if (mainWindow) {
            mainWindow->switchToBackground("background4.png");
        }
        
        // Reset user selection flag when showing the manager
        userHasMadeSelection = false;
        
        // Start refresh timer (update every 5 seconds)
        refreshTimer = g_timeout_add(5000, onRefreshTimer, this);
        
        // Immediate updates
        updateDisplayModes();
        updateBrightnessSlider();
        updateThemeImage();
    }
}

void DisplayManager::hide() {
    if (displayContainer) {
        gtk_widget_set_visible(displayContainer, FALSE);
        
        // Stop refresh timer
        if (refreshTimer > 0) {
            g_source_remove(refreshTimer);
            refreshTimer = 0;
        }
    }
}

std::string DisplayManager::detectThemeImage() {
    std::string basePath = getAssetPath("");
    
    std::string output = executeCommand("gsettings get org.gnome.desktop.interface color-scheme");
    
    if (output.find("prefer-light") != std::string::npos) {
        return basePath + "light1.png";
    } else {
        return basePath + "dark1.png";
    }
}

std::vector<DisplayMode> DisplayManager::getMonitorModes() {
    std::vector<DisplayMode> modes;
    
    std::string output = executeCommand("hyprctl monitors");
    
    // Parse hyprctl output for display modes
    std::regex modeRegex(R"((\d{3,4})x(\d{3,4})@(\d{2,3}))");
    std::sregex_iterator iter(output.begin(), output.end(), modeRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        DisplayMode mode;
        mode.resolution = match[1].str() + "x" + match[2].str();
        mode.refreshRate = match[3].str();
        mode.fullMode = match[0].str();
        
        // Avoid duplicates
        bool found = false;
        for (const auto& existing : modes) {
            if (existing.fullMode == mode.fullMode) {
                found = true;
                break;
            }
        }
        if (!found) {
            modes.push_back(mode);
        }
    }
    
    // Sort by resolution and refresh rate
    std::sort(modes.begin(), modes.end(), [](const DisplayMode& a, const DisplayMode& b) {
        return a.fullMode < b.fullMode;
    });
    
    return modes;
}

void DisplayManager::updateDisplayModes() {
    if (!resolutionCombo) return;
    
    // Store current selection if user has made one
    int currentSelection = -1;
    if (userHasMadeSelection) {
        currentSelection = gtk_combo_box_get_active(GTK_COMBO_BOX(resolutionCombo));
    }
    
    availableModes = getMonitorModes();
    
    // Clear and add new modes
    gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(resolutionCombo));
    
    for (const auto& mode : availableModes) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(resolutionCombo), mode.fullMode.c_str());
    }
    
    // Restore user selection if they had made one, otherwise set to current system mode
    if (userHasMadeSelection && currentSelection >= 0 && currentSelection < static_cast<int>(availableModes.size())) {
        gtk_combo_box_set_active(GTK_COMBO_BOX(resolutionCombo), currentSelection);
        std::cout << "Restored user selection: " << availableModes[currentSelection].fullMode << std::endl;
    } else {
        // Set current mode from system
        std::string currentMode = getCurrentMode();
        if (!currentMode.empty()) {
            // Find and set the current mode in dropdown
            for (size_t i = 0; i < availableModes.size(); ++i) {
                if (availableModes[i].fullMode == currentMode) {
                    gtk_combo_box_set_active(GTK_COMBO_BOX(resolutionCombo), i);
                    std::cout << "Set current display mode: " << currentMode << std::endl;
                    break;
                }
            }
        } else {
            std::cout << "No current display mode detected from hyprland.conf" << std::endl;
        }
        // Reset the flag since we're setting to system mode
        userHasMadeSelection = false;
    }
}

std::string DisplayManager::getCurrentMode() {
    // Read from hyprland.conf to get current mode - EXACTLY like display.cpp lines 196-221
    const char* home = getenv("HOME");
    if (!home) return "";
    
    std::string configPath = std::string(home) + "/.config/hypr/hyprland.conf";
    std::ifstream file(configPath);
    std::string line;
    std::string currentMode;
    
    if (file.is_open()) {
        while (std::getline(file, line)) {
            // Check if line starts with "monitor" (not "#monitor") and contains "@"
            if (line.find("monitor") == 0 && line.find("@") != std::string::npos) {
                // Extract resolution@refresh pattern using regex
                std::regex re(R"(\d{3,4}x\d{3,4}@\d{2,3})");
                std::smatch match;
                if (std::regex_search(line, match, re)) {
                    currentMode = match[0].str();
                }
                
                // Extract monitor name for future use
                std::istringstream iss(line);
                std::string token;
                std::vector<std::string> parts;
                while (iss >> token) {
                    parts.push_back(token);
                }
                if (parts.size() > 1) {
                    currentMonitorName = parts[1];
                }
                break; // Stop after first monitor line found
            }
        }
        file.close();
    }
    
    return currentMode;
}

void DisplayManager::updateBrightnessSlider() {
    if (!brightnessScale || !brightnessValueLabel) return;
    
    int currentBrightness = getCurrentBrightness();
    
    // Block signal to prevent recursive calls
    g_signal_handlers_block_by_func(brightnessScale, (gpointer)onBrightnessChanged, this);
    gtk_range_set_value(GTK_RANGE(brightnessScale), currentBrightness);
    g_signal_handlers_unblock_by_func(brightnessScale, (gpointer)onBrightnessChanged, this);
    
    // Update value label
    std::string valueText = std::to_string(currentBrightness) + "%";
    gtk_label_set_text(GTK_LABEL(brightnessValueLabel), valueText.c_str());
}

int DisplayManager::getCurrentBrightness() {
    std::string output = executeCommand("brightnessctl g");
    std::string maxOutput = executeCommand("brightnessctl m");
    
    try {
        int current = std::stoi(output);
        int max = std::stoi(maxOutput);
        
        if (max > 0) {
            return (current * 100) / max;
        }
    } catch (...) {
        // Fall through to default
    }
    
    return 50; // Default value
}

int DisplayManager::getMaxBrightness() {
    std::string output = executeCommand("brightnessctl m");
    
    try {
        return std::stoi(output);
    } catch (...) {
        return 100; // Default value
    }
}

void DisplayManager::updateThemeImage() {
    if (!themeImageLabel) return;
    
    std::string imagePath = detectThemeImage();
    if (fileExists(imagePath)) {
        gtk_picture_set_filename(GTK_PICTURE(themeImageLabel), imagePath.c_str());
        gtk_picture_set_can_shrink(GTK_PICTURE(themeImageLabel), TRUE);
        gtk_picture_set_content_fit(GTK_PICTURE(themeImageLabel), GTK_CONTENT_FIT_COVER);
    }
}

void DisplayManager::setBrightness(int value) {
    std::string command = "brightnessctl set " + std::to_string(value) + "%";
    executeCommand(command);
    
    // Update value label
    std::string valueText = std::to_string(value) + "%";
    gtk_label_set_text(GTK_LABEL(brightnessValueLabel), valueText.c_str());
}

void DisplayManager::applyDisplayChanges() {
    if (!resolutionCombo) return;
    
    int activeIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(resolutionCombo));
    if (activeIndex >= 0 && activeIndex < static_cast<int>(availableModes.size())) {
        std::string newMode = availableModes[activeIndex].fullMode;
        
        std::cout << "Applying display changes..." << std::endl;
        std::cout << "Selected mode: " << newMode << std::endl;
        
        if (writeHyprlandConfig(newMode)) {
            // Show success message
            std::cout << "✓ Display mode successfully changed to " << newMode << std::endl;
            std::cout << "✓ Restart Hyprland for changes to take effect." << std::endl;
            
            // Reset the user selection flag since changes have been applied
            userHasMadeSelection = false;
            
            // Update the current mode detection after successful write
            updateDisplayModes();
        } else {
            std::cout << "✗ Failed to update Hyprland configuration." << std::endl;
            std::cout << "✗ Please check file permissions and try again." << std::endl;
        }
    } else {
        std::cout << "✗ No valid display mode selected." << std::endl;
    }
}

bool DisplayManager::writeHyprlandConfig(const std::string& newMode) {
    // EXACTLY like display.cpp getCurrentMonitorLine and applyDisplayChange
    const char* home = getenv("HOME");
    if (!home) return false;
    
    std::string configPath = std::string(home) + "/.config/hypr/hyprland.conf";
    
    // Read all lines from config file
    std::ifstream inFile(configPath);
    std::vector<std::string> lines;
    std::string line;
    
    if (inFile.is_open()) {
        while (std::getline(inFile, line)) {
            lines.push_back(line);
        }
        inFile.close();
    } else {
        std::cout << "Could not open hyprland.conf for reading" << std::endl;
        return false;
    }
    
    // Find the monitor line that starts with "monitor" (not "#monitor")
    int monitorLineIndex = -1;
    for (size_t i = 0; i < lines.size(); ++i) {
        if (lines[i].find("monitor") == 0 && lines[i].find("@") != std::string::npos) {
            monitorLineIndex = i;
            break;
        }
    }
    
    if (monitorLineIndex == -1) {
        std::cout << "Could not find monitor configuration line." << std::endl;
        return false;
    }
    
    // Replace the resolution@refresh part in the found line
    std::regex re(R"(\d{3,4}x\d{3,4}@\d{2,3})");
    lines[monitorLineIndex] = std::regex_replace(lines[monitorLineIndex], re, newMode);
    
    // Write updated config back to file
    std::ofstream outFile(configPath);
    if (outFile.is_open()) {
        for (const auto& configLine : lines) {
            outFile << configLine << "\n";
        }
        outFile.close();
        std::cout << "Successfully updated hyprland.conf with new mode: " << newMode << std::endl;
        return true;
    } else {
        std::cout << "Failed to write to hyprland.conf" << std::endl;
        return false;
    }
}

std::string DisplayManager::executeCommand(const std::string& command) {
    std::string result;
    FILE* pipe = popen(command.c_str(), "r");
    if (pipe) {
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
        pclose(pipe);
    }
    
    // Trim whitespace
    result.erase(0, result.find_first_not_of(" \t\n\r"));
    result.erase(result.find_last_not_of(" \t\n\r") + 1);
    
    return result;
}

std::string DisplayManager::getAssetPath(const std::string& filename) {
    const char* home = getenv("HOME");
    if (!home) return filename;
    
    return std::string(home) + "/.config/Elysia/assets/assets/" + filename;
}

bool DisplayManager::fileExists(const std::string& path) {
    return access(path.c_str(), F_OK) == 0;
}

// Static callback implementations
void DisplayManager::onBackButtonClicked(GtkButton*, gpointer user_data) {
    DisplayManager* manager = static_cast<DisplayManager*>(user_data);
    if (manager && manager->mainWindow) {
        manager->hide();
        manager->mainWindow->showMainMenu();
    }
}

void DisplayManager::onBrightnessChanged(GtkRange* range, gpointer user_data) {
    DisplayManager* manager = static_cast<DisplayManager*>(user_data);
    if (manager) {
        int value = static_cast<int>(gtk_range_get_value(range));
        manager->setBrightness(value);
    }
}

void DisplayManager::onResolutionChanged(GtkComboBox*, gpointer user_data) {
    DisplayManager* manager = static_cast<DisplayManager*>(user_data);
    if (manager) {
        // Mark that user has made a selection
        manager->userHasMadeSelection = true;
        std::cout << "Resolution selection changed. Click Apply to save changes." << std::endl;
    }
}

void DisplayManager::onApplyButtonClicked(GtkButton*, gpointer user_data) {
    DisplayManager* manager = static_cast<DisplayManager*>(user_data);
    if (manager) {
        manager->applyDisplayChanges();
    }
}

gboolean DisplayManager::onRefreshTimer(gpointer user_data) {
    DisplayManager* manager = static_cast<DisplayManager*>(user_data);
    if (manager) {
        // Always update brightness and theme image
        manager->updateBrightnessSlider();
        manager->updateThemeImage();
        
        // Check if system configuration has changed externally
        std::string currentSystemMode = manager->getCurrentMode();
        std::string currentSelectedMode = "";
        
        if (manager->resolutionCombo && !manager->availableModes.empty()) {
            int activeIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(manager->resolutionCombo));
            if (activeIndex >= 0 && activeIndex < static_cast<int>(manager->availableModes.size())) {
                currentSelectedMode = manager->availableModes[activeIndex].fullMode;
            }
        }
        
        // Only update display modes if system configuration changed externally
        if (!currentSystemMode.empty() && currentSystemMode != currentSelectedMode) {
            std::cout << "System configuration changed externally, updating display modes" << std::endl;
            manager->updateDisplayModes();
        }
    }
    return G_SOURCE_CONTINUE;
}
