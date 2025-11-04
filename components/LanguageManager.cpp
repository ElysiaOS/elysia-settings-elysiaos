#include "LanguageManager.h"
#include "../MainWindow.h"
#include "../translations/translations.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <algorithm>
#include <regex>
#include <memory>
#include <sys/stat.h>
#include <cctype>
#include <set>

LanguageManager::LanguageManager(MainWindow* mainWindow, GtkWindow* parentWindow, GtkWidget* overlay)
    : mainWindow(mainWindow), parentWindow(parentWindow), overlay(overlay),
      languageContainer(nullptr), backButton(nullptr), titleLabel(nullptr),
      themeImageLabel(nullptr), languageLabel(nullptr), languageCombo(nullptr),
      applyButton(nullptr), infoContainer(nullptr) {
    
    setupUI();
}

LanguageManager::~LanguageManager() {
    // Clear widget vectors for cleanup
    languageWidgets.clear();
}

void LanguageManager::setupUI() {
    if (!overlay) return;
    
    // Create main container like other managers
    languageContainer = gtk_fixed_new();
    gtk_widget_add_css_class(languageContainer, "language-container");
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), languageContainer);
    gtk_widget_set_visible(languageContainer, FALSE);
    
    initLanguageUI();
}

void LanguageManager::initLanguageUI() {
    if (!languageContainer) return;
    
    // Apply sophisticated CSS styling - EXACTLY like DisplayManager
    GtkCssProvider* cssProvider = gtk_css_provider_new();
    const char* css =
        ".language-container {"
        "  background: linear-gradient(145deg, rgba(255, 255, 255, 0.12), rgba(255, 166, 218, 0.06));"
        "  border: 2px solid rgba(255, 166, 218, 0.25);"
        "  border-radius: 32px;"
        "  box-shadow: 0 20px 60px rgba(0, 0, 0, 0.4), inset 0 2px 0 rgba(255, 255, 255, 0.15);"
        "  backdrop-filter: blur(25px);"
        "}"
        ".language-title {"
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
        "  backdrop-filter: blur(10px);"
        "}"
        ".language-section {"
        "  background: linear-gradient(145deg, rgba(255, 255, 255, 0.08), rgba(255, 166, 218, 0.03));"
        "  border-radius: 24px;"
        "  border: 2px solid rgba(255, 166, 218, 0.15);"
        "  box-shadow: 0 12px 40px rgba(0, 0, 0, 0.2), inset 0 2px 0 rgba(255, 255, 255, 0.1);"
        "  backdrop-filter: blur(20px);"
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
        ".language-combo {"
        "  background: linear-gradient(145deg, rgba(255, 255, 255, 0.15), rgba(255, 166, 218, 0.08));"
        "  color: #ffffff;"
        "  border: 2px solid rgba(255, 166, 218, 0.3);"
        "  border-radius: 20px;"
        "  padding: 12px 16px;"
        "  font-size: 16px;"
        "  font-weight: 600;"
        "  transition: all 0.3s ease;"
        "}"
        ".language-combo:hover {"
        "  background: linear-gradient(145deg, rgba(255, 255, 255, 0.25), rgba(255, 166, 218, 0.12));"
        "  border-color: rgba(255, 166, 218, 0.5);"
        "  box-shadow: 0 6px 20px rgba(255, 166, 218, 0.2);"
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
    
    // Main title - positioned like DisplayManager
    titleLabel = gtk_label_new(TR(TranslationKeys::LANGUAGE_SETTINGS));
    gtk_widget_add_css_class(titleLabel, "language-title");
    gtk_widget_set_size_request(titleLabel, 400, 50);
    gtk_label_set_xalign(GTK_LABEL(titleLabel), 0.5);
    gtk_fixed_put(GTK_FIXED(languageContainer), titleLabel, 600, 80); // Same as DisplayManager
    languageWidgets.push_back(titleLabel);
    
    createLanguageControls();
    createInfoSection();
    
    // Initial updates
    updateLanguageList();
}

