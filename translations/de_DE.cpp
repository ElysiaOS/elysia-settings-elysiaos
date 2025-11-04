#include "de_DE.h"

void loadGermanTranslations(std::map<TranslationKeys, std::string>& translations) {
    // Main Window
    translations[TranslationKeys::WINDOW_TITLE] = "ElysiaOS Einstellungen";
    translations[TranslationKeys::EXIT] = "BEENDEN";
    
    // Common UI
    translations[TranslationKeys::BACK] = "ZURÜCK";
    translations[TranslationKeys::APPLY_CHANGES] = "ÄNDERUNGEN ANWENDEN";
    translations[TranslationKeys::REFRESH] = "AKTUALISIEREN";
    
    // Tile Names
    translations[TranslationKeys::TILE_CAR] = "Auto";
    translations[TranslationKeys::TILE_BATTERY] = "Batterie";
    translations[TranslationKeys::TILE_APPEARANCE] = "Erscheinungsbild";
    translations[TranslationKeys::TILE_POWER] = "Energie";
    translations[TranslationKeys::TILE_BLUETOOTH] = "Bluetooth";
    translations[TranslationKeys::TILE_SOUND] = "Ton";
    translations[TranslationKeys::TILE_ABOUT] = "Über";
    translations[TranslationKeys::TILE_DISPLAY] = "Anzeige";
    translations[TranslationKeys::TILE_NETWORK] = "Netzwerk";
    translations[TranslationKeys::TILE_UPDATES] = "Updates";
    translations[TranslationKeys::TILE_HYPERLAND] = "Hyperland";
    translations[TranslationKeys::TILE_STORAGE] = "Speicher";
    translations[TranslationKeys::TILE_SUPPORT] = "Support";
    
    // About Manager
    translations[TranslationKeys::ABOUT_TITLE] = "ÜBER SYSTEM";
    translations[TranslationKeys::SYSTEM_INFO] = "SYSTEMINFORMATIONEN";
    translations[TranslationKeys::OS_NAME] = "Betriebssystem";
    translations[TranslationKeys::KERNEL_VERSION] = "Kernel-Version";
    translations[TranslationKeys::UPTIME] = "Systemlaufzeit";
    translations[TranslationKeys::CPU_INFO] = "Prozessor";
    translations[TranslationKeys::MEMORY_INFO] = "Arbeitsspeicher";
    translations[TranslationKeys::DISK_USAGE] = "Festplattennutzung";
    
    // Battery Manager
    translations[TranslationKeys::BATTERY_TITLE] = "BATTERIESTATUS";
    translations[TranslationKeys::BATTERY_STATUS] = "Batteriestatus";
    translations[TranslationKeys::BATTERY_HEALTH] = "BATTERIEZUSTAND";
    translations[TranslationKeys::CHARGING] = "Lädt";
    translations[TranslationKeys::DISCHARGING] = "Entlädt";
    translations[TranslationKeys::FULL] = "Voll";
    translations[TranslationKeys::NOT_CHARGING] = "Lädt nicht";
    
    // Display Manager
    translations[TranslationKeys::DISPLAY_TITLE] = "ANZEIGEEINSTELLUNGEN";
    translations[TranslationKeys::DISPLAY_SETTINGS] = "Anzeigeeinstellungen";
    translations[TranslationKeys::DISPLAY_RESOLUTION] = "ANZEIGEAUFLÖSUNG";
    translations[TranslationKeys::SCREEN_BRIGHTNESS] = "BILDSCHIRMHELLIGKEIT";
    translations[TranslationKeys::DISPLAY_INFORMATION] = "ANZEIGEINFORMATIONEN";
    translations[TranslationKeys::DISPLAY_INFO_TEXT] = "Änderungen werden auf Ihre Hyprland-Konfiguration angewendet.\nStarten Sie Hyprland neu, damit Änderungen der Anzeigeauflösung wirksam werden.";
    
    // Sound Manager
    translations[TranslationKeys::SOUND_TITLE] = "TON";
    translations[TranslationKeys::OUTPUT_DEVICES] = "AUSGABEGERÄTE";
    translations[TranslationKeys::INPUT_DEVICES] = "EINGABEGERÄTE";
    translations[TranslationKeys::VOLUME] = "Lautstärke";
    translations[TranslationKeys::PLAYBACK_APPLICATIONS] = "WIEDERGABEANWENDUNGEN";
    
    // Network Manager
    translations[TranslationKeys::NETWORK_TITLE] = "NETZWERK";
    translations[TranslationKeys::WIFI_NETWORKS] = "WiFi-Netzwerke";
    translations[TranslationKeys::WIFI] = "Wi-Fi";
    translations[TranslationKeys::CONNECTION_STATUS] = "Verbindungsstatus";
    translations[TranslationKeys::CONNECT] = "Verbinden";
    translations[TranslationKeys::DISCONNECT] = "Trennen";
    translations[TranslationKeys::CONNECTED] = "Verbunden";
    translations[TranslationKeys::DISCONNECTED] = "Getrennt";
    translations[TranslationKeys::ENTER_PASSWORD] = "Passwort eingeben";
    translations[TranslationKeys::CONNECTING] = "Verbinde...";
    translations[TranslationKeys::CANCEL] = "Abbrechen";
    translations[TranslationKeys::REFRESH_NETWORKS] = "Netzwerke aktualisieren";
    translations[TranslationKeys::NETWORKMANAGER_NOT_AVAILABLE] = "NetworkManager nicht verfügbar";
    translations[TranslationKeys::NETWORKING_DISABLED] = "Netzwerk ist deaktiviert";
    translations[TranslationKeys::ENABLE_NETWORKING] = "Netzwerk aktivieren";
    translations[TranslationKeys::CONNECTED_VIA_ETHERNET] = "Via Ethernet verbunden";
    translations[TranslationKeys::WIFI_HARDWARE_DISABLED] = "Wi-Fi-Hardware ist deaktiviert";
    translations[TranslationKeys::WIFI_DISABLED] = "Wi-Fi ist deaktiviert";
    translations[TranslationKeys::NO_NETWORKS_FOUND] = "Keine Netzwerke gefunden";
    translations[TranslationKeys::IPV4_METHOD] = "IPv4-Methode:";
    translations[TranslationKeys::IP_ADDRESS] = "IP-Adresse:";
    translations[TranslationKeys::NETMASK] = "Netzmaske:";
    translations[TranslationKeys::GATEWAY] = "Gateway:";
    translations[TranslationKeys::DNS_SERVERS] = "DNS-Server:";
    translations[TranslationKeys::IPV6_METHOD] = "IPv6-Methode:";
    translations[TranslationKeys::IPV6_DNS_SERVERS] = "IPv6 DNS-Server:";
    
    // Bluetooth Manager
    translations[TranslationKeys::BLUETOOTH_TITLE] = "BLUETOOTH";
    translations[TranslationKeys::BLUETOOTH_DEVICES] = "Bluetooth-Geräte";
    translations[TranslationKeys::OK] = "Ok";
    translations[TranslationKeys::DEVICES] = "GERÄTE";
    translations[TranslationKeys::PAIR] = "Koppeln";
    translations[TranslationKeys::UNPAIR] = "Entkoppeln";
    translations[TranslationKeys::PAIRED] = "Gekoppelt";
    translations[TranslationKeys::AVAILABLE] = "Verfügbar";
    translations[TranslationKeys::TOGGLE_BLUETOOTH] = "Bluetooth umschalten";
    translations[TranslationKeys::SCAN_DEVICES] = "Geräte scannen";
    translations[TranslationKeys::BLUETOOTH_DISABLED] = "Bluetooth ist deaktiviert";
    translations[TranslationKeys::NO_DEVICES_FOUND_BT] = "Keine Geräte gefunden - versuchen Sie zu scannen";
    translations[TranslationKeys::SCANNING] = "Scanne...";
    translations[TranslationKeys::SCANNING_FOR_DEVICES] = "Suche nach Geräten...";
    translations[TranslationKeys::BLUETOOTH_ENABLED_NO_DEVICES] = "Bluetooth aktiviert - Keine Geräte verbunden";
    translations[TranslationKeys::CONNECTED_TO] = "Verbunden mit: ";
    translations[TranslationKeys::CONNECTED_TO_MULTIPLE] = "Verbunden mit ";
    translations[TranslationKeys::TURN_OFF] = "Ausschalten";
    translations[TranslationKeys::TURN_ON] = "Einschalten";
    translations[TranslationKeys::DISCONNECT_BLU] = "Trennen";
    translations[TranslationKeys::CONNECT_BLU] = "Verbinden";
    translations[TranslationKeys::FORGET] = "Vergessen";
    translations[TranslationKeys::SUCCESS_BLU] = "Erfolgreich";
    translations[TranslationKeys::DISCONNECTED_BLU] = "Getrennt";
    translations[TranslationKeys::REMOVED] = "Entfernt";
    translations[TranslationKeys::CONNECTED_STATUS] = " - Verbunden";
    translations[TranslationKeys::PAIRED_STATUS] = " - Gekoppelt";
    translations[TranslationKeys::NOT_PAIRED_STATUS] = " - Nicht gekoppelt";
    
    // Appearance Manager
    translations[TranslationKeys::APPEARANCE_TITLE] = "ERSCHEINUNGSBILD";
    translations[TranslationKeys::THEME_SETTINGS] = "Theme-Einstellungen";
    translations[TranslationKeys::THEME_SELECTION] = "THEME-AUSWAHL";
    translations[TranslationKeys::WALLPAPER] = "Hintergrundbild";
    translations[TranslationKeys::WALLPAPER_SELECTION] = "HINTERGRUNDBILDAUSWAHL";
    translations[TranslationKeys::COLORS] = "Farben";
    translations[TranslationKeys::LIGHT] = "Hell";
    translations[TranslationKeys::DARK] = "Dunkel";
    translations[TranslationKeys::BROWSE] = "DURCHSUCHEN";
    translations[TranslationKeys::PREVIOUS_WALLPAPER] = "Vorheriges Hintergrundbild";
    translations[TranslationKeys::NEXT_WALLPAPER] = "Nächstes Hintergrundbild";
    translations[TranslationKeys::OPEN_WALLPAPER_FOLDER] = "Hintergrundbildordner öffnen";
    translations[TranslationKeys::SET_THIS_WALLPAPER] = "Dieses Hintergrundbild setzen";
    
    // Power Manager
    translations[TranslationKeys::POWER_TITLE] = "ENERGIE";
    translations[TranslationKeys::POWER_SETTINGS] = "Energieeinstellungen";
    translations[TranslationKeys::SLEEP] = "Ruhezustand";
    translations[TranslationKeys::HIBERNATE] = "Winterschlaf";
    translations[TranslationKeys::SHUTDOWN] = "Herunterfahren";
    translations[TranslationKeys::RESTART] = "Neustart";
    translations[TranslationKeys::PERFORMANCE] = "LEISTUNG";
    translations[TranslationKeys::BALANCED] = "AUSGEWOGEN";
    translations[TranslationKeys::DEFAULT] = "STANDARD";
    translations[TranslationKeys::POWER_PROFILE_INFORMATION] = "ENERGIEPROFIL-INFORMATIONEN";
    translations[TranslationKeys::POWER_PROFILE_INFO_TEXT] = "Änderungen erfordern Administratorrechte und werden sofort angewendet.\nDas aktuelle Energieprofil wird automatisch erkannt, wenn dieses Menü geöffnet wird.";
    translations[TranslationKeys::MISSING] = "FEHLT";
    
    // Storage Manager
    translations[TranslationKeys::STORAGE_TITLE] = "SPEICHER";
    translations[TranslationKeys::DISK_USAGE_TITLE] = "Festplattennutzung";
    translations[TranslationKeys::AVAILABLE_SPACE] = "Verfügbarer Speicher";
    translations[TranslationKeys::USED_SPACE] = "Verwendeter Speicher";
    
    // Applications Manager
    translations[TranslationKeys::APPLICATIONS_TITLE] = "HYPRLAND-ANWENDUNGEN";
    translations[TranslationKeys::KEYBINDS] = "TASTENKÜRZEL";
    translations[TranslationKeys::AUTORUNS] = "AUTOSTART";
    translations[TranslationKeys::WINDOW_RULES] = "FENSTERREGELN";
    
    // Language Manager
    translations[TranslationKeys::LANGUAGE_SETTINGS] = "SPRACHEINSTELLUNGEN";
    translations[TranslationKeys::SYSTEM_LANGUAGE] = "SYSTEMSPRACHE";
    translations[TranslationKeys::LANGUAGE_INFORMATION] = "SPRACHINFORMATIONEN";
    translations[TranslationKeys::LANGUAGE_INFO_TEXT] = "Änderungen werden nach dem Abmelden angewendet.\nIhre Sitzung muss neu gestartet werden.";
    
    // Messages
    translations[TranslationKeys::CHECKING_STATUS] = "Status wird überprüft...";
    translations[TranslationKeys::CHECKING_BATTERY_STATUS] = "Batteriestatus wird überprüft...";
    translations[TranslationKeys::LOADING] = "Lädt...";
    translations[TranslationKeys::ERROR] = "Fehler";
    translations[TranslationKeys::SUCCESS] = "Erfolgreich";
    translations[TranslationKeys::WARNING] = "Warnung";
    translations[TranslationKeys::NO_SYSTEM_INFO] = "Keine Systeminformationen verfügbar";
    translations[TranslationKeys::OUTPUT_DEVICES] = "Ausgabegeräte";
    translations[TranslationKeys::INPUT_DEVICES] = "Eingabegeräte";
    translations[TranslationKeys::APPLICATION_VOLUME] = "Anwendungsvolumen";
    translations[TranslationKeys::NO_AUDIO_APPS] = "Keine Anwendungen spielen derzeit Audio ab";
    
    // Dialog Messages
    translations[TranslationKeys::LOGOUT_REQUIRED] = "Abmeldung erforderlich";
    translations[TranslationKeys::LOGOUT_MESSAGE] = "Sprachänderungen erfordern eine Abmeldung, um wirksam zu werden.\nMöchten Sie sich jetzt abmelden?";
    translations[TranslationKeys::LOGOUT_NOW] = "Jetzt abmelden";
    translations[TranslationKeys::LOGOUT_LATER] = "Später";
    
    // Network Manager Additional
    translations[TranslationKeys::NETWORK_SETTINGS] = "Netzwerkeinstellungen";
    translations[TranslationKeys::ENTER_NETWORK_PASSWORD] = "Netzwerkpasswort eingeben";
    translations[TranslationKeys::IPV4_METHOD_LABEL] = "IPv4-Methode:";
    translations[TranslationKeys::IP_ADDRESS_LABEL] = "IP-Adresse:";
    translations[TranslationKeys::NETMASK_LABEL] = "Netzmaske:";
    translations[TranslationKeys::GATEWAY_LABEL] = "Gateway:";
    translations[TranslationKeys::DNS_SERVERS_LABEL] = "DNS-Server:";
    translations[TranslationKeys::IPV6_METHOD_LABEL] = "IPv6-Methode:";
    translations[TranslationKeys::IPV6_DNS_SERVERS_LABEL] = "IPv6 DNS-Server:";
    translations[TranslationKeys::PORT_SETTINGS] = "Port-Einstellungen:";
    translations[TranslationKeys::HTTP_PROXY_PORT] = "HTTP-Proxy-Port:";
    translations[TranslationKeys::HTTPS_PROXY_PORT] = "HTTPS-Proxy-Port:";
    translations[TranslationKeys::FTP_PROXY_PORT] = "FTP-Proxy-Port:";
    translations[TranslationKeys::SOCKS_PROXY_PORT] = "SOCKS-Proxy-Port:";
    translations[TranslationKeys::CUSTOM_PORT_RANGE] = "Benutzerdefinierter Portbereich:";
    translations[TranslationKeys::PORT_DASH] = " - ";
    translations[TranslationKeys::CONNECTED_TO_PREFIX] = "Verbunden mit: ";
    translations[TranslationKeys::ETHERNET] = "Ethernet";
    translations[TranslationKeys::CONNECTING_STATUS] = "Verbinden...";
    translations[TranslationKeys::NETWORKING_DISABLED_STATUS] = "Netzwerk deaktiviert";
    translations[TranslationKeys::WIFI_DISABLED_STATUS] = "Wi-Fi deaktiviert";
    translations[TranslationKeys::NOT_CONNECTED_STATUS] = "Nicht verbunden";
    
    // Navigation Buttons
    translations[TranslationKeys::PREVIOUS_ARROW] = "‹";
    translations[TranslationKeys::NEXT_ARROW] = "›";
    translations[TranslationKeys::LEFT_ARROW] = "<";
    translations[TranslationKeys::RIGHT_ARROW] = ">";
    translations[TranslationKeys::ADD_BUTTON] = "+";
    translations[TranslationKeys::REMOVE_BUTTON] = "−";
    
    // Language Manager
    translations[TranslationKeys::LANGUAGE_CHANGED] = "Sprache geändert";
    
    // Display Manager
    translations[TranslationKeys::BRIGHTNESS_VALUE] = "50%";
    
    // Sound Manager
    translations[TranslationKeys::VOLUME_VALUE] = "0%";
    
    // Battery Manager Additional
    translations[TranslationKeys::POWER_DETAILS] = "STROMDETAILS";
    translations[TranslationKeys::NO_BATTERY_DETECTED] = "Keine Batterie erkannt";
    translations[TranslationKeys::CAPACITY] = "Kapazität";
    translations[TranslationKeys::TECHNOLOGY] = "Technologie";
    translations[TranslationKeys::CHARGE_CYCLES] = "Ladezyklen";
    translations[TranslationKeys::WARNING_LEVEL] = "Warnstufe";
    translations[TranslationKeys::ENERGY_FULL] = "Energie Voll";
    translations[TranslationKeys::ENERGY_DESIGN] = "Energie Design";
    translations[TranslationKeys::ENERGY_RATE] = "Energierate";
    translations[TranslationKeys::VOLTAGE] = "Spannung";
    
    // Power Manager Additional
    translations[TranslationKeys::PERFORMANCE_MODE] = "Leistung";
    translations[TranslationKeys::BALANCED_MODE] = "Ausgewogen";
    translations[TranslationKeys::DEFAULT_MODE] = "Standard";
    translations[TranslationKeys::PERFORMANCE_DESCRIPTION] = "Maximale CPU-Leistung für anspruchsvolle Aufgaben";
    translations[TranslationKeys::BALANCED_DESCRIPTION] = "Optimiertes Gleichgewicht zwischen Leistung und Stromsparen";
    translations[TranslationKeys::DEFAULT_DESCRIPTION] = "Standard-System-Stromverwaltung";
}
