#include "SoundManager.h"
#include "../MainWindow.h"
#include "../translations/translations.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <regex>
#include <cstdio>
#include <memory>
#include <array>

SoundManager::SoundManager(MainWindow* mainWindow, GtkWindow* parentWindow, GtkWidget* overlay)
    : mainWindow(mainWindow), parentWindow(parentWindow), overlay(overlay),
      soundContainer(nullptr), backButton(nullptr),
      outputLabel(nullptr), outputDeviceCombo(nullptr), outputVolumeScale(nullptr), outputVolumeLabel(nullptr),
      inputLabel(nullptr), inputDeviceCombo(nullptr), inputVolumeScale(nullptr), inputVolumeLabel(nullptr),
      playbackLabel(nullptr), playbackScrolledWindow(nullptr), playbackBox(nullptr),
      outputVolumeTimer(0), inputVolumeTimer(0), refreshTimer(0) {
    
    setupUI();
    
    // Start refresh timer (every 2 seconds)
    refreshTimer = g_timeout_add(2000, onRefreshTimer, this);
}

SoundManager::~SoundManager() {
    if (refreshTimer) {
        g_source_remove(refreshTimer);
        refreshTimer = 0;
    }
    if (outputVolumeTimer) {
        g_source_remove(outputVolumeTimer);
        outputVolumeTimer = 0;
    }
    if (inputVolumeTimer) {
        g_source_remove(inputVolumeTimer);
        inputVolumeTimer = 0;
    }
    
    clientWidgets.clear();
}

void SoundManager::show() {
    if (soundContainer) {
        gtk_widget_set_visible(soundContainer, TRUE);
        // Switch to the sound background
        if (mainWindow) {
            mainWindow->switchToBackground("background4.png");
        }
    }
    
    // Refresh audio devices and clients
    refreshAudioDevices();
    refreshPlaybackClients();
}

void SoundManager::hide() {
    if (soundContainer) {
        gtk_widget_set_visible(soundContainer, FALSE);
    }
}

void SoundManager::setupUI() {
    // Create container for sound page - exactly like NetworkManager
    soundContainer = gtk_fixed_new();
    if (overlay && soundContainer) {
        gtk_overlay_add_overlay(GTK_OVERLAY(overlay), soundContainer);
        gtk_widget_set_visible(soundContainer, FALSE);
        gtk_widget_set_size_request(soundContainer, 1400, 700);
        gtk_widget_add_css_class(soundContainer, "sound-container");
        
        initSoundUI();
        setupBackButton();
    }
}