void LanguageManager::setupBackButton() {
    if (!languageContainer) return;
    
    std::string backPath = getAssetPath("back.png");
    
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
    
    // Style the button to remove borders and background
    gtk_widget_set_size_request(backButton, 120, 60);
    gtk_widget_add_css_class(backButton, "flat");
    
    // Position the back button at top left
    gtk_fixed_put(GTK_FIXED(languageContainer), backButton, 0, 0);
    
    // Connect the callback
    g_signal_connect(backButton, "clicked", G_CALLBACK(onBackButtonClicked), this);
    
    languageWidgets.push_back(backButton);
    languageWidgets.push_back(fixedContainer);
    if (backImage) languageWidgets.push_back(backImage);
    if (label) languageWidgets.push_back(label);
}

void LanguageManager::createLanguageControls() {
    if (!languageContainer) return;
    
    // Theme preview image - positioned like DisplayManager theme section
    themeImageLabel = gtk_picture_new();
    gtk_widget_set_size_request(themeImageLabel, 200, 370);
    gtk_widget_add_css_class(themeImageLabel, "theme-preview");
    gtk_fixed_put(GTK_FIXED(languageContainer), themeImageLabel, 350, 150); // Follow DisplayManager pattern
    languageWidgets.push_back(themeImageLabel);
    
    // Language controls section - positioned like DisplayManager controls section
    GtkWidget* controlsSection = gtk_fixed_new();
    gtk_widget_set_size_request(controlsSection, 400, 200);
    gtk_widget_add_css_class(controlsSection, "language-section");
    gtk_fixed_put(GTK_FIXED(languageContainer), controlsSection, 600, 150); // Follow DisplayManager pattern
    languageWidgets.push_back(controlsSection);
    
    // Language label and combo
    languageLabel = gtk_label_new(TR(TranslationKeys::SYSTEM_LANGUAGE));
    gtk_widget_add_css_class(languageLabel, "section-label");
    gtk_widget_set_size_request(languageLabel, 300, 25);
    gtk_label_set_xalign(GTK_LABEL(languageLabel), 0.0);
    gtk_fixed_put(GTK_FIXED(controlsSection), languageLabel, 30, 30);
    languageWidgets.push_back(languageLabel);
    
    languageCombo = gtk_combo_box_text_new();
    gtk_widget_set_size_request(languageCombo, 340, 50);
    gtk_widget_add_css_class(languageCombo, "language-combo");
    gtk_fixed_put(GTK_FIXED(controlsSection), languageCombo, 30, 70);
    g_signal_connect(languageCombo, "changed", G_CALLBACK(onLanguageChanged), this);
    languageWidgets.push_back(languageCombo);
    
    // Apply button
    applyButton = gtk_button_new_with_label(TR(TranslationKeys::APPLY_CHANGES));
    gtk_widget_set_size_request(applyButton, 200, 50);
    gtk_widget_add_css_class(applyButton, "apply-button");
    gtk_widget_remove_css_class(applyButton, "button"); // Remove default GTK button class
    gtk_widget_set_can_focus(applyButton, FALSE);
    gtk_fixed_put(GTK_FIXED(controlsSection), applyButton, 100, 140);
    g_signal_connect(applyButton, "clicked", G_CALLBACK(onApplyButtonClicked), this);
    languageWidgets.push_back(applyButton);
}

void LanguageManager::createInfoSection() {
    if (!languageContainer) return;
    
    // Info section for additional language information - positioned like DisplayManager
    infoContainer = gtk_fixed_new();
    gtk_widget_set_size_request(infoContainer, 770, 100);
    gtk_widget_add_css_class(infoContainer, "language-section");
    gtk_fixed_put(GTK_FIXED(languageContainer), infoContainer, 380, 550); // Follow DisplayManager pattern
    languageWidgets.push_back(infoContainer);
    
    GtkWidget* infoTitle = gtk_label_new(TR(TranslationKeys::LANGUAGE_INFORMATION));
    gtk_widget_add_css_class(infoTitle, "section-label");
    gtk_widget_set_size_request(infoTitle, 300, 25);
    gtk_label_set_xalign(GTK_LABEL(infoTitle), 0.5);
    gtk_fixed_put(GTK_FIXED(infoContainer), infoTitle, 210, 20);
    languageWidgets.push_back(infoTitle);
    
    GtkWidget* infoText = gtk_label_new(TR(TranslationKeys::LANGUAGE_INFO_TEXT));
    gtk_widget_add_css_class(infoText, "info-label");
    gtk_widget_set_size_request(infoText, 500, 10);
    gtk_label_set_xalign(GTK_LABEL(infoText), 0.5);
    gtk_label_set_justify(GTK_LABEL(infoText), GTK_JUSTIFY_CENTER);
    gtk_fixed_put(GTK_FIXED(infoContainer), infoText, 100, 60);
    languageWidgets.push_back(infoText);
}

