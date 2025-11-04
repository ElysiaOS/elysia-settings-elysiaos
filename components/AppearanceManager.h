#ifndef APPEARANCEMANAGER_H
#define APPEARANCEMANAGER_H

#include <gtk/gtk.h>
#include <string>
#include <vector>
#include <map>

class MainWindow;

struct ThemeOption {
    std::string name;
    std::string displayName;
    std::string imagePath;
    bool isSelected;
};

class AppearanceManager {
public:
    AppearanceManager(MainWindow* mainWindow, GtkWindow* parentWindow, GtkWidget* overlay);
    ~AppearanceManager();
    
    void show();
    void hide();
    
    // Static callback functions
    static void onBackButtonClicked(GtkButton* button, gpointer user_data);
    static void onThemeClicked(GtkGestureClick* gesture, int n_press, double x, double y, gpointer user_data);
    static void onPreviousWallpaperClicked(GtkButton* button, gpointer user_data);
    static void onNextWallpaperClicked(GtkButton* button, gpointer user_data);
    static void onApplyWallpaperClicked(GtkButton* button, gpointer user_data);
    static void onBrowseWallpapersClicked(GtkButton* button, gpointer user_data);
    static gboolean onThemeDetectionTimer(gpointer user_data);

private:
    MainWindow* mainWindow;
    GtkWindow* parentWindow;
    GtkWidget* overlay;
    GtkWidget* appearanceContainer;
    GtkWidget* backButton;
    GtkWidget* titleLabel;
    GtkWidget* themeContainer;
    GtkWidget* wallpaperPreview;
    GtkWidget* wallpaperLabel;
    GtkWidget* previousButton;
    GtkWidget* nextButton;
    GtkWidget* applyButton;
    GtkWidget* browseButton;
    
    std::vector<ThemeOption> themes;
    std::map<std::string, GtkWidget*> themeWidgets;
    std::vector<std::string> wallpapers;
    std::string selectedTheme;
    std::string selectedWallpaper;
    int currentWallpaperIndex;
    guint themeDetectionTimer;
    std::string lastDetectedTheme;
    
    std::vector<GtkWidget*> appearanceWidgets; // For cleanup
    
    void setupUI();
    void initAppearanceUI();
    void setupBackButton();
    void createThemeOptions();
    void setupWallpaperPreview();
    void setupButtons();
    
    // Theme and wallpaper management
    std::string detectSystemTheme();
    void selectTheme(const std::string& themeName);
    void updateThemeVisuals();
    void loadWallpapers();
    void updateWallpaperPreview();
    void previousWallpaper();
    void nextWallpaper();
    void applyWallpaper();
    void browseWallpapers();
    void runThemeScript(const std::string& themeName);
    
    // Utility functions
    std::string getAssetPath(const std::string& filename);
    std::string getWallpaperPath(const std::string& theme, const std::string& wallpaper = "");
    std::string executeCommand(const std::string& command);
    bool fileExists(const std::string& path);
};

#endif // APPEARANCEMANAGER_H
