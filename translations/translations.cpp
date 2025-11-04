#include "translations.h"
#include "en_US.h"
#include "zh_CN.h"
#include "ja_JP.h"
#include "ko_KR.h"
#include "ru_RU.h"
#include "de_DE.h"
#include "fr_FR.h"
#include "vi_VN.h"
#include "id_ID.h"
#include "es_ES.h"
#include <iostream>
#include <cstdlib>
#include <vector>

TranslationManager& TranslationManager::getInstance() {
    static TranslationManager instance;
    return instance;
}

TranslationManager::TranslationManager() : currentLanguage("en_US") {
    // Initialize language names
    languageNames["en_US"] = "English (United States)";
    languageNames["zh_CN"] = "中文 (简体)";
    languageNames["ja_JP"] = "日本語";
    languageNames["ko_KR"] = "한국어";
    languageNames["ru_RU"] = "Русский";
    languageNames["de_DE"] = "Deutsch";
    languageNames["fr_FR"] = "Français";
    languageNames["vi_VN"] = "Tiếng Việt";
    languageNames["id_ID"] = "Bahasa Indonesia";
    languageNames["es_ES"] = "Español";
    
    // Load fallback English translations
    loadEnglishTranslations(fallbackTranslations);
    
    // Load all available translations
    loadTranslations();
}

void TranslationManager::setLanguage(const std::string& languageCode) {
    if (translations.find(languageCode) != translations.end()) {
        currentLanguage = languageCode;
        std::cout << "Language set to: " << languageCode << std::endl;
    } else {
        std::cout << "Language " << languageCode << " not found, using English" << std::endl;
        currentLanguage = "en_US";
    }
}

std::string TranslationManager::getLanguage() const {
    return currentLanguage;
}

const std::string& TranslationManager::translate(TranslationKeys key) const {
    // Safety check: ensure translations are loaded
    if (translations.empty() && fallbackTranslations.empty()) {
        static const std::string error = "Translation system not initialized";
        return error;
    }
    
    // Try current language first
    auto langIt = translations.find(currentLanguage);
    if (langIt != translations.end()) {
        auto keyIt = langIt->second.find(key);
        if (keyIt != langIt->second.end()) {
            return keyIt->second;
        }
    }
    
    // Fall back to English
    auto fallbackIt = fallbackTranslations.find(key);
    if (fallbackIt != fallbackTranslations.end()) {
        return fallbackIt->second;
    }
    
    // Ultimate fallback
    static const std::string unknown = "Unknown";
    return unknown;
}

std::vector<std::string> TranslationManager::getAvailableLanguages() const {
    std::vector<std::string> languages;
    for (const auto& pair : translations) {
        languages.push_back(pair.first);
    }
    return languages;
}

std::string TranslationManager::getLanguageName(const std::string& languageCode) const {
    auto it = languageNames.find(languageCode);
    if (it != languageNames.end()) {
        return it->second;
    }
    return languageCode;
}

void TranslationManager::loadTranslations() {
    // Load English
    loadEnglishTranslations(translations["en_US"]);
    
    // Load Chinese
    loadChineseTranslations(translations["zh_CN"]);
    
    // Load Japanese
    loadJapaneseTranslations(translations["ja_JP"]);
    
    // Load Korean
    loadKoreanTranslations(translations["ko_KR"]);
    
    // Load Russian
    loadRussianTranslations(translations["ru_RU"]);
    
    // Load German
    loadGermanTranslations(translations["de_DE"]);
    
    // Load French
    loadFrenchTranslations(translations["fr_FR"]);
    
    // Load Vietnamese
    loadVietnameseTranslations(translations["vi_VN"]);
    
    // Load Indonesian
    loadIndonesianTranslations(translations["id_ID"]);
    
    // Load Spanish
    loadSpanishTranslations(translations["es_ES"]);
    
    std::cout << "Loaded translations for " << translations.size() << " languages" << std::endl;
}