void SoundManager::initSoundUI() {
    if (!soundContainer) return;
    
    // Add title label - exactly like NetworkManager
    GtkWidget* titleLabel = gtk_label_new(TR(TranslationKeys::SOUND_TITLE));
    if (titleLabel) {
        gtk_widget_set_size_request(titleLabel, 300, 50);
        
        PangoAttrList* attrList = pango_attr_list_new();
        if (attrList) {
            PangoAttribute* sizeAttr = pango_attr_size_new(24 * PANGO_SCALE);
            PangoAttribute* weightAttr = pango_attr_weight_new(PANGO_WEIGHT_BOLD);
            
            if (sizeAttr) pango_attr_list_insert(attrList, sizeAttr);
            if (weightAttr) pango_attr_list_insert(attrList, weightAttr);
            gtk_label_set_attributes(GTK_LABEL(titleLabel), attrList);
            
            pango_attr_list_unref(attrList);
        }
        
        gtk_label_set_justify(GTK_LABEL(titleLabel), GTK_JUSTIFY_CENTER);
        gtk_widget_add_css_class(titleLabel, "sound-title");
        
        gtk_fixed_put(GTK_FIXED(soundContainer), titleLabel, 680, 50);
    }
    
    // Output Devices Section
    outputLabel = gtk_label_new(TR(TranslationKeys::OUTPUT_DEVICES));
    if (outputLabel) {
        PangoAttrList* sectionAttrList = pango_attr_list_new();
        PangoAttribute* sectionSizeAttr = pango_attr_size_new(16 * PANGO_SCALE);
        PangoAttribute* sectionWeightAttr = pango_attr_weight_new(PANGO_WEIGHT_BOLD);
        pango_attr_list_insert(sectionAttrList, sectionSizeAttr);
        pango_attr_list_insert(sectionAttrList, sectionWeightAttr);
        gtk_label_set_attributes(GTK_LABEL(outputLabel), sectionAttrList);
        pango_attr_list_unref(sectionAttrList);
        gtk_widget_add_css_class(outputLabel, "section-label");
        
        gtk_fixed_put(GTK_FIXED(soundContainer), outputLabel, 480, 120);
    }
    
    // Output device combo
    outputDeviceCombo = gtk_combo_box_text_new();
    if (outputDeviceCombo) {
        gtk_widget_set_size_request(outputDeviceCombo, 400, 40);
        gtk_widget_add_css_class(outputDeviceCombo, "device-combo");
        gtk_fixed_put(GTK_FIXED(soundContainer), outputDeviceCombo, 480, 150);
        g_signal_connect(outputDeviceCombo, "changed", G_CALLBACK(onOutputDeviceChanged), this);
    }
    
    // Output volume scale
    outputVolumeScale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0, 100.0, 1.0);
    if (outputVolumeScale) {
        gtk_scale_set_draw_value(GTK_SCALE(outputVolumeScale), FALSE);
        gtk_widget_set_size_request(outputVolumeScale, 350, 30);
        gtk_widget_add_css_class(outputVolumeScale, "volume-scale");
        gtk_fixed_put(GTK_FIXED(soundContainer), outputVolumeScale, 480, 200);
        g_signal_connect(outputVolumeScale, "value-changed", G_CALLBACK(onOutputVolumeChanged), this);
    }
    
    // Output volume label
    outputVolumeLabel = gtk_label_new(TR(TranslationKeys::VOLUME_VALUE));
    if (outputVolumeLabel) {
        gtk_widget_add_css_class(outputVolumeLabel, "volume-label");
        gtk_fixed_put(GTK_FIXED(soundContainer), outputVolumeLabel, 830, 210);
    }
    
    // Input Devices Section
    inputLabel = gtk_label_new(TR(TranslationKeys::INPUT_DEVICES));
    if (inputLabel) {
        PangoAttrList* inputSectionAttrList = pango_attr_list_new();
        PangoAttribute* inputSectionSizeAttr = pango_attr_size_new(16 * PANGO_SCALE);
        PangoAttribute* inputSectionWeightAttr = pango_attr_weight_new(PANGO_WEIGHT_BOLD);
        pango_attr_list_insert(inputSectionAttrList, inputSectionSizeAttr);
        pango_attr_list_insert(inputSectionAttrList, inputSectionWeightAttr);
        gtk_label_set_attributes(GTK_LABEL(inputLabel), inputSectionAttrList);
        pango_attr_list_unref(inputSectionAttrList);
        gtk_widget_add_css_class(inputLabel, "section-label");
        
        gtk_fixed_put(GTK_FIXED(soundContainer), inputLabel, 480, 250);
    }
    
    // Input device combo
    inputDeviceCombo = gtk_combo_box_text_new();
    if (inputDeviceCombo) {
        gtk_widget_set_size_request(inputDeviceCombo, 400, 40);
        gtk_widget_add_css_class(inputDeviceCombo, "device-combo");
        gtk_fixed_put(GTK_FIXED(soundContainer), inputDeviceCombo, 480, 280);
        g_signal_connect(inputDeviceCombo, "changed", G_CALLBACK(onInputDeviceChanged), this);
    }
    
    // Input volume scale
    inputVolumeScale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0, 100.0, 1.0);
    if (inputVolumeScale) {
        gtk_scale_set_draw_value(GTK_SCALE(inputVolumeScale), FALSE);
        gtk_widget_set_size_request(inputVolumeScale, 350, 30);
        gtk_widget_add_css_class(inputVolumeScale, "volume-scale");
        gtk_fixed_put(GTK_FIXED(soundContainer), inputVolumeScale, 480, 330);
        g_signal_connect(inputVolumeScale, "value-changed", G_CALLBACK(onInputVolumeChanged), this);
    }
    
    // Input volume label
    inputVolumeLabel = gtk_label_new(TR(TranslationKeys::VOLUME_VALUE));
    if (inputVolumeLabel) {
        gtk_widget_add_css_class(inputVolumeLabel, "volume-label");
        gtk_fixed_put(GTK_FIXED(soundContainer), inputVolumeLabel, 830, 340);
    }
    
    // Playback Clients Section
    playbackLabel = gtk_label_new(TR(TranslationKeys::APPLICATION_VOLUME));
    if (playbackLabel) {
        PangoAttrList* playbackSectionAttrList = pango_attr_list_new();
        PangoAttribute* playbackSectionSizeAttr = pango_attr_size_new(16 * PANGO_SCALE);
        PangoAttribute* playbackSectionWeightAttr = pango_attr_weight_new(PANGO_WEIGHT_BOLD);
        pango_attr_list_insert(playbackSectionAttrList, playbackSectionSizeAttr);
        pango_attr_list_insert(playbackSectionAttrList, playbackSectionWeightAttr);
        gtk_label_set_attributes(GTK_LABEL(playbackLabel), playbackSectionAttrList);
        pango_attr_list_unref(playbackSectionAttrList);
        gtk_widget_add_css_class(playbackLabel, "section-label");
        
        gtk_fixed_put(GTK_FIXED(soundContainer), playbackLabel, 480, 380);
    }
    
    // Create scrollable container for playback applications - like NetworkManager
    playbackScrolledWindow = gtk_scrolled_window_new();
    if (playbackScrolledWindow) {
        gtk_widget_set_size_request(playbackScrolledWindow, 620, 200);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(playbackScrolledWindow), 
                                       GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
        gtk_widget_add_css_class(playbackScrolledWindow, "playback-scroll");
        
        // Create list box for applications - like NetworkManager's wifiListBox
        playbackBox = gtk_list_box_new();
        if (playbackBox) {
            gtk_widget_add_css_class(playbackBox, "playback-list");
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(playbackScrolledWindow), playbackBox);
        }
        
        gtk_fixed_put(GTK_FIXED(soundContainer), playbackScrolledWindow, 480, 410);
    }
    
    
    // Apply CSS styling to match NetworkManager
    GtkCssProvider* provider = gtk_css_provider_new();
    const char* css = 
        "window { "
        "  background: rgba(255, 255, 255, 0.3); "
        "  border-radius: 21px; "
        "  border: 1px solid #c0c0c0; "
        "  box-shadow: 0 8px 25px rgba(0, 0, 0, 0.25), 0 0 1px rgba(0, 0, 0, 0.8); "
        "} "
                 ".sound-container { "
         "  background: linear-gradient(145deg, rgba(255, 255, 255, 0.18), rgba(255, 255, 255, 0.08)); "
         "  border: 2px solid rgba(229, 167, 198, 0.3); "
         "  border-radius: 25px; "
         "  box-shadow: 0 8px 32px rgba(0, 0, 0, 0.12), inset 0 1px 0 rgba(255, 255, 255, 0.2); "
         "} "
                 ".sound-title { "
         "  font-family: ElysiaOSNew12; "
         "  font-size: 28px; "
         "  font-weight: 700; "
         "  color: white; "
         "  text-shadow: 0 2px 4px rgba(0, 0, 0, 0.3), 0 1px 0 rgba(255, 255, 255, 0.4); "
         "  margin-bottom: 20px; "
         "} "
        ".section-label { "
        "  font-family: ElysiaOSNew12; "
        "  font-size: 18px; "
        "  font-weight: 600; "
        "  color: white; "
        "  text-shadow: 0 1px 0 rgba(255, 255, 255, 0.7); "
        "} "
                 ".device-combo { "
         "  background: linear-gradient(145deg, rgba(255, 255, 255, 0.3), rgba(255, 255, 255, 0.18)); "
         "  border: 2px solid rgba(229, 167, 198, 0.4); "
         "  border-radius: 15px; "
         "  color: white; "
         "  font-family: ElysiaOSNew12; "
         "  font-size: 15px; "
         "  font-weight: 500; "
         "  padding: 12px 16px; "
         "  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.15), inset 0 1px 0 rgba(255, 255, 255, 0.2); "
         "  transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1); "
         "} "
         ".device-combo:hover { "
         "  background: linear-gradient(145deg, rgba(255, 255, 255, 0.4), rgba(255, 255, 255, 0.25)); "
         "  border-color: rgba(229, 167, 198, 0.6); "
         "  box-shadow: 0 6px 20px rgba(0, 0, 0, 0.2), inset 0 1px 0 rgba(255, 255, 255, 0.3); "
         "  transform: translateY(-2px); "
         "} "
        ".volume-scale { "
        "  background: transparent; "
        "} "
        ".volume-scale trough { "
        "  background: linear-gradient(90deg, rgba(255, 255, 255, 0.15), rgba(255, 255, 255, 0.1)); "
        "  border: 1px solid rgba(192, 192, 192, 0.3); "
        "  border-radius: 8px; "
        "  min-height: 10px; "
        "  box-shadow: inset 0 1px 3px rgba(0, 0, 0, 0.2); "
        "} "
        ".volume-scale slider { "
        "  background: linear-gradient(145deg, #e5a7c6, #edcee3); "
        "  border: 1px solid rgba(255, 255, 255, 0.8); "
        "  border-radius: 12px; "
        "  min-width: 24px; "
        "  min-height: 24px; "
        "  box-shadow: 0 2px 6px rgba(0, 0, 0, 0.2); "
        "} "
        ".volume-scale slider:hover { "
        "  background: linear-gradient(145deg, #edcee3, #f5e5f0); "
        "  box-shadow: 0 3px 8px rgba(0, 0, 0, 0.3); "
        "} "
        ".volume-scale highlight { "
        "  background: linear-gradient(90deg, #e5a7c6, #edcee3); "
        "  border-radius: 8px; "
        "} "
        ".volume-label { "
        "  font-family: ElysiaOSNew12; "
        "  font-size: 14px; "
        "  font-weight: 500; "
        "  color: white; "
        "  text-shadow: 0 1px 0 rgba(0, 0, 0, 0.3); "
        "} "
                 ".playback-scroll { "
         "  background: linear-gradient(145deg, rgba(255, 255, 255, 0.1), rgba(255, 255, 255, 0.05)); "
         "  border: 1px solid rgba(192, 192, 192, 0.3); "
         "  border-radius: 12px; "
         "  box-shadow: inset 0 2px 6px rgba(0, 0, 0, 0.1); "
         "} "
         "scrolledwindow { "
         "  background: transparent; "
         "  border: none; "
         "} "
         "scrollbar { "
         "  background: transparent; "
         "  border-radius: 6px; "
         "} "
         "scrollbar slider { "
         "  background: linear-gradient(145deg, rgba(229, 167, 198, 0.6), rgba(237, 206, 227, 0.6)); "
         "  border: 1px solid rgba(255, 255, 255, 0.4); "
         "  border-radius: 6px; "
         "  min-height: 20px; "
         "} "
         "scrollbar slider:hover { "
         "  background: linear-gradient(145deg, rgba(229, 167, 198, 0.8), rgba(237, 206, 227, 0.8)); "
         "} "
         ".playback-list { "
         "  background: transparent; "
         "  border: none; "
         "} "
                                       ".playback-list row { "
           "  background: linear-gradient(145deg, rgba(255, 255, 255, 0.3), rgba(255, 255, 255, 0.18)); "
           "  border: 2px solid rgba(229, 167, 198, 0.4); "
           "  border-radius: 12px; "
           "  margin: 3px 8px; "
           "  padding: 8px 12px; "
           "  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.12), inset 0 1px 0 rgba(255, 255, 255, 0.15); "
           "  transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1); "
           "} "
           ".playback-list row:hover { "
           "  background: linear-gradient(145deg, rgba(255, 255, 255, 0.4), rgba(255, 255, 255, 0.25)); "
           "  border-color: rgba(229, 167, 198, 0.6); "
           "  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.18), inset 0 1px 0 rgba(255, 255, 255, 0.25); "
           "  transform: translateY(-1px); "
           "} "
         ".app-card { "
         "  background: transparent; "
         "  border: none; "
         "  padding: 0; "
         "  margin: 0; "
         "} "
                 ".client-name { "
         "  font-family: ElysiaOSNew12; "
         "  font-size: 12px; "
         "  font-weight: 500; "
         "  color: white; "
         "  text-shadow: 0 1px 0 rgba(0, 0, 0, 0.3); "
         "} "
