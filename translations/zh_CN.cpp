#include "zh_CN.h"

void loadChineseTranslations(std::map<TranslationKeys, std::string>& translations) {
    // Main Window
    translations[TranslationKeys::WINDOW_TITLE] = "ElysiaOS 设置";
    translations[TranslationKeys::EXIT] = "退出";
    
    // Common UI
    translations[TranslationKeys::BACK] = "返回";
    translations[TranslationKeys::APPLY_CHANGES] = "应用更改";
    translations[TranslationKeys::REFRESH] = "刷新";
    
    // Tile Names
    translations[TranslationKeys::TILE_CAR] = "汽车";
    translations[TranslationKeys::TILE_BATTERY] = "电池";
    translations[TranslationKeys::TILE_APPEARANCE] = "外观";
    translations[TranslationKeys::TILE_POWER] = "电源";
    translations[TranslationKeys::TILE_BLUETOOTH] = "蓝牙";
    translations[TranslationKeys::TILE_SOUND] = "声音";
    translations[TranslationKeys::TILE_ABOUT] = "关于";
    translations[TranslationKeys::TILE_DISPLAY] = "显示";
    translations[TranslationKeys::TILE_NETWORK] = "网络";
    translations[TranslationKeys::TILE_UPDATES] = "更新";
    translations[TranslationKeys::TILE_HYPERLAND] = "超域";
    translations[TranslationKeys::TILE_STORAGE] = "存储";
    translations[TranslationKeys::TILE_SUPPORT] = "支持";
    
    // About Manager
    translations[TranslationKeys::ABOUT_TITLE] = "关于系统";
    translations[TranslationKeys::SYSTEM_INFO] = "系统信息";
    translations[TranslationKeys::OS_NAME] = "操作系统";
    translations[TranslationKeys::KERNEL_VERSION] = "内核版本";
    translations[TranslationKeys::UPTIME] = "系统运行时间";
    translations[TranslationKeys::CPU_INFO] = "处理器";
    translations[TranslationKeys::MEMORY_INFO] = "内存";
    translations[TranslationKeys::DISK_USAGE] = "磁盘使用";
    
    // Battery Manager
    translations[TranslationKeys::BATTERY_TITLE] = "电池状态";
    translations[TranslationKeys::BATTERY_STATUS] = "电池状态";
    translations[TranslationKeys::BATTERY_HEALTH] = "电池健康";
    translations[TranslationKeys::CHARGING] = "充电中";
    translations[TranslationKeys::DISCHARGING] = "放电中";
    translations[TranslationKeys::FULL] = "已充满";
    translations[TranslationKeys::NOT_CHARGING] = "未充电";
    
    // Display Manager
    translations[TranslationKeys::DISPLAY_TITLE] = "显示设置";
    translations[TranslationKeys::DISPLAY_SETTINGS] = "显示设置";
    translations[TranslationKeys::DISPLAY_RESOLUTION] = "显示分辨率";
    translations[TranslationKeys::SCREEN_BRIGHTNESS] = "屏幕亮度";
    translations[TranslationKeys::DISPLAY_INFORMATION] = "显示信息";
    translations[TranslationKeys::DISPLAY_INFO_TEXT] = "更改将应用到您的 Hyprland 配置。\n重启 Hyprland 以使显示分辨率更改生效。";
    
    // Sound Manager
    translations[TranslationKeys::SOUND_TITLE] = "声音";
    translations[TranslationKeys::OUTPUT_DEVICES] = "输出设备";
    translations[TranslationKeys::INPUT_DEVICES] = "输入设备";
    translations[TranslationKeys::VOLUME] = "音量";
    translations[TranslationKeys::PLAYBACK_APPLICATIONS] = "播放应用程序";
    
    // Network Manager
    translations[TranslationKeys::NETWORK_TITLE] = "网络";
    translations[TranslationKeys::WIFI_NETWORKS] = "WiFi 网络";
    translations[TranslationKeys::WIFI] = "Wi-Fi";
    translations[TranslationKeys::CONNECTION_STATUS] = "连接状态";
    translations[TranslationKeys::CONNECT] = "连接";
    translations[TranslationKeys::DISCONNECT] = "断开";
    translations[TranslationKeys::CONNECTED] = "已连接";
    translations[TranslationKeys::DISCONNECTED] = "已断开";
    translations[TranslationKeys::ENTER_PASSWORD] = "输入密码";
    translations[TranslationKeys::CONNECTING] = "连接中...";
    translations[TranslationKeys::CANCEL] = "取消";
    translations[TranslationKeys::REFRESH_NETWORKS] = "刷新网络";
    translations[TranslationKeys::NETWORKMANAGER_NOT_AVAILABLE] = "NetworkManager 不可用";
    translations[TranslationKeys::NETWORKING_DISABLED] = "网络已禁用";
    translations[TranslationKeys::ENABLE_NETWORKING] = "启用网络";
    translations[TranslationKeys::CONNECTED_VIA_ETHERNET] = "通过以太网连接";
    translations[TranslationKeys::WIFI_HARDWARE_DISABLED] = "Wi-Fi 硬件已禁用";
    translations[TranslationKeys::WIFI_DISABLED] = "Wi-Fi 已禁用";
    translations[TranslationKeys::NO_NETWORKS_FOUND] = "未找到网络";
    translations[TranslationKeys::IPV4_METHOD] = "IPv4 方法：";
    translations[TranslationKeys::IP_ADDRESS] = "IP 地址：";
    translations[TranslationKeys::NETMASK] = "子网掩码：";
    translations[TranslationKeys::GATEWAY] = "网关：";
    translations[TranslationKeys::DNS_SERVERS] = "DNS 服务器：";
    translations[TranslationKeys::IPV6_METHOD] = "IPv6 方法：";
    translations[TranslationKeys::IPV6_DNS_SERVERS] = "IPv6 DNS 服务器：";
    
    // Bluetooth Manager
    translations[TranslationKeys::BLUETOOTH_TITLE] = "蓝牙";
    translations[TranslationKeys::BLUETOOTH_DEVICES] = "蓝牙设备";
    translations[TranslationKeys::DEVICES] = "设备";
    translations[TranslationKeys::PAIR] = "配对";
    translations[TranslationKeys::UNPAIR] = "取消配对";
    translations[TranslationKeys::PAIRED] = "已配对";
    translations[TranslationKeys::AVAILABLE] = "可用";
    translations[TranslationKeys::TOGGLE_BLUETOOTH] = "切换蓝牙";
    translations[TranslationKeys::SCAN_DEVICES] = "扫描设备";
    translations[TranslationKeys::BLUETOOTH_DISABLED] = "蓝牙已禁用";
    translations[TranslationKeys::NO_DEVICES_FOUND_BT] = "未找到设备 - 尝试扫描";
    translations[TranslationKeys::SCANNING] = "扫描中...";
    translations[TranslationKeys::SCANNING_FOR_DEVICES] = "扫描设备中...";
    translations[TranslationKeys::BLUETOOTH_ENABLED_NO_DEVICES] = "蓝牙已启用 - 无设备连接";
    translations[TranslationKeys::CONNECTED_TO] = "已连接到：";
    translations[TranslationKeys::CONNECTED_TO_MULTIPLE] = "已连接到 ";
    translations[TranslationKeys::TURN_OFF] = "关闭";
    translations[TranslationKeys::TURN_ON] = "开启";
    translations[TranslationKeys::DISCONNECT_BLU] = "断开";
    translations[TranslationKeys::CONNECT_BLU] = "连接";
    translations[TranslationKeys::FORGET] = "忘记";
    translations[TranslationKeys::SUCCESS_BLU] = "成功";
    translations[TranslationKeys::DISCONNECTED_BLU] = "已断开";
    translations[TranslationKeys::REMOVED] = "已移除";
    translations[TranslationKeys::CONNECTED_STATUS] = " - 已连接";
    translations[TranslationKeys::PAIRED_STATUS] = " - 已配对";
    translations[TranslationKeys::NOT_PAIRED_STATUS] = " - 未配对";
    
    // Appearance Manager
    translations[TranslationKeys::APPEARANCE_TITLE] = "外观";
    translations[TranslationKeys::THEME_SETTINGS] = "主题设置";
    translations[TranslationKeys::THEME_SELECTION] = "主题选择";
    translations[TranslationKeys::WALLPAPER] = "壁纸";
    translations[TranslationKeys::WALLPAPER_SELECTION] = "壁纸选择";
    translations[TranslationKeys::COLORS] = "颜色";
    translations[TranslationKeys::LIGHT] = "浅色";
    translations[TranslationKeys::DARK] = "深色";
    translations[TranslationKeys::BROWSE] = "浏览";
    translations[TranslationKeys::PREVIOUS_WALLPAPER] = "上一张壁纸";
    translations[TranslationKeys::NEXT_WALLPAPER] = "下一张壁纸";
    translations[TranslationKeys::OPEN_WALLPAPER_FOLDER] = "打开壁纸文件夹";
    translations[TranslationKeys::SET_THIS_WALLPAPER] = "设置此壁纸";
    
    // Power Manager
    translations[TranslationKeys::POWER_TITLE] = "电源";
    translations[TranslationKeys::POWER_SETTINGS] = "电源设置";
    translations[TranslationKeys::SLEEP] = "睡眠";
    translations[TranslationKeys::HIBERNATE] = "休眠";
    translations[TranslationKeys::SHUTDOWN] = "关机";
    translations[TranslationKeys::RESTART] = "重启";
    translations[TranslationKeys::PERFORMANCE] = "性能";
    translations[TranslationKeys::BALANCED] = "平衡";
    translations[TranslationKeys::DEFAULT] = "默认";
    translations[TranslationKeys::POWER_PROFILE_INFORMATION] = "电源配置文件信息";
    translations[TranslationKeys::POWER_PROFILE_INFO_TEXT] = "更改需要管理员权限并将立即应用。\n打开此菜单时将自动检测当前电源配置文件。";
    translations[TranslationKeys::MISSING] = "缺失";
    
    // Storage Manager
    translations[TranslationKeys::STORAGE_TITLE] = "存储";
    translations[TranslationKeys::DISK_USAGE_TITLE] = "磁盘使用";
    translations[TranslationKeys::AVAILABLE_SPACE] = "可用空间";
    translations[TranslationKeys::USED_SPACE] = "已用空间";
    
    // Applications Manager
    translations[TranslationKeys::APPLICATIONS_TITLE] = "HYPRLAND 应用程序";
    translations[TranslationKeys::KEYBINDS] = "键绑定";
    translations[TranslationKeys::AUTORUNS] = "自动运行";
    translations[TranslationKeys::WINDOW_RULES] = "窗口规则";
    
    // Language Manager
    translations[TranslationKeys::LANGUAGE_SETTINGS] = "语言设置";
    translations[TranslationKeys::SYSTEM_LANGUAGE] = "系统语言";
    translations[TranslationKeys::LANGUAGE_INFORMATION] = "语言信息";
    translations[TranslationKeys::LANGUAGE_INFO_TEXT] = "更改将在注销后生效。\n您需要重新启动会话。";
    
    // Messages
    translations[TranslationKeys::CHECKING_STATUS] = "检查状态中...";
    translations[TranslationKeys::CHECKING_BATTERY_STATUS] = "检查电池状态中...";
    translations[TranslationKeys::LOADING] = "加载中...";
    translations[TranslationKeys::ERROR] = "错误";
    translations[TranslationKeys::SUCCESS] = "成功";
    translations[TranslationKeys::WARNING] = "警告";
    translations[TranslationKeys::NO_SYSTEM_INFO] = "无可用系统信息";
    translations[TranslationKeys::OUTPUT_DEVICES] = "输出设备";
    translations[TranslationKeys::INPUT_DEVICES] = "输入设备";
    translations[TranslationKeys::APPLICATION_VOLUME] = "应用程序音量";
    translations[TranslationKeys::NO_AUDIO_APPS] = "当前没有应用程序正在播放音频";
    
    // Dialog Messages
    translations[TranslationKeys::LOGOUT_REQUIRED] = "需要注销";
    translations[TranslationKeys::LOGOUT_MESSAGE] = "语言更改需要注销才能生效。\n您现在要注销吗？";
    translations[TranslationKeys::LOGOUT_NOW] = "立即注销";
    translations[TranslationKeys::LOGOUT_LATER] = "稍后";

    // Network Manager Additional
    translations[TranslationKeys::NETWORK_SETTINGS] = "网络设置";
    translations[TranslationKeys::ENTER_NETWORK_PASSWORD] = "输入网络密码";
    translations[TranslationKeys::IPV4_METHOD_LABEL] = "IPv4方法：";
    translations[TranslationKeys::IP_ADDRESS_LABEL] = "IP地址：";
    translations[TranslationKeys::NETMASK_LABEL] = "网络掩码：";
    translations[TranslationKeys::GATEWAY_LABEL] = "网关：";
    translations[TranslationKeys::DNS_SERVERS_LABEL] = "DNS服务器：";
    translations[TranslationKeys::IPV6_METHOD_LABEL] = "IPv6方法：";
    translations[TranslationKeys::IPV6_DNS_SERVERS_LABEL] = "IPv6 DNS服务器：";
    translations[TranslationKeys::PORT_SETTINGS] = "端口设置：";
    translations[TranslationKeys::HTTP_PROXY_PORT] = "HTTP代理端口：";
    translations[TranslationKeys::HTTPS_PROXY_PORT] = "HTTPS代理端口：";
    translations[TranslationKeys::FTP_PROXY_PORT] = "FTP代理端口：";
    translations[TranslationKeys::SOCKS_PROXY_PORT] = "SOCKS代理端口：";
    translations[TranslationKeys::CUSTOM_PORT_RANGE] = "自定义端口范围：";
    translations[TranslationKeys::PORT_DASH] = " - ";
    translations[TranslationKeys::CONNECTED_TO_PREFIX] = "已连接到：";
    translations[TranslationKeys::ETHERNET] = "以太网";
    translations[TranslationKeys::CONNECTING_STATUS] = "连接中...";
    translations[TranslationKeys::NETWORKING_DISABLED_STATUS] = "网络已禁用";
    translations[TranslationKeys::WIFI_DISABLED_STATUS] = "Wi-Fi已禁用";
    translations[TranslationKeys::NOT_CONNECTED_STATUS] = "未连接";
    
    // Navigation Buttons
    translations[TranslationKeys::PREVIOUS_ARROW] = "‹";
    translations[TranslationKeys::NEXT_ARROW] = "›";
    translations[TranslationKeys::LEFT_ARROW] = "<";
    translations[TranslationKeys::RIGHT_ARROW] = ">";
    translations[TranslationKeys::ADD_BUTTON] = "+";
    translations[TranslationKeys::REMOVE_BUTTON] = "−";
    
    // Language Manager
    translations[TranslationKeys::LANGUAGE_CHANGED] = "语言已更改";
    
    // Display Manager
    translations[TranslationKeys::BRIGHTNESS_VALUE] = "50%";
    
    // Sound Manager
    translations[TranslationKeys::VOLUME_VALUE] = "0%";
    
    // Battery Manager Additional
    translations[TranslationKeys::POWER_DETAILS] = "电源详情";
    translations[TranslationKeys::NO_BATTERY_DETECTED] = "未检测到电池";
    translations[TranslationKeys::CAPACITY] = "容量";
    translations[TranslationKeys::TECHNOLOGY] = "技术";
    translations[TranslationKeys::CHARGE_CYCLES] = "充电周期";
    translations[TranslationKeys::WARNING_LEVEL] = "警告级别";
    translations[TranslationKeys::ENERGY_FULL] = "满电能量";
    translations[TranslationKeys::ENERGY_DESIGN] = "设计能量";
    translations[TranslationKeys::ENERGY_RATE] = "能量速率";
    translations[TranslationKeys::VOLTAGE] = "电压";
    
    // Power Manager Additional
    translations[TranslationKeys::PERFORMANCE_MODE] = "性能";
    translations[TranslationKeys::BALANCED_MODE] = "平衡";
    translations[TranslationKeys::DEFAULT_MODE] = "默认";
    translations[TranslationKeys::PERFORMANCE_DESCRIPTION] = "为高要求任务提供最大CPU性能";
    translations[TranslationKeys::BALANCED_DESCRIPTION] = "性能与节能之间的优化平衡";
    translations[TranslationKeys::DEFAULT_DESCRIPTION] = "系统默认电源管理";

}
