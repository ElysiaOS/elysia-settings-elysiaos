#include "BluetoothManager.h"
#include "../MainWindow.h"
#include "../translations/translations.h"
#include <cstdlib>
#include <sstream>
#include <algorithm>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>

// Forward declaration of callback data
struct BluetoothBackButtonCallbackData {
    BluetoothManager* bluetoothManager;
};

// Device action data structure for button callbacks
struct DeviceActionData {
    BluetoothManager* manager;
    std::string mac;
    std::string name;
};

BluetoothManager::BluetoothManager(MainWindow* mainWindow, GtkWidget* parentWindow, GtkWidget* overlay) 
    : mainWindow(mainWindow), parentWindow(parentWindow), overlay(overlay), 
      bluetoothContainer(nullptr), backButton(nullptr), statusLabel(nullptr),
      connectionStatusLabel(nullptr), toggleButton(nullptr), scanButton(nullptr), 
      deviceListBox(nullptr), scrolledWindow(nullptr), bluetoothEnabled(false), 
      isScanning(false), refreshTimeoutId(0), scanTimeoutId(0) {
    setupUI();
}

BluetoothManager::~BluetoothManager() {
    // Clean up timeouts
    if (refreshTimeoutId > 0) {
        g_source_remove(refreshTimeoutId);
        refreshTimeoutId = 0;
    }
    if (scanTimeoutId > 0) {
        g_source_remove(scanTimeoutId);
        scanTimeoutId = 0;
    }
    
    // Clear widget tracking - GTK handles the actual widget cleanup
    deviceWidgets.clear();
    
    // All GTK widgets are managed by GTK and will be cleaned up automatically
}

void BluetoothManager::show() {
    if (bluetoothContainer) {
        gtk_widget_set_visible(bluetoothContainer, TRUE);
        // Switch to the bluetooth background
        if (mainWindow) {
            mainWindow->switchToBackground("background4.png");
        }
        updateBluetoothStatus();
        populateDeviceList();
        
        // Start refresh timer
        if (refreshTimeoutId == 0) {
            refreshTimeoutId = g_timeout_add(5000, refreshDevicesTimeout, this);
        }
    }
}

void BluetoothManager::hide() {
    if (bluetoothContainer) {
        gtk_widget_set_visible(bluetoothContainer, FALSE);
        
        // Stop refresh timer
        if (refreshTimeoutId > 0) {
            g_source_remove(refreshTimeoutId);
            refreshTimeoutId = 0;
        }
    }
}

void BluetoothManager::setupUI() {
    if (!overlay) return;
    
    // Create main container
    bluetoothContainer = gtk_fixed_new();
    if (bluetoothContainer) {
        gtk_widget_set_size_request(bluetoothContainer, 1400, 900);
        gtk_overlay_add_overlay(GTK_OVERLAY(overlay), bluetoothContainer);
        gtk_widget_set_visible(bluetoothContainer, FALSE);
        
        setupBackButton();
        initBluetoothUI();
    }
}