;
    
    gtk_css_provider_load_from_string(provider, css);
    
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    g_object_unref(provider);
}

void SoundManager::setupBackButton() {
    if (!soundContainer) return;
    
    std::string backPath = getAssetPath("back.png");
    
    // Create a button - EXACTLY like NetworkManager
    backButton = gtk_button_new();
    if (!backButton) return;
    
    // Create a fixed container to position the label on top of the image
    GtkWidget* fixedContainer = gtk_fixed_new();
    if (!fixedContainer) return;
    
    gtk_widget_set_size_request(fixedContainer, 120, 60);
    
    // Load the back image
    GtkWidget* backImage = gtk_picture_new_for_filename(backPath.c_str());
    if (!backImage) {
        backImage = gtk_image_new_from_icon_name("go-previous");
    } else {
        gtk_widget_set_size_request(backImage, 159, 66);
        gtk_picture_set_can_shrink(GTK_PICTURE(backImage), TRUE);
        gtk_picture_set_content_fit(GTK_PICTURE(backImage), GTK_CONTENT_FIT_CONTAIN);
    }
    
    // Create label
    GtkWidget* label = gtk_label_new(TR(TranslationKeys::BACK));
    if (label) {
        gtk_widget_add_css_class(label, "back-button-label");
    }
    
    // Position image and label in the fixed container
    if (backImage) {
        gtk_fixed_put(GTK_FIXED(fixedContainer), backImage, 0, 0);
    }
    if (label) {
        gtk_fixed_put(GTK_FIXED(fixedContainer), label, 40, 20);
    }
    
    // Add fixed container to button
    gtk_button_set_child(GTK_BUTTON(backButton), fixedContainer);
    
    // Style the button to remove borders and background
    gtk_widget_set_size_request(backButton, 120, 60);
    gtk_widget_add_css_class(backButton, "flat");
    
    // Position the back button at top left of the sound container
    gtk_fixed_put(GTK_FIXED(soundContainer), backButton, 0, 0);
    
    // Create callback data
    SoundBackButtonCallbackData* callbackData = new SoundBackButtonCallbackData{this, mainWindow};
    g_signal_connect(backButton, "clicked", G_CALLBACK(onBackButtonClicked), callbackData);
    g_object_set_data_full(G_OBJECT(backButton), "callback_data", callbackData, g_free);
}

