#include "fr_FR.h"

void loadFrenchTranslations(std::map<TranslationKeys, std::string>& translations) {
    // Main Window
    translations[TranslationKeys::WINDOW_TITLE] = "Paramètres ElysiaOS";
    translations[TranslationKeys::EXIT] = "QUITTER";
    
    // Common UI
    translations[TranslationKeys::BACK] = "RETOUR";
    translations[TranslationKeys::APPLY_CHANGES] = "APPLIQUER LES MODIFICATIONS";
    translations[TranslationKeys::REFRESH] = "ACTUALISER";
    
    // Tile Names
    translations[TranslationKeys::TILE_CAR] = "Voiture";
    translations[TranslationKeys::TILE_BATTERY] = "Batterie";
    translations[TranslationKeys::TILE_APPEARANCE] = "Apparence";
    translations[TranslationKeys::TILE_POWER] = "Alimentation";
    translations[TranslationKeys::TILE_BLUETOOTH] = "Bluetooth";
    translations[TranslationKeys::TILE_SOUND] = "Son";
    translations[TranslationKeys::TILE_ABOUT] = "À propos";
    translations[TranslationKeys::TILE_DISPLAY] = "Affichage";
    translations[TranslationKeys::TILE_NETWORK] = "Réseau";
    translations[TranslationKeys::TILE_UPDATES] = "Mises à jour";
    translations[TranslationKeys::TILE_HYPERLAND] = "Hyperland";
    translations[TranslationKeys::TILE_STORAGE] = "Stockage";
    translations[TranslationKeys::TILE_SUPPORT] = "Support";
    
    // About Manager
    translations[TranslationKeys::ABOUT_TITLE] = "À PROPOS DU SYSTÈME";
    translations[TranslationKeys::SYSTEM_INFO] = "INFORMATIONS SYSTÈME";
    translations[TranslationKeys::OS_NAME] = "Système d'exploitation";
    translations[TranslationKeys::KERNEL_VERSION] = "Version du noyau";
    translations[TranslationKeys::UPTIME] = "Temps de fonctionnement";
    translations[TranslationKeys::CPU_INFO] = "Processeur";
    translations[TranslationKeys::MEMORY_INFO] = "Mémoire";
    translations[TranslationKeys::DISK_USAGE] = "Utilisation du disque";
    
    // Battery Manager
    translations[TranslationKeys::BATTERY_TITLE] = "ÉTAT DE LA BATTERIE";
    translations[TranslationKeys::BATTERY_STATUS] = "État de la batterie";
    translations[TranslationKeys::BATTERY_HEALTH] = "SANTÉ DE LA BATTERIE";
    translations[TranslationKeys::CHARGING] = "En charge";
    translations[TranslationKeys::DISCHARGING] = "En décharge";
    translations[TranslationKeys::FULL] = "Pleine";
    translations[TranslationKeys::NOT_CHARGING] = "Ne charge pas";
    
    // Display Manager
    translations[TranslationKeys::DISPLAY_TITLE] = "PARAMÈTRES D'AFFICHAGE";
    translations[TranslationKeys::DISPLAY_SETTINGS] = "Paramètres d'affichage";
    translations[TranslationKeys::DISPLAY_RESOLUTION] = "RÉSOLUTION D'AFFICHAGE";
    translations[TranslationKeys::SCREEN_BRIGHTNESS] = "LUMINOSITÉ DE L'ÉCRAN";
    translations[TranslationKeys::DISPLAY_INFORMATION] = "INFORMATIONS D'AFFICHAGE";
    translations[TranslationKeys::DISPLAY_INFO_TEXT] = "Les modifications seront appliquées à votre configuration Hyprland.\nRedémarrez Hyprland pour que les changements de résolution d'affichage prennent effet.";
    
    // Sound Manager
    translations[TranslationKeys::SOUND_TITLE] = "SON";
    translations[TranslationKeys::OUTPUT_DEVICES] = "PÉRIPHÉRIQUES DE SORTIE";
    translations[TranslationKeys::INPUT_DEVICES] = "PÉRIPHÉRIQUES D'ENTRÉE";
    translations[TranslationKeys::VOLUME] = "Volume";
    translations[TranslationKeys::PLAYBACK_APPLICATIONS] = "APPLICATIONS DE LECTURE";
    
    // Network Manager
    translations[TranslationKeys::NETWORK_TITLE] = "RÉSEAU";
    translations[TranslationKeys::WIFI_NETWORKS] = "Réseaux WiFi";
    translations[TranslationKeys::WIFI] = "Wi-Fi";
    translations[TranslationKeys::CONNECTION_STATUS] = "État de la connexion";
    translations[TranslationKeys::CONNECT] = "Se connecter";
    translations[TranslationKeys::DISCONNECT] = "Se déconnecter";
    translations[TranslationKeys::CONNECTED] = "Connecté";
    translations[TranslationKeys::DISCONNECTED] = "Déconnecté";
    translations[TranslationKeys::ENTER_PASSWORD] = "Entrer le mot de passe";
    translations[TranslationKeys::CONNECTING] = "Connexion...";
    translations[TranslationKeys::CANCEL] = "Annuler";
    translations[TranslationKeys::REFRESH_NETWORKS] = "Actualiser les réseaux";
    translations[TranslationKeys::NETWORKMANAGER_NOT_AVAILABLE] = "NetworkManager non disponible";
    translations[TranslationKeys::NETWORKING_DISABLED] = "Le réseau est désactivé";
    translations[TranslationKeys::ENABLE_NETWORKING] = "Activer le réseau";
    translations[TranslationKeys::CONNECTED_VIA_ETHERNET] = "Connecté via Ethernet";
    translations[TranslationKeys::WIFI_HARDWARE_DISABLED] = "Le matériel Wi-Fi est désactivé";
    translations[TranslationKeys::WIFI_DISABLED] = "Le Wi-Fi est désactivé";
    translations[TranslationKeys::NO_NETWORKS_FOUND] = "Aucun réseau trouvé";
    translations[TranslationKeys::IPV4_METHOD] = "Méthode IPv4 :";
    translations[TranslationKeys::IP_ADDRESS] = "Adresse IP :";
    translations[TranslationKeys::NETMASK] = "Masque de sous-réseau :";
    translations[TranslationKeys::GATEWAY] = "Passerelle :";
    translations[TranslationKeys::DNS_SERVERS] = "Serveurs DNS :";
    translations[TranslationKeys::IPV6_METHOD] = "Méthode IPv6 :";
    translations[TranslationKeys::IPV6_DNS_SERVERS] = "Serveurs DNS IPv6 :";
    
    // Bluetooth Manager
    translations[TranslationKeys::BLUETOOTH_TITLE] = "BLUETOOTH";
    translations[TranslationKeys::BLUETOOTH_DEVICES] = "Périphériques Bluetooth";
    translations[TranslationKeys::OK] = "Ok";
    translations[TranslationKeys::DEVICES] = "PÉRIPHÉRIQUES";
    translations[TranslationKeys::PAIR] = "Apparier";
    translations[TranslationKeys::UNPAIR] = "Désapparier";
    translations[TranslationKeys::PAIRED] = "Apparié";
    translations[TranslationKeys::AVAILABLE] = "Disponible";
    translations[TranslationKeys::TOGGLE_BLUETOOTH] = "Basculer Bluetooth";
    translations[TranslationKeys::SCAN_DEVICES] = "Scanner les périphériques";
    translations[TranslationKeys::BLUETOOTH_DISABLED] = "Bluetooth est désactivé";
    translations[TranslationKeys::NO_DEVICES_FOUND_BT] = "Aucun périphérique trouvé - essayez de scanner";
    translations[TranslationKeys::SCANNING] = "Scan en cours...";
    translations[TranslationKeys::SCANNING_FOR_DEVICES] = "Recherche de périphériques...";
    translations[TranslationKeys::BLUETOOTH_ENABLED_NO_DEVICES] = "Bluetooth activé - Aucun périphérique connecté";
    translations[TranslationKeys::CONNECTED_TO] = "Connecté à : ";
    translations[TranslationKeys::CONNECTED_TO_MULTIPLE] = "Connecté à ";
    translations[TranslationKeys::TURN_OFF] = "Éteindre";
    translations[TranslationKeys::TURN_ON] = "Allumer";
    translations[TranslationKeys::DISCONNECT_BLU] = "Déconnecter";
    translations[TranslationKeys::CONNECT_BLU] = "Connecter";
    translations[TranslationKeys::FORGET] = "Oublier";
    translations[TranslationKeys::SUCCESS_BLU] = "Succès";
    translations[TranslationKeys::DISCONNECTED_BLU] = "Déconnecté";
    translations[TranslationKeys::REMOVED] = "Supprimé";
    translations[TranslationKeys::CONNECTED_STATUS] = " - Connecté";
    translations[TranslationKeys::PAIRED_STATUS] = " - Apparié";
    translations[TranslationKeys::NOT_PAIRED_STATUS] = " - Non apparié";
    
    // Appearance Manager
    translations[TranslationKeys::APPEARANCE_TITLE] = "APPARENCE";
    translations[TranslationKeys::THEME_SETTINGS] = "Paramètres du thème";
    translations[TranslationKeys::THEME_SELECTION] = "SÉLECTION DU THÈME";
    translations[TranslationKeys::WALLPAPER] = "Fond d'écran";
    translations[TranslationKeys::WALLPAPER_SELECTION] = "SÉLECTION DU FOND D'ÉCRAN";
    translations[TranslationKeys::COLORS] = "Couleurs";
    translations[TranslationKeys::LIGHT] = "Clair";
    translations[TranslationKeys::DARK] = "Sombre";
    translations[TranslationKeys::BROWSE] = "PARCOURIR";
    translations[TranslationKeys::PREVIOUS_WALLPAPER] = "Fond d'écran précédent";
    translations[TranslationKeys::NEXT_WALLPAPER] = "Fond d'écran suivant";
    translations[TranslationKeys::OPEN_WALLPAPER_FOLDER] = "Ouvrir le dossier des fonds d'écran";
    translations[TranslationKeys::SET_THIS_WALLPAPER] = "Définir ce fond d'écran";
    
    // Power Manager
    translations[TranslationKeys::POWER_TITLE] = "ALIMENTATION";
    translations[TranslationKeys::POWER_SETTINGS] = "Paramètres d'alimentation";
    translations[TranslationKeys::SLEEP] = "Veille";
    translations[TranslationKeys::HIBERNATE] = "Hibernation";
    translations[TranslationKeys::SHUTDOWN] = "Arrêt";
    translations[TranslationKeys::RESTART] = "Redémarrage";
    translations[TranslationKeys::PERFORMANCE] = "PERFORMANCE";
    translations[TranslationKeys::BALANCED] = "ÉQUILIBRÉ";
    translations[TranslationKeys::DEFAULT] = "PAR DÉFAUT";
    translations[TranslationKeys::POWER_PROFILE_INFORMATION] = "INFORMATIONS DU PROFIL D'ALIMENTATION";
    translations[TranslationKeys::POWER_PROFILE_INFO_TEXT] = "Les modifications nécessitent des privilèges d'administrateur et seront appliquées immédiatement.\nLe profil d'alimentation actuel sera détecté automatiquement lors de l'ouverture de ce menu.";
    translations[TranslationKeys::MISSING] = "MANQUANT";
    
    // Storage Manager
    translations[TranslationKeys::STORAGE_TITLE] = "STOCKAGE";
    translations[TranslationKeys::DISK_USAGE_TITLE] = "Utilisation du disque";
    translations[TranslationKeys::AVAILABLE_SPACE] = "Espace disponible";
    translations[TranslationKeys::USED_SPACE] = "Espace utilisé";
    
    // Applications Manager
    translations[TranslationKeys::APPLICATIONS_TITLE] = "APPLICATIONS HYPRLAND";
    translations[TranslationKeys::KEYBINDS] = "RACCOURCIS CLAVIER";
    translations[TranslationKeys::AUTORUNS] = "DÉMARRAGE AUTOMATIQUE";
    translations[TranslationKeys::WINDOW_RULES] = "RÈGLES DE FENÊTRE";
    
    // Language Manager
    translations[TranslationKeys::LANGUAGE_SETTINGS] = "PARAMÈTRES DE LANGUE";
    translations[TranslationKeys::SYSTEM_LANGUAGE] = "LANGUE DU SYSTÈME";
    translations[TranslationKeys::LANGUAGE_INFORMATION] = "INFORMATIONS DE LANGUE";
    translations[TranslationKeys::LANGUAGE_INFO_TEXT] = "Les modifications seront appliquées après la déconnexion.\nVotre session devra être redémarrée.";
    
    // Messages
    translations[TranslationKeys::CHECKING_STATUS] = "Vérification du statut...";
    translations[TranslationKeys::CHECKING_BATTERY_STATUS] = "Vérification de l'état de la batterie...";
    translations[TranslationKeys::LOADING] = "Chargement...";
    translations[TranslationKeys::ERROR] = "Erreur";
    translations[TranslationKeys::SUCCESS] = "Succès";
    translations[TranslationKeys::WARNING] = "Avertissement";
    translations[TranslationKeys::NO_SYSTEM_INFO] = "Aucune information système disponible";
    translations[TranslationKeys::OUTPUT_DEVICES] = "Périphériques de sortie";
    translations[TranslationKeys::INPUT_DEVICES] = "Périphériques d'entrée";
    translations[TranslationKeys::APPLICATION_VOLUME] = "Volume de l'application";
    translations[TranslationKeys::NO_AUDIO_APPS] = "Aucune application ne lit actuellement de l'audio";
    
    // Dialog Messages
    translations[TranslationKeys::LOGOUT_REQUIRED] = "Déconnexion requise";
    translations[TranslationKeys::LOGOUT_MESSAGE] = "Les changements de langue nécessitent une déconnexion pour prendre effet.\nVoulez-vous vous déconnecter maintenant ?";
    translations[TranslationKeys::LOGOUT_NOW] = "Se déconnecter maintenant";
    translations[TranslationKeys::LOGOUT_LATER] = "Plus tard";

    // Network Manager Additional
    translations[TranslationKeys::NETWORK_SETTINGS] = "Paramètres réseau";
    translations[TranslationKeys::ENTER_NETWORK_PASSWORD] = "Entrez le mot de passe réseau";
    translations[TranslationKeys::IPV4_METHOD_LABEL] = "Méthode IPv4:";
    translations[TranslationKeys::IP_ADDRESS_LABEL] = "Adresse IP:";
    translations[TranslationKeys::NETMASK_LABEL] = "Masque réseau:";
    translations[TranslationKeys::GATEWAY_LABEL] = "Passerelle:";
    translations[TranslationKeys::DNS_SERVERS_LABEL] = "Serveurs DNS:";
    translations[TranslationKeys::IPV6_METHOD_LABEL] = "Méthode IPv6:";
    translations[TranslationKeys::IPV6_DNS_SERVERS_LABEL] = "Serveurs DNS IPv6:";
    translations[TranslationKeys::PORT_SETTINGS] = "Paramètres de port:";
    translations[TranslationKeys::HTTP_PROXY_PORT] = "Port proxy HTTP:";
    translations[TranslationKeys::HTTPS_PROXY_PORT] = "Port proxy HTTPS:";
    translations[TranslationKeys::FTP_PROXY_PORT] = "Port proxy FTP:";
    translations[TranslationKeys::SOCKS_PROXY_PORT] = "Port proxy SOCKS:";
    translations[TranslationKeys::CUSTOM_PORT_RANGE] = "Plage de ports personnalisée:";
    translations[TranslationKeys::PORT_DASH] = " - ";
    translations[TranslationKeys::CONNECTED_TO_PREFIX] = "Connecté à: ";
    translations[TranslationKeys::ETHERNET] = "Ethernet";
    translations[TranslationKeys::CONNECTING_STATUS] = "Connexion...";
    translations[TranslationKeys::NETWORKING_DISABLED_STATUS] = "Réseau désactivé";
    translations[TranslationKeys::WIFI_DISABLED_STATUS] = "Wi-Fi désactivé";
    translations[TranslationKeys::NOT_CONNECTED_STATUS] = "Non connecté";
    
    // Navigation Buttons
    translations[TranslationKeys::PREVIOUS_ARROW] = "‹";
    translations[TranslationKeys::NEXT_ARROW] = "›";
    translations[TranslationKeys::LEFT_ARROW] = "<";
    translations[TranslationKeys::RIGHT_ARROW] = ">";
    translations[TranslationKeys::ADD_BUTTON] = "+";
    translations[TranslationKeys::REMOVE_BUTTON] = "−";
    
    // Language Manager
    translations[TranslationKeys::LANGUAGE_CHANGED] = "Langue modifiée";
    
    // Display Manager
    translations[TranslationKeys::BRIGHTNESS_VALUE] = "50%";
    
    // Sound Manager
    translations[TranslationKeys::VOLUME_VALUE] = "0%";
    
    // Battery Manager Additional
    translations[TranslationKeys::POWER_DETAILS] = "DÉTAILS D'ÉNERGIE";
    translations[TranslationKeys::NO_BATTERY_DETECTED] = "Aucune batterie détectée";
    translations[TranslationKeys::CAPACITY] = "Capacité";
    translations[TranslationKeys::TECHNOLOGY] = "Technologie";
    translations[TranslationKeys::CHARGE_CYCLES] = "Cycles de Charge";
    translations[TranslationKeys::WARNING_LEVEL] = "Niveau d'Avertissement";
    translations[TranslationKeys::ENERGY_FULL] = "Énergie Pleine";
    translations[TranslationKeys::ENERGY_DESIGN] = "Énergie de Conception";
    translations[TranslationKeys::ENERGY_RATE] = "Taux d'Énergie";
    translations[TranslationKeys::VOLTAGE] = "Tension";
    
    // Power Manager Additional
    translations[TranslationKeys::PERFORMANCE_MODE] = "Performance";
    translations[TranslationKeys::BALANCED_MODE] = "Équilibré";
    translations[TranslationKeys::DEFAULT_MODE] = "Par Défaut";
    translations[TranslationKeys::PERFORMANCE_DESCRIPTION] = "Performance CPU maximale pour les tâches exigeantes";
    translations[TranslationKeys::BALANCED_DESCRIPTION] = "Équilibre optimisé entre performance et économie d'énergie";
    translations[TranslationKeys::DEFAULT_DESCRIPTION] = "Gestion d'énergie par défaut du système";

}
