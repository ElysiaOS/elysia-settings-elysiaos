#include "vi_VN.h"

void loadVietnameseTranslations(std::map<TranslationKeys, std::string>& translations) {
    // Main Window
    translations[TranslationKeys::WINDOW_TITLE] = "Cài đặt ElysiaOS";
    translations[TranslationKeys::EXIT] = "THOÁT";
    
    // Common UI
    translations[TranslationKeys::BACK] = "QUAY LẠI";
    translations[TranslationKeys::APPLY_CHANGES] = "ÁP DỤNG THAY ĐỔI";
    translations[TranslationKeys::REFRESH] = "LÀM MỚI";
    
    // Tile Names
    translations[TranslationKeys::TILE_CAR] = "Xe hơi";
    translations[TranslationKeys::TILE_BATTERY] = "Pin";
    translations[TranslationKeys::TILE_APPEARANCE] = "Giao diện";
    translations[TranslationKeys::TILE_POWER] = "Nguồn";
    translations[TranslationKeys::TILE_BLUETOOTH] = "Bluetooth";
    translations[TranslationKeys::TILE_SOUND] = "Âm thanh";
    translations[TranslationKeys::TILE_ABOUT] = "Giới thiệu";
    translations[TranslationKeys::TILE_DISPLAY] = "Màn hình";
    translations[TranslationKeys::TILE_NETWORK] = "Mạng";
    translations[TranslationKeys::TILE_UPDATES] = "Cập nhật";
    translations[TranslationKeys::TILE_HYPERLAND] = "Hyperland";
    translations[TranslationKeys::TILE_STORAGE] = "Lưu trữ";
    translations[TranslationKeys::TILE_SUPPORT] = "Hỗ trợ";
    
    // About Manager
    translations[TranslationKeys::ABOUT_TITLE] = "THÔNG TIN HỆ THỐNG";
    translations[TranslationKeys::SYSTEM_INFO] = "THÔNG TIN HỆ THỐNG";
    translations[TranslationKeys::OS_NAME] = "Hệ điều hành";
    translations[TranslationKeys::KERNEL_VERSION] = "Phiên bản kernel";
    translations[TranslationKeys::UPTIME] = "Thời gian hoạt động";
    translations[TranslationKeys::CPU_INFO] = "Bộ xử lý";
    translations[TranslationKeys::MEMORY_INFO] = "Bộ nhớ";
    translations[TranslationKeys::DISK_USAGE] = "Sử dụng ổ đĩa";
    
    // Battery Manager
    translations[TranslationKeys::BATTERY_TITLE] = "TRẠNG THÁI PIN";
    translations[TranslationKeys::BATTERY_STATUS] = "Trạng thái pin";
    translations[TranslationKeys::BATTERY_HEALTH] = "TÌNH TRẠNG PIN";
    translations[TranslationKeys::CHARGING] = "Đang sạc";
    translations[TranslationKeys::DISCHARGING] = "Đang xả";
    translations[TranslationKeys::FULL] = "Đầy";
    translations[TranslationKeys::NOT_CHARGING] = "Không sạc";
    
    // Display Manager
    translations[TranslationKeys::DISPLAY_TITLE] = "CÀI ĐẶT MÀN HÌNH";
    translations[TranslationKeys::DISPLAY_SETTINGS] = "Cài đặt màn hình";
    translations[TranslationKeys::DISPLAY_RESOLUTION] = "ĐỘ PHÂN GIẢI MÀN HÌNH";
    translations[TranslationKeys::SCREEN_BRIGHTNESS] = "ĐỘ SÁNG MÀN HÌNH";
    translations[TranslationKeys::DISPLAY_INFORMATION] = "THÔNG TIN MÀN HÌNH";
    translations[TranslationKeys::DISPLAY_INFO_TEXT] = "Các thay đổi sẽ được áp dụng cho cấu hình Hyprland của bạn.\nKhởi động lại Hyprland để thay đổi độ phân giải màn hình có hiệu lực.";
    
    // Sound Manager
    translations[TranslationKeys::SOUND_TITLE] = "ÂM THANH";
    translations[TranslationKeys::OUTPUT_DEVICES] = "THIẾT BỊ ĐẦU RA";
    translations[TranslationKeys::INPUT_DEVICES] = "THIẾT BỊ ĐẦU VÀO";
    translations[TranslationKeys::VOLUME] = "Âm lượng";
    translations[TranslationKeys::PLAYBACK_APPLICATIONS] = "ỨNG DỤNG PHÁT";
    
    // Network Manager
    translations[TranslationKeys::NETWORK_TITLE] = "MẠNG";
    translations[TranslationKeys::WIFI_NETWORKS] = "Mạng WiFi";
    translations[TranslationKeys::WIFI] = "Wi-Fi";
    translations[TranslationKeys::CONNECTION_STATUS] = "Trạng thái kết nối";
    translations[TranslationKeys::CONNECT] = "Kết nối";
    translations[TranslationKeys::DISCONNECT] = "Ngắt kết nối";
    translations[TranslationKeys::CONNECTED] = "Đã kết nối";
    translations[TranslationKeys::DISCONNECTED] = "Đã ngắt kết nối";
    translations[TranslationKeys::ENTER_PASSWORD] = "Nhập mật khẩu";
    translations[TranslationKeys::CONNECTING] = "Đang kết nối...";
    translations[TranslationKeys::CANCEL] = "Hủy";
    translations[TranslationKeys::REFRESH_NETWORKS] = "Làm mới mạng";
    translations[TranslationKeys::NETWORKMANAGER_NOT_AVAILABLE] = "NetworkManager không khả dụng";
    translations[TranslationKeys::NETWORKING_DISABLED] = "Mạng đã bị tắt";
    translations[TranslationKeys::ENABLE_NETWORKING] = "Bật mạng";
    translations[TranslationKeys::CONNECTED_VIA_ETHERNET] = "Kết nối qua Ethernet";
    translations[TranslationKeys::WIFI_HARDWARE_DISABLED] = "Phần cứng Wi-Fi đã bị tắt";
    translations[TranslationKeys::WIFI_DISABLED] = "Wi-Fi đã bị tắt";
    translations[TranslationKeys::NO_NETWORKS_FOUND] = "Không tìm thấy mạng";
    translations[TranslationKeys::IPV4_METHOD] = "Phương thức IPv4:";
    translations[TranslationKeys::IP_ADDRESS] = "Địa chỉ IP:";
    translations[TranslationKeys::NETMASK] = "Mặt nạ mạng:";
    translations[TranslationKeys::GATEWAY] = "Cổng:";
    translations[TranslationKeys::DNS_SERVERS] = "Máy chủ DNS:";
    translations[TranslationKeys::IPV6_METHOD] = "Phương thức IPv6:";
    translations[TranslationKeys::IPV6_DNS_SERVERS] = "Máy chủ DNS IPv6:";
    
    // Bluetooth Manager
    translations[TranslationKeys::BLUETOOTH_TITLE] = "BLUETOOTH";
    translations[TranslationKeys::BLUETOOTH_DEVICES] = "Thiết bị Bluetooth";
    translations[TranslationKeys::OK] = "Ok";
    translations[TranslationKeys::DEVICES] = "THIẾT BỊ";
    translations[TranslationKeys::PAIR] = "Ghép đôi";
    translations[TranslationKeys::UNPAIR] = "Hủy ghép đôi";
    translations[TranslationKeys::PAIRED] = "Đã ghép đôi";
    translations[TranslationKeys::AVAILABLE] = "Có sẵn";
    translations[TranslationKeys::TOGGLE_BLUETOOTH] = "Bật/tắt Bluetooth";
    translations[TranslationKeys::SCAN_DEVICES] = "Quét thiết bị";
    translations[TranslationKeys::BLUETOOTH_DISABLED] = "Bluetooth đã bị tắt";
    translations[TranslationKeys::NO_DEVICES_FOUND_BT] = "Không tìm thấy thiết bị - thử quét";
    translations[TranslationKeys::SCANNING] = "Đang quét...";
    translations[TranslationKeys::SCANNING_FOR_DEVICES] = "Đang tìm thiết bị...";
    translations[TranslationKeys::BLUETOOTH_ENABLED_NO_DEVICES] = "Bluetooth đã bật - Không có thiết bị kết nối";
    translations[TranslationKeys::CONNECTED_TO] = "Kết nối với: ";
    translations[TranslationKeys::CONNECTED_TO_MULTIPLE] = "Kết nối với ";
    translations[TranslationKeys::TURN_OFF] = "Tắt";
    translations[TranslationKeys::TURN_ON] = "Bật";
    translations[TranslationKeys::DISCONNECT_BLU] = "Ngắt kết nối";
    translations[TranslationKeys::CONNECT_BLU] = "Kết nối";
    translations[TranslationKeys::FORGET] = "Quên";
    translations[TranslationKeys::SUCCESS_BLU] = "Thành công";
    translations[TranslationKeys::DISCONNECTED_BLU] = "Đã ngắt kết nối";
    translations[TranslationKeys::REMOVED] = "Đã xóa";
    translations[TranslationKeys::CONNECTED_STATUS] = " - Đã kết nối";
    translations[TranslationKeys::PAIRED_STATUS] = " - Đã ghép đôi";
    translations[TranslationKeys::NOT_PAIRED_STATUS] = " - Chưa ghép đôi";
    
    // Appearance Manager
    translations[TranslationKeys::APPEARANCE_TITLE] = "GIAO DIỆN";
    translations[TranslationKeys::THEME_SETTINGS] = "Cài đặt chủ đề";
    translations[TranslationKeys::THEME_SELECTION] = "CHỌN CHỦ ĐỀ";
    translations[TranslationKeys::WALLPAPER] = "Hình nền";
    translations[TranslationKeys::WALLPAPER_SELECTION] = "CHỌN HÌNH NỀN";
    translations[TranslationKeys::COLORS] = "Màu sắc";
    translations[TranslationKeys::LIGHT] = "Sáng";
    translations[TranslationKeys::DARK] = "Tối";
    translations[TranslationKeys::BROWSE] = "DUYỆT";
    translations[TranslationKeys::PREVIOUS_WALLPAPER] = "Hình nền trước";
    translations[TranslationKeys::NEXT_WALLPAPER] = "Hình nền tiếp";
    translations[TranslationKeys::OPEN_WALLPAPER_FOLDER] = "Mở thư mục hình nền";
    translations[TranslationKeys::SET_THIS_WALLPAPER] = "Đặt hình nền này";
    
    // Power Manager
    translations[TranslationKeys::POWER_TITLE] = "NGUỒN";
    translations[TranslationKeys::POWER_SETTINGS] = "Cài đặt nguồn";
    translations[TranslationKeys::SLEEP] = "Ngủ";
    translations[TranslationKeys::HIBERNATE] = "Ngủ đông";
    translations[TranslationKeys::SHUTDOWN] = "Tắt máy";
    translations[TranslationKeys::RESTART] = "Khởi động lại";
    translations[TranslationKeys::PERFORMANCE] = "HIỆU SUẤT";
    translations[TranslationKeys::BALANCED] = "CÂN BẰNG";
    translations[TranslationKeys::DEFAULT] = "MẶC ĐỊNH";
    translations[TranslationKeys::POWER_PROFILE_INFORMATION] = "THÔNG TIN PROFIL NGUỒN";
    translations[TranslationKeys::POWER_PROFILE_INFO_TEXT] = "Các thay đổi yêu cầu quyền quản trị và sẽ được áp dụng ngay lập tức.\nProfil nguồn hiện tại sẽ được phát hiện tự động khi mở menu này.";
    translations[TranslationKeys::MISSING] = "THIẾU";
    
    // Storage Manager
    translations[TranslationKeys::STORAGE_TITLE] = "LƯU TRỮ";
    translations[TranslationKeys::DISK_USAGE_TITLE] = "Sử dụng ổ đĩa";
    translations[TranslationKeys::AVAILABLE_SPACE] = "Dung lượng trống";
    translations[TranslationKeys::USED_SPACE] = "Dung lượng đã dùng";
    
    // Applications Manager
    translations[TranslationKeys::APPLICATIONS_TITLE] = "ỨNG DỤNG HYPRLAND";
    translations[TranslationKeys::KEYBINDS] = "PHÍM TẮT";
    translations[TranslationKeys::AUTORUNS] = "TỰ KHỞI ĐỘNG";
    translations[TranslationKeys::WINDOW_RULES] = "QUY TẮC CỬA SỔ";
    
    // Language Manager
    translations[TranslationKeys::LANGUAGE_SETTINGS] = "CÀI ĐẶT NGÔN NGỮ";
    translations[TranslationKeys::SYSTEM_LANGUAGE] = "NGÔN NGỮ HỆ THỐNG";
    translations[TranslationKeys::LANGUAGE_INFORMATION] = "THÔNG TIN NGÔN NGỮ";
    translations[TranslationKeys::LANGUAGE_INFO_TEXT] = "Các thay đổi sẽ được áp dụng sau khi đăng xuất.\nPhiên làm việc của bạn cần được khởi động lại.";
    
    // Messages
    translations[TranslationKeys::CHECKING_STATUS] = "Đang kiểm tra trạng thái...";
    translations[TranslationKeys::CHECKING_BATTERY_STATUS] = "Đang kiểm tra trạng thái pin...";
    translations[TranslationKeys::LOADING] = "Đang tải...";
    translations[TranslationKeys::ERROR] = "Lỗi";
    translations[TranslationKeys::SUCCESS] = "Thành công";
    translations[TranslationKeys::WARNING] = "Cảnh báo";
    translations[TranslationKeys::NO_SYSTEM_INFO] = "Không có thông tin hệ thống";
    translations[TranslationKeys::OUTPUT_DEVICES] = "Thiết bị đầu ra";
    translations[TranslationKeys::INPUT_DEVICES] = "Thiết bị đầu vào";
    translations[TranslationKeys::APPLICATION_VOLUME] = "Âm lượng ứng dụng";
    translations[TranslationKeys::NO_AUDIO_APPS] = "Không có ứng dụng nào đang phát âm thanh";
    
    // Dialog Messages
    translations[TranslationKeys::LOGOUT_REQUIRED] = "Yêu cầu đăng xuất";
    translations[TranslationKeys::LOGOUT_MESSAGE] = "Thay đổi ngôn ngữ yêu cầu đăng xuất để có hiệu lực.\nBạn có muốn đăng xuất ngay bây giờ không?";
    translations[TranslationKeys::LOGOUT_NOW] = "Đăng xuất ngay";
    translations[TranslationKeys::LOGOUT_LATER] = "Để sau";

    // Network Manager Additional
    translations[TranslationKeys::NETWORK_SETTINGS] = "Cài đặt mạng";
    translations[TranslationKeys::ENTER_NETWORK_PASSWORD] = "Nhập mật khẩu mạng";
    translations[TranslationKeys::IPV4_METHOD_LABEL] = "Phương thức IPv4:";
    translations[TranslationKeys::IP_ADDRESS_LABEL] = "Địa chỉ IP:";
    translations[TranslationKeys::NETMASK_LABEL] = "Mặt nạ mạng:";
    translations[TranslationKeys::GATEWAY_LABEL] = "Cổng mặc định:";
    translations[TranslationKeys::DNS_SERVERS_LABEL] = "Máy chủ DNS:";
    translations[TranslationKeys::IPV6_METHOD_LABEL] = "Phương thức IPv6:";
    translations[TranslationKeys::IPV6_DNS_SERVERS_LABEL] = "Máy chủ DNS IPv6:";
    translations[TranslationKeys::PORT_SETTINGS] = "Cài đặt cổng:";
    translations[TranslationKeys::HTTP_PROXY_PORT] = "Cổng proxy HTTP:";
    translations[TranslationKeys::HTTPS_PROXY_PORT] = "Cổng proxy HTTPS:";
    translations[TranslationKeys::FTP_PROXY_PORT] = "Cổng proxy FTP:";
    translations[TranslationKeys::SOCKS_PROXY_PORT] = "Cổng proxy SOCKS:";
    translations[TranslationKeys::CUSTOM_PORT_RANGE] = "Phạm vi cổng tùy chỉnh:";
    translations[TranslationKeys::PORT_DASH] = " - ";
    translations[TranslationKeys::CONNECTED_TO_PREFIX] = "Đã kết nối tới: ";
    translations[TranslationKeys::ETHERNET] = "Ethernet";
    translations[TranslationKeys::CONNECTING_STATUS] = "Đang kết nối...";
    translations[TranslationKeys::NETWORKING_DISABLED_STATUS] = "Mạng bị vô hiệu hóa";
    translations[TranslationKeys::WIFI_DISABLED_STATUS] = "Wi-Fi bị vô hiệu hóa";
    translations[TranslationKeys::NOT_CONNECTED_STATUS] = "Chưa kết nối";
    
    // Navigation Buttons
    translations[TranslationKeys::PREVIOUS_ARROW] = "‹";
    translations[TranslationKeys::NEXT_ARROW] = "›";
    translations[TranslationKeys::LEFT_ARROW] = "<";
    translations[TranslationKeys::RIGHT_ARROW] = ">";
    translations[TranslationKeys::ADD_BUTTON] = "+";
    translations[TranslationKeys::REMOVE_BUTTON] = "−";
    
    // Language Manager
    translations[TranslationKeys::LANGUAGE_CHANGED] = "Ngôn ngữ đã thay đổi";
    
    // Display Manager
    translations[TranslationKeys::BRIGHTNESS_VALUE] = "50%";
    
    // Sound Manager
    translations[TranslationKeys::VOLUME_VALUE] = "0%";
    
    // Battery Manager Additional
    translations[TranslationKeys::POWER_DETAILS] = "CHI TIẾT NĂNG LƯỢNG";
    translations[TranslationKeys::NO_BATTERY_DETECTED] = "Không phát hiện pin";
    translations[TranslationKeys::CAPACITY] = "Dung lượng";
    translations[TranslationKeys::TECHNOLOGY] = "Công nghệ";
    translations[TranslationKeys::CHARGE_CYCLES] = "Chu Kỳ Sạc";
    translations[TranslationKeys::WARNING_LEVEL] = "Mức Cảnh Báo";
    translations[TranslationKeys::ENERGY_FULL] = "Năng Lượng Đầy";
    translations[TranslationKeys::ENERGY_DESIGN] = "Năng Lượng Thiết Kế";
    translations[TranslationKeys::ENERGY_RATE] = "Tỷ Lệ Năng Lượng";
    translations[TranslationKeys::VOLTAGE] = "Điện Áp";
    
    // Power Manager Additional
    translations[TranslationKeys::PERFORMANCE_MODE] = "Hiệu Suất";
    translations[TranslationKeys::BALANCED_MODE] = "Cân Bằng";
    translations[TranslationKeys::DEFAULT_MODE] = "Mặc Định";
    translations[TranslationKeys::PERFORMANCE_DESCRIPTION] = "Hiệu suất CPU tối đa cho các tác vụ đòi hỏi cao";
    translations[TranslationKeys::BALANCED_DESCRIPTION] = "Cân bằng tối ưu giữa hiệu suất và tiết kiệm năng lượng";
    translations[TranslationKeys::DEFAULT_DESCRIPTION] = "Quản lý năng lượng mặc định của hệ thống";

}
