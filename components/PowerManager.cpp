#include "PowerManager.h"
#include "../MainWindow.h"
#include "../translations/translations.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>

PowerManager::PowerManager(MainWindow* mainWindow, GtkWindow* parentWindow, GtkWidget* overlay)
    : mainWindow(mainWindow), parentWindow(parentWindow), overlay(overlay),
      powerContainer(nullptr), backButton(nullptr), titleLabel(nullptr),
      modesContainer(nullptr), infoContainer(nullptr), applyButton(nullptr),
      selectedMode(""), currentMode(""), detectionTimer(0), isDetecting(false) {
    
    // Initialize power modes
    powerModes = {
        {"performance", TR(TranslationKeys::PERFORMANCE), "Maximum CPU performance for demanding tasks", "performance.png", "auto-cpufreq --force=performance"},
        {"balanced", TR(TranslationKeys::BALANCED), "Optimized balance between performance and power saving", "balanced.png", "auto-cpufreq --force=powersave"},
        {"default", TR(TranslationKeys::DEFAULT), "System default power management", "default.png", "auto-cpufreq --force=reset"}
    };
    
    setupUI();
}

PowerManager::~PowerManager() {
    if (detectionTimer > 0) {
        g_source_remove(detectionTimer);
        detectionTimer = 0;
    }
    
    // Clean up callback data
    for (auto* data : backButtonCallbacks) {
        delete data;
    }
    backButtonCallbacks.clear();
    
    for (auto* data : modeCallbacks) {
        delete data;
    }
    modeCallbacks.clear();
    
    // Clean up widgets
    powerWidgets.clear();
}

void PowerManager::setupUI() {
    if (!overlay) return;
    
    // Create main power container - positioned like DisplayManager
    powerContainer = gtk_fixed_new();
    gtk_widget_set_size_request(powerContainer, 1400, 700);
    gtk_widget_add_css_class(powerContainer, "power-container");
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), powerContainer);
    gtk_widget_set_visible(powerContainer, FALSE);
    powerWidgets.push_back(powerContainer);
    
    initPowerUI();
}

