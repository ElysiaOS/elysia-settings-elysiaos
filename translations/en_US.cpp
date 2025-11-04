#include "en_US.h"

void loadEnglishTranslations(std::map<TranslationKeys, std::string>& translations) {
    // Main Window
    translations[TranslationKeys::WINDOW_TITLE] = "ElysiaOS Settings";
    translations[TranslationKeys::EXIT] = "EXIT";
    
    // Common UI
    translations[TranslationKeys::BACK] = "BACK";
    translations[TranslationKeys::APPLY_CHANGES] = "APPLY CHANGES";
    translations[TranslationKeys::REFRESH] = "REFRESH";
    
    // Tile Names
    translations[TranslationKeys::TILE_CAR] = "Car";
    translations[TranslationKeys::TILE_BATTERY] = "Battery";
    translations[TranslationKeys::TILE_APPEARANCE] = "Appearance";
    translations[TranslationKeys::TILE_POWER] = "Power";
    translations[TranslationKeys::TILE_BLUETOOTH] = "Bluetooth";
    translations[TranslationKeys::TILE_SOUND] = "Sound";
    translations[TranslationKeys::TILE_ABOUT] = "About";
    translations[TranslationKeys::TILE_DISPLAY] = "Display";
    translations[TranslationKeys::TILE_NETWORK] = "Network";
    translations[TranslationKeys::TILE_UPDATES] = "Updates";
    translations[TranslationKeys::TILE_HYPERLAND] = "Hyperland";
    translations[TranslationKeys::TILE_STORAGE] = "Storage";
    translations[TranslationKeys::TILE_SUPPORT] = "Support";
    
    // About Manager
    translations[TranslationKeys::ABOUT_TITLE] = "ABOUT SYSTEM";
    translations[TranslationKeys::SYSTEM_INFO] = "SYSTEM INFORMATION";
    translations[TranslationKeys::OS_NAME] = "Operating System";
    translations[TranslationKeys::KERNEL_VERSION] = "Kernel Version";
    translations[TranslationKeys::UPTIME] = "System Uptime";
    translations[TranslationKeys::CPU_INFO] = "Processor";
    translations[TranslationKeys::MEMORY_INFO] = "Memory";
    translations[TranslationKeys::DISK_USAGE] = "Disk Usage";
    
    // Battery Manager
    translations[TranslationKeys::BATTERY_TITLE] = "BATTERY STATUS";
    translations[TranslationKeys::BATTERY_STATUS] = "Battery Status";
    translations[TranslationKeys::BATTERY_HEALTH] = "BATTERY HEALTH";
    translations[TranslationKeys::CHARGING] = "Charging";
    translations[TranslationKeys::DISCHARGING] = "Discharging";
    translations[TranslationKeys::FULL] = "Full";
    translations[TranslationKeys::NOT_CHARGING] = "Not Charging";
    
    // Display Manager
    translations[TranslationKeys::DISPLAY_TITLE] = "DISPLAY SETTINGS";
    translations[TranslationKeys::DISPLAY_SETTINGS] = "Display Settings";
    translations[TranslationKeys::DISPLAY_RESOLUTION] = "DISPLAY RESOLUTION";
    translations[TranslationKeys::SCREEN_BRIGHTNESS] = "SCREEN BRIGHTNESS";
    translations[TranslationKeys::DISPLAY_INFORMATION] = "DISPLAY INFORMATION";
    translations[TranslationKeys::DISPLAY_INFO_TEXT] = "Changes will be applied to your Hyprland configuration.\nRestart Hyprland for display resolution changes to take effect.";
    
    // Sound Manager
    translations[TranslationKeys::SOUND_TITLE] = "SOUND";
    translations[TranslationKeys::OUTPUT_DEVICES] = "OUTPUT DEVICES";
    translations[TranslationKeys::INPUT_DEVICES] = "INPUT DEVICES";
    translations[TranslationKeys::VOLUME] = "Volume";
    translations[TranslationKeys::PLAYBACK_APPLICATIONS] = "PLAYBACK APPLICATIONS";
    
    // Network Manager
    translations[TranslationKeys::NETWORK_TITLE] = "NETWORK";
    translations[TranslationKeys::WIFI_NETWORKS] = "WiFi Networks";
    translations[TranslationKeys::WIFI] = "Wi-Fi";
    translations[TranslationKeys::CONNECTION_STATUS] = "Connection Status";
    translations[TranslationKeys::CONNECT] = "Connect";
    translations[TranslationKeys::DISCONNECT] = "Disconnect";
    translations[TranslationKeys::CONNECTED] = "Connected";
    translations[TranslationKeys::DISCONNECTED] = "Disconnected";
    translations[TranslationKeys::ENTER_PASSWORD] = "Enter Password";
    translations[TranslationKeys::CONNECTING] = "Connecting...";
    translations[TranslationKeys::CANCEL] = "Cancel";
    translations[TranslationKeys::REFRESH_NETWORKS] = "Refresh Networks";
    translations[TranslationKeys::NETWORKMANAGER_NOT_AVAILABLE] = "NetworkManager not available";
    translations[TranslationKeys::NETWORKING_DISABLED] = "Networking is disabled";
    translations[TranslationKeys::ENABLE_NETWORKING] = "Enable Networking";
    translations[TranslationKeys::CONNECTED_VIA_ETHERNET] = "Connected via Ethernet";
    translations[TranslationKeys::WIFI_HARDWARE_DISABLED] = "Wi-Fi hardware is disabled";
    translations[TranslationKeys::WIFI_DISABLED] = "Wi-Fi is disabled";
    translations[TranslationKeys::NO_NETWORKS_FOUND] = "No networks found";
    translations[TranslationKeys::IPV4_METHOD] = "IPv4 Method:";
    translations[TranslationKeys::IP_ADDRESS] = "IP Address:";
    translations[TranslationKeys::NETMASK] = "Netmask:";
    translations[TranslationKeys::GATEWAY] = "Gateway:";
    translations[TranslationKeys::DNS_SERVERS] = "DNS Servers:";
    translations[TranslationKeys::IPV6_METHOD] = "IPv6 Method:";
    translations[TranslationKeys::IPV6_DNS_SERVERS] = "IPv6 DNS Servers:";
    
    // Bluetooth Manager
    translations[TranslationKeys::BLUETOOTH_TITLE] = "BLUETOOTH";
    translations[TranslationKeys::BLUETOOTH_DEVICES] = "Bluetooth Devices";
    translations[TranslationKeys::OK] = "Ok";
    translations[TranslationKeys::DEVICES] = "DEVICES";
    translations[TranslationKeys::PAIR] = "Pair";
    translations[TranslationKeys::UNPAIR] = "Unpair";
    translations[TranslationKeys::PAIRED] = "Paired";
    translations[TranslationKeys::AVAILABLE] = "Available";
    translations[TranslationKeys::TOGGLE_BLUETOOTH] = "Toggle Bluetooth";
    translations[TranslationKeys::SCAN_DEVICES] = "Scan Devices";
    translations[TranslationKeys::BLUETOOTH_DISABLED] = "Bluetooth is disabled";
    translations[TranslationKeys::NO_DEVICES_FOUND_BT] = "No devices found - try scanning";
    translations[TranslationKeys::SCANNING] = "Scanning...";
    translations[TranslationKeys::SCANNING_FOR_DEVICES] = "Scanning for devices...";
    translations[TranslationKeys::BLUETOOTH_ENABLED_NO_DEVICES] = "Bluetooth enabled - No devices connected";
    translations[TranslationKeys::CONNECTED_TO] = "Connected to: ";
    translations[TranslationKeys::CONNECTED_TO_MULTIPLE] = "Connected to ";
    translations[TranslationKeys::TURN_OFF] = "Turn Off";
    translations[TranslationKeys::TURN_ON] = "Turn On";
    translations[TranslationKeys::DISCONNECT_BLU] = "Disconnect";
    translations[TranslationKeys::CONNECT_BLU] = "Connect";
    translations[TranslationKeys::FORGET] = "Forget";
    translations[TranslationKeys::SUCCESS_BLU] = "Success";
    translations[TranslationKeys::DISCONNECTED_BLU] = "Disconnected";
    translations[TranslationKeys::REMOVED] = "Removed";
    translations[TranslationKeys::CONNECTED_STATUS] = " - Connected";
    translations[TranslationKeys::PAIRED_STATUS] = " - Paired";
    translations[TranslationKeys::NOT_PAIRED_STATUS] = " - Not paired";
    
    // Appearance Manager
    translations[TranslationKeys::APPEARANCE_TITLE] = "APPEARANCE";
    translations[TranslationKeys::THEME_SETTINGS] = "Theme Settings";
    translations[TranslationKeys::THEME_SELECTION] = "THEME SELECTION";
    translations[TranslationKeys::WALLPAPER] = "Wallpaper";
    translations[TranslationKeys::WALLPAPER_SELECTION] = "WALLPAPER SELECTION";
    translations[TranslationKeys::COLORS] = "Colors";
    translations[TranslationKeys::LIGHT] = "Light";
    translations[TranslationKeys::DARK] = "Dark";
    translations[TranslationKeys::BROWSE] = "BROWSE";
    translations[TranslationKeys::PREVIOUS_WALLPAPER] = "Previous wallpaper";
    translations[TranslationKeys::NEXT_WALLPAPER] = "Next wallpaper";
    translations[TranslationKeys::OPEN_WALLPAPER_FOLDER] = "Open wallpaper folder";
    translations[TranslationKeys::SET_THIS_WALLPAPER] = "Set this wallpaper";
    
    // Power Manager
    translations[TranslationKeys::POWER_TITLE] = "POWER";
    translations[TranslationKeys::POWER_SETTINGS] = "Power Settings";
    translations[TranslationKeys::SLEEP] = "Sleep";
    translations[TranslationKeys::HIBERNATE] = "Hibernate";
    translations[TranslationKeys::SHUTDOWN] = "Shutdown";
    translations[TranslationKeys::RESTART] = "Restart";
    translations[TranslationKeys::PERFORMANCE] = "PERFORMANCE";
    translations[TranslationKeys::BALANCED] = "BALANCED";
    translations[TranslationKeys::DEFAULT] = "DEFAULT";
    translations[TranslationKeys::POWER_PROFILE_INFORMATION] = "POWER PROFILE INFORMATION";
    translations[TranslationKeys::POWER_PROFILE_INFO_TEXT] = "Changes require administrator privileges and will be applied immediately.\nCurrent power profile will be detected automatically when opening this menu.";
    translations[TranslationKeys::MISSING] = "MISSING";
    
    // Storage Manager
    translations[TranslationKeys::STORAGE_TITLE] = "STORAGE";
    translations[TranslationKeys::DISK_USAGE_TITLE] = "Disk Usage";
    translations[TranslationKeys::AVAILABLE_SPACE] = "Available Space";
    translations[TranslationKeys::USED_SPACE] = "Used Space";
    
    // Applications Manager
    translations[TranslationKeys::APPLICATIONS_TITLE] = "HYPRLAND APPLICATIONS";
    translations[TranslationKeys::KEYBINDS] = "KEYBINDS";
    translations[TranslationKeys::AUTORUNS] = "AUTORUNS";
    translations[TranslationKeys::WINDOW_RULES] = "WINDOW_RULES";
    
    // Language Manager
    translations[TranslationKeys::LANGUAGE_SETTINGS] = "LANGUAGE SETTINGS";
    translations[TranslationKeys::SYSTEM_LANGUAGE] = "SYSTEM LANGUAGE";
    translations[TranslationKeys::LANGUAGE_INFORMATION] = "LANGUAGE INFORMATION";
    translations[TranslationKeys::LANGUAGE_INFO_TEXT] = "Changes will be applied after logout.\nYour session will need to be restarted.";
    
    // Messages
    translations[TranslationKeys::CHECKING_STATUS] = "Checking status...";
    translations[TranslationKeys::CHECKING_BATTERY_STATUS] = "Checking battery status...";
    translations[TranslationKeys::LOADING] = "Loading...";
    translations[TranslationKeys::ERROR] = "Error";
    translations[TranslationKeys::SUCCESS] = "Success";
    translations[TranslationKeys::WARNING] = "Warning";
    translations[TranslationKeys::NO_SYSTEM_INFO] = "No system information available";
    translations[TranslationKeys::OUTPUT_DEVICES] = "Output Devices";
    translations[TranslationKeys::INPUT_DEVICES] = "Input Devices";
    translations[TranslationKeys::APPLICATION_VOLUME] = "Application Volume";
    translations[TranslationKeys::NO_AUDIO_APPS] = "No applications currently playing audio";
    
    // Dialog Messages
    translations[TranslationKeys::LOGOUT_REQUIRED] = "Logout Required";
    translations[TranslationKeys::LOGOUT_MESSAGE] = "Language changes require a logout to take effect.\nWould you like to logout now?";
    translations[TranslationKeys::LOGOUT_NOW] = "Logout Now";
    translations[TranslationKeys::LOGOUT_LATER] = "Later";
    
    // Network Manager Additional
    translations[TranslationKeys::NETWORK_SETTINGS] = "Network Settings";
    translations[TranslationKeys::ENTER_NETWORK_PASSWORD] = "Enter Network Password";
    translations[TranslationKeys::IPV4_METHOD_LABEL] = "IPv4 Method:";
    translations[TranslationKeys::IP_ADDRESS_LABEL] = "IP Address:";
    translations[TranslationKeys::NETMASK_LABEL] = "Netmask:";
    translations[TranslationKeys::GATEWAY_LABEL] = "Gateway:";
    translations[TranslationKeys::DNS_SERVERS_LABEL] = "DNS Servers:";
    translations[TranslationKeys::IPV6_METHOD_LABEL] = "IPv6 Method:";
    translations[TranslationKeys::IPV6_DNS_SERVERS_LABEL] = "IPv6 DNS Servers:";
    translations[TranslationKeys::PORT_SETTINGS] = "Port Settings:";
    translations[TranslationKeys::HTTP_PROXY_PORT] = "HTTP Proxy Port:";
    translations[TranslationKeys::HTTPS_PROXY_PORT] = "HTTPS Proxy Port:";
    translations[TranslationKeys::FTP_PROXY_PORT] = "FTP Proxy Port:";
    translations[TranslationKeys::SOCKS_PROXY_PORT] = "SOCKS Proxy Port:";
    translations[TranslationKeys::CUSTOM_PORT_RANGE] = "Custom Port Range:";
    translations[TranslationKeys::PORT_DASH] = " - ";
    translations[TranslationKeys::CONNECTED_TO_PREFIX] = "Connected to: ";
    translations[TranslationKeys::ETHERNET] = "Ethernet";
    translations[TranslationKeys::CONNECTING_STATUS] = "Connecting...";
    translations[TranslationKeys::NETWORKING_DISABLED_STATUS] = "Networking disabled";
    translations[TranslationKeys::WIFI_DISABLED_STATUS] = "Wi-Fi disabled";
    translations[TranslationKeys::NOT_CONNECTED_STATUS] = "Not connected";
    
    // Navigation Buttons
    translations[TranslationKeys::PREVIOUS_ARROW] = "‹";
    translations[TranslationKeys::NEXT_ARROW] = "›";
    translations[TranslationKeys::LEFT_ARROW] = "<";
    translations[TranslationKeys::RIGHT_ARROW] = ">";
    translations[TranslationKeys::ADD_BUTTON] = "+";
    translations[TranslationKeys::REMOVE_BUTTON] = "−";
    
    // Language Manager
    translations[TranslationKeys::LANGUAGE_CHANGED] = "Language Changed";
    
    // Display Manager
    translations[TranslationKeys::BRIGHTNESS_VALUE] = "50%";
    
    // Sound Manager
    translations[TranslationKeys::VOLUME_VALUE] = "0%";
    
    // Battery Manager Additional
    translations[TranslationKeys::POWER_DETAILS] = "POWER DETAILS";
    translations[TranslationKeys::NO_BATTERY_DETECTED] = "No battery detected";
    translations[TranslationKeys::CAPACITY] = "Capacity";
    translations[TranslationKeys::TECHNOLOGY] = "Technology";
    translations[TranslationKeys::CHARGE_CYCLES] = "Charge Cycles";
    translations[TranslationKeys::WARNING_LEVEL] = "Warning Level";
    translations[TranslationKeys::ENERGY_FULL] = "Energy Full";
    translations[TranslationKeys::ENERGY_DESIGN] = "Energy Design";
    translations[TranslationKeys::ENERGY_RATE] = "Energy Rate";
    translations[TranslationKeys::VOLTAGE] = "Voltage";
    
    // Power Manager Additional
    translations[TranslationKeys::PERFORMANCE_MODE] = "Performance";
    translations[TranslationKeys::BALANCED_MODE] = "Balanced";
    translations[TranslationKeys::DEFAULT_MODE] = "Default";
    translations[TranslationKeys::PERFORMANCE_DESCRIPTION] = "Maximum CPU performance for demanding tasks";
    translations[TranslationKeys::BALANCED_DESCRIPTION] = "Optimized balance between performance and power saving";
    translations[TranslationKeys::DEFAULT_DESCRIPTION] = "System default power management";
}