void BluetoothManager::initBluetoothUI() {
    if (!bluetoothContainer) return;
    
    // Add connection status label
    connectionStatusLabel = gtk_label_new(TR(TranslationKeys::CHECKING_STATUS));
    if (connectionStatusLabel) {
        gtk_widget_set_size_request(connectionStatusLabel, 400, 30);
        
        PangoAttrList* statusAttrList = pango_attr_list_new();
        if (statusAttrList) {
            PangoAttribute* statusSizeAttr = pango_attr_size_new(16 * PANGO_SCALE);
            PangoAttribute* statusWeightAttr = pango_attr_weight_new(PANGO_WEIGHT_NORMAL);
            
            if (statusSizeAttr) pango_attr_list_insert(statusAttrList, statusSizeAttr);
            if (statusWeightAttr) pango_attr_list_insert(statusAttrList, statusWeightAttr);
            gtk_label_set_attributes(GTK_LABEL(connectionStatusLabel), statusAttrList);
            
            pango_attr_list_unref(statusAttrList);
        }
        
        gtk_label_set_justify(GTK_LABEL(connectionStatusLabel), GTK_JUSTIFY_CENTER);
        gtk_widget_add_css_class(connectionStatusLabel, "bluetooth-status");
        
        gtk_fixed_put(GTK_FIXED(bluetoothContainer), connectionStatusLabel, 600, 70);
    }
    
    // Add title label
    GtkWidget* titleLabel = gtk_label_new(TR(TranslationKeys::BLUETOOTH_TITLE));
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
        gtk_widget_add_css_class(titleLabel, "bluetooth-title");
        
        gtk_fixed_put(GTK_FIXED(bluetoothContainer), titleLabel, 650, 120);
    }
    
    // Bluetooth header with toggle and scan buttons
    GtkWidget* bluetoothHeader = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 15);
    if (bluetoothHeader) {
        gtk_widget_set_size_request(bluetoothHeader, 600, 40);
        
        // Toggle Bluetooth button
        toggleButton = gtk_button_new_with_label(TR(TranslationKeys::TOGGLE_BLUETOOTH));
        if (toggleButton) {
            gtk_widget_set_size_request(toggleButton, 200, 40);
            gtk_widget_add_css_class(toggleButton, "bluetooth-toggle-btn");
            g_signal_connect(toggleButton, "clicked", G_CALLBACK(onToggleBluetoothClicked), this);
        }
        
        // Scan devices button
        scanButton = gtk_button_new_with_label(TR(TranslationKeys::SCAN_DEVICES));
        if (scanButton) {
            gtk_widget_set_size_request(scanButton, 200, 40);
            gtk_widget_add_css_class(scanButton, "bluetooth-scan-btn");
            g_signal_connect(scanButton, "clicked", G_CALLBACK(onScanDevicesClicked), this);
        }
        
        gtk_box_append(GTK_BOX(bluetoothHeader), toggleButton);
        gtk_box_append(GTK_BOX(bluetoothHeader), scanButton);
        
        gtk_fixed_put(GTK_FIXED(bluetoothContainer), bluetoothHeader, 500, 180);
    }
    
    // Device list label
    GtkWidget* deviceLabel = gtk_label_new(TR(TranslationKeys::DEVICES));
    if (deviceLabel) {
        gtk_widget_set_size_request(deviceLabel, 200, 30);
        
        PangoAttrList* deviceAttrList = pango_attr_list_new();
        if (deviceAttrList) {
            PangoAttribute* deviceSizeAttr = pango_attr_size_new(18 * PANGO_SCALE);
            PangoAttribute* deviceWeightAttr = pango_attr_weight_new(PANGO_WEIGHT_BOLD);
            
            if (deviceSizeAttr) pango_attr_list_insert(deviceAttrList, deviceSizeAttr);
            if (deviceWeightAttr) pango_attr_list_insert(deviceAttrList, deviceWeightAttr);
            gtk_label_set_attributes(GTK_LABEL(deviceLabel), deviceAttrList);
            
            pango_attr_list_unref(deviceAttrList);
        }
        
        gtk_label_set_justify(GTK_LABEL(deviceLabel), GTK_JUSTIFY_CENTER);
        gtk_widget_add_css_class(deviceLabel, "device-label");
        
        gtk_fixed_put(GTK_FIXED(bluetoothContainer), deviceLabel, 700, 240);
    }
    
    // Scrolled window for device list
    scrolledWindow = gtk_scrolled_window_new();
    if (scrolledWindow) {
        gtk_widget_set_size_request(scrolledWindow, 600, 350);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow),
                                     GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
        gtk_widget_add_css_class(scrolledWindow, "bluetooth-scroll");
        
        // Device list box
        deviceListBox = gtk_list_box_new();
        if (deviceListBox) {
            gtk_widget_add_css_class(deviceListBox, "bluetooth-list");
            gtk_list_box_set_selection_mode(GTK_LIST_BOX(deviceListBox), GTK_SELECTION_NONE);
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolledWindow), deviceListBox);
        }
        
        gtk_fixed_put(GTK_FIXED(bluetoothContainer), scrolledWindow, 500, 280);
    }
    
    // Apply CSS styling
    GtkCssProvider* cssProvider = gtk_css_provider_new();
    if (cssProvider) {
        
        std::string css = 
            ".bluetooth-title { "
            "  font-family: ElysiaOSNew12; "
            "  font-size: 24px; "
            "  font-weight: 600; "
            "  color: white; "
            "  margin: 8px 0px 12px 0px; "
            "  text-shadow: 0 1px 0 rgba(255, 255, 255, 0.8); "
            "} "
            ".bluetooth-status { "
            "  font-family: ElysiaOSNew12; "
            "  font-size: 16px; "
            "  font-weight: 500; "
            "  color: #4ade80; "
            "  margin: 4px 0px 8px 0px; "
            "  text-shadow: 0 1px 0 rgba(255, 255, 255, 0.7); "
            "} "
            ".bluetooth-status.disabled { "
            "  color: #f87171; "
            "} "
            ".bluetooth-status.scanning { "
            "  color: #fbbf24; "
            "} "
            ".device-label { "
            "  color: white; "
            "  font-family: ElysiaOSNew12; "
            "  font-size: 18px; "
            "  font-weight: 600; "
            "  text-shadow: 0 1px 0 rgba(255, 255, 255, 0.8); "
            "} "
            ".bluetooth-toggle-btn, .bluetooth-scan-btn { "
            "  background: linear-gradient(135deg, #ffa6da 0%, #fed0f4 30%, #ffeef8 70%, #ffffff 100%);"
            "  color: #8b5a6b;"
            "  border: none;"
            "  outline: none;"
            "  padding: 6px 22px;"
            "  border-radius: 20px; "
            "  letter-spacing: 2px;"
            "  font-family: ElysiaOSNew12; "
            "  font-weight: 800; "
            "  font-size: 18px; "
            "  text-shadow: 0 1px 3px rgba(0, 0, 0, 0.2);"
            "  box-shadow: 0 8px 20px rgba(255, 166, 218, 0.4);"
            "} "
            ".bluetooth-toggle-btn:hover, .bluetooth-scan-btn:hover { "
            "  background: linear-gradient(135deg, #fed0f4 0%, #ffeef8 30%, #ffffff 70%, #f8f9fa 100%);"
            "  transform: translateY(-2px);"
            "  box-shadow: 0 12px 25px rgba(255, 166, 218, 0.5);"
            "  text-shadow: 0 2px 4px rgba(0, 0, 0, 0.3);"
            "} "
            ".bluetooth-scroll { "
            "  background: rgba(255, 255, 255, 0.3); "
            "  border-radius: 21px; "
            "  border: 1px solid #c0c0c0; "
            "  box-shadow: 0 8px 25px rgba(0, 0, 0, 0.25), 0 0 1px rgba(0, 0, 0, 0.8); "
            "} "
            ".bluetooth-list { "
            "  background: transparent; "
            "  border: none; "
            "} "
            ".bluetooth-list > row { "
            "  background: transparent; "
            "  border: none; "
            "  padding: 8px; "
            "  border-radius: 8px; "
            "  margin: 4px; "
            "} "
            ".bluetooth-list > row:hover { "
            "  background: rgba(255, 255, 255, 0.1); "
            "} "
            ".device-row { "
            "  background: transparent; "
            "  padding: 12px; "
            "} "
            ".device-name { "
            "  color: white; "
            "  font-family: ElysiaOSNew12; "
            "  font-size: 14px; "
            "  font-weight: 500; "
            "} "
            ".device-mac { "
            "  color: rgba(255, 255, 255, 0.7); "
            "  font-family: ElysiaOSNew12; "
            "  font-size: 12px; "
            "  font-weight: 400; "
            "} "
            ".device-status { "
            "  color: #4ade80; "
            "  font-family: ElysiaOSNew12; "
            "  font-size: 12px; "
            "  font-weight: 500; "
            "} "
            ".device-status.disconnected { "
            "  color: #f87171; "
            "} "
            ".device-connect-btn, .device-disconnect-btn, .device-forget-btn { "
            "  background: rgba(255, 255, 255, 0.15); "
            "  color: white; "
            "  border: 1px solid rgba(255, 255, 255, 0.2); "
            "  border-radius: 8px; "
            "  font-family: ElysiaOSNew12; "
            "  font-size: 10px; "
            "  font-weight: 600; "
            "  padding: 5px 10px; "
            "} "
            ".device-connect-btn:hover { "
            "  background: rgba(255, 255, 255, 0.25); "
            "} "
            ".device-disconnect-btn:hover { "
            "  background: rgba(255, 0, 0, 0.25); "
            "} "
            ".device-forget-btn { "
            "  background: rgba(255, 0, 0, 0.15); "
            "} "
            ".device-forget-btn:hover { "
            "  background: rgba(255, 0, 0, 0.3); "
            "} ";
        
        gtk_css_provider_load_from_string(cssProvider, css.c_str());
        gtk_style_context_add_provider_for_display(
            gdk_display_get_default(),
            GTK_STYLE_PROVIDER(cssProvider),
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

        g_object_unref(cssProvider);
    }
}