void PowerManager::initPowerUI() {
    if (!powerContainer) return;
    
    // Apply CSS styling similar to DisplayManager
    const char* css = 
        ".power-container {"
        "  background: transparent;"
        "}"
        ".power-title {"
        "  font-size: 28px;"
        "  font-weight: bold;"
        "  color: white;"
        "  font-family: ElysiaOSNew12;"
        "  text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.5);"
        "}"
        ".power-section {"
        "  background: rgba(255, 255, 255, 0.05);"
        "  border: 2px solid rgba(255, 255, 255, 0.15);"
        "  border-radius: 30px;"
        "  padding: 20px;"
        "  box-shadow: 0 8px 32px rgba(0, 0, 0, 0.3);"
        "}"
        ".power-mode-container {"
        "  background: transparent;"
        "  border-radius: 20px;"
        "  padding: 15px;"
        "}"
        ".power-mode-frame {"
        "  background: rgba(255, 255, 255, 0.02);"
        "  border: 2px solid transparent;"
        "  border-radius: 26px;"
        "  padding: 15px;"
        "  transition: all 0.3s ease;"
        "}"
        ".power-mode-frame:hover {"
        "  background: rgba(255, 255, 255, 0.08);"
        "  border: 2px solid rgba(229, 167, 198, 0.5);"
        "  transform: scale(1.02);"
        "}"
        ".power-mode-frame.selected {"
        "  background: rgba(229, 167, 198, 0.1);"
        "  border: 3px solid #e5a7c6;"
        "  box-shadow: 0 0 20px rgba(229, 167, 198, 0.3);"
        "}"
        ".power-mode-label {"
        "  font-size: 14px;"
        "  font-weight: bold;"
        "  color: white;"
        "  font-family: ElysiaOSNew12;"
        "  text-align: center;"
        "}"
        ".power-mode-desc {"
        "  font-size: 11px;"
        "  color: rgba(255, 255, 255, 0.8);"
        "  font-family: JetBrains Mono Nerd Font;"
        "  text-align: center;"
        "}"
        ".power-checkmark {"
        "  font-size: 24px;"
        "  color: #e5a7c6;"
        "  font-weight: bold;"
        "}"
        ".power-apply-button {"
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
        ".power-apply-button:hover {"
        "  background: linear-gradient(135deg, #fed0f4 0%, #ffeef8 30%, #ffffff 70%, #f8f9fa 100%);"
        "  transform: translateY(-2px);"
        "  box-shadow: 0 12px 25px rgba(255, 166, 218, 0.5);"
        "  text-shadow: 0 2px 4px rgba(0, 0, 0, 0.3);"
        "}"
        ".power-info-section {"
        "  background: rgba(255, 255, 255, 0.05);"
        "  border: 2px solid rgba(255, 255, 255, 0.15);"
        "  border-radius: 30px;"
        "  padding: 20px;"
        "}"
        ".power-info-title {"
        "  font-size: 16px;"
        "  font-weight: bold;"
        "  color: white;"
        "  font-family: ElysiaOSNew12;"
        "  text-align: center;"
        "}"
        ".power-info-text {"
        "  font-size: 12px;"
        "  color: rgba(255, 255, 255, 0.8);"
        "  font-family: ElysiaOSNew12;"
        "  text-align: center;"
        "  line-height: 1.4;"
        "}";
    
    GtkCssProvider* cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(cssProvider, css);
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(cssProvider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    
    setupBackButton();
    
    // Main title - positioned like DisplayManager
    titleLabel = gtk_label_new(TR(TranslationKeys::POWER_TITLE));
    gtk_widget_add_css_class(titleLabel, "power-title");
    gtk_widget_set_size_request(titleLabel, 400, 50);
    gtk_label_set_xalign(GTK_LABEL(titleLabel), 0.5);
    gtk_fixed_put(GTK_FIXED(powerContainer), titleLabel, 600, 80);
    powerWidgets.push_back(titleLabel);
    
    createPowerModes();
    createInfoSection();
    setupApplyButton();
}

void PowerManager::setupBackButton() {
    if (!powerContainer) return;
    
    std::string backPath = getAssetPath("back.png");
    
    backButton = gtk_button_new();
    if (!backButton) return;
    
    GtkWidget* fixedContainer = gtk_fixed_new();
    if (!fixedContainer) return;
    
    gtk_widget_set_size_request(fixedContainer, 120, 60);
    
    GtkWidget* backImage = gtk_picture_new_for_filename(backPath.c_str());
    if (!backImage) {
        backImage = gtk_image_new_from_icon_name("go-previous");
    } else {
        gtk_widget_set_size_request(backImage, 159, 66);
        gtk_picture_set_can_shrink(GTK_PICTURE(backImage), TRUE);
        gtk_picture_set_content_fit(GTK_PICTURE(backImage), GTK_CONTENT_FIT_CONTAIN);
    }
    
    GtkWidget* label = gtk_label_new(TR(TranslationKeys::BACK));
    if (label) {
        gtk_widget_add_css_class(label, "back-button-label");
    }
    
    if (backImage) {
        gtk_fixed_put(GTK_FIXED(fixedContainer), backImage, 0, 0);
    }
    if (label) {
        gtk_fixed_put(GTK_FIXED(fixedContainer), label, 40, 20);
    }
    
    gtk_button_set_child(GTK_BUTTON(backButton), fixedContainer);
    gtk_widget_set_size_request(backButton, 120, 60);
    gtk_widget_add_css_class(backButton, "flat");
    
    gtk_fixed_put(GTK_FIXED(powerContainer), backButton, 0, 0);
    powerWidgets.push_back(backButton);
    
    PowerBackButtonCallbackData* callbackData = new PowerBackButtonCallbackData{this};
    backButtonCallbacks.push_back(callbackData);
    g_signal_connect(backButton, "clicked", G_CALLBACK(onBackButtonClicked), callbackData);
}

void PowerManager::createPowerModes() {
    if (!powerContainer) return;
    
    // Create modes section container - positioned like DisplayManager controls
    modesContainer = gtk_fixed_new();
    gtk_widget_set_size_request(modesContainer, 600, 300);
    gtk_widget_add_css_class(modesContainer, "power-section");
    gtk_fixed_put(GTK_FIXED(powerContainer), modesContainer, 450, 150);
    powerWidgets.push_back(modesContainer);
    
    int xOffset = 50;
    int spacing = 180;
    
    for (size_t i = 0; i < powerModes.size(); ++i) {
        const PowerMode& mode = powerModes[i];
        
        // Create mode container
        GtkWidget* modeContainer = gtk_fixed_new();
        gtk_widget_set_size_request(modeContainer, 150, 250);
        gtk_widget_add_css_class(modeContainer, "power-mode-container");
        gtk_fixed_put(GTK_FIXED(modesContainer), modeContainer, xOffset + (i * spacing), 20);
        powerWidgets.push_back(modeContainer);
        
        // Create clickable frame
        GtkWidget* frame = gtk_frame_new(nullptr);
        gtk_widget_set_size_request(frame, 130, 180);
        gtk_widget_add_css_class(frame, "power-mode-frame");
        gtk_fixed_put(GTK_FIXED(modeContainer), frame, 10, 10);
        powerWidgets.push_back(frame);
        
        // Frame content container
        GtkWidget* frameContent = gtk_fixed_new();
        gtk_frame_set_child(GTK_FRAME(frame), frameContent);
        
        // Mode icon
        std::string iconPath = getAssetPath(mode.iconFile);
        GtkWidget* modeImage = nullptr;
        if (fileExists(iconPath)) {
            modeImage = gtk_picture_new_for_filename(iconPath.c_str());
            gtk_widget_set_size_request(modeImage, 80, 80);
            gtk_picture_set_can_shrink(GTK_PICTURE(modeImage), TRUE);
            gtk_picture_set_content_fit(GTK_PICTURE(modeImage), GTK_CONTENT_FIT_CONTAIN);
        } else {
            modeImage = gtk_label_new(TR(TranslationKeys::MISSING));
            gtk_widget_add_css_class(modeImage, "power-mode-label");
        }
        gtk_fixed_put(GTK_FIXED(frameContent), modeImage, 25, 20);
        powerWidgets.push_back(modeImage);
        
        // Mode label
        GtkWidget* modeLabel = gtk_label_new(mode.label.c_str());
        gtk_widget_add_css_class(modeLabel, "power-mode-label");
        gtk_widget_set_size_request(modeLabel, 120, 20);
        gtk_label_set_xalign(GTK_LABEL(modeLabel), 0.5);
        gtk_fixed_put(GTK_FIXED(frameContent), modeLabel, 5, 110);
        powerWidgets.push_back(modeLabel);
        
        // Mode description
        GtkWidget* modeDesc = gtk_label_new(mode.description.c_str());
        gtk_widget_add_css_class(modeDesc, "power-mode-desc");
        gtk_widget_set_size_request(modeDesc, 120, 40);
        gtk_label_set_xalign(GTK_LABEL(modeDesc), 0.5);
        gtk_label_set_wrap(GTK_LABEL(modeDesc), TRUE);
        gtk_label_set_wrap_mode(GTK_LABEL(modeDesc), PANGO_WRAP_WORD);
        gtk_fixed_put(GTK_FIXED(frameContent), modeDesc, 5, 135);
        powerWidgets.push_back(modeDesc);
        
        // Checkmark (initially hidden)
        GtkWidget* checkmark = gtk_label_new("");
        gtk_widget_add_css_class(checkmark, "power-checkmark");
        gtk_widget_set_size_request(checkmark, 30, 30);
        gtk_label_set_xalign(GTK_LABEL(checkmark), 0.5);
        gtk_fixed_put(GTK_FIXED(frameContent), checkmark, 100, 5);
        gtk_widget_set_visible(checkmark, FALSE);
        powerWidgets.push_back(checkmark);
        
        // Store widget references
        PowerModeWidget modeWidget = {
            modeContainer,
            frame,
            modeImage,
            modeLabel,
            checkmark,
            false
        };
        modeWidgets[mode.key] = modeWidget;
        
        // Add click handler
        GtkGesture* gesture = gtk_gesture_click_new();
        gtk_widget_add_controller(frame, GTK_EVENT_CONTROLLER(gesture));
        
        PowerModeCallbackData* callbackData = new PowerModeCallbackData{this, mode.key};
        modeCallbacks.push_back(callbackData);
        g_signal_connect(gesture, "pressed", G_CALLBACK(onModeClicked), callbackData);
    }
}

void PowerManager::createInfoSection() {
    if (!powerContainer) return;
    
    // Info section - positioned like DisplayManager info section
    infoContainer = gtk_fixed_new();
    gtk_widget_set_size_request(infoContainer, 700, 100);
    gtk_widget_add_css_class(infoContainer, "power-info-section");
    gtk_fixed_put(GTK_FIXED(powerContainer), infoContainer, 430, 480);
    powerWidgets.push_back(infoContainer);
    
    GtkWidget* infoTitle = gtk_label_new(TR(TranslationKeys::POWER_PROFILE_INFORMATION));
    gtk_widget_add_css_class(infoTitle, "power-info-title");
    gtk_widget_set_size_request(infoTitle, 400, 25);
    gtk_label_set_xalign(GTK_LABEL(infoTitle), 0.5);
    gtk_fixed_put(GTK_FIXED(infoContainer), infoTitle, 100, 10);
    powerWidgets.push_back(infoTitle);
    
    GtkWidget* infoText = gtk_label_new(TR(TranslationKeys::POWER_PROFILE_INFO_TEXT));
    gtk_widget_add_css_class(infoText, "power-info-text");
    gtk_widget_set_size_request(infoText, 560, 50);
    gtk_label_set_xalign(GTK_LABEL(infoText), 0.5);
    gtk_label_set_justify(GTK_LABEL(infoText), GTK_JUSTIFY_CENTER);
    gtk_fixed_put(GTK_FIXED(infoContainer), infoText, 50, 40);
    powerWidgets.push_back(infoText);
}

void PowerManager::setupApplyButton() {
    if (!powerContainer) return;
    
    applyButton = gtk_button_new_with_label(TR(TranslationKeys::APPLY_CHANGES));
    gtk_widget_add_css_class(applyButton, "power-apply-button");
    gtk_widget_set_size_request(applyButton, 170, 50);
    gtk_fixed_put(GTK_FIXED(powerContainer), applyButton, 680, 630);
    powerWidgets.push_back(applyButton);
    
    g_signal_connect(applyButton, "clicked", G_CALLBACK(onApplyButtonClicked), this);
}

void PowerManager::show() {
    if (!powerContainer) return;
    
    // Switch background like other managers
    if (mainWindow) {
        mainWindow->switchToBackground("background4.png");
    }
    
    gtk_widget_set_visible(powerContainer, TRUE);
    
    // Start background detection - UI shows first, then detection happens
    isDetecting = true;
    detectionTimer = g_timeout_add(500, onDetectionTimer, this); // 500ms delay to show UI first
}

void PowerManager::hide() {
    if (!powerContainer) {
        return;
    }
    
    if (detectionTimer > 0) {
        g_source_remove(detectionTimer);
        detectionTimer = 0;
    }
    
    gtk_widget_set_visible(powerContainer, FALSE);
    
    if (mainWindow) {
        mainWindow->showMainMenu();
    }
}

void PowerManager::detectCurrentMode() {
    if (isDetecting) {
        isDetecting = false;
        
        // Run detection in background without blocking UI
        std::string output = executeCommand("pkexec auto-cpufreq --debug 2>/dev/null");
        
        std::string detectedMode;
        if (output.find("Currently using: performance governor") != std::string::npos) {
            detectedMode = "performance";
        } else if (output.find("Currently using: powersave governor") != std::string::npos) {
            detectedMode = "balanced";
        } else if (output.find("Currently using: reset governor") != std::string::npos) {
            detectedMode = "default";
        }
        
        if (!detectedMode.empty()) {
            currentMode = detectedMode;
            selectMode(detectedMode);
            std::cout << "Detected current power mode: " << detectedMode << std::endl;
        } else {
            std::cout << "Could not detect current power mode" << std::endl;
        }
    }
}

void PowerManager::selectMode(const std::string& mode) {
    selectedMode = mode;
    updateModeVisuals();
}

void PowerManager::updateModeVisuals() {
    for (auto& [key, widget] : modeWidgets) {
        bool isSelected = (key == selectedMode);
        widget.isSelected = isSelected;
        
        if (isSelected) {
            gtk_widget_add_css_class(widget.frame, "selected");
        } else {
            gtk_widget_remove_css_class(widget.frame, "selected");
        }
        
        gtk_widget_set_visible(widget.checkmark, isSelected);
    }
}

void PowerManager::applyPowerMode() {
    if (selectedMode.empty()) {
        std::cout << "No power mode selected" << std::endl;
        return;
    }
    
    // Find the selected mode
    PowerMode* mode = nullptr;
    for (auto& m : powerModes) {
        if (m.key == selectedMode) {
            mode = &m;
            break;
        }
    }
    
    if (!mode) {
        std::cout << "Invalid power mode: " << selectedMode << std::endl;
        return;
    }
    
    std::cout << "Applying power mode: " << selectedMode << std::endl;
    
    // Execute the power command with pkexec
    std::string command = "pkexec " + mode->command;
    std::string result = executeCommand(command);
    
    if (result.empty()) {
        std::cout << "Power mode applied successfully" << std::endl;
        currentMode = selectedMode;
    } else {
        std::cout << "Failed to apply power mode. Output: " << result << std::endl;
    }
}

std::string PowerManager::executeCommand(const std::string& command) {
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    
    if (!pipe) {
        return result;
    }
    
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
        result += buffer;
    }
    
    return result;
}

