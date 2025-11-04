#include "ja_JP.h"

void loadJapaneseTranslations(std::map<TranslationKeys, std::string>& translations) {
    // Main Window
    translations[TranslationKeys::WINDOW_TITLE] = "ElysiaOS 設定";
    translations[TranslationKeys::EXIT] = "終了";
    
    // Common UI
    translations[TranslationKeys::BACK] = "戻る";
    translations[TranslationKeys::APPLY_CHANGES] = "変更を適用";
    translations[TranslationKeys::REFRESH] = "更新";
    
    // Tile Names
    translations[TranslationKeys::TILE_CAR] = "車";
    translations[TranslationKeys::TILE_BATTERY] = "バッテリー";
    translations[TranslationKeys::TILE_APPEARANCE] = "外観";
    translations[TranslationKeys::TILE_POWER] = "電源";
    translations[TranslationKeys::TILE_BLUETOOTH] = "Bluetooth";
    translations[TranslationKeys::TILE_SOUND] = "サウンド";
    translations[TranslationKeys::TILE_ABOUT] = "システム情報";
    translations[TranslationKeys::TILE_DISPLAY] = "ディスプレイ";
    translations[TranslationKeys::TILE_NETWORK] = "ネットワーク";
    translations[TranslationKeys::TILE_UPDATES] = "アップデート";
    translations[TranslationKeys::TILE_HYPERLAND] = "ハイパーランド";
    translations[TranslationKeys::TILE_STORAGE] = "ストレージ";
    translations[TranslationKeys::TILE_SUPPORT] = "サポート";
    
    // About Manager
    translations[TranslationKeys::ABOUT_TITLE] = "システム情報";
    translations[TranslationKeys::SYSTEM_INFO] = "システム情報";
    translations[TranslationKeys::OS_NAME] = "オペレーティングシステム";
    translations[TranslationKeys::KERNEL_VERSION] = "カーネルバージョン";
    translations[TranslationKeys::UPTIME] = "システム稼働時間";
    translations[TranslationKeys::CPU_INFO] = "プロセッサー";
    translations[TranslationKeys::MEMORY_INFO] = "メモリ";
    translations[TranslationKeys::DISK_USAGE] = "ディスク使用量";
    
    // Battery Manager
    translations[TranslationKeys::BATTERY_TITLE] = "バッテリー状態";
    translations[TranslationKeys::BATTERY_STATUS] = "バッテリー状態";
    translations[TranslationKeys::BATTERY_HEALTH] = "バッテリーの健康状態";
    translations[TranslationKeys::CHARGING] = "充電中";
    translations[TranslationKeys::DISCHARGING] = "放電中";
    translations[TranslationKeys::FULL] = "満充電";
    translations[TranslationKeys::NOT_CHARGING] = "充電していません";
    
    // Display Manager
    translations[TranslationKeys::DISPLAY_TITLE] = "ディスプレイ設定";
    translations[TranslationKeys::DISPLAY_SETTINGS] = "ディスプレイ設定";
    translations[TranslationKeys::DISPLAY_RESOLUTION] = "ディスプレイ解像度";
    translations[TranslationKeys::SCREEN_BRIGHTNESS] = "画面の明るさ";
    translations[TranslationKeys::DISPLAY_INFORMATION] = "ディスプレイ情報";
    translations[TranslationKeys::DISPLAY_INFO_TEXT] = "変更はHyprland設定に適用されます。\nディスプレイ解像度の変更を有効にするには、Hyprlandを再起動してください。";
    
    // Sound Manager
    translations[TranslationKeys::SOUND_TITLE] = "サウンド";
    translations[TranslationKeys::OUTPUT_DEVICES] = "出力デバイス";
    translations[TranslationKeys::INPUT_DEVICES] = "入力デバイス";
    translations[TranslationKeys::VOLUME] = "音量";
    translations[TranslationKeys::PLAYBACK_APPLICATIONS] = "再生アプリケーション";
    
    // Network Manager
    translations[TranslationKeys::NETWORK_TITLE] = "ネットワーク";
    translations[TranslationKeys::WIFI_NETWORKS] = "WiFi ネットワーク";
    translations[TranslationKeys::WIFI] = "Wi-Fi";
    translations[TranslationKeys::CONNECTION_STATUS] = "接続状態";
    translations[TranslationKeys::CONNECT] = "接続";
    translations[TranslationKeys::DISCONNECT] = "切断";
    translations[TranslationKeys::CONNECTED] = "接続済み";
    translations[TranslationKeys::DISCONNECTED] = "切断済み";
    translations[TranslationKeys::ENTER_PASSWORD] = "パスワードを入力";
    translations[TranslationKeys::CONNECTING] = "接続中...";
    translations[TranslationKeys::CANCEL] = "キャンセル";
    translations[TranslationKeys::REFRESH_NETWORKS] = "ネットワークを更新";
    translations[TranslationKeys::NETWORKMANAGER_NOT_AVAILABLE] = "NetworkManagerが利用できません";
    translations[TranslationKeys::NETWORKING_DISABLED] = "ネットワークが無効です";
    translations[TranslationKeys::ENABLE_NETWORKING] = "ネットワークを有効にする";
    translations[TranslationKeys::CONNECTED_VIA_ETHERNET] = "イーサネット経由で接続";
    translations[TranslationKeys::WIFI_HARDWARE_DISABLED] = "Wi-Fiハードウェアが無効です";
    translations[TranslationKeys::WIFI_DISABLED] = "Wi-Fiが無効です";
    translations[TranslationKeys::NO_NETWORKS_FOUND] = "ネットワークが見つかりません";
    translations[TranslationKeys::IPV4_METHOD] = "IPv4方式：";
    translations[TranslationKeys::IP_ADDRESS] = "IPアドレス：";
    translations[TranslationKeys::NETMASK] = "ネットマスク：";
    translations[TranslationKeys::GATEWAY] = "ゲートウェイ：";
    translations[TranslationKeys::DNS_SERVERS] = "DNSサーバー：";
    translations[TranslationKeys::IPV6_METHOD] = "IPv6方式：";
    translations[TranslationKeys::IPV6_DNS_SERVERS] = "IPv6 DNSサーバー：";
    
    // Bluetooth Manager
    translations[TranslationKeys::BLUETOOTH_TITLE] = "Bluetooth";
    translations[TranslationKeys::BLUETOOTH_DEVICES] = "Bluetoothデバイス";
    translations[TranslationKeys::DEVICES] = "デバイス";
    translations[TranslationKeys::OK] = "OK";
    translations[TranslationKeys::PAIR] = "ペアリング";
    translations[TranslationKeys::UNPAIR] = "ペアリング解除";
    translations[TranslationKeys::PAIRED] = "ペアリング済み";
    translations[TranslationKeys::AVAILABLE] = "利用可能";
    translations[TranslationKeys::TOGGLE_BLUETOOTH] = "Bluetoothの切り替え";
    translations[TranslationKeys::SCAN_DEVICES] = "デバイスをスキャン";
    translations[TranslationKeys::BLUETOOTH_DISABLED] = "Bluetoothが無効です";
    translations[TranslationKeys::NO_DEVICES_FOUND_BT] = "デバイスが見つかりません - スキャンしてください";
    translations[TranslationKeys::SCANNING] = "スキャン中...";
    translations[TranslationKeys::SCANNING_FOR_DEVICES] = "デバイスをスキャン中...";
    translations[TranslationKeys::BLUETOOTH_ENABLED_NO_DEVICES] = "Bluetooth有効 - デバイス未接続";
    translations[TranslationKeys::CONNECTED_TO] = "接続先：";
    translations[TranslationKeys::CONNECTED_TO_MULTIPLE] = "接続中 ";
    translations[TranslationKeys::TURN_OFF] = "オフ";
    translations[TranslationKeys::TURN_ON] = "オン";
    translations[TranslationKeys::DISCONNECT_BLU] = "切断";
    translations[TranslationKeys::CONNECT_BLU] = "接続";
    translations[TranslationKeys::FORGET] = "削除";
    translations[TranslationKeys::SUCCESS_BLU] = "成功";
    translations[TranslationKeys::DISCONNECTED_BLU] = "切断済み";
    translations[TranslationKeys::REMOVED] = "削除済み";
    translations[TranslationKeys::CONNECTED_STATUS] = " - 接続済み";
    translations[TranslationKeys::PAIRED_STATUS] = " - ペアリング済み";
    translations[TranslationKeys::NOT_PAIRED_STATUS] = " - 未ペアリング";
    
    // Appearance Manager
    translations[TranslationKeys::APPEARANCE_TITLE] = "外観";
    translations[TranslationKeys::THEME_SETTINGS] = "テーマ設定";
    translations[TranslationKeys::THEME_SELECTION] = "テーマ選択";
    translations[TranslationKeys::WALLPAPER] = "壁紙";
    translations[TranslationKeys::WALLPAPER_SELECTION] = "壁紙選択";
    translations[TranslationKeys::COLORS] = "色";
    translations[TranslationKeys::LIGHT] = "ライト";
    translations[TranslationKeys::DARK] = "ダーク";
    translations[TranslationKeys::BROWSE] = "参照";
    translations[TranslationKeys::PREVIOUS_WALLPAPER] = "前の壁紙";
    translations[TranslationKeys::NEXT_WALLPAPER] = "次の壁紙";
    translations[TranslationKeys::OPEN_WALLPAPER_FOLDER] = "壁紙フォルダを開く";
    translations[TranslationKeys::SET_THIS_WALLPAPER] = "この壁紙を設定";
    
    // Power Manager
    translations[TranslationKeys::POWER_TITLE] = "電源";
    translations[TranslationKeys::POWER_SETTINGS] = "電源設定";
    translations[TranslationKeys::SLEEP] = "スリープ";
    translations[TranslationKeys::HIBERNATE] = "休止状態";
    translations[TranslationKeys::SHUTDOWN] = "シャットダウン";
    translations[TranslationKeys::RESTART] = "再起動";
    translations[TranslationKeys::PERFORMANCE] = "パフォーマンス";
    translations[TranslationKeys::BALANCED] = "バランス";
    translations[TranslationKeys::DEFAULT] = "デフォルト";
    translations[TranslationKeys::POWER_PROFILE_INFORMATION] = "電源プロファイル情報";
    translations[TranslationKeys::POWER_PROFILE_INFO_TEXT] = "変更には管理者権限が必要で、すぐに適用されます。\n現在の電源プロファイルは、このメニューを開く際に自動的に検出されます。";
    translations[TranslationKeys::MISSING] = "不明";
    
    // Storage Manager
    translations[TranslationKeys::STORAGE_TITLE] = "ストレージ";
    translations[TranslationKeys::DISK_USAGE_TITLE] = "ディスク使用量";
    translations[TranslationKeys::AVAILABLE_SPACE] = "利用可能領域";
    translations[TranslationKeys::USED_SPACE] = "使用済み領域";
    
    // Applications Manager
    translations[TranslationKeys::APPLICATIONS_TITLE] = "HYPRLAND アプリケーション";
    translations[TranslationKeys::KEYBINDS] = "キーバインド";
    translations[TranslationKeys::AUTORUNS] = "自動実行";
    translations[TranslationKeys::WINDOW_RULES] = "ウィンドウルール";
    
    // Language Manager
    translations[TranslationKeys::LANGUAGE_SETTINGS] = "言語設定";
    translations[TranslationKeys::SYSTEM_LANGUAGE] = "システム言語";
    translations[TranslationKeys::LANGUAGE_INFORMATION] = "言語情報";
    translations[TranslationKeys::LANGUAGE_INFO_TEXT] = "変更はログアウト後に適用されます。\nセッションを再起動する必要があります。";
    
    // Messages
    translations[TranslationKeys::CHECKING_STATUS] = "状態を確認中...";
    translations[TranslationKeys::CHECKING_BATTERY_STATUS] = "バッテリー状態を確認中...";
    translations[TranslationKeys::LOADING] = "読み込み中...";
    translations[TranslationKeys::ERROR] = "エラー";
    translations[TranslationKeys::SUCCESS] = "成功";
    translations[TranslationKeys::WARNING] = "警告";
    translations[TranslationKeys::NO_SYSTEM_INFO] = "利用可能なシステム情報がありません";
    translations[TranslationKeys::OUTPUT_DEVICES] = "出力デバイス";
    translations[TranslationKeys::INPUT_DEVICES] = "入力デバイス";
    translations[TranslationKeys::APPLICATION_VOLUME] = "アプリケーション音量";
    translations[TranslationKeys::NO_AUDIO_APPS] = "現在オーディオを再生しているアプリケーションはありません";
    
    // Dialog Messages
    translations[TranslationKeys::LOGOUT_REQUIRED] = "ログアウトが必要です";
    translations[TranslationKeys::LOGOUT_MESSAGE] = "言語の変更を有効にするにはログアウトが必要です。\n今すぐログアウトしますか？";
    translations[TranslationKeys::LOGOUT_NOW] = "今すぐログアウト";
    translations[TranslationKeys::LOGOUT_LATER] = "後で";

    // Network Manager Additional
    translations[TranslationKeys::NETWORK_SETTINGS] = "ネットワーク設定";
    translations[TranslationKeys::ENTER_NETWORK_PASSWORD] = "ネットワークパスワードを入力";
    translations[TranslationKeys::IPV4_METHOD_LABEL] = "IPv4メソッド：";
    translations[TranslationKeys::IP_ADDRESS_LABEL] = "IPアドレス：";
    translations[TranslationKeys::NETMASK_LABEL] = "ネットマスク：";
    translations[TranslationKeys::GATEWAY_LABEL] = "ゲートウェイ：";
    translations[TranslationKeys::DNS_SERVERS_LABEL] = "DNSサーバー：";
    translations[TranslationKeys::IPV6_METHOD_LABEL] = "IPv6メソッド：";
    translations[TranslationKeys::IPV6_DNS_SERVERS_LABEL] = "IPv6 DNSサーバー：";
    translations[TranslationKeys::PORT_SETTINGS] = "ポート設定：";
    translations[TranslationKeys::HTTP_PROXY_PORT] = "HTTPプロキシポート：";
    translations[TranslationKeys::HTTPS_PROXY_PORT] = "HTTPSプロキシポート：";
    translations[TranslationKeys::FTP_PROXY_PORT] = "FTPプロキシポート：";
    translations[TranslationKeys::SOCKS_PROXY_PORT] = "SOCKSプロキシポート：";
    translations[TranslationKeys::CUSTOM_PORT_RANGE] = "カスタムポート範囲：";
    translations[TranslationKeys::PORT_DASH] = " - ";
    translations[TranslationKeys::CONNECTED_TO_PREFIX] = "接続先：";
    translations[TranslationKeys::ETHERNET] = "イーサネット";
    translations[TranslationKeys::CONNECTING_STATUS] = "接続中...";
    translations[TranslationKeys::NETWORKING_DISABLED_STATUS] = "ネットワークが無効";
    translations[TranslationKeys::WIFI_DISABLED_STATUS] = "Wi-Fiが無効";
    translations[TranslationKeys::NOT_CONNECTED_STATUS] = "未接続";
    
    // Navigation Buttons
    translations[TranslationKeys::PREVIOUS_ARROW] = "‹";
    translations[TranslationKeys::NEXT_ARROW] = "›";
    translations[TranslationKeys::LEFT_ARROW] = "<";
    translations[TranslationKeys::RIGHT_ARROW] = ">";
    translations[TranslationKeys::ADD_BUTTON] = "+";
    translations[TranslationKeys::REMOVE_BUTTON] = "−";
    
    // Language Manager
    translations[TranslationKeys::LANGUAGE_CHANGED] = "言語が変更されました";
    
    // Display Manager
    translations[TranslationKeys::BRIGHTNESS_VALUE] = "50%";
    
    // Sound Manager
    translations[TranslationKeys::VOLUME_VALUE] = "0%";
    
    // Battery Manager Additional
    translations[TranslationKeys::POWER_DETAILS] = "電源詳細";
    translations[TranslationKeys::NO_BATTERY_DETECTED] = "バッテリーが検出されません";
    translations[TranslationKeys::CAPACITY] = "容量";
    translations[TranslationKeys::TECHNOLOGY] = "技術";
    translations[TranslationKeys::CHARGE_CYCLES] = "充電サイクル";
    translations[TranslationKeys::WARNING_LEVEL] = "警告レベル";
    translations[TranslationKeys::ENERGY_FULL] = "フルエネルギー";
    translations[TranslationKeys::ENERGY_DESIGN] = "設計エネルギー";
    translations[TranslationKeys::ENERGY_RATE] = "エネルギー率";
    translations[TranslationKeys::VOLTAGE] = "電圧";
    
    // Power Manager Additional
    translations[TranslationKeys::PERFORMANCE_MODE] = "パフォーマンス";
    translations[TranslationKeys::BALANCED_MODE] = "バランス";
    translations[TranslationKeys::DEFAULT_MODE] = "デフォルト";
    translations[TranslationKeys::PERFORMANCE_DESCRIPTION] = "要求の厳しいタスクのための最大CPUパフォーマンス";
    translations[TranslationKeys::BALANCED_DESCRIPTION] = "パフォーマンスと省電力の最適化されたバランス";
    translations[TranslationKeys::DEFAULT_DESCRIPTION] = "システムデフォルトの電源管理";

}