void BluetoothManager::setupBackButton() {
    if (!bluetoothContainer) return;
    
    std::string backPath = getAssetPath("back.png");
    
    // Create a button
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
    
    // Position the back button at top left of the bluetooth container
    gtk_fixed_put(GTK_FIXED(bluetoothContainer), backButton, 0, 0);
    
    // Create callback data using smart pointer for automatic cleanup
    std::unique_ptr<BluetoothBackButtonCallbackData> callbackData = std::make_unique<BluetoothBackButtonCallbackData>();
    callbackData->bluetoothManager = this;
    
    // Add click gesture
    GtkGesture* clickGesture = gtk_gesture_click_new();
    if (clickGesture) {
        gtk_widget_add_controller(backButton, GTK_EVENT_CONTROLLER(clickGesture));
        
        // Store the callback data in a way that GTK can manage it
        BluetoothBackButtonCallbackData* dataPtr = callbackData.release(); // Release ownership to GTK
        
        g_signal_connect_data(clickGesture, "pressed", 
            G_CALLBACK(onBackButtonClicked), 
            dataPtr, 
            [](gpointer data, GClosure*) {
                // Cleanup callback data
                delete static_cast<BluetoothBackButtonCallbackData*>(data);
            }, 
            G_CONNECT_DEFAULT);
    }
}

