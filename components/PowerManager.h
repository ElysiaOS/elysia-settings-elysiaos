#pragma once

#include <gtk/gtk.h>
#include <string>
#include <vector>
#include <map>
#include <memory>

class MainWindow;

struct PowerMode {
    std::string key;
    std::string label;
    std::string description;
    std::string iconFile;
    std::string command;
};

struct PowerModeWidget {
    GtkWidget* container;
    GtkWidget* frame;
    GtkWidget* image;
    GtkWidget* label;
    GtkWidget* checkmark;
    bool isSelected;
};

class PowerManager {
public:
    PowerManager(MainWindow* mainWindow, GtkWindow* parentWindow, GtkWidget* overlay);
    ~PowerManager();
    
    void show();
    void hide();

private:
    MainWindow* mainWindow;
    GtkWindow* parentWindow;
    GtkWidget* overlay;
    GtkWidget* powerContainer;
    GtkWidget* backButton;
    GtkWidget* titleLabel;
    GtkWidget* modesContainer;
    GtkWidget* infoContainer;
    GtkWidget* applyButton;
    
    std::vector<PowerMode> powerModes;
    std::map<std::string, PowerModeWidget> modeWidgets;
    std::string selectedMode;
    std::string currentMode;
    
    guint detectionTimer;
    bool isDetecting;
    
    void setupUI();
    void initPowerUI();
    void setupBackButton();
    void createPowerModes();
    void createInfoSection();
    void setupApplyButton();
    void updatePowerModeTranslations();
    
    void detectCurrentMode();
    void selectMode(const std::string& mode);
    void updateModeVisuals();
    void applyPowerMode();
    
    std::string executeCommand(const std::string& command);
    std::string getAssetPath(const std::string& filename);
    bool fileExists(const std::string& path);
    
    // Static callbacks
    static void onBackButtonClicked(GtkButton* button, gpointer user_data);
    static void onModeClicked(GtkGestureClick* gesture, int n_press, double x, double y, gpointer user_data);
    static void onApplyButtonClicked(GtkButton* button, gpointer user_data);
    static gboolean onDetectionTimer(gpointer user_data);
    
    // Callback data structures
    struct PowerBackButtonCallbackData {
        PowerManager* manager;
    };
    
    struct PowerModeCallbackData {
        PowerManager* manager;
        std::string modeKey;
    };
    
    std::vector<GtkWidget*> powerWidgets;
    std::vector<PowerBackButtonCallbackData*> backButtonCallbacks;
    std::vector<PowerModeCallbackData*> modeCallbacks;
};


