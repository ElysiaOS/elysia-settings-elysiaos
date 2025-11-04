#include "ru_RU.h"

void loadRussianTranslations(std::map<TranslationKeys, std::string>& translations) {
    // Main Window
    translations[TranslationKeys::WINDOW_TITLE] = "Настройки ElysiaOS";
    translations[TranslationKeys::EXIT] = "ВЫХОД";
    
    // Common UI
    translations[TranslationKeys::BACK] = "НАЗАД";
    translations[TranslationKeys::APPLY_CHANGES] = "ПРИМЕНИТЬ ИЗМЕНЕНИЯ";
    translations[TranslationKeys::REFRESH] = "ОБНОВИТЬ";
    
    // Tile Names
    translations[TranslationKeys::TILE_CAR] = "Автомобиль";
    translations[TranslationKeys::TILE_BATTERY] = "Батарея";
    translations[TranslationKeys::TILE_APPEARANCE] = "Внешний вид";
    translations[TranslationKeys::TILE_POWER] = "Питание";
    translations[TranslationKeys::TILE_BLUETOOTH] = "Bluetooth";
    translations[TranslationKeys::TILE_SOUND] = "Звук";
    translations[TranslationKeys::TILE_ABOUT] = "О системе";
    translations[TranslationKeys::TILE_DISPLAY] = "Дисплей";
    translations[TranslationKeys::TILE_NETWORK] = "Сеть";
    translations[TranslationKeys::TILE_UPDATES] = "Обновления";
    translations[TranslationKeys::TILE_HYPERLAND] = "Hyperland";
    translations[TranslationKeys::TILE_STORAGE] = "Хранилище";
    translations[TranslationKeys::TILE_SUPPORT] = "Поддержка";
    
    // About Manager
    translations[TranslationKeys::ABOUT_TITLE] = "О СИСТЕМЕ";
    translations[TranslationKeys::SYSTEM_INFO] = "ИНФОРМАЦИЯ О СИСТЕМЕ";
    translations[TranslationKeys::OS_NAME] = "Операционная система";
    translations[TranslationKeys::KERNEL_VERSION] = "Версия ядра";
    translations[TranslationKeys::UPTIME] = "Время работы системы";
    translations[TranslationKeys::CPU_INFO] = "Процессор";
    translations[TranslationKeys::MEMORY_INFO] = "Память";
    translations[TranslationKeys::DISK_USAGE] = "Использование диска";
    
    // Battery Manager
    translations[TranslationKeys::BATTERY_TITLE] = "СТАТУС БАТАРЕИ";
    translations[TranslationKeys::BATTERY_STATUS] = "Статус батареи";
    translations[TranslationKeys::BATTERY_HEALTH] = "СОСТОЯНИЕ БАТАРЕИ";
    translations[TranslationKeys::CHARGING] = "Зарядка";
    translations[TranslationKeys::DISCHARGING] = "Разрядка";
    translations[TranslationKeys::FULL] = "Полная";
    translations[TranslationKeys::NOT_CHARGING] = "Не заряжается";
    
    // Display Manager
    translations[TranslationKeys::DISPLAY_TITLE] = "НАСТРОЙКИ ДИСПЛЕЯ";
    translations[TranslationKeys::DISPLAY_SETTINGS] = "Настройки дисплея";
    translations[TranslationKeys::DISPLAY_RESOLUTION] = "РАЗРЕШЕНИЕ ДИСПЛЕЯ";
    translations[TranslationKeys::SCREEN_BRIGHTNESS] = "ЯРКОСТЬ ЭКРАНА";
    translations[TranslationKeys::DISPLAY_INFORMATION] = "ИНФОРМАЦИЯ О ДИСПЛЕЕ";
    translations[TranslationKeys::DISPLAY_INFO_TEXT] = "Изменения будут применены к вашей конфигурации Hyprland.\nПерезапустите Hyprland для применения изменений разрешения дисплея.";
    
    // Sound Manager
    translations[TranslationKeys::SOUND_TITLE] = "ЗВУК";
    translations[TranslationKeys::OUTPUT_DEVICES] = "УСТРОЙСТВА ВЫВОДА";
    translations[TranslationKeys::INPUT_DEVICES] = "УСТРОЙСТВА ВВОДА";
    translations[TranslationKeys::VOLUME] = "Громкость";
    translations[TranslationKeys::PLAYBACK_APPLICATIONS] = "ПРИЛОЖЕНИЯ ВОСПРОИЗВЕДЕНИЯ";
    
    // Network Manager
    translations[TranslationKeys::NETWORK_TITLE] = "СЕТЬ";
    translations[TranslationKeys::WIFI_NETWORKS] = "WiFi Сети";
    translations[TranslationKeys::WIFI] = "Wi-Fi";
    translations[TranslationKeys::CONNECTION_STATUS] = "Статус подключения";
    translations[TranslationKeys::CONNECT] = "Подключить";
    translations[TranslationKeys::DISCONNECT] = "Отключить";
    translations[TranslationKeys::CONNECTED] = "Подключено";
    translations[TranslationKeys::DISCONNECTED] = "Отключено";
    translations[TranslationKeys::ENTER_PASSWORD] = "Введите пароль";
    translations[TranslationKeys::CONNECTING] = "Подключение...";
    translations[TranslationKeys::CANCEL] = "Отмена";
    translations[TranslationKeys::REFRESH_NETWORKS] = "Обновить сети";
    translations[TranslationKeys::NETWORKMANAGER_NOT_AVAILABLE] = "NetworkManager недоступен";
    translations[TranslationKeys::NETWORKING_DISABLED] = "Сетевое подключение отключено";
    translations[TranslationKeys::ENABLE_NETWORKING] = "Включить сетевое подключение";
    translations[TranslationKeys::CONNECTED_VIA_ETHERNET] = "Подключено через Ethernet";
    translations[TranslationKeys::WIFI_HARDWARE_DISABLED] = "Wi-Fi оборудование отключено";
    translations[TranslationKeys::WIFI_DISABLED] = "Wi-Fi отключен";
    translations[TranslationKeys::NO_NETWORKS_FOUND] = "Сети не найдены";
    translations[TranslationKeys::IPV4_METHOD] = "Метод IPv4:";
    translations[TranslationKeys::IP_ADDRESS] = "IP-адрес:";
    translations[TranslationKeys::NETMASK] = "Маска сети:";
    translations[TranslationKeys::GATEWAY] = "Шлюз:";
    translations[TranslationKeys::DNS_SERVERS] = "DNS серверы:";
    translations[TranslationKeys::IPV6_METHOD] = "Метод IPv6:";
    translations[TranslationKeys::IPV6_DNS_SERVERS] = "IPv6 DNS серверы:";
    
    // Bluetooth Manager
    translations[TranslationKeys::BLUETOOTH_TITLE] = "BLUETOOTH";
    translations[TranslationKeys::BLUETOOTH_DEVICES] = "Bluetooth устройства";
    translations[TranslationKeys::OK] = "Ок";
    translations[TranslationKeys::DEVICES] = "УСТРОЙСТВА";
    translations[TranslationKeys::PAIR] = "Сопрячь";
    translations[TranslationKeys::UNPAIR] = "Разорвать сопряжение";
    translations[TranslationKeys::PAIRED] = "Сопряжено";
    translations[TranslationKeys::AVAILABLE] = "Доступно";
    translations[TranslationKeys::TOGGLE_BLUETOOTH] = "Переключить Bluetooth";
    translations[TranslationKeys::SCAN_DEVICES] = "Сканировать устройства";
    translations[TranslationKeys::BLUETOOTH_DISABLED] = "Bluetooth отключен";
    translations[TranslationKeys::NO_DEVICES_FOUND_BT] = "Устройства не найдены - попробуйте сканирование";
    translations[TranslationKeys::SCANNING] = "Сканирование...";
    translations[TranslationKeys::SCANNING_FOR_DEVICES] = "Поиск устройств...";
    translations[TranslationKeys::BLUETOOTH_ENABLED_NO_DEVICES] = "Bluetooth включен - Устройства не подключены";
    translations[TranslationKeys::CONNECTED_TO] = "Подключено к: ";
    translations[TranslationKeys::CONNECTED_TO_MULTIPLE] = "Подключено к ";
    translations[TranslationKeys::TURN_OFF] = "Выключить";
    translations[TranslationKeys::TURN_ON] = "Включить";
    translations[TranslationKeys::DISCONNECT_BLU] = "Отключить";
    translations[TranslationKeys::CONNECT_BLU] = "Подключить";
    translations[TranslationKeys::FORGET] = "Забыть";
    translations[TranslationKeys::SUCCESS_BLU] = "Успешно";
    translations[TranslationKeys::DISCONNECTED_BLU] = "Отключено";
    translations[TranslationKeys::REMOVED] = "Удалено";
    translations[TranslationKeys::CONNECTED_STATUS] = " - Подключено";
    translations[TranslationKeys::PAIRED_STATUS] = " - Сопряжено";
    translations[TranslationKeys::NOT_PAIRED_STATUS] = " - Не сопряжено";
    
    // Appearance Manager
    translations[TranslationKeys::APPEARANCE_TITLE] = "ВНЕШНИЙ ВИД";
    translations[TranslationKeys::THEME_SETTINGS] = "Настройки темы";
    translations[TranslationKeys::THEME_SELECTION] = "ВЫБОР ТЕМЫ";
    translations[TranslationKeys::WALLPAPER] = "Обои";
    translations[TranslationKeys::WALLPAPER_SELECTION] = "ВЫБОР ОБОЕВ";
    translations[TranslationKeys::COLORS] = "Цвета";
    translations[TranslationKeys::LIGHT] = "Светлая";
    translations[TranslationKeys::DARK] = "Темная";
    translations[TranslationKeys::BROWSE] = "ОБЗОР";
    translations[TranslationKeys::PREVIOUS_WALLPAPER] = "Предыдущие обои";
    translations[TranslationKeys::NEXT_WALLPAPER] = "Следующие обои";
    translations[TranslationKeys::OPEN_WALLPAPER_FOLDER] = "Открыть папку с обоями";
    translations[TranslationKeys::SET_THIS_WALLPAPER] = "Установить эти обои";
    
    // Power Manager
    translations[TranslationKeys::POWER_TITLE] = "ПИТАНИЕ";
    translations[TranslationKeys::POWER_SETTINGS] = "Настройки питания";
    translations[TranslationKeys::SLEEP] = "Сон";
    translations[TranslationKeys::HIBERNATE] = "Гибернация";
    translations[TranslationKeys::SHUTDOWN] = "Выключение";
    translations[TranslationKeys::RESTART] = "Перезагрузка";
    translations[TranslationKeys::PERFORMANCE] = "ПРОИЗВОДИТЕЛЬНОСТЬ";
    translations[TranslationKeys::BALANCED] = "СБАЛАНСИРОВАННЫЙ";
    translations[TranslationKeys::DEFAULT] = "ПО УМОЛЧАНИЮ";
    translations[TranslationKeys::POWER_PROFILE_INFORMATION] = "ИНФОРМАЦИЯ О ПРОФИЛЕ ПИТАНИЯ";
    translations[TranslationKeys::POWER_PROFILE_INFO_TEXT] = "Изменения требуют привилегий администратора и будут применены немедленно.\nТекущий профиль питания будет автоматически определен при открытии этого меню.";
    translations[TranslationKeys::MISSING] = "ОТСУТСТВУЕТ";
    
    // Storage Manager
    translations[TranslationKeys::STORAGE_TITLE] = "ХРАНИЛИЩЕ";
    translations[TranslationKeys::DISK_USAGE_TITLE] = "Использование диска";
    translations[TranslationKeys::AVAILABLE_SPACE] = "Доступное место";
    translations[TranslationKeys::USED_SPACE] = "Используемое место";
    
    // Applications Manager
    translations[TranslationKeys::APPLICATIONS_TITLE] = "ПРИЛОЖЕНИЯ HYPRLAND";
    translations[TranslationKeys::KEYBINDS] = "ГОРЯЧИЕ КЛАВИШИ";
    translations[TranslationKeys::AUTORUNS] = "АВТОЗАПУСК";
    translations[TranslationKeys::WINDOW_RULES] = "ПРАВИЛА ОКОН";
    
    // Language Manager
    translations[TranslationKeys::LANGUAGE_SETTINGS] = "НАСТРОЙКИ ЯЗЫКА";
    translations[TranslationKeys::SYSTEM_LANGUAGE] = "ЯЗЫК СИСТЕМЫ";
    translations[TranslationKeys::LANGUAGE_INFORMATION] = "ИНФОРМАЦИЯ О ЯЗЫКЕ";
    translations[TranslationKeys::LANGUAGE_INFO_TEXT] = "Изменения будут применены после выхода из системы.\nВаш сеанс потребует перезапуска.";
    
    // Messages
    translations[TranslationKeys::CHECKING_STATUS] = "Проверка статуса...";
    translations[TranslationKeys::CHECKING_BATTERY_STATUS] = "Проверка статуса батареи...";
    translations[TranslationKeys::LOADING] = "Загрузка...";
    translations[TranslationKeys::ERROR] = "Ошибка";
    translations[TranslationKeys::SUCCESS] = "Успешно";
    translations[TranslationKeys::WARNING] = "Предупреждение";
    translations[TranslationKeys::NO_SYSTEM_INFO] = "Информация о системе недоступна";
    translations[TranslationKeys::OUTPUT_DEVICES] = "Устройства вывода";
    translations[TranslationKeys::INPUT_DEVICES] = "Устройства ввода";
    translations[TranslationKeys::APPLICATION_VOLUME] = "Громкость приложения";
    translations[TranslationKeys::NO_AUDIO_APPS] = "Нет приложений, воспроизводящих аудио";
    
    // Dialog Messages
    translations[TranslationKeys::LOGOUT_REQUIRED] = "Требуется выход";
    translations[TranslationKeys::LOGOUT_MESSAGE] = "Изменения языка требуют выхода из системы для применения.\nХотите выйти сейчас?";
    translations[TranslationKeys::LOGOUT_NOW] = "Выйти сейчас";
    translations[TranslationKeys::LOGOUT_LATER] = "Позже";
    
    // Network Manager Additional
    translations[TranslationKeys::NETWORK_SETTINGS] = "Настройки сети";
    translations[TranslationKeys::ENTER_NETWORK_PASSWORD] = "Введите пароль сети";
    translations[TranslationKeys::IPV4_METHOD_LABEL] = "Метод IPv4:";
    translations[TranslationKeys::IP_ADDRESS_LABEL] = "IP-адрес:";
    translations[TranslationKeys::NETMASK_LABEL] = "Маска сети:";
    translations[TranslationKeys::GATEWAY_LABEL] = "Шлюз:";
    translations[TranslationKeys::DNS_SERVERS_LABEL] = "DNS-серверы:";
    translations[TranslationKeys::IPV6_METHOD_LABEL] = "Метод IPv6:";
    translations[TranslationKeys::IPV6_DNS_SERVERS_LABEL] = "DNS-серверы IPv6:";
    translations[TranslationKeys::PORT_SETTINGS] = "Настройки портов:";
    translations[TranslationKeys::HTTP_PROXY_PORT] = "Порт HTTP-прокси:";
    translations[TranslationKeys::HTTPS_PROXY_PORT] = "Порт HTTPS-прокси:";
    translations[TranslationKeys::FTP_PROXY_PORT] = "Порт FTP-прокси:";
    translations[TranslationKeys::SOCKS_PROXY_PORT] = "Порт SOCKS-прокси:";
    translations[TranslationKeys::CUSTOM_PORT_RANGE] = "Диапазон портов:";
    translations[TranslationKeys::PORT_DASH] = " - ";
    translations[TranslationKeys::CONNECTED_TO_PREFIX] = "Подключено к: ";
    translations[TranslationKeys::ETHERNET] = "Ethernet";
    translations[TranslationKeys::CONNECTING_STATUS] = "Подключение...";
    translations[TranslationKeys::NETWORKING_DISABLED_STATUS] = "Сеть отключена";
    translations[TranslationKeys::WIFI_DISABLED_STATUS] = "Wi-Fi отключен";
    translations[TranslationKeys::NOT_CONNECTED_STATUS] = "Не подключено";
    
    // Navigation Buttons
    translations[TranslationKeys::PREVIOUS_ARROW] = "‹";
    translations[TranslationKeys::NEXT_ARROW] = "›";
    translations[TranslationKeys::LEFT_ARROW] = "<";
    translations[TranslationKeys::RIGHT_ARROW] = ">";
    translations[TranslationKeys::ADD_BUTTON] = "+";
    translations[TranslationKeys::REMOVE_BUTTON] = "−";
    
    // Language Manager
    translations[TranslationKeys::LANGUAGE_CHANGED] = "Язык изменен";
    
    // Display Manager
    translations[TranslationKeys::BRIGHTNESS_VALUE] = "50%";
    
    // Sound Manager
    translations[TranslationKeys::VOLUME_VALUE] = "0%";
    
    // Battery Manager Additional
    translations[TranslationKeys::POWER_DETAILS] = "ДЕТАЛИ ПИТАНИЯ";
    translations[TranslationKeys::NO_BATTERY_DETECTED] = "Батарея не обнаружена";
    translations[TranslationKeys::CAPACITY] = "Емкость";
    translations[TranslationKeys::TECHNOLOGY] = "Технология";
    translations[TranslationKeys::CHARGE_CYCLES] = "Циклы Зарядки";
    translations[TranslationKeys::WARNING_LEVEL] = "Уровень Предупреждения";
    translations[TranslationKeys::ENERGY_FULL] = "Полная Энергия";
    translations[TranslationKeys::ENERGY_DESIGN] = "Расчетная Энергия";
    translations[TranslationKeys::ENERGY_RATE] = "Скорость Энергии";
    translations[TranslationKeys::VOLTAGE] = "Напряжение";
    
    // Power Manager Additional
    translations[TranslationKeys::PERFORMANCE_MODE] = "Производительность";
    translations[TranslationKeys::BALANCED_MODE] = "Сбалансированный";
    translations[TranslationKeys::DEFAULT_MODE] = "По Умолчанию";
    translations[TranslationKeys::PERFORMANCE_DESCRIPTION] = "Максимальная производительность CPU для требовательных задач";
    translations[TranslationKeys::BALANCED_DESCRIPTION] = "Оптимизированный баланс между производительностью и экономией энергии";
    translations[TranslationKeys::DEFAULT_DESCRIPTION] = "Системное управление питанием по умолчанию";
}
