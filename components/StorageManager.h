#ifndef STORAGEMANAGER_H
#define STORAGEMANAGER_H

#include <gtk/gtk.h>
#include <vector>
#include <string>

class MainWindow; // Forward declaration

struct DriveInfo {
    std::string device;
    std::string mountpoint;
    std::string fstype;
    long long total;
    long long used;
    long long free;
    int percent;
};

class StorageManager {
public:
    StorageManager(MainWindow* mainWindow, GtkWidget* parentWindow, GtkWidget* overlay);
    ~StorageManager();
    
    void show();
    void hide();
    void updateDisks();

private:
    MainWindow* mainWindow;
    GtkWidget* parentWindow;
    GtkWidget* overlay;
    GtkWidget* storageContainer;
    GtkWidget* backButton;
    GtkWidget* drivesContainer;
    std::vector<GtkWidget*> driveWidgets;
    
    void setupUI();
    void setupBackButton();
    void loadBackground();
    void initUI();
    
    // Utility functions
    std::string getAssetPath(const std::string& filename);
    std::vector<DriveInfo> getDiskInfo();
    std::string bytesToGB(long long bytes);
};

#endif // STORAGEMANAGER_H