std::string PowerManager::getAssetPath(const std::string& filename) {
    const char* home = getenv("HOME");
    if (!home) return filename;
    
    return std::string(home) + "/.config/Elysia/assets/assets/" + filename;
}

bool PowerManager::fileExists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

// Static callbacks
void PowerManager::onBackButtonClicked(GtkButton*, gpointer user_data) {
    PowerBackButtonCallbackData* data = static_cast<PowerBackButtonCallbackData*>(user_data);
    if (data && data->manager) {
        data->manager->hide();
    }
}

void PowerManager::onModeClicked(GtkGestureClick*, int, double, double, gpointer user_data) {
    PowerModeCallbackData* data = static_cast<PowerModeCallbackData*>(user_data);
    if (data && data->manager) {
        data->manager->selectMode(data->modeKey);
    }
}

void PowerManager::onApplyButtonClicked(GtkButton*, gpointer user_data) {
    PowerManager* manager = static_cast<PowerManager*>(user_data);
    if (manager) {
        manager->applyPowerMode();
    }
}

gboolean PowerManager::onDetectionTimer(gpointer user_data) {
    PowerManager* manager = static_cast<PowerManager*>(user_data);
    if (manager) {
        manager->detectCurrentMode();
    }
    return G_SOURCE_REMOVE; // Remove timer after one execution
}
