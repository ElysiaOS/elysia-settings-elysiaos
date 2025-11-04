#ifndef TRANSLATIONS_H
#define TRANSLATIONS_H

#include <string>
#include <map>
#include <memory>
#include <vector>

// Translation keys enum for type safety
enum class TranslationKeys {
    // Main Window
    WINDOW_TITLE,
    EXIT,
    
    // Common UI
    BACK,
    APPLY_CHANGES,
    REFRESH,
    
    // Tile Names
    TILE_CAR,
    TILE_BATTERY,
    TILE_APPEARANCE,
    TILE_POWER,
    TILE_BLUETOOTH,
    TILE_SOUND,
    TILE_ABOUT,
    TILE_DISPLAY,
    TILE_NETWORK,
    TILE_UPDATES,
    TILE_HYPERLAND,
    TILE_STORAGE,
    TILE_SUPPORT,
    
    // About Manager
    ABOUT_TITLE,
    SYSTEM_INFO,
    OS_NAME,
    KERNEL_VERSION,
    UPTIME,
    CPU_INFO,
    MEMORY_INFO,
    DISK_USAGE,
    
    // Battery Manager
    BATTERY_TITLE,
    BATTERY_STATUS,
    BATTERY_HEALTH,
    CHARGING,
    DISCHARGING,
    FULL,
    NOT_CHARGING,
    
    // Display Manager
    DISPLAY_TITLE,
    DISPLAY_SETTINGS,
    DISPLAY_RESOLUTION,
    SCREEN_BRIGHTNESS,
    DISPLAY_INFORMATION,
    DISPLAY_INFO_TEXT,
    
    // Sound Manager
    SOUND_TITLE,
    OUTPUT_DEVICES,
    INPUT_DEVICES,
    VOLUME,
    PLAYBACK_APPLICATIONS,
    
    // Network Manager
    NETWORK_TITLE,
    WIFI_NETWORKS,
    WIFI,
    CONNECTION_STATUS,
    CONNECT,
    DISCONNECT,
    CONNECTED,
    DISCONNECTED,
    ENTER_PASSWORD,
    CONNECTING,
    CANCEL,
    REFRESH_NETWORKS,
    NETWORKMANAGER_NOT_AVAILABLE,
    NETWORKING_DISABLED,
    ENABLE_NETWORKING,
    CONNECTED_VIA_ETHERNET,
    WIFI_HARDWARE_DISABLED,
    WIFI_DISABLED,
    NO_NETWORKS_FOUND,
    IPV4_METHOD,
    IP_ADDRESS,
    NETMASK,
    GATEWAY,
    DNS_SERVERS,
    IPV6_METHOD,
    IPV6_DNS_SERVERS,
    
    // Bluetooth Manager
    BLUETOOTH_TITLE,
    BLUETOOTH_DEVICES,
    DEVICES,
    OK,
    PAIR,
    UNPAIR,
    PAIRED,
    AVAILABLE,
    TOGGLE_BLUETOOTH,
    SCAN_DEVICES,
    BLUETOOTH_DISABLED,
    NO_DEVICES_FOUND_BT,
    SCANNING,
    SCANNING_FOR_DEVICES,
    BLUETOOTH_ENABLED_NO_DEVICES,
    CONNECTED_TO,
    CONNECTED_TO_MULTIPLE,
    TURN_OFF,
    TURN_ON,
    DISCONNECT_BLU,
    CONNECT_BLU,
    FORGET,
    SUCCESS_BLU,
    DISCONNECTED_BLU,
    REMOVED,
    CONNECTED_STATUS,
    PAIRED_STATUS,
    NOT_PAIRED_STATUS,
    
    // Appearance Manager
    APPEARANCE_TITLE,
    THEME_SETTINGS,
    THEME_SELECTION,
    WALLPAPER,
    WALLPAPER_SELECTION,
    COLORS,
    LIGHT,
    DARK,
    BROWSE,
    PREVIOUS_WALLPAPER,
    NEXT_WALLPAPER,
    OPEN_WALLPAPER_FOLDER,
    SET_THIS_WALLPAPER,
    
