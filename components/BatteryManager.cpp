#include "BatteryManager.h"
#include "../MainWindow.h"
#include "../translations/translations.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <unistd.h>
#include <algorithm>
#include <regex>

BatteryManager::BatteryManager(MainWindow* mainWindow, GtkWindow* parentWindow, GtkWidget* overlay)
    : mainWindow(mainWindow), parentWindow(parentWindow), overlay(overlay),
      batteryContainer(nullptr), backButton(nullptr), titleLabel(nullptr),
      batteryIcon(nullptr), percentageLabel(nullptr), statusLabel(nullptr),
      progressBar(nullptr), infoContainer(nullptr), healthContainer(nullptr),
      powerContainer(nullptr), updateTimer(0) {
    
    batteryDevice = findBatteryDevice();
    setupUI();
}

BatteryManager::~BatteryManager() {
    // Stop update timer
    if (updateTimer > 0) {
        g_source_remove(updateTimer);
        updateTimer = 0;
    }
    
    // Clear widget vectors for cleanup
    batteryWidgets.clear();
}

void BatteryManager::setupUI() {
    if (!overlay) return;
    
    // Create main container like other managers
    batteryContainer = gtk_fixed_new();
    gtk_widget_add_css_class(batteryContainer, "battery-container");
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), batteryContainer);
    gtk_widget_set_visible(batteryContainer, FALSE);
    
    initBatteryUI();
}

