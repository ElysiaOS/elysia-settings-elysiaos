#include "es_ES.h"

void loadSpanishTranslations(std::map<TranslationKeys, std::string>& translations) {
    // Main Window
    translations[TranslationKeys::WINDOW_TITLE] = "Configuración ElysiaOS";
    translations[TranslationKeys::EXIT] = "SALIR";
    
    // Common UI
    translations[TranslationKeys::BACK] = "ATRÁS";
    translations[TranslationKeys::APPLY_CHANGES] = "APLICAR CAMBIOS";
    translations[TranslationKeys::REFRESH] = "ACTUALIZAR";
    
    // Tile Names
    translations[TranslationKeys::TILE_CAR] = "Coche";
    translations[TranslationKeys::TILE_BATTERY] = "Batería";
    translations[TranslationKeys::TILE_APPEARANCE] = "Apariencia";
    translations[TranslationKeys::TILE_POWER] = "Energía";
    translations[TranslationKeys::TILE_BLUETOOTH] = "Bluetooth";
    translations[TranslationKeys::TILE_SOUND] = "Sonido";
    translations[TranslationKeys::TILE_ABOUT] = "Acerca de";
    translations[TranslationKeys::TILE_DISPLAY] = "Pantalla";
    translations[TranslationKeys::TILE_NETWORK] = "Red";
    translations[TranslationKeys::TILE_UPDATES] = "Actualizaciones";
    translations[TranslationKeys::TILE_HYPERLAND] = "Hyperland";
    translations[TranslationKeys::TILE_STORAGE] = "Almacenamiento";
    translations[TranslationKeys::TILE_SUPPORT] = "Soporte";
    
    // About Manager
    translations[TranslationKeys::ABOUT_TITLE] = "ACERCA DEL SISTEMA";
    translations[TranslationKeys::SYSTEM_INFO] = "INFORMACIÓN DEL SISTEMA";
    translations[TranslationKeys::OS_NAME] = "Sistema operativo";
    translations[TranslationKeys::KERNEL_VERSION] = "Versión del kernel";
    translations[TranslationKeys::UPTIME] = "Tiempo de actividad";
    translations[TranslationKeys::CPU_INFO] = "Procesador";
    translations[TranslationKeys::MEMORY_INFO] = "Memoria";
    translations[TranslationKeys::DISK_USAGE] = "Uso del disco";
    
    // Battery Manager
    translations[TranslationKeys::BATTERY_TITLE] = "ESTADO DE LA BATERÍA";
    translations[TranslationKeys::BATTERY_STATUS] = "Estado de la batería";
    translations[TranslationKeys::BATTERY_HEALTH] = "SALUD DE LA BATERÍA";
    translations[TranslationKeys::CHARGING] = "Cargando";
    translations[TranslationKeys::DISCHARGING] = "Descargando";
    translations[TranslationKeys::FULL] = "Completa";
    translations[TranslationKeys::NOT_CHARGING] = "No cargando";
    
    // Display Manager
    translations[TranslationKeys::DISPLAY_TITLE] = "CONFIGURACIÓN DE PANTALLA";
    translations[TranslationKeys::DISPLAY_SETTINGS] = "Configuración de pantalla";
    translations[TranslationKeys::DISPLAY_RESOLUTION] = "RESOLUCIÓN DE PANTALLA";
    translations[TranslationKeys::SCREEN_BRIGHTNESS] = "BRILLO DE PANTALLA";
    translations[TranslationKeys::DISPLAY_INFORMATION] = "INFORMACIÓN DE PANTALLA";
    translations[TranslationKeys::DISPLAY_INFO_TEXT] = "Los cambios se aplicarán a su configuración de Hyprland.\nReinicie Hyprland para que los cambios de resolución de pantalla surtan efecto.";
    
    // Sound Manager
    translations[TranslationKeys::SOUND_TITLE] = "SONIDO";
    translations[TranslationKeys::OUTPUT_DEVICES] = "DISPOSITIVOS DE SALIDA";
    translations[TranslationKeys::INPUT_DEVICES] = "DISPOSITIVOS DE ENTRADA";
    translations[TranslationKeys::VOLUME] = "Volumen";
    translations[TranslationKeys::PLAYBACK_APPLICATIONS] = "APLICACIONES DE REPRODUCCIÓN";
    
    // Network Manager
    translations[TranslationKeys::NETWORK_TITLE] = "RED";
    translations[TranslationKeys::WIFI_NETWORKS] = "Redes WiFi";
    translations[TranslationKeys::WIFI] = "Wi-Fi";
    translations[TranslationKeys::CONNECTION_STATUS] = "Estado de conexión";
    translations[TranslationKeys::CONNECT] = "Conectar";
    translations[TranslationKeys::DISCONNECT] = "Desconectar";
    translations[TranslationKeys::CONNECTED] = "Conectado";
    translations[TranslationKeys::DISCONNECTED] = "Desconectado";
    translations[TranslationKeys::ENTER_PASSWORD] = "Ingresar contraseña";
    translations[TranslationKeys::CONNECTING] = "Conectando...";
    translations[TranslationKeys::CANCEL] = "Cancelar";
    translations[TranslationKeys::REFRESH_NETWORKS] = "Actualizar redes";
    translations[TranslationKeys::NETWORKMANAGER_NOT_AVAILABLE] = "NetworkManager no disponible";
    translations[TranslationKeys::NETWORKING_DISABLED] = "La red está deshabilitada";
    translations[TranslationKeys::ENABLE_NETWORKING] = "Habilitar red";
    translations[TranslationKeys::CONNECTED_VIA_ETHERNET] = "Conectado vía Ethernet";
    translations[TranslationKeys::WIFI_HARDWARE_DISABLED] = "El hardware Wi-Fi está deshabilitado";
    translations[TranslationKeys::WIFI_DISABLED] = "Wi-Fi está deshabilitado";
    translations[TranslationKeys::NO_NETWORKS_FOUND] = "No se encontraron redes";
    translations[TranslationKeys::IPV4_METHOD] = "Método IPv4:";
    translations[TranslationKeys::IP_ADDRESS] = "Dirección IP:";
    translations[TranslationKeys::NETMASK] = "Máscara de red:";
    translations[TranslationKeys::GATEWAY] = "Puerta de enlace:";
    translations[TranslationKeys::DNS_SERVERS] = "Servidores DNS:";
    translations[TranslationKeys::IPV6_METHOD] = "Método IPv6:";
    translations[TranslationKeys::IPV6_DNS_SERVERS] = "Servidores DNS IPv6:";
    
    // Bluetooth Manager
    translations[TranslationKeys::BLUETOOTH_TITLE] = "BLUETOOTH";
    translations[TranslationKeys::BLUETOOTH_DEVICES] = "Dispositivos Bluetooth";
    translations[TranslationKeys::OK] = "Ok";
    translations[TranslationKeys::DEVICES] = "DISPOSITIVOS";
    translations[TranslationKeys::PAIR] = "Emparejar";
    translations[TranslationKeys::UNPAIR] = "Desemparejar";
    translations[TranslationKeys::PAIRED] = "Emparejado";
    translations[TranslationKeys::AVAILABLE] = "Disponible";
    translations[TranslationKeys::TOGGLE_BLUETOOTH] = "Alternar Bluetooth";
    translations[TranslationKeys::SCAN_DEVICES] = "Escanear dispositivos";
    translations[TranslationKeys::BLUETOOTH_DISABLED] = "Bluetooth está deshabilitado";
    translations[TranslationKeys::NO_DEVICES_FOUND_BT] = "No se encontraron dispositivos - intente escanear";
    translations[TranslationKeys::SCANNING] = "Escaneando...";
    translations[TranslationKeys::SCANNING_FOR_DEVICES] = "Buscando dispositivos...";
    translations[TranslationKeys::BLUETOOTH_ENABLED_NO_DEVICES] = "Bluetooth habilitado - Sin dispositivos conectados";
    translations[TranslationKeys::CONNECTED_TO] = "Conectado a: ";
    translations[TranslationKeys::CONNECTED_TO_MULTIPLE] = "Conectado a ";
    translations[TranslationKeys::TURN_OFF] = "Apagar";
    translations[TranslationKeys::TURN_ON] = "Encender";
    translations[TranslationKeys::DISCONNECT_BLU] = "Desconectar";
    translations[TranslationKeys::CONNECT_BLU] = "Conectar";
    translations[TranslationKeys::FORGET] = "Olvidar";
    translations[TranslationKeys::SUCCESS_BLU] = "Éxito";
    translations[TranslationKeys::DISCONNECTED_BLU] = "Desconectado";
    translations[TranslationKeys::REMOVED] = "Eliminado";
    translations[TranslationKeys::CONNECTED_STATUS] = " - Conectado";
    translations[TranslationKeys::PAIRED_STATUS] = " - Emparejado";
    translations[TranslationKeys::NOT_PAIRED_STATUS] = " - No emparejado";
    
    // Appearance Manager
    translations[TranslationKeys::APPEARANCE_TITLE] = "APARIENCIA";
    translations[TranslationKeys::THEME_SETTINGS] = "Configuración del tema";
    translations[TranslationKeys::THEME_SELECTION] = "SELECCIÓN DE TEMA";
    translations[TranslationKeys::WALLPAPER] = "Fondo de pantalla";
    translations[TranslationKeys::WALLPAPER_SELECTION] = "SELECCIÓN DE FONDO";
    translations[TranslationKeys::COLORS] = "Colores";
    translations[TranslationKeys::LIGHT] = "Claro";
    translations[TranslationKeys::DARK] = "Oscuro";
    translations[TranslationKeys::BROWSE] = "EXAMINAR";
    translations[TranslationKeys::PREVIOUS_WALLPAPER] = "Fondo anterior";
    translations[TranslationKeys::NEXT_WALLPAPER] = "Siguiente fondo";
    translations[TranslationKeys::OPEN_WALLPAPER_FOLDER] = "Abrir carpeta de fondos";
    translations[TranslationKeys::SET_THIS_WALLPAPER] = "Establecer este fondo";
    
    // Power Manager
    translations[TranslationKeys::POWER_TITLE] = "ENERGÍA";
    translations[TranslationKeys::POWER_SETTINGS] = "Configuración de energía";
    translations[TranslationKeys::SLEEP] = "Suspender";
    translations[TranslationKeys::HIBERNATE] = "Hibernar";
    translations[TranslationKeys::SHUTDOWN] = "Apagar";
    translations[TranslationKeys::RESTART] = "Reiniciar";
    translations[TranslationKeys::PERFORMANCE] = "RENDIMIENTO";
    translations[TranslationKeys::BALANCED] = "EQUILIBRADO";
    translations[TranslationKeys::DEFAULT] = "PREDETERMINADO";
    translations[TranslationKeys::POWER_PROFILE_INFORMATION] = "INFORMACIÓN DEL PERFIL DE ENERGÍA";
    translations[TranslationKeys::POWER_PROFILE_INFO_TEXT] = "Los cambios requieren privilegios de administrador y se aplicarán inmediatamente.\nEl perfil de energía actual se detectará automáticamente al abrir este menú.";
    translations[TranslationKeys::MISSING] = "FALTANTE";
    
    // Storage Manager
    translations[TranslationKeys::STORAGE_TITLE] = "ALMACENAMIENTO";
    translations[TranslationKeys::DISK_USAGE_TITLE] = "Uso del disco";
    translations[TranslationKeys::AVAILABLE_SPACE] = "Espacio disponible";
    translations[TranslationKeys::USED_SPACE] = "Espacio usado";
    
    // Applications Manager
    translations[TranslationKeys::APPLICATIONS_TITLE] = "APLICACIONES HYPRLAND";
    translations[TranslationKeys::KEYBINDS] = "ATAJOS DE TECLADO";
    translations[TranslationKeys::AUTORUNS] = "AUTOEJECUTAR";
    translations[TranslationKeys::WINDOW_RULES] = "REGLAS DE VENTANA";
    
    // Language Manager
    translations[TranslationKeys::LANGUAGE_SETTINGS] = "CONFIGURACIÓN DE IDIOMA";
    translations[TranslationKeys::SYSTEM_LANGUAGE] = "IDIOMA DEL SISTEMA";
    translations[TranslationKeys::LANGUAGE_INFORMATION] = "INFORMACIÓN DE IDIOMA";
    translations[TranslationKeys::LANGUAGE_INFO_TEXT] = "Los cambios se aplicarán después del cierre de sesión.\nSu sesión necesitará ser reiniciada.";
    
    // Messages
    translations[TranslationKeys::CHECKING_STATUS] = "Verificando estado...";
    translations[TranslationKeys::CHECKING_BATTERY_STATUS] = "Verificando estado de la batería...";
    translations[TranslationKeys::LOADING] = "Cargando...";
    translations[TranslationKeys::ERROR] = "Error";
    translations[TranslationKeys::SUCCESS] = "Éxito";
    translations[TranslationKeys::WARNING] = "Advertencia";
    translations[TranslationKeys::NO_SYSTEM_INFO] = "No hay información del sistema disponible";
    translations[TranslationKeys::OUTPUT_DEVICES] = "Dispositivos de salida";
    translations[TranslationKeys::INPUT_DEVICES] = "Dispositivos de entrada";
    translations[TranslationKeys::APPLICATION_VOLUME] = "Volumen de aplicación";
    translations[TranslationKeys::NO_AUDIO_APPS] = "No hay aplicaciones reproduciendo audio actualmente";
    
    // Dialog Messages
    translations[TranslationKeys::LOGOUT_REQUIRED] = "Cierre de sesión requerido";
    translations[TranslationKeys::LOGOUT_MESSAGE] = "Los cambios de idioma requieren un cierre de sesión para surtir efecto.\n¿Desea cerrar sesión ahora?";
    translations[TranslationKeys::LOGOUT_NOW] = "Cerrar sesión ahora";
    translations[TranslationKeys::LOGOUT_LATER] = "Más tarde";

    // Network Manager Additional
    translations[TranslationKeys::NETWORK_SETTINGS] = "Configuración de red";
    translations[TranslationKeys::ENTER_NETWORK_PASSWORD] = "Ingrese contraseña de red";
    translations[TranslationKeys::IPV4_METHOD_LABEL] = "Método IPv4:";
    translations[TranslationKeys::IP_ADDRESS_LABEL] = "Dirección IP:";
    translations[TranslationKeys::NETMASK_LABEL] = "Máscara de red:";
    translations[TranslationKeys::GATEWAY_LABEL] = "Puerta de enlace:";
    translations[TranslationKeys::DNS_SERVERS_LABEL] = "Servidores DNS:";
    translations[TranslationKeys::IPV6_METHOD_LABEL] = "Método IPv6:";
    translations[TranslationKeys::IPV6_DNS_SERVERS_LABEL] = "Servidores DNS IPv6:";
    translations[TranslationKeys::PORT_SETTINGS] = "Configuración de puertos:";
    translations[TranslationKeys::HTTP_PROXY_PORT] = "Puerto proxy HTTP:";
    translations[TranslationKeys::HTTPS_PROXY_PORT] = "Puerto proxy HTTPS:";
    translations[TranslationKeys::FTP_PROXY_PORT] = "Puerto proxy FTP:";
    translations[TranslationKeys::SOCKS_PROXY_PORT] = "Puerto proxy SOCKS:";
    translations[TranslationKeys::CUSTOM_PORT_RANGE] = "Rango de puertos personalizado:";
    translations[TranslationKeys::PORT_DASH] = " - ";
    translations[TranslationKeys::CONNECTED_TO_PREFIX] = "Conectado a: ";
    translations[TranslationKeys::ETHERNET] = "Ethernet";
    translations[TranslationKeys::CONNECTING_STATUS] = "Conectando...";
    translations[TranslationKeys::NETWORKING_DISABLED_STATUS] = "Red deshabilitada";
    translations[TranslationKeys::WIFI_DISABLED_STATUS] = "Wi-Fi deshabilitado";
    translations[TranslationKeys::NOT_CONNECTED_STATUS] = "No conectado";
    
    // Navigation Buttons
    translations[TranslationKeys::PREVIOUS_ARROW] = "‹";
    translations[TranslationKeys::NEXT_ARROW] = "›";
    translations[TranslationKeys::LEFT_ARROW] = "<";
    translations[TranslationKeys::RIGHT_ARROW] = ">";
    translations[TranslationKeys::ADD_BUTTON] = "+";
    translations[TranslationKeys::REMOVE_BUTTON] = "−";
    
    // Language Manager
    translations[TranslationKeys::LANGUAGE_CHANGED] = "Idioma cambiado";
    
    // Display Manager
    translations[TranslationKeys::BRIGHTNESS_VALUE] = "50%";
    
    // Sound Manager
    translations[TranslationKeys::VOLUME_VALUE] = "0%";
    
    // Battery Manager Additional
    translations[TranslationKeys::POWER_DETAILS] = "DETALLES DE ENERGÍA";
    translations[TranslationKeys::NO_BATTERY_DETECTED] = "No se detectó batería";
    translations[TranslationKeys::CAPACITY] = "Capacidad";
    translations[TranslationKeys::TECHNOLOGY] = "Tecnología";
    translations[TranslationKeys::CHARGE_CYCLES] = "Ciclos de Carga";
    translations[TranslationKeys::WARNING_LEVEL] = "Nivel de Advertencia";
    translations[TranslationKeys::ENERGY_FULL] = "Energía Completa";
    translations[TranslationKeys::ENERGY_DESIGN] = "Energía de Diseño";
    translations[TranslationKeys::ENERGY_RATE] = "Tasa de Energía";
    translations[TranslationKeys::VOLTAGE] = "Voltaje";
    
    // Power Manager Additional
    translations[TranslationKeys::PERFORMANCE_MODE] = "Rendimiento";
    translations[TranslationKeys::BALANCED_MODE] = "Equilibrado";
    translations[TranslationKeys::DEFAULT_MODE] = "Predeterminado";
    translations[TranslationKeys::PERFORMANCE_DESCRIPTION] = "Máximo rendimiento de CPU para tareas exigentes";
    translations[TranslationKeys::BALANCED_DESCRIPTION] = "Equilibrio optimizado entre rendimiento y ahorro de energía";
    translations[TranslationKeys::DEFAULT_DESCRIPTION] = "Gestión de energía predeterminada del sistema";

}