    // Power Manager
    POWER_TITLE,
    POWER_SETTINGS,
    SLEEP,
    HIBERNATE,
    SHUTDOWN,
    RESTART,
    PERFORMANCE,
    BALANCED,
    DEFAULT,
    POWER_PROFILE_INFORMATION,
    POWER_PROFILE_INFO_TEXT,
    MISSING,
    
    // Storage Manager
    STORAGE_TITLE,
    DISK_USAGE_TITLE,
    AVAILABLE_SPACE,
    USED_SPACE,
    
    // Applications Manager
    APPLICATIONS_TITLE,
    KEYBINDS,
    AUTORUNS,
    WINDOW_RULES,
    
    // Language Manager
    LANGUAGE_SETTINGS,
    SYSTEM_LANGUAGE,
    LANGUAGE_INFORMATION,
    LANGUAGE_INFO_TEXT,
    
    // Messages
    CHECKING_STATUS,
    CHECKING_BATTERY_STATUS,
    LOADING,
    ERROR,
    SUCCESS,
    WARNING,
    NO_SYSTEM_INFO,
    APPLICATION_VOLUME,
    NO_AUDIO_APPS,
    
    // Dialog Messages
    LOGOUT_REQUIRED,
    LOGOUT_MESSAGE,
    LOGOUT_NOW,
    LOGOUT_LATER,
    
    // Network Manager Additional
    NETWORK_SETTINGS,
    ENTER_NETWORK_PASSWORD,
    IPV4_METHOD_LABEL,
    IP_ADDRESS_LABEL,
    NETMASK_LABEL,
    GATEWAY_LABEL,
    DNS_SERVERS_LABEL,
    IPV6_METHOD_LABEL,
    IPV6_DNS_SERVERS_LABEL,
    PORT_SETTINGS,
    HTTP_PROXY_PORT,
    HTTPS_PROXY_PORT,
    FTP_PROXY_PORT,
    SOCKS_PROXY_PORT,
    CUSTOM_PORT_RANGE,
    PORT_DASH,
    CONNECTED_TO_PREFIX,
    ETHERNET,
    CONNECTING_STATUS,
    NETWORKING_DISABLED_STATUS,
    WIFI_DISABLED_STATUS,
    NOT_CONNECTED_STATUS,
    
    // Navigation Buttons
    PREVIOUS_ARROW,
    NEXT_ARROW,
    LEFT_ARROW,
    RIGHT_ARROW,
    ADD_BUTTON,
    REMOVE_BUTTON,
    
    // Language Manager
    LANGUAGE_CHANGED,
    
    // Display Manager
    BRIGHTNESS_VALUE,
    
    // Sound Manager
    VOLUME_VALUE,
    
    // Battery Manager Additional
    POWER_DETAILS,
    NO_BATTERY_DETECTED,
    CAPACITY,
    TECHNOLOGY,
    CHARGE_CYCLES,
    WARNING_LEVEL,
    ENERGY_FULL,
    ENERGY_DESIGN,
    ENERGY_RATE,
    VOLTAGE,
    
    // Power Manager Additional
    PERFORMANCE_MODE,
    BALANCED_MODE,
    DEFAULT_MODE,
    PERFORMANCE_DESCRIPTION,
    BALANCED_DESCRIPTION,
    DEFAULT_DESCRIPTION,
    
    COUNT // Keep this last for counting
};

// Translation manager singleton class
class TranslationManager {
public:
    static TranslationManager& getInstance();
    
    void setLanguage(const std::string& languageCode);
    std::string getLanguage() const;
    const std::string& translate(TranslationKeys key) const;
    
    // Get list of available languages
    std::vector<std::string> getAvailableLanguages() const;
    std::string getLanguageName(const std::string& languageCode) const;
    
private:
    TranslationManager();
    ~TranslationManager() = default;
    
    // Prevent copying
    TranslationManager(const TranslationManager&) = delete;
    TranslationManager& operator=(const TranslationManager&) = delete;
    
    void loadTranslations();
    void loadLanguageFile(const std::string& languageCode);
    
    std::string currentLanguage;
    std::map<std::string, std::map<TranslationKeys, std::string>> translations;
    std::map<std::string, std::string> languageNames;
    
    // Fallback English text
    std::map<TranslationKeys, std::string> fallbackTranslations;
};

// Convenience macro for translation
#define TR(key) TranslationManager::getInstance().translate(key).c_str()

#endif // TRANSLATIONS_H