void BatteryManager::initBatteryUI() {
    if (!batteryContainer) return;
    
    // Apply sophisticated CSS styling
    GtkCssProvider* cssProvider = gtk_css_provider_new();
    const char* css =
        ".battery-container {"
        "  background: linear-gradient(145deg, rgba(255, 255, 255, 0.12), rgba(255, 166, 218, 0.06));"
        "  border: 2px solid rgba(255, 166, 218, 0.25);"
        "  border-radius: 32px;"
        "  box-shadow: 0 20px 60px rgba(0, 0, 0, 0.4), inset 0 2px 0 rgba(255, 255, 255, 0.15);"
        "  backdrop-filter: blur(25px);"
        "}"
        ".battery-title {"
        "  font-family: ElysiaOSNew12; "
        "  color: #ffffff;"
        "  font-size: 32px;"
        "  font-weight: 900;"
        "  letter-spacing: 4px;"
        "  text-shadow: 0 4px 20px rgba(0, 0, 0, 0.6), 0 2px 4px rgba(255, 166, 218, 0.4);"
        "}"
        ".battery-percentage {"
        "  color: #ffa6da;"
        "  font-size: 48px;"
        "  font-weight: 900;"
        "  text-shadow: 0 4px 20px rgba(0, 0, 0, 0.6), 0 0 30px rgba(255, 166, 218, 0.5);"
        "}"
        ".battery-status {"
        "  font-family: ElysiaOSNew12; "
        "  color: #ffffff;"
        "  font-size: 20px;"
        "  font-weight: 700;"
        "  text-shadow: 0 2px 8px rgba(0, 0, 0, 0.5);"
        "}"
        ".battery-progress {"
        "  background: rgba(255, 255, 255, 0.1);"
        "  border-radius: 15px;"
        "  border: 2px solid rgba(255, 166, 218, 0.3);"
        "}"
        ".battery-progress progress {"
        "  background: linear-gradient(90deg, #ffa6da, #fed0f4);"
        "  border-radius: 12px;"
        "}"
        ".info-section {"
        "  font-family: ElysiaOSNew12; "
        "  background: linear-gradient(145deg, rgba(255, 255, 255, 0.08), rgba(255, 166, 218, 0.03));"
        "  border-radius: 20px;"
        "  border: 2px solid rgba(255, 166, 218, 0.15);"
        "  box-shadow: 0 8px 25px rgba(0, 0, 0, 0.2), inset 0 1px 0 rgba(255, 255, 255, 0.1);"
        "  backdrop-filter: blur(15px);"
        "  padding: 20px;"
        "}"
        ".section-title {"
        "  font-family: ElysiaOSNew12; "
        "  color: #ffa6da;"
        "  font-size: 18px;"
        "  font-weight: 800;"
        "  letter-spacing: 2px;"
        "  text-shadow: 0 2px 8px rgba(0, 0, 0, 0.4);"
        "}"
        ".info-label {"
        "  font-family: ElysiaOSNew12; "
        "  color: #ffffff;"
        "  font-size: 14px;"
        "  font-weight: 600;"
        "  text-shadow: 0 1px 4px rgba(0, 0, 0, 0.4);"
        "}"
        ".info-value {"
        "  color: #ffa6da;"
        "  font-size: 14px;"
        "  font-weight: 700;"
        "  text-shadow: 0 1px 4px rgba(0, 0, 0, 0.4);"
        "}"
        ".battery-icon {"
        "  filter: drop-shadow(0 4px 12px rgba(255, 166, 218, 0.4));"
        "}"
        ".warning {"
        "  color: #f59e0b;"
        "}"
        ".critical {"
        "  color: #ef4444;"
        "}"
        ".charging {"
        "  color: #25d979;"
        "}";
    
    gtk_css_provider_load_from_string(cssProvider, css);
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(cssProvider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    
    setupBackButton();
    
    // Main title - positioned like AppearanceManager
    titleLabel = gtk_label_new(TR(TranslationKeys::BATTERY_TITLE));
    gtk_widget_add_css_class(titleLabel, "battery-title");
    gtk_widget_set_size_request(titleLabel, 400, 50);
    gtk_label_set_xalign(GTK_LABEL(titleLabel), 0.5);
    gtk_fixed_put(GTK_FIXED(batteryContainer), titleLabel, 600, 80); // Same as AppearanceManager
    batteryWidgets.push_back(titleLabel);
    
    createBatteryDisplay();
    createInfoSections();
    
    // Initial update
    updateBatteryDisplay();
}

void BatteryManager::setupBackButton() {
    if (!batteryContainer) return;
    
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
    gtk_fixed_put(GTK_FIXED(batteryContainer), backButton, 0, 0);
    
    // Connect the callback
    g_signal_connect(backButton, "clicked", G_CALLBACK(onBackButtonClicked), this);
    
    batteryWidgets.push_back(backButton);
    batteryWidgets.push_back(fixedContainer);
    if (backImage) batteryWidgets.push_back(backImage);
    if (label) batteryWidgets.push_back(label);
}

void BatteryManager::createBatteryDisplay() {
    if (!batteryContainer) return;
    
    // Main battery display area - positioned like AppearanceManager pattern
    GtkWidget* displayContainer = gtk_fixed_new();
    gtk_widget_set_size_request(displayContainer, 500, 200);
    gtk_fixed_put(GTK_FIXED(batteryContainer), displayContainer, 550, 150); // Follow AppearanceManager pattern
    batteryWidgets.push_back(displayContainer);
    
    // Battery icon - use custom battery.png and make it bigger like about manager
    std::string batteryIconPath = getAssetPath("elybattery.png");
    if (fileExists(batteryIconPath)) {
        batteryIcon = gtk_picture_new_for_filename(batteryIconPath.c_str());
        gtk_widget_set_size_request(batteryIcon, 120, 120); // Bigger like about manager
        gtk_picture_set_can_shrink(GTK_PICTURE(batteryIcon), TRUE);
        gtk_picture_set_content_fit(GTK_PICTURE(batteryIcon), GTK_CONTENT_FIT_CONTAIN);
    } else {
        batteryIcon = gtk_image_new_from_icon_name("battery-full");
        gtk_widget_set_size_request(batteryIcon, 120, 120);
    }
    gtk_widget_add_css_class(batteryIcon, "battery-icon");
    gtk_fixed_put(GTK_FIXED(displayContainer), batteryIcon, 190, 10); // Centered in container
    batteryWidgets.push_back(batteryIcon);
    
    // Percentage display
    percentageLabel = gtk_label_new("---%");
    gtk_widget_add_css_class(percentageLabel, "battery-percentage");
    gtk_widget_set_size_request(percentageLabel, 200, 60);
    gtk_label_set_xalign(GTK_LABEL(percentageLabel), 0.5);
    gtk_fixed_put(GTK_FIXED(displayContainer), percentageLabel, 150, 140); // Below icon
    batteryWidgets.push_back(percentageLabel);
    
    // Status label
    statusLabel = gtk_label_new(TR(TranslationKeys::CHECKING_BATTERY_STATUS));
    gtk_widget_add_css_class(statusLabel, "battery-status");
    gtk_widget_set_size_request(statusLabel, 400, 30);
    gtk_label_set_xalign(GTK_LABEL(statusLabel), 0.5);
    gtk_fixed_put(GTK_FIXED(displayContainer), statusLabel, 50, 200); // Below percentage
    batteryWidgets.push_back(statusLabel);
}

void BatteryManager::createInfoSections() {
    if (!batteryContainer) return;
    
    // Battery Health Section - positioned like AppearanceManager
    healthContainer = gtk_fixed_new();
    gtk_widget_set_size_request(healthContainer, 300, 200);
    gtk_widget_add_css_class(healthContainer, "info-section");
    gtk_fixed_put(GTK_FIXED(batteryContainer), healthContainer, 400, 410); // Follow AppearanceManager pattern
    batteryWidgets.push_back(healthContainer);
    
    GtkWidget* healthTitle = gtk_label_new(TR(TranslationKeys::BATTERY_HEALTH));
    gtk_widget_add_css_class(healthTitle, "section-title");
    gtk_widget_set_size_request(healthTitle, 260, 25);
    gtk_label_set_xalign(GTK_LABEL(healthTitle), 0.5);
    gtk_fixed_put(GTK_FIXED(healthContainer), healthTitle, 20, 10);
    batteryWidgets.push_back(healthTitle);
    
    // Power Information Section - positioned like AppearanceManager
    powerContainer = gtk_fixed_new();
    gtk_widget_set_size_request(powerContainer, 300, 200);
    gtk_widget_add_css_class(powerContainer, "info-section");
    gtk_fixed_put(GTK_FIXED(batteryContainer), powerContainer, 750, 410); // Follow AppearanceManager pattern
    batteryWidgets.push_back(powerContainer);
    
    GtkWidget* powerTitle = gtk_label_new(TR(TranslationKeys::POWER_DETAILS));
    gtk_widget_add_css_class(powerTitle, "section-title");
    gtk_widget_set_size_request(powerTitle, 260, 25);
    gtk_label_set_xalign(GTK_LABEL(powerTitle), 0.5);
    gtk_fixed_put(GTK_FIXED(powerContainer), powerTitle, 20, 10);
    batteryWidgets.push_back(powerTitle);
}

void BatteryManager::show() {
    if (batteryContainer) {
        gtk_widget_set_visible(batteryContainer, TRUE);
        
        // Switch background
        if (mainWindow) {
            mainWindow->switchToBackground("background4.png");
        }
        
        // Start update timer (update every 2 seconds)
        updateTimer = g_timeout_add(2000, onUpdateTimer, this);
        
        // Immediate update
        updateBatteryDisplay();
    }
}

void BatteryManager::hide() {
    if (batteryContainer) {
        gtk_widget_set_visible(batteryContainer, FALSE);
        
        // Stop update timer
        if (updateTimer > 0) {
            g_source_remove(updateTimer);
            updateTimer = 0;
        }
    }
}

std::string BatteryManager::findBatteryDevice() {
    std::string output = executeCommand("upower -e");
    std::istringstream stream(output);
    std::string line;
    
    while (std::getline(stream, line)) {
        if (line.find("battery_") != std::string::npos || line.find("BAT") != std::string::npos) {
            return line;
        }
    }
    
    return "";
}

BatteryInfo BatteryManager::parseBatteryInfo(const std::string& device) {
    BatteryInfo info = {};
    
    if (device.empty()) {
        return info;
    }
    
    std::string command = "upower -i " + device;
    std::string output = executeCommand(command);
    std::istringstream stream(output);
    std::string line;
    
    while (std::getline(stream, line)) {
        if (line.find(":") == std::string::npos) continue;
        
        size_t colonPos = line.find(":");
        std::string key = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        if (key == "state") {
            info.state = value;
            info.isCharging = (value == "charging");
        } else if (key == "warning-level") {
            info.warningLevel = value;
        } else if (key == "energy-full") {
            info.energyFull = value;
        } else if (key == "energy-full-design") {
            info.energyFullDesign = value;
        } else if (key == "energy-rate") {
            info.energyRate = value;
        } else if (key == "voltage") {
            info.voltage = value;
        } else if (key == "charge-cycles") {
            info.chargeCycles = value;
        } else if (key == "percentage") {
            info.percentage = value;
            // Extract numeric value
            std::regex percentRegex(R"((\d+)%)");
            std::smatch match;
            if (std::regex_search(value, match, percentRegex)) {
                info.percentageValue = std::stoi(match[1]);
            }
        } else if (key == "capacity") {
            info.capacity = value;
        } else if (key == "technology") {
            info.technology = value;
        } else if (key == "time to empty") {
            info.timeToEmpty = formatBatteryTime(value);
        } else if (key == "time to full") {
            info.timeToFull = formatBatteryTime(value);
        }
    }
    
    return info;
}

void BatteryManager::updateBatteryDisplay() {
    if (batteryDevice.empty()) {
        gtk_label_set_text(GTK_LABEL(percentageLabel), "N/A");
        gtk_label_set_text(GTK_LABEL(statusLabel), TR(TranslationKeys::NO_BATTERY_DETECTED));
        return;
    }
    
    currentInfo = parseBatteryInfo(batteryDevice);
    
    // Update percentage
    gtk_label_set_text(GTK_LABEL(percentageLabel), currentInfo.percentage.c_str());
    
    // Update status
    std::string statusText = currentInfo.state;
    if (currentInfo.isCharging && !currentInfo.timeToFull.empty()) {
        statusText += " - " + currentInfo.timeToFull + " until full";
    } else if (!currentInfo.isCharging && !currentInfo.timeToEmpty.empty()) {
        statusText += " - " + currentInfo.timeToEmpty + " remaining";
    }
    gtk_label_set_text(GTK_LABEL(statusLabel), statusText.c_str());
    
    // Note: We're using a custom static battery.png icon, so no dynamic icon update needed
    
    // Update color based on battery level
    gtk_widget_remove_css_class(percentageLabel, "warning");
    gtk_widget_remove_css_class(percentageLabel, "critical");
    gtk_widget_remove_css_class(percentageLabel, "charging");
    
    if (currentInfo.isCharging) {
        gtk_widget_add_css_class(percentageLabel, "charging");
    } else if (currentInfo.percentageValue <= 15) {
        gtk_widget_add_css_class(percentageLabel, "critical");
    } else if (currentInfo.percentageValue <= 30) {
        gtk_widget_add_css_class(percentageLabel, "warning");
    }
    
    // Update info sections
    updateInfoSections();
}

void BatteryManager::updateInfoSections() {
    // For GTK4, we'll track and manage info widgets manually
    // Clear any existing dynamic info widgets by recreating the containers
    
    // Add health info
    int yPos = 45;
    std::vector<std::pair<std::string, std::string>> healthInfo = {
        {TR(TranslationKeys::CAPACITY), currentInfo.capacity},
        {TR(TranslationKeys::TECHNOLOGY), currentInfo.technology},
        {TR(TranslationKeys::CHARGE_CYCLES), currentInfo.chargeCycles},
        {TR(TranslationKeys::WARNING_LEVEL), currentInfo.warningLevel}
    };
    
    for (const auto& [label, value] : healthInfo) {
        if (!value.empty() && value != "N/A") {
            GtkWidget* infoLabel = gtk_label_new((label + ":").c_str());
            gtk_widget_add_css_class(infoLabel, "info-label");
            gtk_widget_set_size_request(infoLabel, 120, 20);
            gtk_label_set_xalign(GTK_LABEL(infoLabel), 0.0);
            gtk_fixed_put(GTK_FIXED(healthContainer), infoLabel, 20, yPos);
            
            GtkWidget* valueLabel = gtk_label_new(value.c_str());
            gtk_widget_add_css_class(valueLabel, "info-value");
            gtk_widget_set_size_request(valueLabel, 140, 20);
            gtk_label_set_xalign(GTK_LABEL(valueLabel), 1.0);
            gtk_fixed_put(GTK_FIXED(healthContainer), valueLabel, 140, yPos);
            
            batteryWidgets.push_back(infoLabel);
            batteryWidgets.push_back(valueLabel);
            yPos += 30;
        }
    }
    
    // Power section info will be added dynamically
    
    // Add power info
    yPos = 45;
    std::vector<std::pair<std::string, std::string>> powerInfo = {
        {TR(TranslationKeys::ENERGY_FULL), currentInfo.energyFull},
        {TR(TranslationKeys::ENERGY_DESIGN), currentInfo.energyFullDesign},
        {TR(TranslationKeys::ENERGY_RATE), currentInfo.energyRate},
        {TR(TranslationKeys::VOLTAGE), currentInfo.voltage}
    };
    
    for (const auto& [label, value] : powerInfo) {
        if (!value.empty() && value != "N/A") {
            GtkWidget* infoLabel = gtk_label_new((label + ":").c_str());
            gtk_widget_add_css_class(infoLabel, "info-label");
            gtk_widget_set_size_request(infoLabel, 120, 20);
            gtk_label_set_xalign(GTK_LABEL(infoLabel), 0.0);
            gtk_fixed_put(GTK_FIXED(powerContainer), infoLabel, 20, yPos);
            
            GtkWidget* valueLabel = gtk_label_new(value.c_str());
            gtk_widget_add_css_class(valueLabel, "info-value");
            gtk_widget_set_size_request(valueLabel, 140, 20);
            gtk_label_set_xalign(GTK_LABEL(valueLabel), 1.0);
            gtk_fixed_put(GTK_FIXED(powerContainer), valueLabel, 140, yPos);
            
            batteryWidgets.push_back(infoLabel);
            batteryWidgets.push_back(valueLabel);
            yPos += 30;
        }
    }
}

std::string BatteryManager::formatBatteryTime(const std::string& timeStr) {
    if (timeStr.empty() || timeStr == "N/A") return "";
    
    // Parse time format like "2.5 hours" or "45 minutes"
    std::regex timeRegex(R"((\d+(?:\.\d+)?)\s*(hours?|minutes?))");
    std::smatch match;
    
    if (std::regex_search(timeStr, match, timeRegex)) {
        double value = std::stod(match[1]);
        std::string unit = match[2];
        
        if (unit.find("hour") != std::string::npos) {
            if (value < 1.0) {
                int minutes = static_cast<int>(value * 60);
                return std::to_string(minutes) + " min";
            } else {
                return std::to_string(static_cast<int>(value)) + "h " + 
                       std::to_string(static_cast<int>((value - static_cast<int>(value)) * 60)) + "m";
            }
        } else {
            return std::to_string(static_cast<int>(value)) + " min";
        }
    }
    
    return timeStr;
}

std::string BatteryManager::getBatteryIconName(int percentage, bool isCharging) {
    if (isCharging) {
        return "battery-charging";
    }
    
    if (percentage >= 90) return "battery-full";
    else if (percentage >= 70) return "battery-good";
    else if (percentage >= 50) return "battery-medium";
    else if (percentage >= 30) return "battery-low";
    else if (percentage >= 15) return "battery-caution";
    else return "battery-empty";
}

std::string BatteryManager::executeCommand(const std::string& command) {
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

std::string BatteryManager::getAssetPath(const std::string& filename) {
    const char* home = getenv("HOME");
    if (!home) return filename;
    
    return std::string(home) + "/.config/Elysia/assets/assets/" + filename;
}

bool BatteryManager::fileExists(const std::string& path) {
    return access(path.c_str(), F_OK) == 0;
}

// Static callback implementations
void BatteryManager::onBackButtonClicked(GtkButton*, gpointer user_data) {
    BatteryManager* manager = static_cast<BatteryManager*>(user_data);
    if (manager && manager->mainWindow) {
        manager->hide();
        manager->mainWindow->showMainMenu();
    }
}

gboolean BatteryManager::onUpdateTimer(gpointer user_data) {
    BatteryManager* manager = static_cast<BatteryManager*>(user_data);
    if (manager) {
        manager->updateBatteryDisplay();
    }
    return G_SOURCE_CONTINUE;
}
