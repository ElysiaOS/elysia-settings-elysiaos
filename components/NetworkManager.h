#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <gtk/gtk.h>
#include <vector>
#include <string>
#include <NetworkManager.h>

class MainWindow; // Forward declaration

struct WifiNetwork {
    std::string ssid;
    std::string security;
    int strength;
    bool isActive;
    bool isSaved;
    bool isSecured;
    NMAccessPoint* ap;
    std::string status;
};

class NetworkManager {
public:
    NetworkManager(MainWindow* mainWindow, GtkWidget* parentWindow, GtkWidget* overlay);
    ~NetworkManager();
    
    void show();
    void hide();
    void refreshNetworks();

private:
    MainWindow* mainWindow;
    GtkWidget* parentWindow;
    GtkWidget* overlay;
    GtkWidget* networkContainer;
    GtkWidget* backButton;
    GtkWidget* wifiSwitch;
    GtkWidget* wifiListBox;
    GtkWidget* refreshButton;
    GtkWidget* scrolledWindow;
    GtkWidget* connectionStatusLabel;
    
    // NetworkManager client
    NMClient* nmClient;
    std::vector<GtkWidget*> networkWidgets;
    
    // State tracking
    bool updatingWifiSwitch;
    bool networkingEnabled;
    bool hasEthernetConnection;
    guint updateTimeoutId;
    
    void setupUI();
    void setupBackButton();
    void initNetworkUI();
    void setupNetworkManager();
    void populateWifiList();
    void scanWifiNetworks();
    void reflectWifiSwitchState();
    void updateNetworkState();
    void enableNetworking();
    void buildNetworkRow(const WifiNetwork& network);
    void updateConnectionStatus();
    void connectToNetwork(const std::string& ssid, NMAccessPoint* ap, bool isSecured);
    void showPasswordDialog(const std::string& ssid, NMAccessPoint* ap);
    void showNetworkSettings(const std::string& ssid, NMActiveConnection* activeConnection);
    
    // Network utility functions
    NMDeviceWifi* getPrimaryWifiDevice();
    std::string ssidFromBytes(GBytes* ssidBytes);
    bool isAccessPointSecured(NMAccessPoint* ap);
    NMRemoteConnection* findSavedConnection(const std::string& ssid);
    std::vector<WifiNetwork> getAvailableNetworks();
    bool checkNetworkingEnabled();
    bool checkEthernetConnection();
    
    // Event handlers
    static void onBackButtonClicked(GtkGestureClick* gesture, int n_press, double x, double y, gpointer user_data);
    static gboolean onWifiSwitchStateSet(GtkSwitch* sw, gboolean state, gpointer user_data);
    static void onRefreshButtonClicked(GtkButton* button, gpointer user_data);
    static void onConnectButtonClicked(GtkButton* button, gpointer user_data);
    static void onEnableNetworkingClicked(GtkButton* button, gpointer user_data);
    static void onNetworkManagerChanged(NMClient* client, gpointer user_data);
    static void onWifiDeviceChanged(GObject* gobj, GParamSpec* pspec, gpointer user_data);
    static void onPasswordConnectClicked(GtkButton* button, gpointer user_data);
    static void onPasswordDialogDestroy(GtkWidget* dialog, gpointer user_data);
    static void onNetworkSettingsClicked(GtkButton* button, gpointer user_data);
    static void onSettingsDialogResponse(GtkDialog* dialog, gint response_id, gpointer user_data);
    static void onIPv4MethodChanged(GtkComboBox* combo, gpointer user_data);
    static void onDNSAutoToggled(GtkCheckButton* check, gpointer user_data);
    static void onIPv6MethodChanged(GtkComboBox* combo, gpointer user_data);
    static void onIPv6DNSAutoToggled(GtkCheckButton* check, gpointer user_data);
    static gboolean updateNetworkStateTimeout(gpointer user_data);
    static gboolean populateWifiListTimeout(gpointer user_data);
    static gboolean scanWifiNetworksTimeout(gpointer user_data);
    static gboolean reflectWifiSwitchStateTimeout(gpointer user_data);
    
    // Utility functions
    std::string getAssetPath(const std::string& filename);
};

#endif // NETWORKMANAGER_H