// Bluetooth utility functions
bool BluetoothManager::getBluetoothPowerState() {
    std::string output = executeBluetoothCommand({"show"});
    return output.find("Powered: yes") != std::string::npos;
}

std::vector<BluetoothDevice> BluetoothManager::getBluetoothDevices() {
    std::vector<BluetoothDevice> devices;
    
    if (!getBluetoothPowerState()) {
        return devices; // Return empty list if Bluetooth is off
    }
    
    std::string output = executeBluetoothCommand({"devices"});
    std::istringstream stream(output);
    std::string line;
    
    while (std::getline(stream, line)) {
        if (line.find("Device ") == 0) {
            // Parse: "Device XX:XX:XX:XX:XX:XX Device Name"
            size_t firstSpace = line.find(' ');
            size_t secondSpace = line.find(' ', firstSpace + 1);
            
            if (firstSpace != std::string::npos && secondSpace != std::string::npos) {
                BluetoothDevice device;
                device.mac = line.substr(firstSpace + 1, secondSpace - firstSpace - 1);
                device.name = line.substr(secondSpace + 1);
                
                // Check if device is connected and paired
                device.isConnected = isDeviceConnected(device.mac);
                device.isPaired = isDevicePaired(device.mac);
                
                devices.push_back(device);
            }
        }
    }
    
    return devices;
}