void LanguageManager::show() {
    if (languageContainer) {
        gtk_widget_set_visible(languageContainer, TRUE);
        
        // Switch background - EXACTLY like DisplayManager
        if (mainWindow) {
            mainWindow->switchToBackground("background4.png");
        }
        
        // Immediate updates
        updateLanguageList();
        
        // Update theme image
        std::string imagePath = detectThemeImage();
        if (fileExists(imagePath) && themeImageLabel) {
            gtk_picture_set_filename(GTK_PICTURE(themeImageLabel), imagePath.c_str());
            gtk_picture_set_can_shrink(GTK_PICTURE(themeImageLabel), TRUE);
            gtk_picture_set_content_fit(GTK_PICTURE(themeImageLabel), GTK_CONTENT_FIT_COVER);
        }
    }
}

void LanguageManager::hide() {
    if (languageContainer) {
        gtk_widget_set_visible(languageContainer, FALSE);
    }
}

std::string LanguageManager::detectThemeImage() {
    std::string basePath = getAssetPath("");
    
    std::string output = executeCommand("gsettings get org.gnome.desktop.interface color-scheme");
    
    if (output.find("prefer-light") != std::string::npos) {
        return basePath + "light1.png";
    } else {
        return basePath + "dark1.png";
    }
}

std::vector<Language> LanguageManager::getAvailableLanguages() {
    std::vector<Language> languages;
    
    // Read all available locales from /etc/locale.gen
    std::ifstream localeGenFile("/etc/locale.gen");
    std::string line;
    
    if (localeGenFile.is_open()) {
        while (std::getline(localeGenFile, line)) {
            // Skip empty lines
            if (line.empty()) continue;
            
            // Look for lines that contain UTF-8 locales (both commented and uncommented)
            if (line.find("UTF-8") != std::string::npos) {
                std::string workingLine = line;
                
                // Remove leading # if commented out
                if (workingLine[0] == '#') {
                    workingLine = workingLine.substr(1);
                }
                
                // Trim leading whitespace
                size_t firstNonSpace = workingLine.find_first_not_of(" \t");
                if (firstNonSpace != std::string::npos) {
                    workingLine = workingLine.substr(firstNonSpace);
                }
                
                 // Extract the locale code (first part before space)
                 std::istringstream iss(workingLine);
                 std::string localeCode;
                 if (iss >> localeCode) {
                     // Filter out broken/incomplete language entries
                     if (isValidLanguageCode(localeCode)) {
                         Language lang;
                         lang.code = localeCode;
                         lang.name = getLanguageName(localeCode);
                         lang.display = lang.name + " - " + lang.code;
                         languages.push_back(lang);
                     }
                 }
            }
        }
        localeGenFile.close();
    } else {
        std::cout << "Warning: Could not read /etc/locale.gen, falling back to system locales" << std::endl;
        
        // Fallback: Get available locales from the system
        std::string output = executeCommand("locale -a");
        std::istringstream stream(output);
        
        while (std::getline(stream, line)) {
            // Skip empty lines and C/POSIX locales
            if (line.empty() || line == "C" || line == "POSIX") continue;
            
             // Only include UTF-8 locales
             if (line.find("utf8") != std::string::npos || line.find("UTF-8") != std::string::npos) {
                 // Filter out broken/incomplete language entries
                 if (isValidLanguageCode(line)) {
                     Language lang;
                     lang.code = line;
                     lang.name = getLanguageName(line);
                     lang.display = lang.name + " - " + lang.code;
                     languages.push_back(lang);
                 }
             }
        }
    }
    
    // Remove duplicates
    std::sort(languages.begin(), languages.end(), [](const Language& a, const Language& b) {
        return a.code < b.code;
    });
    languages.erase(std::unique(languages.begin(), languages.end(), [](const Language& a, const Language& b) {
        return a.code == b.code;
    }), languages.end());
    
    // Sort alphabetically by display name
    std::sort(languages.begin(), languages.end(), [](const Language& a, const Language& b) {
        return a.display < b.display;
    });
    
    return languages;
}

