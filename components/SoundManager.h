#pragma once

#include <gtk/gtk.h>
#include <vector>
#include <string>
#include <map>

// Forward declaration
class MainWindow;

struct AudioDevice {
    std::string name;
    std::string description;
    std::string activePort;
    std::vector<std::pair<std::string, std::string>> ports; // port name, port description
    int volume;
    bool isDefault;
};

struct PlaybackClient {
    uint32_t index;
    std::string name;
    std::string applicationName;
    int volume;
};

struct SoundManagerCallbackData {
    class SoundManager* soundManager;
    std::string deviceName;
    std::string portName;
    uint32_t index;
};

class SoundManager {
public:
    SoundManager(MainWindow* mainWindow, GtkWindow* parentWindow, GtkWidget* overlay);
    ~SoundManager();
    
    void show();
    void hide();

private:
    MainWindow* mainWindow;
    GtkWindow* parentWindow;
    GtkWidget* overlay;
    GtkWidget* soundContainer;
    GtkWidget* backButton;
    
    // UI Elements
    GtkWidget* outputLabel;
    GtkWidget* outputDeviceCombo;
    GtkWidget* outputVolumeScale;
    GtkWidget* outputVolumeLabel;
    
    GtkWidget* inputLabel;
    GtkWidget* inputDeviceCombo;
    GtkWidget* inputVolumeScale;
    GtkWidget* inputVolumeLabel;
    
    GtkWidget* playbackLabel;
    GtkWidget* playbackScrolledWindow;
    GtkWidget* playbackBox;
    
    
    // Data
    std::vector<AudioDevice> outputDevices;
    std::vector<AudioDevice> inputDevices;
    std::vector<PlaybackClient> playbackClients;
    std::map<uint32_t, GtkWidget*> clientWidgets;
    
    // Timers for debouncing
    guint outputVolumeTimer;
    guint inputVolumeTimer;
    guint refreshTimer;
    
    // Methods
    void setupUI();
    void initSoundUI();
    void setupBackButton();
    void refreshAudioDevices();
    void refreshPlaybackClients();
    void updateOutputDevices();
    void updateInputDevices();
    void updatePlaybackClientsUI();
    void setOutputDevice(const std::string& deviceName, const std::string& portName);
    void setInputDevice(const std::string& deviceName, const std::string& portName);
    void setOutputVolume(int volume);
    void setInputVolume(int volume);
    void setPlaybackClientVolume(uint32_t index, int volume);
    std::string getAssetPath(const std::string& filename);
    std::string simplifyDeviceDescription(const std::string& description);
    std::string executeCommand(const std::string& command);
    std::string getDefaultDevice(const std::string& type);
    std::vector<AudioDevice> getAudioDevices(const std::string& type);
    std::vector<PlaybackClient> getPlaybackClients();
    
    // Static callbacks
    static void onBackButtonClicked(GtkButton* button, gpointer user_data);
    static void onOutputDeviceChanged(GtkComboBox* combo, gpointer user_data);
    static void onInputDeviceChanged(GtkComboBox* combo, gpointer user_data);
    static gboolean onOutputVolumeChanged(GtkRange* range, gpointer user_data);
    static gboolean onInputVolumeChanged(GtkRange* range, gpointer user_data);
    static gboolean onPlaybackVolumeChanged(GtkRange* range, gpointer user_data);
    static gboolean onRefreshTimer(gpointer user_data);
    static gboolean onOutputVolumeDebounce(gpointer user_data);
    static gboolean onInputVolumeDebounce(gpointer user_data);
    static void onClientVolumeLabelUpdate(GtkRange* range, gpointer user_data);
};

struct SoundBackButtonCallbackData {
    SoundManager* soundManager;
    MainWindow* mainWindow;
};