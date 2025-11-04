#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gtk/gtk.h>
#include <string>
#include <vector>
#include <memory>
#include "components/AboutManager.h"
#include "components/StorageManager.h"
#include "components/NetworkManager.h"
#include "components/BluetoothManager.h"
#include "components/SoundManager.h"
#include "components/AppearanceManager.h"
#include "components/BatteryManager.h"
#include "components/DisplayManager.h"
#include "components/PowerManager.h"
#include "components/ApplicationsManager.h"
#include "components/LanguageManager.h"
#include "translations/translations.h"

struct SettingsTile {
    std::string name;
    std::string imagePath;
    int x, y;
    int width, height;
};

class MainWindow {
public:
    MainWindow(GtkApplication* app);
    MainWindow(GtkApplication* app, const std::string& section);
    ~MainWindow();
    void show();
    void showMainMenu();
    void hideMainMenu();
    void switchToBackground(const std::string& backgroundName);
    void openSection(const std::string& section);

private:
    GtkWidget* window;
    GtkWidget* overlay;
    GtkWidget* fixed;
    GtkWidget* background;
    GtkWidget* exitButton;
    GtkWidget* themeButton;
    GtkWidget* hoverInfoOverlay;
    std::string currentBackground;
    std::vector<SettingsTile> tiles;
    std::vector<GtkWidget*> tileWidgets;
    
    // Theme state
    enum class Theme { ElysianRealm, Amphoreus };
    Theme currentTheme;
    AboutManager* aboutManager;
    StorageManager* storageManager;
    NetworkManager* networkManager;
    BluetoothManager* bluetoothManager;
    SoundManager* soundManager;
    AppearanceManager* appearanceManager;
    BatteryManager* batteryManager;
    DisplayManager* displayManager;
    PowerManager* powerManager;
    ApplicationsManager* applicationsManager;
    LanguageManager* languageManager;
    
    void setupWindow();
    void loadBackground();
    void setupTiles();
    void setupExitButton();
    void setupThemeButton();
    void createTile(const SettingsTile& tile, const std::pair<int, int>& labelPosition);
    void setupThemeData();
    void switchTheme();
    void rebuildForTheme();
    void setupElysianRealmTheme();
    void setupAmphoreusTheme();
    
    // Event handlers
    static void onTileClicked(GtkGestureClick* gesture, int n_press, double x, double y, gpointer data);
    static void onTileEnter(GtkEventControllerMotion* controller, double x, double y, gpointer data);
    static void onTileLeave(GtkEventControllerMotion* controller, gpointer data);
    static void onExitButtonClicked(GtkButton* button, gpointer user_data);
    static void onThemeButtonClicked(GtkButton* button, gpointer user_data);
    
    // Utility functions
    std::string getAssetPath(const std::string& filename);
    void initializeTranslations();
    void showHoverInfo(const std::string& tileName);
    void hideHoverInfo();
    Theme getInitialTheme();

    // Shatter effect
    struct ShatterFragment {
        GtkWidget* widget;
        double startX, startY;
        double targetX, targetY;
        double rotation;
        double rotationSpeed;
        double velocityX, velocityY;
        double gravity;
        guint64 startTime;
    };
    
    class ShatterEffect {
    public:
        ShatterEffect(MainWindow* mainWindow, GtkWidget* tileWidget, const SettingsTile& tile);
        ~ShatterEffect();
        void startShatter();
        
    private:
        MainWindow* mainWindow;
        GtkWidget* originalTile;
        SettingsTile tileData;
        std::vector<ShatterFragment> fragments;
        guint animationTimeoutId;
        guint64 effectStartTime;
        
        void createFragments();
        static gboolean animateFragments(gpointer user_data);
        void cleanupEffect();
    };
    
    std::unique_ptr<ShatterEffect> currentShatterEffect;
    bool isAnimating;
    
    // Bouncing animation
    struct BouncingTile {
        GtkWidget* widget;
        int baseX, baseY;
        double bounceOffset;
        int bounceDir;
        guint bounceTimerId;
    };
    std::vector<BouncingTile> bouncingTiles;
    
    // Image bouncing animation (separate from position bouncing)
    struct ImageBouncingTile {
        GtkWidget* widget;
        double scaleOffset;
        int scaleDir;
        guint imageBounceTimerId;
    };
    std::vector<ImageBouncingTile> imageBouncingTiles;
    
    // Bouncing animation methods
    static gboolean updateBounceAnimation(gpointer user_data);
    void startBounceAnimation(GtkWidget* tileWidget, int baseX, int baseY);
    void stopBounceAnimation(GtkWidget* tileWidget);
    
    // Image bouncing animation methods
    static gboolean updateImageBounceAnimation(gpointer user_data);
    void startImageBounceAnimation(GtkWidget* tileWidget);
    void stopImageBounceAnimation(GtkWidget* tileWidget);
};

#endif // MAINWINDOW_H