bool LanguageManager::isValidLanguageCode(const std::string& code) {
    // Extract base code without encoding suffix
    std::string baseCode = code;
    size_t dotPos = baseCode.find('.');
    if (dotPos != std::string::npos) {
        baseCode = baseCode.substr(0, dotPos);
    }
    
    // Remove @variant suffix
    size_t atPos = baseCode.find('@');
    if (atPos != std::string::npos) {
        baseCode = baseCode.substr(0, atPos);
    }
    
    // Additional validation: must have proper format (xx_XX)
    if (baseCode.length() < 5 || baseCode.find('_') == std::string::npos) {
        return false;
    }
    
    // Split into language and country parts
    size_t underscorePos = baseCode.find('_');
    std::string langPart = baseCode.substr(0, underscorePos);
    std::string countryPart = baseCode.substr(underscorePos + 1);
    
    // Language part should be 2-3 characters, all lowercase letters
    if (langPart.length() < 2 || langPart.length() > 3) {
        return false;
    }
    for (char c : langPart) {
        if (!std::islower(c)) {
            return false;
        }
    }
    
    // Country part should be 2 characters, all uppercase letters
    if (countryPart.length() != 2) {
        return false;
    }
    for (char c : countryPart) {
        if (!std::isupper(c)) {
            return false;
        }
    }
    
    // Only allow the 11 languages we have translations for
    static const std::set<std::string> supportedLanguages = {
        "de_DE", "en_US", "es_ES", "fr_FR", "id_ID", 
        "ja_JP", "ko_KR", "ru_RU", "vi_VN", "zh_CN", "ar_EG"
    };
    
    return supportedLanguages.find(baseCode) != supportedLanguages.end();
}

std::string LanguageManager::getLanguageName(const std::string& code) {
    // Safety check: ensure code is not empty
    if (code.empty()) {
        return "Unknown";
    }
    
    // Only the 11 languages we have translations for
    static const std::map<std::string, std::string> languageMap = {
        {"de_DE", "German (Germany)"},
        {"en_US", "English (United States)"},
        {"es_ES", "Spanish (Spain)"},
        {"fr_FR", "French (France)"},
        {"id_ID", "Indonesian (Indonesia)"},
        {"ja_JP", "Japanese (Japan)"},
        {"ko_KR", "Korean (South Korea)"},
        {"ru_RU", "Russian (Russia)"},
        {"vi_VN", "Vietnamese (Vietnam)"},
        {"zh_CN", "Chinese (Simplified)"},
        {"ar_EG", "Arabic (Egypt)"}
    };
    
    // Extract language code without encoding suffix
    std::string baseCode = code;
    size_t dotPos = baseCode.find('.');
    if (dotPos != std::string::npos) {
        baseCode = baseCode.substr(0, dotPos);
    }
    
    // Remove @variant suffix for lookup
    size_t atPos = baseCode.find('@');
    std::string lookupCode = baseCode;
    if (atPos != std::string::npos) {
        lookupCode = baseCode.substr(0, atPos);
    }
    
    auto it = languageMap.find(baseCode);
    if (it != languageMap.end()) {
        return it->second;
    }
    
    // Try without @variant
    if (atPos != std::string::npos) {
        auto it2 = languageMap.find(lookupCode);
        if (it2 != languageMap.end()) {
            return it2->second;
        }
    }
    
    // If not found in map, try to create a readable name from the code
    if (baseCode.find('_') != std::string::npos) {
        size_t underscorePos = baseCode.find('_');
        if (underscorePos != std::string::npos && underscorePos > 0 && underscorePos < baseCode.length() - 1) {
            std::string lang = baseCode.substr(0, underscorePos);
            std::string country = baseCode.substr(underscorePos + 1);
            
            // Convert to readable format
            std::transform(lang.begin(), lang.end(), lang.begin(), ::tolower);
            std::transform(country.begin(), country.end(), country.begin(), ::toupper);
            
            // Capitalize first letter of language
            if (!lang.empty()) {
                lang[0] = std::toupper(lang[0]);
            }
            
            return lang + " (" + country + ")";
        }
    }
    
    // Last resort: return the base code
    return baseCode;
}