bool BluetoothManager::isDeviceConnected(const std::string& mac) {
    std::string output = executeBluetoothCommand({"info", mac});
    return output.find("Connected: yes") != std::string::npos;
}

bool BluetoothManager::isDevicePaired(const std::string& mac) {
    std::string output = executeBluetoothCommand({"info", mac});
    return output.find("Paired: yes") != std::string::npos;
}

std::string BluetoothManager::executeBluetoothCommand(const std::vector<std::string>& args) {
    std::string command = "bluetoothctl";
    for (const auto& arg : args) {
        command += " " + arg;
    }
    
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    
    if (!pipe) {
        std::cerr << "Failed to run command: " << command << std::endl;
        return "";
    }
    
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    
    return result;
}

void BluetoothManager::toggleBluetoothPower() {
    bool currentState = getBluetoothPowerState();
    std::string newState = currentState ? "off" : "on";
    
    std::cout << "Toggling Bluetooth power to: " << newState << std::endl;
    executeBluetoothCommand({"power", newState});
    
    // Update UI after a short delay
    g_timeout_add(1000, refreshDevicesTimeout, this);
}

void BluetoothManager::scanForDevices() {
    if (isScanning) return;
    
    isScanning = true;
    gtk_button_set_label(GTK_BUTTON(scanButton), TR(TranslationKeys::SCANNING));
    gtk_widget_set_sensitive(scanButton, FALSE);
    
    updateBluetoothStatus();
    
    std::cout << "Starting Bluetooth scan..." << std::endl;
    
    // Start scan in background
    g_timeout_add(100, [](gpointer user_data) -> gboolean {
        BluetoothManager* manager = static_cast<BluetoothManager*>(user_data);
        
        if (manager && manager->bluetoothContainer) {
            // Execute scan commands
            manager->executeBluetoothCommand({"--timeout", "3", "scan", "on"});
            manager->executeBluetoothCommand({"--timeout", "2", "scan", "off"});
            
            // Schedule scan completion
            manager->scanTimeoutId = g_timeout_add(3000, scanCompleteTimeout, manager);
        }
        
        return G_SOURCE_REMOVE;
    }, this);
}

void BluetoothManager::connectToDevice(const std::string& mac, const std::string& name) {
    std::cout << "Connecting to device: " << name << " (" << mac << ")" << std::endl;
    
    // Pair and connect
    executeBluetoothCommand({"pair", mac});
    executeBluetoothCommand({"connect", mac});
    
    showMessage(TR(TranslationKeys::SUCCESS_BLU), std::string(TR(TranslationKeys::CONNECTED_TO)) + name);
    
    // Refresh device list
    g_timeout_add(1000, refreshDevicesTimeout, this);
}

void BluetoothManager::disconnectDevice(const std::string& mac, const std::string& name) {
    std::cout << "Disconnecting device: " << name << " (" << mac << ")" << std::endl;
    
    executeBluetoothCommand({"disconnect", mac});
    
    showMessage(TR(TranslationKeys::DISCONNECTED_BLU), std::string(TR(TranslationKeys::DISCONNECTED_BLU)) + " from " + name);
    
    // Refresh device list
    g_timeout_add(1000, refreshDevicesTimeout, this);
}

void BluetoothManager::forgetDevice(const std::string& mac, const std::string& name) {
    std::cout << "Forgetting device: " << name << " (" << mac << ")" << std::endl;
    
    executeBluetoothCommand({"remove", mac});
    
    showMessage(TR(TranslationKeys::REMOVED), "Forgot device " + name);
    
    // Refresh device list
    g_timeout_add(1000, refreshDevicesTimeout, this);
}

