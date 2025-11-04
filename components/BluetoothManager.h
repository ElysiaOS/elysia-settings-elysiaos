#pragma once

#include <gtk/gtk.h>
#include <string>
#include <vector>
#include <iostream>

// Forward declaration
class MainWindow;

struct BluetoothDevice {
    std::string mac;
    std::string name;
    bool isConnected;
    bool isPaired;
};

class BluetoothManager {
public:
    BluetoothManager(MainWindow* mainWindow, GtkWidget* parentWindow, GtkWidget* overlay);
    ~BluetoothManager();
    
    void show();
    void hide();

private:
    // UI setup
    void setupUI();
    void initBluetoothUI();
    void setupBackButton();
    void populateDeviceList();
    void buildDeviceRow(const BluetoothDevice& device);
    void updateBluetoothStatus();
    
    // Bluetooth utility functions
    bool getBluetoothPowerState();
    std::vector<BluetoothDevice> getBluetoothDevices();
    bool isDeviceConnected(const std::string& mac);
    bool isDevicePaired(const std::string& mac);
    void toggleBluetoothPower();
    void scanForDevices();
    void connectToDevice(const std::string& mac, const std::string& name);
    void disconnectDevice(const std::string& mac, const std::string& name);
    void forgetDevice(const std::string& mac, const std::string& name);
    std::string executeBluetoothCommand(const std::vector<std::string>& args);
    void showMessage(const std::string& title, const std::string& message);
    
    // Event handlers
    static void onBackButtonClicked(GtkGestureClick* gesture, int n_press, double x, double y, gpointer user_data);
    static void onToggleBluetoothClicked(GtkButton* button, gpointer user_data);
    static void onScanDevicesClicked(GtkButton* button, gpointer user_data);
    static void onConnectButtonClicked(GtkButton* button, gpointer user_data);
    static void onDisconnectButtonClicked(GtkButton* button, gpointer user_data);
    static void onForgetButtonClicked(GtkButton* button, gpointer user_data);
    static gboolean refreshDevicesTimeout(gpointer user_data);
    static gboolean scanCompleteTimeout(gpointer user_data);
    static void onMessageDialogResponse(GtkDialog* dialog, gint response_id, gpointer user_data);
    
    // Utility functions
    std::string getAssetPath(const std::string& filename);
    
    // Member variables
    MainWindow* mainWindow;
    GtkWidget* parentWindow;
    GtkWidget* overlay;
    GtkWidget* bluetoothContainer;
    GtkWidget* backButton;
    GtkWidget* statusLabel;
    GtkWidget* connectionStatusLabel;
    GtkWidget* toggleButton;
    GtkWidget* scanButton;
    GtkWidget* deviceListBox;
    GtkWidget* scrolledWindow;
    
    // State variables
    bool bluetoothEnabled;
    bool isScanning;
    guint refreshTimeoutId;
    guint scanTimeoutId;
    std::vector<GtkWidget*> deviceWidgets;
};