std::string LanguageManager::getCurrentLanguage() {
    // Read current language from /etc/locale.conf
    std::ifstream file("/etc/locale.conf");
    std::string line;
    std::string currentLang;
    
    if (file.is_open()) {
        while (std::getline(file, line)) {
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') continue;
            
            if (line.find("LANG=") == 0) {
                currentLang = line.substr(5); // Remove "LANG=" prefix
                // Remove quotes if present
                if (!currentLang.empty() && currentLang.front() == '"' && currentLang.back() == '"') {
                    currentLang = currentLang.substr(1, currentLang.length() - 2);
                }
                // Trim whitespace
                currentLang.erase(0, currentLang.find_first_not_of(" \t"));
                currentLang.erase(currentLang.find_last_not_of(" \t") + 1);
                break;
            }
        }
        file.close();
    }
    
    // If not found in file, try environment variable
    if (currentLang.empty()) {
        const char* lang = getenv("LANG");
        if (lang) {
            currentLang = std::string(lang);
        }
    }
    
    // Fallback to English if still empty
    if (currentLang.empty()) {
        currentLang = "en_US.UTF-8";
        std::cout << "Warning: No locale found, using fallback: " << currentLang << std::endl;
    }
    
    return currentLang;
}

void LanguageManager::updateLanguageList() {
    if (!languageCombo) return;
    
    try {
        availableLanguages = getAvailableLanguages();
        currentLanguage = getCurrentLanguage();
        
        // Safety check: ensure we have valid languages
        if (availableLanguages.empty()) {
            std::cout << "Warning: No available languages found, using fallback" << std::endl;
            // Add a fallback English language
            Language fallbackLang;
            fallbackLang.code = "en_US.UTF-8";
            fallbackLang.name = "English (United States)";
            fallbackLang.display = fallbackLang.name + " - " + fallbackLang.code;
            availableLanguages.push_back(fallbackLang);
        }
        
        // Clear and add new languages
        gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(languageCombo));
        
        int currentIndex = -1;
        for (size_t i = 0; i < availableLanguages.size(); ++i) {
            // Safety check: ensure display string is not empty
            std::string displayText = availableLanguages[i].display;
            if (displayText.empty()) {
                displayText = availableLanguages[i].code;
            }
            
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(languageCombo), 
                                           displayText.c_str());
            
            // Check if this is the current language - try multiple matching strategies
            if (availableLanguages[i].code == currentLanguage) {
                currentIndex = i;
            }
            // Try matching without encoding suffix (e.g. en_US.UTF-8 vs en_US)
            else if (!currentLanguage.empty()) {
                std::string currentBase = currentLanguage;
                size_t dotPos = currentBase.find('.');
                if (dotPos != std::string::npos) {
                    currentBase = currentBase.substr(0, dotPos);
                }
                
                std::string availableBase = availableLanguages[i].code;
                dotPos = availableBase.find('.');
                if (dotPos != std::string::npos) {
                    availableBase = availableBase.substr(0, dotPos);
                }
                
                if (availableBase == currentBase) {
                    currentIndex = i;
                }
            }
        }
        
        // Set current language in dropdown with bounds checking
        if (currentIndex >= 0 && currentIndex < static_cast<int>(availableLanguages.size())) {
            gtk_combo_box_set_active(GTK_COMBO_BOX(languageCombo), currentIndex);
            std::cout << "Current system language: " << currentLanguage << " (matched at index " << currentIndex << ")" << std::endl;
        } else {
            std::cout << "Current language not found in available list: " << currentLanguage << std::endl;
            std::cout << "Available languages count: " << availableLanguages.size() << std::endl;
            if (!availableLanguages.empty()) {
                std::cout << "First few available languages:" << std::endl;
                for (size_t i = 0; i < std::min(size_t(5), availableLanguages.size()); ++i) {
                    std::cout << "  " << i << ": " << availableLanguages[i].code << std::endl;
                }
            }
            // Set to first available language as fallback
            if (!availableLanguages.empty()) {
                gtk_combo_box_set_active(GTK_COMBO_BOX(languageCombo), 0);
                std::cout << "Set to first available language as fallback" << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in updateLanguageList: " << e.what() << std::endl;
        // Ensure combo box is in a valid state
        if (gtk_combo_box_get_model(GTK_COMBO_BOX(languageCombo))) {
            gtk_combo_box_set_active(GTK_COMBO_BOX(languageCombo), 0);
        }
    }
}

