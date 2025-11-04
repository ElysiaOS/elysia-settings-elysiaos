#pragma once

#include <gtk/gtk.h>
#include <string>
#include <vector>

class MainWindow;

class ApplicationsManager {
public:
    ApplicationsManager(MainWindow* mainWindow, GtkWidget* parentContainer, GtkWidget* parentFixed);
    ~ApplicationsManager();
    
    void show();
    void hide();

private:
    // Core functionality
    void setupUI();
    void setupBackButton();
    void loadConfig();
    void saveConfig();
    void rebuildLines();
    void refreshData();
    
    // Navigation
    void handleLeftArrow();
    void handleRightArrow();
    void updateConfigDisplay();
    
    // Line management
    void handleLineEdited(int idx, const std::string& newText);
    void handleAddLine(int idx);
    void handleRemoveLine(int idx);
    
    // File I/O
    std::vector<std::string> readConfigFile(const std::string& path);
    void writeConfigFile(const std::string& path, const std::vector<std::string>& lines);
    
    // Utility
    std::string trim(const std::string& str);
    
    // Static callbacks
    static void onBackButtonClicked(GtkButton* button, gpointer user_data);
    static void onLeftArrowClicked(GtkButton* button, gpointer user_data);
    static void onRightArrowClicked(GtkButton* button, gpointer user_data);
    static void onRefreshButtonClicked(GtkButton* button, gpointer user_data);
    static void onLineEditChanged(GtkEditable* editable, gpointer user_data);
    static void onAddButtonClicked(GtkButton* button, gpointer user_data);
    static void onRemoveButtonClicked(GtkButton* button, gpointer user_data);
    static gboolean onRefreshTimer(gpointer user_data);
    
    // UI Components
    MainWindow* mainWindow;
    GtkWidget* parentContainer;
    GtkWidget* parentFixed;
    GtkWidget* applicationsContainer;
    GtkWidget* backButton;
    GtkWidget* titleLabel;
    GtkWidget* headerWidget;
    GtkWidget* configLabel;
    GtkWidget* leftArrow;
    GtkWidget* rightArrow;
    GtkWidget* refreshButton;
    GtkWidget* mainBox;
    GtkWidget* scrolledWindow;
    GtkWidget* linesBox;
    
    // Configuration data
    std::vector<std::string> configFiles;
    std::vector<std::string> configNames;
    std::vector<std::string> currentLines;
    std::vector<std::string> preservedHeader;
    std::vector<GtkWidget*> lineWidgets;
    std::vector<GtkWidget*> lineEntries;
    
    int currentConfigIdx;
    guint refreshTimerId;
    
    // Line edit data structure for callbacks
    struct LineEditData {
        ApplicationsManager* manager;
        int index;
        GtkWidget* entry;
    };
    std::vector<LineEditData*> lineEditDataList;
};