std::string SoundManager::executeCommand(const std::string& command) {
    std::array<char, 128> buffer;
    std::string result;
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        return "";
    }
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    pclose(pipe);
    return result;
}

std::vector<AudioDevice> SoundManager::getAudioDevices(const std::string& type) {
    std::vector<AudioDevice> devices;
    
    // EXACTLY like QT6: Get short and full output
    std::string shortCommand = "pactl list short " + type + "s";
    std::string shortOutput = executeCommand(shortCommand);
    
    std::string fullCommand = "pactl list " + type + "s";
    std::string fullOutput = executeCommand(fullCommand);
    
    if (fullOutput.empty()) {
        std::cout << "No " << type << " devices found or pactl failed" << std::endl;
        return devices;
    }
    
    // EXACTLY like QT6: Split by block header
    std::string blockHeader = (type == "sink") ? "Sink #" : "Source #";
    
    // Split fullOutput by blockHeader - EXACTLY like QT6's split
    std::vector<std::string> blocks;
    size_t pos = 0;
    while ((pos = fullOutput.find(blockHeader, pos)) != std::string::npos) {
        size_t nextPos = fullOutput.find(blockHeader, pos + blockHeader.length());
        if (nextPos == std::string::npos) {
            blocks.push_back(fullOutput.substr(pos));
        } else {
            blocks.push_back(fullOutput.substr(pos, nextPos - pos));
        }
        pos = nextPos;
        if (nextPos == std::string::npos) break;
    }
    
    // Build maps EXACTLY like QT6
    std::map<std::string, std::string> nameToDesc;
    std::map<std::string, std::vector<std::pair<std::string, std::string>>> nameToPorts;
    std::map<std::string, std::string> nameToActivePort;
    
    for (const auto& block : blocks) {
        std::string name, desc, activePort;
        std::vector<std::pair<std::string, std::string>> ports;
        
        std::istringstream blockStream(block);
        std::string line;
        std::vector<std::string> lines;
        
        while (std::getline(blockStream, line)) {
            lines.push_back(line);
        }
        
        for (int i = 0; i < static_cast<int>(lines.size()); ++i) {
            std::string trimmedLine = lines[i];
            // Trim like QT6
            trimmedLine.erase(0, trimmedLine.find_first_not_of(" \t"));
            trimmedLine.erase(trimmedLine.find_last_not_of(" \t\n\r") + 1);
            
            if (trimmedLine.find("Name:") == 0) {
                // Extract after "Name:" - like QT6's section(":", 1).trimmed()
                size_t colonPos = trimmedLine.find(":");
                if (colonPos != std::string::npos) {
                    name = trimmedLine.substr(colonPos + 1);
                    name.erase(0, name.find_first_not_of(" \t"));
                    name.erase(name.find_last_not_of(" \t\n\r") + 1);
                }
            } else if (trimmedLine.find("Description:") == 0) {
                size_t colonPos = trimmedLine.find(":");
                if (colonPos != std::string::npos) {
                    desc = trimmedLine.substr(colonPos + 1);
                    desc.erase(0, desc.find_first_not_of(" \t"));
                    desc.erase(desc.find_last_not_of(" \t\n\r") + 1);
                }
            } else if (trimmedLine.find("Ports:") == 0) {
                // Parse ports until next empty line or non-indented line - EXACTLY like QT6
                int j = i + 1;
                while (j < static_cast<int>(lines.size()) && 
                       (lines[j].find(' ') == 0 || lines[j].find('\t') == 0)) {
                    std::string portLine = lines[j];
                    portLine.erase(0, portLine.find_first_not_of(" \t"));
                    portLine.erase(portLine.find_last_not_of(" \t\n\r") + 1);
                    
                    if (portLine.find(":") != std::string::npos) {
                        // EXACTLY like QT6: portLine.section(":", 0, 0).trimmed()
                        size_t colonPos = portLine.find(":");
                        std::string portName = portLine.substr(0, colonPos);
                        portName.erase(0, portName.find_first_not_of(" \t"));
                        portName.erase(portName.find_last_not_of(" \t\n\r") + 1);
                        
                        // EXACTLY like QT6: portLine.section(":", 1).section("(", 0, 0).trimmed()
                        std::string portDesc = portLine.substr(colonPos + 1);
                        size_t parenPos = portDesc.find("(");
                        if (parenPos != std::string::npos) {
                            portDesc = portDesc.substr(0, parenPos);
                        }
                        portDesc.erase(0, portDesc.find_first_not_of(" \t"));
                        portDesc.erase(portDesc.find_last_not_of(" \t\n\r") + 1);
                        
                        ports.emplace_back(portName, portDesc);
                    }
                    ++j;
                }
            } else if (trimmedLine.find("Active Port:") == 0) {
                size_t colonPos = trimmedLine.find(":");
                if (colonPos != std::string::npos) {
                    activePort = trimmedLine.substr(colonPos + 1);
                    activePort.erase(0, activePort.find_first_not_of(" \t"));
                    activePort.erase(activePort.find_last_not_of(" \t\n\r") + 1);
                }
            }
        }
        
        if (!name.empty()) {
            std::string cleanDesc = desc.empty() ? name : simplifyDeviceDescription(desc);
            nameToDesc[name] = cleanDesc;
            nameToPorts[name] = ports;
            nameToActivePort[name] = activePort;
        }
    }
    
    // Process short list EXACTLY like QT6
    std::istringstream shortStream(shortOutput);
    std::string line;
    while (std::getline(shortStream, line)) {
        if (line.empty()) continue;
        
        // Split by tab like QT6
        std::istringstream lineStream(line);
        std::string part;
        std::vector<std::string> parts;
        while (std::getline(lineStream, part, '\t')) {
            parts.push_back(part);
        }
        
        if (parts.size() < 2) continue;
        
        std::string devName = parts[1];
        if (!devName.empty()) {
            AudioDevice device;
            device.name = devName;
            device.description = nameToDesc[devName];
            device.ports = nameToPorts[devName];
            device.activePort = nameToActivePort[devName];
            device.isDefault = false;
            device.volume = 0;
            
            devices.push_back(device);
            
            std::cout << "Device: " << device.name << ", Active Port: " << device.activePort << ", Ports: " << device.ports.size() << std::endl;
        }
    }
    
    // Get volume for each device
    for (auto& device : devices) {
        std::string volumeCommand = "pactl get-" + type + "-volume " + device.name;
        std::string volumeOutput = executeCommand(volumeCommand);
        
        std::regex volumeRegex("(\\d+)%");
        std::smatch match;
        if (std::regex_search(volumeOutput, match, volumeRegex)) {
            device.volume = std::stoi(match[1]);
        }
    }
    
    std::cout << "Found " << devices.size() << " " << type << " devices" << std::endl;
    return devices;
}