void LanguageManager::applyLanguageChanges() {
    if (!languageCombo) return;
    
    int activeIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(languageCombo));
    if (activeIndex >= 0 && activeIndex < static_cast<int>(availableLanguages.size())) {
        std::string newLanguage = availableLanguages[activeIndex].code;
        
        std::cout << "Applying language changes..." << std::endl;
        std::cout << "Selected language: " << newLanguage << std::endl;
        
        if (writeLocaleConfig(newLanguage)) {
            // Show success message
            std::cout << "✓ Language successfully changed to " << newLanguage << std::endl;
            std::cout << "✓ Showing logout dialog..." << std::endl;
            
            // Show logout dialog
            showLogoutDialog();
        } else {
            std::cout << "✗ Failed to update locale configuration." << std::endl;
            std::cout << "✗ Please check permissions and try again." << std::endl;
        }
    } else {
        std::cout << "✗ No valid language selected." << std::endl;
    }
}

bool LanguageManager::writeLocaleConfig(const std::string& newLanguage) {
    std::cout << "Setting up language: " << newLanguage << std::endl;
    
    // First, ensure the locale is generated by uncommenting it in locale.gen and running locale-gen
    std::string enableLocaleScript = "/tmp/elysia_enable_locale_" + std::to_string(getpid()) + ".sh";
    
    // Create a script to enable the locale
    std::ofstream scriptOut(enableLocaleScript);
    if (!scriptOut.is_open()) {
        std::cout << "Failed to create locale enable script" << std::endl;
        return false;
    }
    
    scriptOut << "#!/bin/bash\n";
    scriptOut << "# Enable locale in /etc/locale.gen\n";
    scriptOut << "sed -i 's/^#\\s*" << newLanguage << "/" << newLanguage << "/' /etc/locale.gen\n";
    scriptOut << "# Generate locales\n";
    scriptOut << "locale-gen\n";
    scriptOut << "echo \"Locale " << newLanguage << " has been enabled and generated\"\n";
    scriptOut.close();
    
    // Make script executable
    chmod(enableLocaleScript.c_str(), 0755);
    
    // Run the script with pkexec
    std::string enableCommand = "pkexec " + enableLocaleScript;
    std::cout << "Enabling locale with command: " << enableCommand << std::endl;
    int enableResult = system(enableCommand.c_str());
    
    // Clean up script
    unlink(enableLocaleScript.c_str());
    
    if (enableResult != 0) {
        std::cout << "Warning: Failed to enable locale, but continuing..." << std::endl;
    }
    
    // Now update /etc/locale.conf
    std::string tempFile = "/tmp/elysia_locale_" + std::to_string(getpid());
    
    // Create temporary file with new locale content
    std::ofstream tempOut(tempFile);
    if (!tempOut.is_open()) {
        std::cout << "Failed to create temporary file" << std::endl;
        return false;
    }
    
    // Read current locale.conf and replace LANG= line
    std::ifstream currentFile("/etc/locale.conf");
    std::string line;
    bool langLineFound = false;
    
    if (currentFile.is_open()) {
        while (std::getline(currentFile, line)) {
            if (line.find("LANG=") == 0) {
                tempOut << "LANG=" << newLanguage << std::endl;
                langLineFound = true;
            } else {
                tempOut << line << std::endl;
            }
        }
        currentFile.close();
    }
    
    // If LANG= line wasn't found, add it
    if (!langLineFound) {
        tempOut << "LANG=" << newLanguage << std::endl;
    }
    
    tempOut.close();
    
    // Use pkexec to copy the temporary file to /etc/locale.conf
    std::string command = "pkexec cp " + tempFile + " /etc/locale.conf";
    int result = system(command.c_str());
    
    // Clean up temporary file
    unlink(tempFile.c_str());
    
    if (result == 0) {
        std::cout << "Successfully updated /etc/locale.conf with new language: " << newLanguage << std::endl;
        return true;
    } else {
        std::cout << "Failed to write to /etc/locale.conf (pkexec failed)" << std::endl;
        return false;
    }
}

