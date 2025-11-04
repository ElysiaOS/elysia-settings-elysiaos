#include "id_ID.h"

void loadIndonesianTranslations(std::map<TranslationKeys, std::string>& translations) {
    // Main Window
    translations[TranslationKeys::WINDOW_TITLE] = "Pengaturan ElysiaOS";
    translations[TranslationKeys::EXIT] = "KELUAR";
    
    // Common UI
    translations[TranslationKeys::BACK] = "KEMBALI";
    translations[TranslationKeys::APPLY_CHANGES] = "TERAPKAN PERUBAHAN";
    translations[TranslationKeys::REFRESH] = "SEGARKAN";
    
    // Tile Names
    translations[TranslationKeys::TILE_CAR] = "Mobil";
    translations[TranslationKeys::TILE_BATTERY] = "Baterai";
    translations[TranslationKeys::TILE_APPEARANCE] = "Tampilan";
    translations[TranslationKeys::TILE_POWER] = "Daya";
    translations[TranslationKeys::TILE_BLUETOOTH] = "Bluetooth";
    translations[TranslationKeys::TILE_SOUND] = "Suara";
    translations[TranslationKeys::TILE_ABOUT] = "Tentang";
    translations[TranslationKeys::TILE_DISPLAY] = "Tampilan";
    translations[TranslationKeys::TILE_NETWORK] = "Jaringan";
    translations[TranslationKeys::TILE_UPDATES] = "Pembaruan";
    translations[TranslationKeys::TILE_HYPERLAND] = "Hyperland";
    translations[TranslationKeys::TILE_STORAGE] = "Penyimpanan";
    translations[TranslationKeys::TILE_SUPPORT] = "Dukungan";
    
    // About Manager
    translations[TranslationKeys::ABOUT_TITLE] = "TENTANG SISTEM";
    translations[TranslationKeys::SYSTEM_INFO] = "INFORMASI SISTEM";
    translations[TranslationKeys::OS_NAME] = "Sistem operasi";
    translations[TranslationKeys::KERNEL_VERSION] = "Versi kernel";
    translations[TranslationKeys::UPTIME] = "Waktu aktif sistem";
    translations[TranslationKeys::CPU_INFO] = "Prosesor";
    translations[TranslationKeys::MEMORY_INFO] = "Memori";
    translations[TranslationKeys::DISK_USAGE] = "Penggunaan disk";
    
    // Battery Manager
    translations[TranslationKeys::BATTERY_TITLE] = "STATUS BATERAI";
    translations[TranslationKeys::BATTERY_STATUS] = "Status baterai";
    translations[TranslationKeys::BATTERY_HEALTH] = "KESEHATAN BATERAI";
    translations[TranslationKeys::CHARGING] = "Mengisi";
    translations[TranslationKeys::DISCHARGING] = "Mengosongkan";
    translations[TranslationKeys::FULL] = "Penuh";
    translations[TranslationKeys::NOT_CHARGING] = "Tidak mengisi";
    
    // Display Manager
    translations[TranslationKeys::DISPLAY_TITLE] = "PENGATURAN TAMPILAN";
    translations[TranslationKeys::DISPLAY_SETTINGS] = "Pengaturan tampilan";
    translations[TranslationKeys::DISPLAY_RESOLUTION] = "RESOLUSI TAMPILAN";
    translations[TranslationKeys::SCREEN_BRIGHTNESS] = "KECERAHAN LAYAR";
    translations[TranslationKeys::DISPLAY_INFORMATION] = "INFORMASI TAMPILAN";
    translations[TranslationKeys::DISPLAY_INFO_TEXT] = "Perubahan akan diterapkan ke konfigurasi Hyprland Anda.\nMulai ulang Hyprland agar perubahan resolusi tampilan berlaku.";
    
    // Sound Manager
    translations[TranslationKeys::SOUND_TITLE] = "SUARA";
    translations[TranslationKeys::OUTPUT_DEVICES] = "PERANGKAT KELUARAN";
    translations[TranslationKeys::INPUT_DEVICES] = "PERANGKAT MASUKAN";
    translations[TranslationKeys::VOLUME] = "Volume";
    translations[TranslationKeys::PLAYBACK_APPLICATIONS] = "APLIKASI PEMUTAR";
    
    // Network Manager
    translations[TranslationKeys::NETWORK_TITLE] = "JARINGAN";
    translations[TranslationKeys::WIFI_NETWORKS] = "Jaringan WiFi";
    translations[TranslationKeys::WIFI] = "Wi-Fi";
    translations[TranslationKeys::CONNECTION_STATUS] = "Status koneksi";
    translations[TranslationKeys::CONNECT] = "Hubungkan";
    translations[TranslationKeys::DISCONNECT] = "Putuskan";
    translations[TranslationKeys::CONNECTED] = "Terhubung";
    translations[TranslationKeys::DISCONNECTED] = "Terputus";
    translations[TranslationKeys::ENTER_PASSWORD] = "Masukkan kata sandi";
    translations[TranslationKeys::CONNECTING] = "Menghubungkan...";
    translations[TranslationKeys::CANCEL] = "Batal";
    translations[TranslationKeys::REFRESH_NETWORKS] = "Segarkan jaringan";
    translations[TranslationKeys::NETWORKMANAGER_NOT_AVAILABLE] = "NetworkManager tidak tersedia";
    translations[TranslationKeys::NETWORKING_DISABLED] = "Jaringan dinonaktifkan";
    translations[TranslationKeys::ENABLE_NETWORKING] = "Aktifkan jaringan";
    translations[TranslationKeys::CONNECTED_VIA_ETHERNET] = "Terhubung via Ethernet";
    translations[TranslationKeys::WIFI_HARDWARE_DISABLED] = "Perangkat keras Wi-Fi dinonaktifkan";
    translations[TranslationKeys::WIFI_DISABLED] = "Wi-Fi dinonaktifkan";
    translations[TranslationKeys::NO_NETWORKS_FOUND] = "Tidak ada jaringan ditemukan";
    translations[TranslationKeys::IPV4_METHOD] = "Metode IPv4:";
    translations[TranslationKeys::IP_ADDRESS] = "Alamat IP:";
    translations[TranslationKeys::NETMASK] = "Netmask:";
    translations[TranslationKeys::GATEWAY] = "Gateway:";
    translations[TranslationKeys::DNS_SERVERS] = "Server DNS:";
    translations[TranslationKeys::IPV6_METHOD] = "Metode IPv6:";
    translations[TranslationKeys::IPV6_DNS_SERVERS] = "Server DNS IPv6:";
    
    // Bluetooth Manager
    translations[TranslationKeys::BLUETOOTH_TITLE] = "BLUETOOTH";
    translations[TranslationKeys::BLUETOOTH_DEVICES] = "Perangkat Bluetooth";
    translations[TranslationKeys::OK] = "Ok";
    translations[TranslationKeys::DEVICES] = "PERANGKAT";
    translations[TranslationKeys::PAIR] = "Pasangkan";
    translations[TranslationKeys::UNPAIR] = "Lepas pasangan";
    translations[TranslationKeys::PAIRED] = "Dipasangkan";
    translations[TranslationKeys::AVAILABLE] = "Tersedia";
    translations[TranslationKeys::TOGGLE_BLUETOOTH] = "Alihkan Bluetooth";
    translations[TranslationKeys::SCAN_DEVICES] = "Pindai perangkat";
    translations[TranslationKeys::BLUETOOTH_DISABLED] = "Bluetooth dinonaktifkan";
    translations[TranslationKeys::NO_DEVICES_FOUND_BT] = "Tidak ada perangkat ditemukan - coba pindai";
    translations[TranslationKeys::SCANNING] = "Memindai...";
    translations[TranslationKeys::SCANNING_FOR_DEVICES] = "Mencari perangkat...";
    translations[TranslationKeys::BLUETOOTH_ENABLED_NO_DEVICES] = "Bluetooth aktif - Tidak ada perangkat terhubung";
    translations[TranslationKeys::CONNECTED_TO] = "Terhubung ke: ";
    translations[TranslationKeys::CONNECTED_TO_MULTIPLE] = "Terhubung ke ";
    translations[TranslationKeys::TURN_OFF] = "Matikan";
    translations[TranslationKeys::TURN_ON] = "Nyalakan";
    translations[TranslationKeys::DISCONNECT_BLU] = "Putuskan";
    translations[TranslationKeys::CONNECT_BLU] = "Hubungkan";
    translations[TranslationKeys::FORGET] = "Lupakan";
    translations[TranslationKeys::SUCCESS_BLU] = "Berhasil";
    translations[TranslationKeys::DISCONNECTED_BLU] = "Terputus";
    translations[TranslationKeys::REMOVED] = "Dihapus";
    translations[TranslationKeys::CONNECTED_STATUS] = " - Terhubung";
    translations[TranslationKeys::PAIRED_STATUS] = " - Dipasangkan";
    translations[TranslationKeys::NOT_PAIRED_STATUS] = " - Tidak dipasangkan";
    
    // Appearance Manager
    translations[TranslationKeys::APPEARANCE_TITLE] = "TAMPILAN";
    translations[TranslationKeys::THEME_SETTINGS] = "Pengaturan tema";
    translations[TranslationKeys::THEME_SELECTION] = "PILIHAN TEMA";
    translations[TranslationKeys::WALLPAPER] = "Wallpaper";
    translations[TranslationKeys::WALLPAPER_SELECTION] = "PILIHAN WALLPAPER";
    translations[TranslationKeys::COLORS] = "Warna";
    translations[TranslationKeys::LIGHT] = "Terang";
    translations[TranslationKeys::DARK] = "Gelap";
    translations[TranslationKeys::BROWSE] = "JELAJAHI";
    translations[TranslationKeys::PREVIOUS_WALLPAPER] = "Wallpaper sebelumnya";
    translations[TranslationKeys::NEXT_WALLPAPER] = "Wallpaper berikutnya";
    translations[TranslationKeys::OPEN_WALLPAPER_FOLDER] = "Buka folder wallpaper";
    translations[TranslationKeys::SET_THIS_WALLPAPER] = "Atur wallpaper ini";
    
    // Power Manager
    translations[TranslationKeys::POWER_TITLE] = "DAYA";
    translations[TranslationKeys::POWER_SETTINGS] = "Pengaturan daya";
    translations[TranslationKeys::SLEEP] = "Tidur";
    translations[TranslationKeys::HIBERNATE] = "Hibernasi";
    translations[TranslationKeys::SHUTDOWN] = "Matikan";
    translations[TranslationKeys::RESTART] = "Mulai ulang";
    translations[TranslationKeys::PERFORMANCE] = "PERFORMA";
    translations[TranslationKeys::BALANCED] = "SEIMBANG";
    translations[TranslationKeys::DEFAULT] = "BAWAAN";
    translations[TranslationKeys::POWER_PROFILE_INFORMATION] = "INFORMASI PROFIL DAYA";
    translations[TranslationKeys::POWER_PROFILE_INFO_TEXT] = "Perubahan memerlukan hak administrator dan akan diterapkan segera.\nProfil daya saat ini akan terdeteksi otomatis saat membuka menu ini.";
    translations[TranslationKeys::MISSING] = "HILANG";
    
    // Storage Manager
    translations[TranslationKeys::STORAGE_TITLE] = "PENYIMPANAN";
    translations[TranslationKeys::DISK_USAGE_TITLE] = "Penggunaan disk";
    translations[TranslationKeys::AVAILABLE_SPACE] = "Ruang tersedia";
    translations[TranslationKeys::USED_SPACE] = "Ruang terpakai";
    
    // Applications Manager
    translations[TranslationKeys::APPLICATIONS_TITLE] = "APLIKASI HYPRLAND";
    translations[TranslationKeys::KEYBINDS] = "SHORTCUT TOMBOL";
    translations[TranslationKeys::AUTORUNS] = "AUTORUN";
    translations[TranslationKeys::WINDOW_RULES] = "ATURAN JENDELA";
    
    // Language Manager
    translations[TranslationKeys::LANGUAGE_SETTINGS] = "PENGATURAN BAHASA";
    translations[TranslationKeys::SYSTEM_LANGUAGE] = "BAHASA SISTEM";
    translations[TranslationKeys::LANGUAGE_INFORMATION] = "INFORMASI BAHASA";
    translations[TranslationKeys::LANGUAGE_INFO_TEXT] = "Perubahan akan diterapkan setelah logout.\nSesi Anda perlu dimulai ulang.";
    
    // Messages
    translations[TranslationKeys::CHECKING_STATUS] = "Memeriksa status...";
    translations[TranslationKeys::CHECKING_BATTERY_STATUS] = "Memeriksa status baterai...";
    translations[TranslationKeys::LOADING] = "Memuat...";
    translations[TranslationKeys::ERROR] = "Kesalahan";
    translations[TranslationKeys::SUCCESS] = "Berhasil";
    translations[TranslationKeys::WARNING] = "Peringatan";
    translations[TranslationKeys::NO_SYSTEM_INFO] = "Tidak ada informasi sistem tersedia";
    translations[TranslationKeys::OUTPUT_DEVICES] = "Perangkat keluaran";
    translations[TranslationKeys::INPUT_DEVICES] = "Perangkat masukan";
    translations[TranslationKeys::APPLICATION_VOLUME] = "Volume aplikasi";
    translations[TranslationKeys::NO_AUDIO_APPS] = "Tidak ada aplikasi yang sedang memutar audio";
    
    // Dialog Messages
    translations[TranslationKeys::LOGOUT_REQUIRED] = "Logout diperlukan";
    translations[TranslationKeys::LOGOUT_MESSAGE] = "Perubahan bahasa memerlukan logout untuk berlaku.\nApakah Anda ingin logout sekarang?";
    translations[TranslationKeys::LOGOUT_NOW] = "Logout sekarang";
    translations[TranslationKeys::LOGOUT_LATER] = "Nanti";

    // Network Manager Additional
    translations[TranslationKeys::NETWORK_SETTINGS] = "Pengaturan Jaringan";
    translations[TranslationKeys::ENTER_NETWORK_PASSWORD] = "Masukkan Kata Sandi Jaringan";
    translations[TranslationKeys::IPV4_METHOD_LABEL] = "Metode IPv4:";
    translations[TranslationKeys::IP_ADDRESS_LABEL] = "Alamat IP:";
    translations[TranslationKeys::NETMASK_LABEL] = "Netmask:";
    translations[TranslationKeys::GATEWAY_LABEL] = "Gateway:";
    translations[TranslationKeys::DNS_SERVERS_LABEL] = "Server DNS:";
    translations[TranslationKeys::IPV6_METHOD_LABEL] = "Metode IPv6:";
    translations[TranslationKeys::IPV6_DNS_SERVERS_LABEL] = "Server DNS IPv6:";
    translations[TranslationKeys::PORT_SETTINGS] = "Pengaturan Port:";
    translations[TranslationKeys::HTTP_PROXY_PORT] = "Port Proxy HTTP:";
    translations[TranslationKeys::HTTPS_PROXY_PORT] = "Port Proxy HTTPS:";
    translations[TranslationKeys::FTP_PROXY_PORT] = "Port Proxy FTP:";
    translations[TranslationKeys::SOCKS_PROXY_PORT] = "Port Proxy SOCKS:";
    translations[TranslationKeys::CUSTOM_PORT_RANGE] = "Rentang Port Kustom:";
    translations[TranslationKeys::PORT_DASH] = " - ";
    translations[TranslationKeys::CONNECTED_TO_PREFIX] = "Terhubung ke: ";
    translations[TranslationKeys::ETHERNET] = "Ethernet";
    translations[TranslationKeys::CONNECTING_STATUS] = "Menghubungkan...";
    translations[TranslationKeys::NETWORKING_DISABLED_STATUS] = "Jaringan dinonaktifkan";
    translations[TranslationKeys::WIFI_DISABLED_STATUS] = "Wi-Fi dinonaktifkan";
    translations[TranslationKeys::NOT_CONNECTED_STATUS] = "Tidak terhubung";
    
    // Navigation Buttons
    translations[TranslationKeys::PREVIOUS_ARROW] = "‹";
    translations[TranslationKeys::NEXT_ARROW] = "›";
    translations[TranslationKeys::LEFT_ARROW] = "<";
    translations[TranslationKeys::RIGHT_ARROW] = ">";
    translations[TranslationKeys::ADD_BUTTON] = "+";
    translations[TranslationKeys::REMOVE_BUTTON] = "−";
    
    // Language Manager
    translations[TranslationKeys::LANGUAGE_CHANGED] = "Bahasa Diubah";
    
    // Display Manager
    translations[TranslationKeys::BRIGHTNESS_VALUE] = "50%";
    
    // Sound Manager
    translations[TranslationKeys::VOLUME_VALUE] = "0%";
    
    // Battery Manager Additional
    translations[TranslationKeys::POWER_DETAILS] = "DETAIL DAYA";
    translations[TranslationKeys::NO_BATTERY_DETECTED] = "Tidak ada baterai terdeteksi";
    translations[TranslationKeys::CAPACITY] = "Kapasitas";
    translations[TranslationKeys::TECHNOLOGY] = "Teknologi";
    translations[TranslationKeys::CHARGE_CYCLES] = "Siklus Pengisian";
    translations[TranslationKeys::WARNING_LEVEL] = "Tingkat Peringatan";
    translations[TranslationKeys::ENERGY_FULL] = "Energi Penuh";
    translations[TranslationKeys::ENERGY_DESIGN] = "Energi Desain";
    translations[TranslationKeys::ENERGY_RATE] = "Tingkat Energi";
    translations[TranslationKeys::VOLTAGE] = "Tegangan";
    
    // Power Manager Additional
    translations[TranslationKeys::PERFORMANCE_MODE] = "Performa";
    translations[TranslationKeys::BALANCED_MODE] = "Seimbang";
    translations[TranslationKeys::DEFAULT_MODE] = "Default";
    translations[TranslationKeys::PERFORMANCE_DESCRIPTION] = "Performa CPU maksimal untuk tugas yang menuntut";
    translations[TranslationKeys::BALANCED_DESCRIPTION] = "Keseimbangan optimal antara performa dan penghematan daya";
    translations[TranslationKeys::DEFAULT_DESCRIPTION] = "Manajemen daya default sistem";

}