void BluetoothManager::showMessage(const std::string& title, const std::string& message) {
    GtkWidget* dialog = gtk_dialog_new_with_buttons(
        title.c_str(),
        GTK_WINDOW(parentWindow),
        GTK_DIALOG_MODAL,
        TR(TranslationKeys::OK), GTK_RESPONSE_OK,
        nullptr
    );
    
    GtkWidget* content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget* label = gtk_label_new(message.c_str());
    gtk_widget_set_margin_top(label, 20);
    gtk_widget_set_margin_bottom(label, 20);
    gtk_widget_set_margin_start(label, 20);
    gtk_widget_set_margin_end(label, 20);
    gtk_box_append(GTK_BOX(content_area), label);
    
    g_signal_connect(dialog, "response", G_CALLBACK(onMessageDialogResponse), nullptr);
    
    gtk_window_present(GTK_WINDOW(dialog));
}

void BluetoothManager::updateBluetoothStatus() {
    if (!connectionStatusLabel) return;
    
    // Remove existing CSS classes
    gtk_widget_remove_css_class(connectionStatusLabel, "disabled");
    gtk_widget_remove_css_class(connectionStatusLabel, "scanning");
    
    std::string statusText;
    std::string cssClass;
    
    bluetoothEnabled = getBluetoothPowerState();
    
    if (isScanning) {
        statusText = TR(TranslationKeys::SCANNING_FOR_DEVICES);
        cssClass = "scanning";
    } else if (bluetoothEnabled) {
        // Count connected devices
        auto devices = getBluetoothDevices();
        int connectedCount = 0;
        std::string connectedDeviceName;
        
        for (const auto& device : devices) {
            if (device.isConnected) {
                connectedCount++;
                if (connectedCount == 1) {
                    connectedDeviceName = device.name;
                }
            }
        }
        
        if (connectedCount == 0) {
            statusText = TR(TranslationKeys::BLUETOOTH_ENABLED_NO_DEVICES);
        } else if (connectedCount == 1) {
            statusText = std::string(TR(TranslationKeys::CONNECTED_TO)) + connectedDeviceName;
        } else {
            statusText = std::string(TR(TranslationKeys::CONNECTED_TO_MULTIPLE)) + std::to_string(connectedCount) + " devices";
        }
    } else {
        statusText = TR(TranslationKeys::BLUETOOTH_DISABLED);
        cssClass = "disabled";
    }
    
    gtk_label_set_text(GTK_LABEL(connectionStatusLabel), statusText.c_str());
    
    if (!cssClass.empty()) {
        gtk_widget_add_css_class(connectionStatusLabel, cssClass.c_str());
    }
    
    // Update button states
    if (toggleButton) {
        gtk_button_set_label(GTK_BUTTON(toggleButton), 
                           bluetoothEnabled ? TR(TranslationKeys::TURN_OFF) : TR(TranslationKeys::TURN_ON));
    }
    
    if (scanButton) {
        gtk_widget_set_sensitive(scanButton, bluetoothEnabled && !isScanning);
    }
}

void BluetoothManager::populateDeviceList() {
    if (!deviceListBox) return;
    
    // Clear existing list
    GtkWidget* child = gtk_widget_get_first_child(deviceListBox);
    while (child) {
        GtkWidget* next = gtk_widget_get_next_sibling(child);
        gtk_list_box_remove(GTK_LIST_BOX(deviceListBox), child);
        child = next;
    }
    
    // Clear device widgets vector
    deviceWidgets.clear();
    
    if (!bluetoothEnabled) {
        GtkWidget* row = gtk_list_box_row_new();
        GtkWidget* label = gtk_label_new(TR(TranslationKeys::BLUETOOTH_DISABLED));
        gtk_widget_add_css_class(label, "device-name");
        gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), label);
        gtk_list_box_append(GTK_LIST_BOX(deviceListBox), row);
        return;
    }
    
    auto devices = getBluetoothDevices();
    
    if (devices.empty()) {
        GtkWidget* row = gtk_list_box_row_new();
        GtkWidget* label = gtk_label_new(TR(TranslationKeys::NO_DEVICES_FOUND_BT));
        gtk_widget_add_css_class(label, "device-name");
        gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), label);
        gtk_list_box_append(GTK_LIST_BOX(deviceListBox), row);
        return;
    }
    
    for (const auto& device : devices) {
        buildDeviceRow(device);
    }
}