void LanguageManager::showLogoutDialog() {
    if (!parentWindow) return;
    
    GtkWidget* dialog = gtk_message_dialog_new(
        parentWindow,
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_QUESTION,
        GTK_BUTTONS_YES_NO,
        "Do you want to logout now to apply changes?"
    );
    
    gtk_window_set_title(GTK_WINDOW(dialog), TR(TranslationKeys::LANGUAGE_CHANGED));
    
    // Connect response signal
    g_signal_connect(dialog, "response", G_CALLBACK(onLogoutDialogResponse), this);
    
    gtk_widget_show(dialog);
}

void LanguageManager::logoutSession() {
    // Try different logout commands for different desktop environments
    std::vector<std::string> logoutCommands = {
        "hyprctl dispatch exit",           // Hyprland
        "loginctl terminate-user $USER",   // systemd
        "pkill -KILL -u $USER",           // Force kill user session
        "gnome-session-quit --logout --no-prompt", // GNOME
        "qdbus org.kde.ksmserver /KSMServer logout 0 0 0", // KDE
        "xfce4-session-logout --logout --fast", // XFCE
        "lxsession-logout"                 // LXDE
    };
    
    for (const auto& command : logoutCommands) {
        std::cout << "Trying logout command: " << command << std::endl;
        if (system(command.c_str()) == 0) {
            break;
        }
    }
}

std::string LanguageManager::executeCommand(const std::string& command) {
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

std::string LanguageManager::getAssetPath(const std::string& filename) {
    const char* home = getenv("HOME");
    if (!home) return filename;
    
    return std::string(home) + "/.config/Elysia/assets/assets/" + filename;
}

bool LanguageManager::fileExists(const std::string& path) {
    return access(path.c_str(), F_OK) == 0;
}

// Static callback implementations
void LanguageManager::onBackButtonClicked(GtkButton*, gpointer user_data) {
    LanguageManager* manager = static_cast<LanguageManager*>(user_data);
    if (manager && manager->mainWindow) {
        manager->hide();
        manager->mainWindow->showMainMenu();
    }
}

void LanguageManager::onLanguageChanged(GtkComboBox*, gpointer user_data) {
    LanguageManager* manager = static_cast<LanguageManager*>(user_data);
    if (manager) {
        // Language change will be applied when Apply button is clicked
        std::cout << "Language selection changed. Click Apply to save changes." << std::endl;
    }
}

void LanguageManager::onApplyButtonClicked(GtkButton*, gpointer user_data) {
    LanguageManager* manager = static_cast<LanguageManager*>(user_data);
    if (manager) {
        manager->applyLanguageChanges();
    }
}

void LanguageManager::onLogoutDialogResponse(GtkDialog* dialog, gint response_id, gpointer user_data) {
    LanguageManager* manager = static_cast<LanguageManager*>(user_data);
    
    if (response_id == GTK_RESPONSE_YES) {
        std::cout << "User chose to logout. Logging out..." << std::endl;
        if (manager) {
            manager->logoutSession();
        }
    } else {
        std::cout << "User chose not to logout. Changes will take effect on next login." << std::endl;
    }
    
    gtk_window_destroy(GTK_WINDOW(dialog));
}