std::vector<PlaybackClient> SoundManager::getPlaybackClients() {
    std::vector<PlaybackClient> clients;
    
    std::string output = executeCommand("pactl list sink-inputs");
    if (output.empty()) {
        std::cout << "No sink-inputs found or pactl failed" << std::endl;
        return clients;
    }
    
    std::istringstream stream(output);
    std::string line;
    
    PlaybackClient currentClient;
    bool inClient = false;
    
    while (std::getline(stream, line)) {
        if (line.find("Sink Input #") == 0) {
            if (inClient && !currentClient.name.empty()) {
                clients.push_back(currentClient);
            }
            currentClient = PlaybackClient{};
            inClient = true;
            
            // Extract index
            std::regex indexRegex("Sink Input #(\\d+)");
            std::smatch match;
            if (std::regex_search(line, match, indexRegex)) {
                currentClient.index = std::stoul(match[1]);
            }
        } else if (inClient) {
            if (line.find("application.name") != std::string::npos) {
                std::regex nameRegex("\"([^\"]+)\"");
                std::smatch match;
                if (std::regex_search(line, match, nameRegex)) {
                    currentClient.applicationName = match[1];
                    currentClient.name = currentClient.applicationName;
                }
            } else if (line.find("Volume:") != std::string::npos) {
                std::regex volumeRegex("(\\d+)%");
                std::smatch match;
                if (std::regex_search(line, match, volumeRegex)) {
                    currentClient.volume = std::stoi(match[1]);
                }
            }
        }
    }
    
    if (inClient && !currentClient.name.empty()) {
        clients.push_back(currentClient);
    }
    
    std::cout << "Found " << clients.size() << " playback clients" << std::endl;
    return clients;
}

std::string SoundManager::getAssetPath(const std::string& filename) {
    const char* home = std::getenv("HOME");
    if (!home) {
        std::cerr << "HOME environment variable not set" << std::endl;
        return filename;
    }
    return std::string(home) + "/.config/Elysia/assets/assets/" + filename;
}

