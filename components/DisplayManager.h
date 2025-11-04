#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <gtk/gtk.h>
#include <string>
#include <vector>
#include <map>

class MainWindow;

struct DisplayMode {
    std::string resolution;
    std::string refreshRate;
    std::string fullMode; // e.g., "1920x1080@60"
};

class DisplayManager {
public:
    DisplayManager(MainWindow* mainWindow, GtkWindow* parentWindow, GtkWidget* overlay);
    ~DisplayManager();
    
    void show();
    void hide();
    
    // Static callback functions
    static void onBackButtonClicked(GtkButton* button, gpointer user_data);
    static void onBrightnessChanged(GtkRange* range, gpointer user_data);
    static void onResolutionChanged(GtkComboBox* combo, gpointer user_data);
    static void onApplyButtonClicked(GtkButton* button, gpointer user_data);
    static gboolean onRefreshTimer(gpointer user_data);

private:
    MainWindow* mainWindow;
    GtkWindow* parentWindow;
    GtkWidget* overlay;
    GtkWidget* displayContainer;
    GtkWidget* backButton;
    GtkWidget* titleLabel;
    GtkWidget* themeImageLabel;
    GtkWidget* resolutionLabel;
    GtkWidget* resolutionCombo;
    GtkWidget* brightnessLabel;
    GtkWidget* brightnessScale;
    GtkWidget* brightnessValueLabel;
    GtkWidget* applyButton;
    GtkWidget* infoContainer;
    
    std::vector<GtkWidget*> displayWidgets; // For cleanup
    std::vector<DisplayMode> availableModes;
    std::string currentMonitorName;
    guint refreshTimer;
    bool userHasMadeSelection; // Simple flag to track user selection
    
    void setupUI();
    void initDisplayUI();
    void setupBackButton();
    void createDisplayControls();
    void createInfoSection();
    void updateDisplayModes();
    void updateBrightnessSlider();
    void updateThemeImage();
    void setBrightness(int value);
    void applyDisplayChanges();
    
    // System interaction functions
    std::string detectThemeImage();
    std::vector<DisplayMode> getMonitorModes();
    std::string getCurrentMode();
    int getCurrentBrightness();
    int getMaxBrightness();
    std::string executeCommand(const std::string& command);
    bool writeHyprlandConfig(const std::string& newMode);
    
    // Utility functions
    std::string getAssetPath(const std::string& filename);
    bool fileExists(const std::string& path);
};

#endif // DISPLAYMANAGER_H