void BluetoothManager::buildDeviceRow(const BluetoothDevice& device) {
    GtkWidget* row = gtk_list_box_row_new();
    GtkWidget* mainBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    
    gtk_widget_add_css_class(mainBox, "device-row");
    gtk_widget_set_size_request(mainBox, 580, 60);
    
    // Device info box
    GtkWidget* infoBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_widget_set_hexpand(infoBox, TRUE);
    
    // Device name
    GtkWidget* nameLabel = gtk_label_new(device.name.c_str());
    gtk_widget_add_css_class(nameLabel, "device-name");
    gtk_label_set_xalign(GTK_LABEL(nameLabel), 0.0);
    gtk_box_append(GTK_BOX(infoBox), nameLabel);
    
    // Device MAC and status
    std::string statusText = device.mac;
    if (device.isConnected) {
        statusText += TR(TranslationKeys::CONNECTED_STATUS);
    } else if (device.isPaired) {
        statusText += TR(TranslationKeys::PAIRED_STATUS);
    } else {
        statusText += TR(TranslationKeys::NOT_PAIRED_STATUS);
    }
    
    GtkWidget* statusLabel = gtk_label_new(statusText.c_str());
    if (device.isConnected) {
        gtk_widget_add_css_class(statusLabel, "device-status");
    } else {
        gtk_widget_add_css_class(statusLabel, "device-status");
        gtk_widget_add_css_class(statusLabel, "disconnected");
    }
    gtk_label_set_xalign(GTK_LABEL(statusLabel), 0.0);
    gtk_box_append(GTK_BOX(infoBox), statusLabel);
    
    gtk_box_append(GTK_BOX(mainBox), infoBox);
    
    // Button box
    GtkWidget* buttonBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    
    // Connect/Disconnect button
    if (device.isConnected) {
        GtkWidget* disconnectBtn = gtk_button_new_with_label(TR(TranslationKeys::DISCONNECT_BLU));
        gtk_widget_add_css_class(disconnectBtn, "device-disconnect-btn");
        gtk_widget_set_size_request(disconnectBtn, 80, 30);
        
        // Store device info for callback
        DeviceActionData* actionData = new DeviceActionData{this, device.mac, device.name};
        g_object_set_data_full(G_OBJECT(disconnectBtn), "action_data", actionData, 
                              [](gpointer data) { delete static_cast<DeviceActionData*>(data); });
        
        g_signal_connect(disconnectBtn, "clicked", G_CALLBACK(onDisconnectButtonClicked), actionData);
        gtk_box_append(GTK_BOX(buttonBox), disconnectBtn);
    } else {
        GtkWidget* connectBtn = gtk_button_new_with_label(TR(TranslationKeys::CONNECT_BLU));
        gtk_widget_add_css_class(connectBtn, "device-connect-btn");
        gtk_widget_set_size_request(connectBtn, 80, 30);
        
        // Store device info for callback
        DeviceActionData* actionData = new DeviceActionData{this, device.mac, device.name};
        g_object_set_data_full(G_OBJECT(connectBtn), "action_data", actionData,
                              [](gpointer data) { delete static_cast<DeviceActionData*>(data); });
        
        g_signal_connect(connectBtn, "clicked", G_CALLBACK(onConnectButtonClicked), actionData);
        gtk_box_append(GTK_BOX(buttonBox), connectBtn);
    }
    
    // Forget button
    GtkWidget* forgetBtn = gtk_button_new_with_label(TR(TranslationKeys::FORGET));
    gtk_widget_add_css_class(forgetBtn, "device-forget-btn");
    gtk_widget_set_size_request(forgetBtn, 80, 30);
    
    // Store device info for callback
    DeviceActionData* actionData = new DeviceActionData{this, device.mac, device.name};
    g_object_set_data_full(G_OBJECT(forgetBtn), "action_data", actionData,
                          [](gpointer data) { delete static_cast<DeviceActionData*>(data); });
    
    g_signal_connect(forgetBtn, "clicked", G_CALLBACK(onForgetButtonClicked), actionData);
    gtk_box_append(GTK_BOX(buttonBox), forgetBtn);
    
    gtk_box_append(GTK_BOX(mainBox), buttonBox);
    
    gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), mainBox);
    gtk_list_box_append(GTK_LIST_BOX(deviceListBox), row);
    
    deviceWidgets.push_back(row);
}