std::string SoundManager::simplifyDeviceDescription(const std::string& description) {
    std::string result = description;
    
    // Remove common redundant text
    std::regex analogStereo("Analog Stereo");
    result = std::regex_replace(result, analogStereo, "");
    
    std::regex familyController("Family\\s+[\\w/]+?\\s+HD\\s+Audio\\s+Controller", std::regex_constants::icase);
    result = std::regex_replace(result, familyController, "");
    
    std::regex trailingSpaces("[\\s\\-]+$");
    result = std::regex_replace(result, trailingSpaces, "");
    
    // Trim whitespace
    size_t start = result.find_first_not_of(" \t");
    size_t end = result.find_last_not_of(" \t");
    
    if (start != std::string::npos && end != std::string::npos) {
        result = result.substr(start, end - start + 1);
    } else {
        result = "";
    }
    
    return result.empty() ? description : result;
}

std::string SoundManager::getDefaultDevice(const std::string& type) {
    std::string command = "pactl get-default-" + type;
    std::string result = executeCommand(command);
    
    // Remove trailing newline
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    
    return result;
}

void SoundManager::refreshAudioDevices() {
    outputDevices = getAudioDevices("sink");
    inputDevices = getAudioDevices("source");
    
    // Get default devices - just for volume control
    std::string defaultSink = getDefaultDevice("sink");
    std::string defaultSource = getDefaultDevice("source");
    
    std::cout << "Default sink: " << defaultSink << std::endl;
    std::cout << "Default source: " << defaultSource << std::endl;
    
    // Mark default devices (for volume control only)
    for (auto& device : outputDevices) {
        device.isDefault = (device.name == defaultSink);
    }
    
    for (auto& device : inputDevices) {
        device.isDefault = (device.name == defaultSource);
    }
    
    updateOutputDevices();
    updateInputDevices();
}

void SoundManager::refreshPlaybackClients() {
    playbackClients = getPlaybackClients();
    updatePlaybackClientsUI();
}

void SoundManager::updateOutputDevices() {
    if (!outputDeviceCombo) return;
    
    g_signal_handlers_block_by_func(outputDeviceCombo, (gpointer)onOutputDeviceChanged, this);
    gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(outputDeviceCombo));
    
    int activeIndex = 0;
    int currentIndex = 0;
    
    // EXACTLY like QT6 version: find the device with active port
    for (const auto& device : outputDevices) {
        for (const auto& port : device.ports) {
            std::string label = simplifyDeviceDescription(device.description) + " - " + port.second;
            gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(outputDeviceCombo), 
                                      (device.name + "|" + port.first).c_str(), 
                                      label.c_str());
            
            // Select if this device's active port matches this port
            if (device.activePort == port.first) {
                activeIndex = currentIndex;
                std::cout << "Found active output: " << device.name << " port: " << port.first << " at index: " << activeIndex << std::endl;
            }
            currentIndex++;
        }
    }
    
    gtk_combo_box_set_active(GTK_COMBO_BOX(outputDeviceCombo), activeIndex);
    g_signal_handlers_unblock_by_func(outputDeviceCombo, (gpointer)onOutputDeviceChanged, this);
    
    // Update volume for default device
    if (!outputDevices.empty() && outputVolumeScale && outputVolumeLabel) {
        for (const auto& device : outputDevices) {
            if (device.isDefault) {
                gtk_range_set_value(GTK_RANGE(outputVolumeScale), device.volume);
                std::string volumeText = std::to_string(device.volume) + "%";
                gtk_label_set_text(GTK_LABEL(outputVolumeLabel), volumeText.c_str());
                std::cout << "Set output volume to: " << device.volume << "% for device: " << device.name << std::endl;
                break;
            }
        }
    }
}

void SoundManager::updateInputDevices() {
    if (!inputDeviceCombo) return;
    
    g_signal_handlers_block_by_func(inputDeviceCombo, (gpointer)onInputDeviceChanged, this);
    gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(inputDeviceCombo));
    
    int activeIndex = 0;
    int currentIndex = 0;
    
    // EXACTLY like QT6 version: find the device with active port, skip monitors
    for (const auto& device : inputDevices) {
        // Skip monitor sources
        if (device.name.find(".monitor") != std::string::npos) continue;
        
        for (const auto& port : device.ports) {
            std::string label = simplifyDeviceDescription(device.description) + " - " + port.second;
            gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(inputDeviceCombo), 
                                      (device.name + "|" + port.first).c_str(), 
                                      label.c_str());
            
            // Select if this device's active port matches this port
            if (device.activePort == port.first) {
                activeIndex = currentIndex;
                std::cout << "Found active input: " << device.name << " port: " << port.first << " at index: " << activeIndex << std::endl;
            }
            currentIndex++;
        }
    }
    
    gtk_combo_box_set_active(GTK_COMBO_BOX(inputDeviceCombo), activeIndex);
    g_signal_handlers_unblock_by_func(inputDeviceCombo, (gpointer)onInputDeviceChanged, this);
    
    // Update volume for default device
    if (!inputDevices.empty() && inputVolumeScale && inputVolumeLabel) {
        for (const auto& device : inputDevices) {
            if (device.isDefault && device.name.find(".monitor") == std::string::npos) {
                gtk_range_set_value(GTK_RANGE(inputVolumeScale), device.volume);
                std::string volumeText = std::to_string(device.volume) + "%";
                gtk_label_set_text(GTK_LABEL(inputVolumeLabel), volumeText.c_str());
                std::cout << "Set input volume to: " << device.volume << "% for device: " << device.name << std::endl;
                break;
            }
        }
    }
}

