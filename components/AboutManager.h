#ifndef ABOUTMANAGER_H
#define ABOUTMANAGER_H

#include <gtk/gtk.h>
#include <vector>
#include <string>

class MainWindow; // Forward declaration

class AboutManager {
public:
    AboutManager(MainWindow* mainWindow, GtkWidget* parentWindow, GtkWidget* overlay);
    ~AboutManager();
    
    void show();
    void hide();
    
private:
    MainWindow* mainWindow;
    GtkWidget* parentWindow;
    GtkWidget* overlay;
    GtkWidget* aboutContainer;
    GtkWidget* backButton;
    GtkWidget* background;
    std::vector<GtkWidget*> infoLabels;
    
    void setupUI();
    void loadBackground();
    void setupBackButton();
    void loadSystemInfo();
    
    // Utility functions
    std::string getAssetPath(const std::string& filename);
    std::vector<std::string> getFastfetchOutput();
};

#endif // ABOUTMANAGER_H