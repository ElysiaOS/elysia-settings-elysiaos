#include "ko_KR.h"

void loadKoreanTranslations(std::map<TranslationKeys, std::string>& translations) {
    // Main Window
    translations[TranslationKeys::WINDOW_TITLE] = "ElysiaOS 설정";
    translations[TranslationKeys::EXIT] = "종료";
    
    // Common UI
    translations[TranslationKeys::BACK] = "뒤로";
    translations[TranslationKeys::APPLY_CHANGES] = "변경사항 적용";
    translations[TranslationKeys::REFRESH] = "새로고침";
    
    // Tile Names
    translations[TranslationKeys::TILE_CAR] = "자동차";
    translations[TranslationKeys::TILE_BATTERY] = "배터리";
    translations[TranslationKeys::TILE_APPEARANCE] = "모양";
    translations[TranslationKeys::TILE_POWER] = "전원";
    translations[TranslationKeys::TILE_BLUETOOTH] = "블루투스";
    translations[TranslationKeys::TILE_SOUND] = "사운드";
    translations[TranslationKeys::TILE_ABOUT] = "정보";
    translations[TranslationKeys::TILE_DISPLAY] = "디스플레이";
    translations[TranslationKeys::TILE_NETWORK] = "네트워크";
    translations[TranslationKeys::TILE_UPDATES] = "업데이트";
    translations[TranslationKeys::TILE_HYPERLAND] = "하이퍼랜드";
    translations[TranslationKeys::TILE_STORAGE] = "저장소";
    translations[TranslationKeys::TILE_SUPPORT] = "지원";
    
    // About Manager
    translations[TranslationKeys::ABOUT_TITLE] = "시스템 정보";
    translations[TranslationKeys::SYSTEM_INFO] = "시스템 정보";
    translations[TranslationKeys::OS_NAME] = "운영 체제";
    translations[TranslationKeys::KERNEL_VERSION] = "커널 버전";
    translations[TranslationKeys::UPTIME] = "시스템 가동 시간";
    translations[TranslationKeys::CPU_INFO] = "프로세서";
    translations[TranslationKeys::MEMORY_INFO] = "메모리";
    translations[TranslationKeys::DISK_USAGE] = "디스크 사용량";
    
    // Battery Manager
    translations[TranslationKeys::BATTERY_TITLE] = "배터리 상태";
    translations[TranslationKeys::BATTERY_STATUS] = "배터리 상태";
    translations[TranslationKeys::BATTERY_HEALTH] = "배터리 상태";
    translations[TranslationKeys::CHARGING] = "충전 중";
    translations[TranslationKeys::DISCHARGING] = "방전 중";
    translations[TranslationKeys::FULL] = "완충";
    translations[TranslationKeys::NOT_CHARGING] = "충전하지 않음";
    
    // Display Manager
    translations[TranslationKeys::DISPLAY_TITLE] = "디스플레이 설정";
    translations[TranslationKeys::DISPLAY_SETTINGS] = "디스플레이 설정";
    translations[TranslationKeys::DISPLAY_RESOLUTION] = "디스플레이 해상도";
    translations[TranslationKeys::SCREEN_BRIGHTNESS] = "화면 밝기";
    translations[TranslationKeys::DISPLAY_INFORMATION] = "디스플레이 정보";
    translations[TranslationKeys::DISPLAY_INFO_TEXT] = "변경사항이 Hyprland 구성에 적용됩니다.\n디스플레이 해상도 변경을 적용하려면 Hyprland를 재시작하세요.";
    
    // Sound Manager
    translations[TranslationKeys::SOUND_TITLE] = "사운드";
    translations[TranslationKeys::OUTPUT_DEVICES] = "출력 장치";
    translations[TranslationKeys::INPUT_DEVICES] = "입력 장치";
    translations[TranslationKeys::VOLUME] = "볼륨";
    translations[TranslationKeys::PLAYBACK_APPLICATIONS] = "재생 응용 프로그램";
    
    // Network Manager
    translations[TranslationKeys::NETWORK_TITLE] = "네트워크";
    translations[TranslationKeys::WIFI_NETWORKS] = "WiFi 네트워크";
    translations[TranslationKeys::WIFI] = "Wi-Fi";
    translations[TranslationKeys::CONNECTION_STATUS] = "연결 상태";
    translations[TranslationKeys::CONNECT] = "연결";
    translations[TranslationKeys::DISCONNECT] = "연결 해제";
    translations[TranslationKeys::CONNECTED] = "연결됨";
    translations[TranslationKeys::DISCONNECTED] = "연결 해제됨";
    translations[TranslationKeys::ENTER_PASSWORD] = "비밀번호 입력";
    translations[TranslationKeys::CONNECTING] = "연결 중...";
    translations[TranslationKeys::CANCEL] = "취소";
    translations[TranslationKeys::REFRESH_NETWORKS] = "네트워크 새로고침";
    translations[TranslationKeys::NETWORKMANAGER_NOT_AVAILABLE] = "NetworkManager를 사용할 수 없습니다";
    translations[TranslationKeys::NETWORKING_DISABLED] = "네트워크가 비활성화되었습니다";
    translations[TranslationKeys::ENABLE_NETWORKING] = "네트워크 활성화";
    translations[TranslationKeys::CONNECTED_VIA_ETHERNET] = "이더넷을 통해 연결됨";
    translations[TranslationKeys::WIFI_HARDWARE_DISABLED] = "Wi-Fi 하드웨어가 비활성화되었습니다";
    translations[TranslationKeys::WIFI_DISABLED] = "Wi-Fi가 비활성화되었습니다";
    translations[TranslationKeys::NO_NETWORKS_FOUND] = "네트워크를 찾을 수 없습니다";
    translations[TranslationKeys::IPV4_METHOD] = "IPv4 방식：";
    translations[TranslationKeys::IP_ADDRESS] = "IP 주소：";
    translations[TranslationKeys::NETMASK] = "넷마스크：";
    translations[TranslationKeys::GATEWAY] = "게이트웨이：";
    translations[TranslationKeys::DNS_SERVERS] = "DNS 서버：";
    translations[TranslationKeys::IPV6_METHOD] = "IPv6 방식：";
    translations[TranslationKeys::IPV6_DNS_SERVERS] = "IPv6 DNS 서버：";
    
    // Bluetooth Manager
    translations[TranslationKeys::BLUETOOTH_TITLE] = "블루투스";
    translations[TranslationKeys::BLUETOOTH_DEVICES] = "블루투스 장치";
    translations[TranslationKeys::DEVICES] = "장치";
    translations[TranslationKeys::OK] = "확인";
    translations[TranslationKeys::PAIR] = "페어링";
    translations[TranslationKeys::UNPAIR] = "페어링 해제";
    translations[TranslationKeys::PAIRED] = "페어링됨";
    translations[TranslationKeys::AVAILABLE] = "사용 가능";
    translations[TranslationKeys::TOGGLE_BLUETOOTH] = "블루투스 전환";
    translations[TranslationKeys::SCAN_DEVICES] = "장치 스캔";
    translations[TranslationKeys::BLUETOOTH_DISABLED] = "블루투스가 비활성화되었습니다";
    translations[TranslationKeys::NO_DEVICES_FOUND_BT] = "장치를 찾을 수 없습니다 - 스캔해 보세요";
    translations[TranslationKeys::SCANNING] = "스캔 중...";
    translations[TranslationKeys::SCANNING_FOR_DEVICES] = "장치 스캔 중...";
    translations[TranslationKeys::BLUETOOTH_ENABLED_NO_DEVICES] = "블루투스 활성화 - 연결된 장치 없음";
    translations[TranslationKeys::CONNECTED_TO] = "연결됨: ";
    translations[TranslationKeys::CONNECTED_TO_MULTIPLE] = "연결됨 ";
    translations[TranslationKeys::TURN_OFF] = "끄기";
    translations[TranslationKeys::TURN_ON] = "켜기";
    translations[TranslationKeys::DISCONNECT_BLU] = "연결 해제";
    translations[TranslationKeys::CONNECT_BLU] = "연결";
    translations[TranslationKeys::FORGET] = "삭제";
    translations[TranslationKeys::SUCCESS_BLU] = "성공";
    translations[TranslationKeys::DISCONNECTED_BLU] = "연결 해제됨";
    translations[TranslationKeys::REMOVED] = "제거됨";
    translations[TranslationKeys::CONNECTED_STATUS] = " - 연결됨";
    translations[TranslationKeys::PAIRED_STATUS] = " - 페어링됨";
    translations[TranslationKeys::NOT_PAIRED_STATUS] = " - 페어링 안됨";
    
    // Appearance Manager
    translations[TranslationKeys::APPEARANCE_TITLE] = "모양";
    translations[TranslationKeys::THEME_SETTINGS] = "테마 설정";
    translations[TranslationKeys::THEME_SELECTION] = "테마 선택";
    translations[TranslationKeys::WALLPAPER] = "배경화면";
    translations[TranslationKeys::WALLPAPER_SELECTION] = "배경화면 선택";
    translations[TranslationKeys::COLORS] = "색상";
    translations[TranslationKeys::LIGHT] = "밝음";
    translations[TranslationKeys::DARK] = "어두움";
    translations[TranslationKeys::BROWSE] = "찾아보기";
    translations[TranslationKeys::PREVIOUS_WALLPAPER] = "이전 배경화면";
    translations[TranslationKeys::NEXT_WALLPAPER] = "다음 배경화면";
    translations[TranslationKeys::OPEN_WALLPAPER_FOLDER] = "배경화면 폴더 열기";
    translations[TranslationKeys::SET_THIS_WALLPAPER] = "이 배경화면 설정";
    
    // Power Manager
    translations[TranslationKeys::POWER_TITLE] = "전원";
    translations[TranslationKeys::POWER_SETTINGS] = "전원 설정";
    translations[TranslationKeys::SLEEP] = "절전 모드";
    translations[TranslationKeys::HIBERNATE] = "최대 절전 모드";
    translations[TranslationKeys::SHUTDOWN] = "시스템 종료";
    translations[TranslationKeys::RESTART] = "재시작";
    translations[TranslationKeys::PERFORMANCE] = "성능";
    translations[TranslationKeys::BALANCED] = "균형";
    translations[TranslationKeys::DEFAULT] = "기본값";
    translations[TranslationKeys::POWER_PROFILE_INFORMATION] = "전원 프로파일 정보";
    translations[TranslationKeys::POWER_PROFILE_INFO_TEXT] = "변경사항은 관리자 권한이 필요하며 즉시 적용됩니다.\n현재 전원 프로파일은 이 메뉴를 열 때 자동으로 감지됩니다.";
    translations[TranslationKeys::MISSING] = "누락";
    
    // Storage Manager
    translations[TranslationKeys::STORAGE_TITLE] = "저장소";
    translations[TranslationKeys::DISK_USAGE_TITLE] = "디스크 사용량";
    translations[TranslationKeys::AVAILABLE_SPACE] = "사용 가능한 공간";
    translations[TranslationKeys::USED_SPACE] = "사용된 공간";
    
    // Applications Manager
    translations[TranslationKeys::APPLICATIONS_TITLE] = "HYPRLAND 응용 프로그램";
    translations[TranslationKeys::KEYBINDS] = "키 바인딩";
    translations[TranslationKeys::AUTORUNS] = "자동 실행";
    translations[TranslationKeys::WINDOW_RULES] = "창 규칙";
    
    // Language Manager
    translations[TranslationKeys::LANGUAGE_SETTINGS] = "언어 설정";
    translations[TranslationKeys::SYSTEM_LANGUAGE] = "시스템 언어";
    translations[TranslationKeys::LANGUAGE_INFORMATION] = "언어 정보";
    translations[TranslationKeys::LANGUAGE_INFO_TEXT] = "변경사항이 로그아웃 후에 적용됩니다.\n세션을 다시 시작해야 합니다.";
    
    // Messages
    translations[TranslationKeys::CHECKING_STATUS] = "상태 확인 중...";
    translations[TranslationKeys::CHECKING_BATTERY_STATUS] = "배터리 상태 확인 중...";
    translations[TranslationKeys::LOADING] = "로딩 중...";
    translations[TranslationKeys::ERROR] = "오류";
    translations[TranslationKeys::SUCCESS] = "성공";
    translations[TranslationKeys::WARNING] = "경고";
    translations[TranslationKeys::NO_SYSTEM_INFO] = "사용 가능한 시스템 정보가 없습니다";
    translations[TranslationKeys::OUTPUT_DEVICES] = "출력 장치";
    translations[TranslationKeys::INPUT_DEVICES] = "입력 장치";
    translations[TranslationKeys::APPLICATION_VOLUME] = "응용 프로그램 볼륨";
    translations[TranslationKeys::NO_AUDIO_APPS] = "현재 오디오를 재생하는 응용 프로그램이 없습니다";
    
    // Dialog Messages
    translations[TranslationKeys::LOGOUT_REQUIRED] = "로그아웃 필요";
    translations[TranslationKeys::LOGOUT_MESSAGE] = "언어 변경을 적용하려면 로그아웃이 필요합니다.\n지금 로그아웃하시겠습니까?";
    translations[TranslationKeys::LOGOUT_NOW] = "지금 로그아웃";
    translations[TranslationKeys::LOGOUT_LATER] = "나중에";

    // Network Manager Additional
    translations[TranslationKeys::NETWORK_SETTINGS] = "네트워크 설정";
    translations[TranslationKeys::ENTER_NETWORK_PASSWORD] = "네트워크 비밀번호 입력";
    translations[TranslationKeys::IPV4_METHOD_LABEL] = "IPv4 방법:";
    translations[TranslationKeys::IP_ADDRESS_LABEL] = "IP 주소:";
    translations[TranslationKeys::NETMASK_LABEL] = "네트마스크:";
    translations[TranslationKeys::GATEWAY_LABEL] = "게이트웨이:";
    translations[TranslationKeys::DNS_SERVERS_LABEL] = "DNS 서버:";
    translations[TranslationKeys::IPV6_METHOD_LABEL] = "IPv6 방법:";
    translations[TranslationKeys::IPV6_DNS_SERVERS_LABEL] = "IPv6 DNS 서버:";
    translations[TranslationKeys::PORT_SETTINGS] = "포트 설정:";
    translations[TranslationKeys::HTTP_PROXY_PORT] = "HTTP 프록시 포트:";
    translations[TranslationKeys::HTTPS_PROXY_PORT] = "HTTPS 프록시 포트:";
    translations[TranslationKeys::FTP_PROXY_PORT] = "FTP 프록시 포트:";
    translations[TranslationKeys::SOCKS_PROXY_PORT] = "SOCKS 프록시 포트:";
    translations[TranslationKeys::CUSTOM_PORT_RANGE] = "사용자 정의 포트 범위:";
    translations[TranslationKeys::PORT_DASH] = " - ";
    translations[TranslationKeys::CONNECTED_TO_PREFIX] = "연결됨: ";
    translations[TranslationKeys::ETHERNET] = "이더넷";
    translations[TranslationKeys::CONNECTING_STATUS] = "연결 중...";
    translations[TranslationKeys::NETWORKING_DISABLED_STATUS] = "네트워크 비활성화";
    translations[TranslationKeys::WIFI_DISABLED_STATUS] = "Wi-Fi 비활성화";
    translations[TranslationKeys::NOT_CONNECTED_STATUS] = "연결되지 않음";
    
    // Navigation Buttons
    translations[TranslationKeys::PREVIOUS_ARROW] = "‹";
    translations[TranslationKeys::NEXT_ARROW] = "›";
    translations[TranslationKeys::LEFT_ARROW] = "<";
    translations[TranslationKeys::RIGHT_ARROW] = ">";
    translations[TranslationKeys::ADD_BUTTON] = "+";
    translations[TranslationKeys::REMOVE_BUTTON] = "−";
    
    // Language Manager
    translations[TranslationKeys::LANGUAGE_CHANGED] = "언어가 변경되었습니다";
    
    // Display Manager
    translations[TranslationKeys::BRIGHTNESS_VALUE] = "50%";
    
    // Sound Manager
    translations[TranslationKeys::VOLUME_VALUE] = "0%";
    
    // Battery Manager Additional
    translations[TranslationKeys::POWER_DETAILS] = "전원 세부사항";
    translations[TranslationKeys::NO_BATTERY_DETECTED] = "배터리가 감지되지 않음";
    translations[TranslationKeys::CAPACITY] = "용량";
    translations[TranslationKeys::TECHNOLOGY] = "기술";
    translations[TranslationKeys::CHARGE_CYCLES] = "충전 사이클";
    translations[TranslationKeys::WARNING_LEVEL] = "경고 수준";
    translations[TranslationKeys::ENERGY_FULL] = "전체 에너지";
    translations[TranslationKeys::ENERGY_DESIGN] = "설계 에너지";
    translations[TranslationKeys::ENERGY_RATE] = "에너지 비율";
    translations[TranslationKeys::VOLTAGE] = "전압";
    
    // Power Manager Additional
    translations[TranslationKeys::PERFORMANCE_MODE] = "성능";
    translations[TranslationKeys::BALANCED_MODE] = "균형";
    translations[TranslationKeys::DEFAULT_MODE] = "기본값";
    translations[TranslationKeys::PERFORMANCE_DESCRIPTION] = "요구가 높은 작업을 위한 최대 CPU 성능";
    translations[TranslationKeys::BALANCED_DESCRIPTION] = "성능과 전력 절약의 최적화된 균형";
    translations[TranslationKeys::DEFAULT_DESCRIPTION] = "시스템 기본 전력 관리";

}