// Event handlers
void BluetoothManager::onBackButtonClicked(GtkGestureClick* gesture, int n_press, double x, double y, gpointer user_data) {
    (void)gesture; (void)n_press; (void)x; (void)y;
    
    BluetoothBackButtonCallbackData* data = static_cast<BluetoothBackButtonCallbackData*>(user_data);
    if (data && data->bluetoothManager) {
        // Hide bluetooth page
        data->bluetoothManager->hide();
        
        // Show main menu
        if (data->bluetoothManager->mainWindow) {
            data->bluetoothManager->mainWindow->showMainMenu();
        }
    }
}

void BluetoothManager::onToggleBluetoothClicked(GtkButton* button, gpointer user_data) {
    (void)button;
    BluetoothManager* manager = static_cast<BluetoothManager*>(user_data);
    if (manager) {
        manager->toggleBluetoothPower();
    }
}

void BluetoothManager::onScanDevicesClicked(GtkButton* button, gpointer user_data) {
    (void)button;
    BluetoothManager* manager = static_cast<BluetoothManager*>(user_data);
    if (manager && manager->bluetoothEnabled) {
        manager->scanForDevices();
    }
}

void BluetoothManager::onConnectButtonClicked(GtkButton* button, gpointer user_data) {
    (void)button;
    DeviceActionData* data = static_cast<DeviceActionData*>(user_data);
    if (data && data->manager) {
        data->manager->connectToDevice(data->mac, data->name);
    }
}

void BluetoothManager::onDisconnectButtonClicked(GtkButton* button, gpointer user_data) {
    (void)button;
    DeviceActionData* data = static_cast<DeviceActionData*>(user_data);
    if (data && data->manager) {
        data->manager->disconnectDevice(data->mac, data->name);
    }
}

void BluetoothManager::onForgetButtonClicked(GtkButton* button, gpointer user_data) {
    (void)button;
    DeviceActionData* data = static_cast<DeviceActionData*>(user_data);
    if (data && data->manager) {
        data->manager->forgetDevice(data->mac, data->name);
    }
}

gboolean BluetoothManager::refreshDevicesTimeout(gpointer user_data) {
    BluetoothManager* manager = static_cast<BluetoothManager*>(user_data);
    if (manager && manager->bluetoothContainer) {
        manager->updateBluetoothStatus();
        manager->populateDeviceList();
    }
    return G_SOURCE_CONTINUE; // Keep the timeout running
}

gboolean BluetoothManager::scanCompleteTimeout(gpointer user_data) {
    BluetoothManager* manager = static_cast<BluetoothManager*>(user_data);
    if (manager && manager->bluetoothContainer) {
        manager->isScanning = false;
        manager->scanTimeoutId = 0;
        
        if (manager->scanButton) {
            gtk_button_set_label(GTK_BUTTON(manager->scanButton), "Scan Devices");
            gtk_widget_set_sensitive(manager->scanButton, manager->bluetoothEnabled);
        }
        
        manager->updateBluetoothStatus();
        manager->populateDeviceList();
    }
    
    return G_SOURCE_REMOVE;
}

void BluetoothManager::onMessageDialogResponse(GtkDialog* dialog, gint response_id, gpointer user_data) {
    (void)response_id;
    (void)user_data;
    gtk_window_destroy(GTK_WINDOW(dialog));
}

std::string BluetoothManager::getAssetPath(const std::string& filename) {
    const char* home = std::getenv("HOME");
    if (!home) {
        std::cerr << "HOME environment variable not set" << std::endl;
        return filename;
    }
    return std::string(home) + "/.config/Elysia/assets/assets/" + filename;
}
