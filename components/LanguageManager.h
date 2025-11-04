#ifndef LANGUAGEMANAGER_H
#define LANGUAGEMANAGER_H

#include <gtk/gtk.h>
#include <string>
#include <vector>
#include <map>

class MainWindow;

struct Language {
    std::string code;      // e.g., "en_US.UTF-8"
    std::string name;      // e.g., "English (United States)"
    std::string display;   // e.g., "English (United States) - en_US.UTF-8"
};

class LanguageManager {
public:
    LanguageManager(MainWindow* mainWindow, GtkWindow* parentWindow, GtkWidget* overlay);
    ~LanguageManager();
    
    void show();
    void hide();
    
    // Static callback functions
    static void onBackButtonClicked(GtkButton* button, gpointer user_data);
    static void onLanguageChanged(GtkComboBox* combo, gpointer user_data);
    static void onApplyButtonClicked(GtkButton* button, gpointer user_data);
    static void onLogoutDialogResponse(GtkDialog* dialog, gint response_id, gpointer user_data);

private:
    MainWindow* mainWindow;
    GtkWindow* parentWindow;
    GtkWidget* overlay;
    GtkWidget* languageContainer;
    GtkWidget* backButton;
    GtkWidget* titleLabel;
    GtkWidget* themeImageLabel;
    GtkWidget* languageLabel;
    GtkWidget* languageCombo;
    GtkWidget* applyButton;
    GtkWidget* infoContainer;
    
    std::vector<GtkWidget*> languageWidgets; // For cleanup
    std::vector<Language> availableLanguages;
    std::string currentLanguage;
    
    void setupUI();
    void initLanguageUI();
    void setupBackButton();
    void createLanguageControls();
    void createInfoSection();
    void updateLanguageList();
    void applyLanguageChanges();
    void showLogoutDialog();
    
    // System interaction functions
    std::string detectThemeImage();
    std::vector<Language> getAvailableLanguages();
    std::string getCurrentLanguage();
    std::string executeCommand(const std::string& command);
    bool writeLocaleConfig(const std::string& newLanguage);
    void logoutSession();
    
    // Utility functions
    std::string getAssetPath(const std::string& filename);
    bool fileExists(const std::string& path);
    std::string getLanguageName(const std::string& code);
    bool isValidLanguageCode(const std::string& code);
};

#endif // LANGUAGEMANAGER_H
