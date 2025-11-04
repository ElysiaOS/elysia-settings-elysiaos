#ifndef BATTERYMANAGER_H
#define BATTERYMANAGER_H

#include <gtk/gtk.h>
#include <string>
#include <vector>
#include <map>

class MainWindow;

struct BatteryInfo {
    std::string state;
    std::string warningLevel;
    std::string energyFull;
    std::string energyFullDesign;
    std::string energyRate;
    std::string voltage;
    std::string chargeCycles;
    std::string percentage;
    std::string capacity;
    std::string technology;
    std::string timeToEmpty;
    std::string timeToFull;
    bool isCharging;
    int percentageValue;
};

class BatteryManager {
public:
    BatteryManager(MainWindow* mainWindow, GtkWindow* parentWindow, GtkWidget* overlay);
    ~BatteryManager();
    
    void show();
    void hide();
    
    // Static callback functions
    static void onBackButtonClicked(GtkButton* button, gpointer user_data);
    static gboolean onUpdateTimer(gpointer user_data);

private:
    MainWindow* mainWindow;
    GtkWindow* parentWindow;
    GtkWidget* overlay;
    GtkWidget* batteryContainer;
    GtkWidget* backButton;
    GtkWidget* titleLabel;
    GtkWidget* batteryIcon;
    GtkWidget* percentageLabel;
    GtkWidget* statusLabel;
    GtkWidget* progressBar;
    GtkWidget* infoContainer;
    GtkWidget* healthContainer;
    GtkWidget* powerContainer;
    
    std::vector<GtkWidget*> batteryWidgets; // For cleanup
    std::string batteryDevice;
    BatteryInfo currentInfo;
    guint updateTimer;
    
    void setupUI();
    void initBatteryUI();
    void setupBackButton();
    void createBatteryDisplay();
    void createInfoSections();
    void updateBatteryDisplay();
    void updateInfoSections();
    
    // Battery information functions
    std::string findBatteryDevice();
    BatteryInfo parseBatteryInfo(const std::string& device);
    std::string executeCommand(const std::string& command);
    std::string formatBatteryTime(const std::string& timeStr);
    std::string getBatteryIconName(int percentage, bool isCharging);
    
    // Utility functions
    std::string getAssetPath(const std::string& filename);
    bool fileExists(const std::string& path);
};

#endif // BATTERYMANAGER_H