void SoundManager::updatePlaybackClientsUI() {
    if (!playbackBox) return;
    
    // Clear existing client widgets from list box
    GtkWidget* child = gtk_widget_get_first_child(playbackBox);
    while (child) {
        GtkWidget* next = gtk_widget_get_next_sibling(child);
        gtk_list_box_remove(GTK_LIST_BOX(playbackBox), child);
        child = next;
    }
    clientWidgets.clear();
    
    if (playbackClients.empty()) {
        // Create a list row for the "no apps" message
        GtkWidget* noAppsRow = gtk_list_box_row_new();
        GtkWidget* noAppsLabel = gtk_label_new(TR(TranslationKeys::NO_AUDIO_APPS));
        gtk_widget_add_css_class(noAppsLabel, "client-name");
        gtk_widget_set_halign(noAppsLabel, GTK_ALIGN_CENTER);
        gtk_widget_set_margin_top(noAppsLabel, 20);
        gtk_widget_set_margin_bottom(noAppsLabel, 20);
        gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(noAppsRow), noAppsLabel);
        gtk_list_box_append(GTK_LIST_BOX(playbackBox), noAppsRow);
    } else {
        for (const auto& client : playbackClients) {
            // Create list row - like NetworkManager's network items
            GtkWidget* clientRow = gtk_list_box_row_new();
            
            // Create client widget container - app card style
            GtkWidget* clientWidget = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
            gtk_widget_add_css_class(clientWidget, "app-card");
            gtk_widget_set_margin_start(clientWidget, 6);
            gtk_widget_set_margin_end(clientWidget, 6);
            gtk_widget_set_margin_top(clientWidget, 4);
            gtk_widget_set_margin_bottom(clientWidget, 4);
            
            gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(clientRow), clientWidget);
            gtk_list_box_append(GTK_LIST_BOX(playbackBox), clientRow);
            
            // Application name label
            GtkWidget* nameLabel = gtk_label_new(client.applicationName.c_str());
            gtk_widget_add_css_class(nameLabel, "client-name");
            gtk_box_append(GTK_BOX(clientWidget), nameLabel);
            gtk_widget_set_halign(nameLabel, GTK_ALIGN_START);
            
            // Volume control container
            GtkWidget* volumeBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
            gtk_box_append(GTK_BOX(clientWidget), volumeBox);
            
            // Volume scale - use same styling as main volume scales but smaller
            GtkWidget* volumeScale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0, 100.0, 1.0);
            gtk_scale_set_draw_value(GTK_SCALE(volumeScale), FALSE);
            gtk_widget_set_hexpand(volumeScale, TRUE);
            gtk_widget_set_size_request(volumeScale, 220, 20);
            gtk_range_set_value(GTK_RANGE(volumeScale), client.volume);
            gtk_widget_add_css_class(volumeScale, "volume-scale"); // Use same class as main volume scales
            gtk_box_append(GTK_BOX(volumeBox), volumeScale);
            
            // Volume label
            GtkWidget* volumeLabel = gtk_label_new((std::to_string(client.volume) + "%").c_str());
            gtk_widget_add_css_class(volumeLabel, "volume-label");
            gtk_box_append(GTK_BOX(volumeBox), volumeLabel);
            
            // Store widget reference
            clientWidgets[client.index] = clientWidget;
            
            // Connect signal with client index
            SoundManagerCallbackData* data = new SoundManagerCallbackData{this, "", "", client.index};
            g_signal_connect(volumeScale, "value-changed", G_CALLBACK(onPlaybackVolumeChanged), data);
            g_object_set_data_full(G_OBJECT(volumeScale), "callback_data", data, g_free);
            
            // Update volume label on change
            g_signal_connect(volumeScale, "value-changed", G_CALLBACK(onClientVolumeLabelUpdate), volumeLabel);
        }
    }
}

// Static callback implementations
void SoundManager::onBackButtonClicked(GtkButton* button, gpointer user_data) {
    (void)button;
    SoundBackButtonCallbackData* data = static_cast<SoundBackButtonCallbackData*>(user_data);
    if (data && data->soundManager && data->mainWindow) {
        data->soundManager->hide();
        data->mainWindow->showMainMenu();
    }
}

void SoundManager::onOutputDeviceChanged(GtkComboBox* combo, gpointer user_data) {
    SoundManager* manager = static_cast<SoundManager*>(user_data);
    if (!manager) return;
    
    const char* devicePort = gtk_combo_box_get_active_id(combo);
    if (!devicePort) return;
    
    std::string devicePortStr(devicePort);
    size_t pipePos = devicePortStr.find('|');
    if (pipePos != std::string::npos) {
        std::string deviceName = devicePortStr.substr(0, pipePos);
        std::string portName = devicePortStr.substr(pipePos + 1);
        manager->setOutputDevice(deviceName, portName);
    } else {
        // Device without port
        manager->setOutputDevice(devicePortStr, "");
    }
}

void SoundManager::onInputDeviceChanged(GtkComboBox* combo, gpointer user_data) {
    SoundManager* manager = static_cast<SoundManager*>(user_data);
    if (!manager) return;
    
    const char* devicePort = gtk_combo_box_get_active_id(combo);
    if (!devicePort) return;
    
    std::string devicePortStr(devicePort);
    size_t pipePos = devicePortStr.find('|');
    if (pipePos != std::string::npos) {
        std::string deviceName = devicePortStr.substr(0, pipePos);
        std::string portName = devicePortStr.substr(pipePos + 1);
        manager->setInputDevice(deviceName, portName);
    } else {
        // Device without port
        manager->setInputDevice(devicePortStr, "");
    }
}

gboolean SoundManager::onOutputVolumeChanged(GtkRange* range, gpointer user_data) {
    SoundManager* manager = static_cast<SoundManager*>(user_data);
    if (!manager || !manager->outputVolumeLabel) return FALSE;
    
    int value = static_cast<int>(gtk_range_get_value(range));
    std::string volumeText = std::to_string(value) + "%";
    gtk_label_set_text(GTK_LABEL(manager->outputVolumeLabel), volumeText.c_str());
    
    // Debounce the volume change
    if (manager->outputVolumeTimer) {
        g_source_remove(manager->outputVolumeTimer);
    }
    manager->outputVolumeTimer = g_timeout_add(100, onOutputVolumeDebounce, user_data);
    
    return FALSE;
}

gboolean SoundManager::onInputVolumeChanged(GtkRange* range, gpointer user_data) {
    SoundManager* manager = static_cast<SoundManager*>(user_data);
    if (!manager || !manager->inputVolumeLabel) return FALSE;
    
    int value = static_cast<int>(gtk_range_get_value(range));
    std::string volumeText = std::to_string(value) + "%";
    gtk_label_set_text(GTK_LABEL(manager->inputVolumeLabel), volumeText.c_str());
    
    // Debounce the volume change
    if (manager->inputVolumeTimer) {
        g_source_remove(manager->inputVolumeTimer);
    }
    manager->inputVolumeTimer = g_timeout_add(100, onInputVolumeDebounce, user_data);
    
    return FALSE;
}

gboolean SoundManager::onPlaybackVolumeChanged(GtkRange* range, gpointer user_data) {
    SoundManagerCallbackData* data = static_cast<SoundManagerCallbackData*>(user_data);
    if (!data || !data->soundManager) return FALSE;
    
    int value = static_cast<int>(gtk_range_get_value(range));
    data->soundManager->setPlaybackClientVolume(data->index, value);
    
    return FALSE;
}


gboolean SoundManager::onRefreshTimer(gpointer user_data) {
    SoundManager* manager = static_cast<SoundManager*>(user_data);
    if (!manager) return G_SOURCE_REMOVE;
    
    // Refresh devices and clients periodically
    static int counter = 0;
    if (++counter >= 5) { // Every 10 seconds (5 * 2 seconds)
        counter = 0;
        if (manager->soundContainer && gtk_widget_get_visible(manager->soundContainer)) {
            manager->refreshAudioDevices();
            manager->refreshPlaybackClients();
        }
    }
    
    return G_SOURCE_CONTINUE;
}

gboolean SoundManager::onOutputVolumeDebounce(gpointer user_data) {
    SoundManager* manager = static_cast<SoundManager*>(user_data);
    if (!manager || !manager->outputVolumeScale) return G_SOURCE_REMOVE;
    
    int value = static_cast<int>(gtk_range_get_value(GTK_RANGE(manager->outputVolumeScale)));
    manager->setOutputVolume(value);
    manager->outputVolumeTimer = 0;
    
    return G_SOURCE_REMOVE;
}

gboolean SoundManager::onInputVolumeDebounce(gpointer user_data) {
    SoundManager* manager = static_cast<SoundManager*>(user_data);
    if (!manager || !manager->inputVolumeScale) return G_SOURCE_REMOVE;
    
    int value = static_cast<int>(gtk_range_get_value(GTK_RANGE(manager->inputVolumeScale)));
    manager->setInputVolume(value);
    manager->inputVolumeTimer = 0;
    
    return G_SOURCE_REMOVE;
}

void SoundManager::onClientVolumeLabelUpdate(GtkRange* range, gpointer user_data) {
    GtkWidget* label = static_cast<GtkWidget*>(user_data);
    int value = static_cast<int>(gtk_range_get_value(range));
    std::string text = std::to_string(value) + "%";
    gtk_label_set_text(GTK_LABEL(label), text.c_str());
}

// Device control methods using pactl commands
void SoundManager::setOutputDevice(const std::string& deviceName, const std::string& portName) {
    // Set default sink
    std::string command = "pactl set-default-sink " + deviceName;
    executeCommand(command);
    std::cout << "Setting output device: " << deviceName << std::endl;
    
    // Set sink port
    if (!portName.empty()) {
        command = "pactl set-sink-port " + deviceName + " " + portName;
        executeCommand(command);
        std::cout << "Setting output port: " << portName << std::endl;
    }
}

void SoundManager::setInputDevice(const std::string& deviceName, const std::string& portName) {
    // Set default source
    std::string command = "pactl set-default-source " + deviceName;
    executeCommand(command);
    std::cout << "Setting input device: " << deviceName << std::endl;
    
    // Set source port
    if (!portName.empty()) {
        command = "pactl set-source-port " + deviceName + " " + portName;
        executeCommand(command);
        std::cout << "Setting input port: " << portName << std::endl;
    }
}

void SoundManager::setOutputVolume(int volume) {
    // Use the default sink directly
    std::string defaultSink = getDefaultDevice("sink");
    if (!defaultSink.empty()) {
        std::string command = "pactl set-sink-volume " + defaultSink + " " + std::to_string(volume) + "%";
        executeCommand(command);
        std::cout << "Setting output volume: " << volume << "% for device: " << defaultSink << std::endl;
    }
}

void SoundManager::setInputVolume(int volume) {
    // Use the default source directly
    std::string defaultSource = getDefaultDevice("source");
    if (!defaultSource.empty()) {
        std::string command = "pactl set-source-volume " + defaultSource + " " + std::to_string(volume) + "%";
        executeCommand(command);
        std::cout << "Setting input volume: " << volume << "% for device: " << defaultSource << std::endl;
    }
}

void SoundManager::setPlaybackClientVolume(uint32_t index, int volume) {
    std::string command = "pactl set-sink-input-volume " + std::to_string(index) + " " + std::to_string(volume) + "%";
    executeCommand(command);
    std::cout << "Setting client " << index << " volume: " << volume << "%" << std::endl;
}
