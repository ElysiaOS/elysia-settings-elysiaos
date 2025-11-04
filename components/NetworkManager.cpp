#include "NetworkManager.h"
#include "../MainWindow.h"
#include "../translations/translations.h"
#include <iostream>
#include <glib.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <sys/wait.h>
#include <memory>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <arpa/inet.h>
#include <netinet/in.h>

// Forward declaration of callback data
struct NetworkBackButtonCallbackData {
    NetworkManager* networkManager;
};

struct ConnectButtonData {
    NetworkManager* networkManager;
    std::string ssid;
    NMAccessPoint* ap;
    bool isSecured;
};

struct PasswordDialogData {
    NetworkManager* networkManager;
    NMDeviceWifi* wifiDev;
    NMAccessPoint* ap;
    std::string ssid;
    GtkWidget* entry;
    GtkWidget* dialog;
};

struct NetworkSettingsData {
    NetworkManager* networkManager;
    std::string ssid;
    NMActiveConnection* activeConnection;
};

NetworkManager::NetworkManager(MainWindow* mainWindow, GtkWidget* parentWindow, GtkWidget* overlay) 
    : mainWindow(mainWindow), parentWindow(parentWindow), overlay(overlay), 
      networkContainer(nullptr), backButton(nullptr), wifiSwitch(nullptr), 
      wifiListBox(nullptr), refreshButton(nullptr), scrolledWindow(nullptr),
      connectionStatusLabel(nullptr), nmClient(nullptr), updatingWifiSwitch(false), 
      networkingEnabled(false), hasEthernetConnection(false), updateTimeoutId(0) {
    setupUI();
}

NetworkManager::~NetworkManager() {
    // Clean up timeouts
    if (updateTimeoutId > 0) {
        g_source_remove(updateTimeoutId);
        updateTimeoutId = 0;
    }
    
    // Clear widget tracking
    networkWidgets.clear();
    
    // Clean up NetworkManager client
    if (nmClient) {
        g_object_unref(nmClient);
        nmClient = nullptr;
    }
    
    // All GTK widgets are managed by GTK and will be cleaned up automatically
}

void NetworkManager::show() {
    if (networkContainer) {
        gtk_widget_set_visible(networkContainer, TRUE);
        // Switch to the network background
        if (mainWindow) {
            mainWindow->switchToBackground("background4.png");
        }
        refreshNetworks();
        updateConnectionStatus();
    }
}

void NetworkManager::hide() {
    if (networkContainer) {
        gtk_widget_set_visible(networkContainer, FALSE);
    }
}

void NetworkManager::setupUI() {
    // Create container for network page
    networkContainer = gtk_fixed_new();
    if (overlay && networkContainer) {
        gtk_overlay_add_overlay(GTK_OVERLAY(overlay), networkContainer);
        gtk_widget_set_visible(networkContainer, FALSE);
        
        initNetworkUI();
        setupBackButton();
        setupNetworkManager();
    }
}

void NetworkManager::initNetworkUI() {
    if (!networkContainer) return;
    
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
        gtk_widget_add_css_class(connectionStatusLabel, "connection-status");
        
        gtk_fixed_put(GTK_FIXED(networkContainer), connectionStatusLabel, 600, 70);
    }
    
    // Add title label
    GtkWidget* titleLabel = gtk_label_new(TR(TranslationKeys::NETWORK_TITLE));
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
        gtk_widget_add_css_class(titleLabel, "network-title");
        
        gtk_fixed_put(GTK_FIXED(networkContainer), titleLabel, 650, 120);
    }
    
    // WiFi header with switch and refresh button
    GtkWidget* wifiHeader = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 15);
    if (wifiHeader) {
        gtk_widget_set_size_request(wifiHeader, 600, 40);
        
        // WiFi icon
        GtkWidget* wifiIcon = gtk_image_new_from_icon_name("network-wireless-symbolic");
        if (wifiIcon) {
            gtk_image_set_pixel_size(GTK_IMAGE(wifiIcon), 20);
            gtk_box_append(GTK_BOX(wifiHeader), wifiIcon);
        }
        
        // WiFi label
        GtkWidget* wifiLabel = gtk_label_new(TR(TranslationKeys::WIFI));
        if (wifiLabel) {
            gtk_widget_add_css_class(wifiLabel, "wifi-label");
            gtk_box_append(GTK_BOX(wifiHeader), wifiLabel);
        }
        
        // Spacer
        GtkWidget* spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        gtk_widget_set_hexpand(spacer, TRUE);
        gtk_box_append(GTK_BOX(wifiHeader), spacer);
        
        // WiFi switch
        wifiSwitch = gtk_switch_new();
        if (wifiSwitch) {
            gtk_switch_set_active(GTK_SWITCH(wifiSwitch), TRUE);
            gtk_widget_add_css_class(wifiSwitch, "wifi-switch");
            g_signal_connect(wifiSwitch, "state-set", G_CALLBACK(onWifiSwitchStateSet), this);
            gtk_box_append(GTK_BOX(wifiHeader), wifiSwitch);
        }
        
        // Refresh button
        refreshButton = gtk_button_new();
        if (refreshButton) {
            GtkWidget* refreshIcon = gtk_image_new_from_icon_name("view-refresh-symbolic");
            gtk_image_set_pixel_size(GTK_IMAGE(refreshIcon), 16);
            gtk_button_set_child(GTK_BUTTON(refreshButton), refreshIcon);
            gtk_widget_add_css_class(refreshButton, "refresh-button");
            gtk_widget_add_css_class(refreshButton, "flat");
            gtk_widget_add_css_class(refreshButton, "circular");
            gtk_widget_set_tooltip_text(refreshButton, TR(TranslationKeys::REFRESH_NETWORKS));
            g_signal_connect(refreshButton, "clicked", G_CALLBACK(onRefreshButtonClicked), this);
            gtk_box_append(GTK_BOX(wifiHeader), refreshButton);
        }
        
        gtk_fixed_put(GTK_FIXED(networkContainer), wifiHeader, 500, 180);
    }
    
    // Create scrollable container for networks
    scrolledWindow = gtk_scrolled_window_new();
    if (scrolledWindow) {
        gtk_widget_set_size_request(scrolledWindow, 620, 420);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow), 
                                      GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
        
        // Create list box for networks
        wifiListBox = gtk_list_box_new();
        if (wifiListBox) {
            gtk_widget_add_css_class(wifiListBox, "wifi-list");
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolledWindow), wifiListBox);
        }
        
        gtk_fixed_put(GTK_FIXED(networkContainer), scrolledWindow, 490, 240);
    }
    
    // CSS for network page with light pink ElysiaOS aesthetic
    GtkCssProvider* provider = gtk_css_provider_new();
    if (provider) {
        const char* css =
            "window { "
            "  background: rgba(255, 255, 255, 0.3); "
            "  border-radius: 21px; "
            "  border: 1px solid #c0c0c0; "
            "  box-shadow: 0 8px 25px rgba(0, 0, 0, 0.25), 0 0 1px rgba(0, 0, 0, 0.8); "
            "} "
            ".network-title { "
            "  font-family: ElysiaOSNew12; "
            "  font-size: 24px; "
            "  font-weight: 600; "
            "  color: white; "
            "  margin: 8px 0px 12px 0px; "
            "  text-shadow: 0 1px 0 rgba(255, 255, 255, 0.8); "
            "} "
            ".connection-status { "
            "  font-family: ElysiaOSNew12; "
            "  font-size: 16px; "
            "  font-weight: 500; "
            "  color: #4ade80; "
            "  margin: 4px 0px 8px 0px; "
            "  text-shadow: 0 1px 0 rgba(255, 255, 255, 0.7); "
            "} "
            ".connection-status.disconnected { "
            "  color: #f87171; "
            "} "
            ".connection-status.checking { "
            "  color: #fbbf24; "
            "} "
            ".wifi-label { "
            "  color: white; "
            "  font-family: ElysiaOSNew12; "
            "  font-size: 18px; "
            "  font-weight: 600; "
            "  text-shadow: 0 1px 0 rgba(255, 255, 255, 0.7); "
            "} "
            ".wifi-switch { "
            "  background: linear-gradient(145deg, rgba(255, 255, 255, 0.2), rgba(255, 255, 255, 0.1)); "
            "  border: 1px solid rgba(192, 192, 192, 0.6); "
            "  border-radius: 12px; "
            "} "
            ".wifi-switch:checked { "
            "  background: linear-gradient(90deg, #e5a7c6 0%, #edcee3 100%); "
            "} "
            ".refresh-button { "
            "  background: linear-gradient(145deg, rgba(255, 255, 255, 0.2), rgba(255, 255, 255, 0.1)); "
            "  border: 1px solid rgba(192, 192, 192, 0.6); "
            "  border-radius: 8px; "
            "  color: white; "
            "} "
            ".refresh-button:hover { "
            "  background: linear-gradient(62deg, #fd84cb 20%, #fed0f4 70%); "
            "} "
            ".wifi-list { "
            "  background: transparent; "
            "  border: none; "
            "} "
            ".wifi-list row { "
            "  background: linear-gradient(145deg, rgba(255, 255, 255, 0.2), rgba(255, 255, 255, 0.1)); "
            "  border: 1px solid rgba(192, 192, 192, 0.6); "
            "  border-radius: 14px; "
            "  margin: 4px 0; "
            "  padding: 8px; "
            "  box-shadow: 0 1px 3px rgba(0, 0, 0, 0.15); "
            "} "
            ".wifi-list row:hover { "
            "  background: linear-gradient(62deg, rgba(253, 132, 203, 0.3) 20%, rgba(254, 208, 244, 0.4) 70%); "
            "} "
            ".network-name { "
            "  color: white; "
            "  font-family: ElysiaOSNew12; "
            "  font-size: 16px; "
            "  font-weight: 600; "
            "  text-shadow: 0 1px 0 rgba(255, 255, 255, 0.7); "
            "} "
            ".network-status { "
            "  color: white; "
            "  font-family: ElysiaOSNew12; "
            "  font-size: 12px; "
            "  text-shadow: 0 1px 0 rgba(255, 255, 255, 0.7); "
            "} "
            ".network-status.connected { "
            "  color: #4ade80; "
            "  font-weight: 600; "
            "} "
            ".network-status.saved { "
            "  color: #60a5fa; "
            "  font-weight: 500; "
            "} "
            ".network-status.secured { "
            "  color: #fbbf24; "
            "  font-weight: 400; "
            "} "
            ".connect-button { "
            "  background: linear-gradient(145deg, rgba(255, 255, 255, 0.2), rgba(255, 255, 255, 0.1)); "
            "  border: 1px solid rgba(192, 192, 192, 0.6); "
            "  border-radius: 8px; "
            "  color: white; "
            "  min-width: 32px; "
            "  min-height: 32px; "
            "} "
            ".connect-button:hover { "
            "  background: linear-gradient(62deg, #fd84cb 20%, #fed0f4 70%); "
            "} "
            ".settings-button { "
            "  background: linear-gradient(145deg, rgba(255, 255, 255, 0.2), rgba(255, 255, 255, 0.1)); "
            "  border: 1px solid rgba(192, 192, 192, 0.6); "
            "  border-radius: 8px; "
            "  color: white; "
            "  min-width: 32px; "
            "  min-height: 32px; "
            "  margin-end: 8px; "
            "} "
            ".settings-button:hover { "
            "  background: linear-gradient(62deg, #a7c6e5 20%, #c6e3fd 70%); "
            "} "
            ".enable-networking-button { "
            "  background: linear-gradient(90deg, #e5a7c6 0%, #edcee3 100%); "
            "  border: 1px solid rgba(229, 167, 198, 0.6); "
            "  border-radius: 8px; "
            "  color: white; "
            "  font-family: ElysiaOSNew12; "
            "  font-size: 14px; "
            "  font-weight: 600; "
            "  padding: 8px 16px; "
            "  text-shadow: 0 1px 0 rgba(255, 255, 255, 0.7); "
            "} "
            ".enable-networking-button:hover { "
            "  background: linear-gradient(62deg, #fd84cb 20%, #fed0f4 70%); "
            "} "
            ".message-label { "
            "  color: white; "
            "  font-family: ElysiaOSNew12; "
            "  font-size: 14px; "
            "  text-shadow: 0 1px 0 rgba(255, 255, 255, 0.7); "
            "  text-align: center; "
            "} "
            ".back-button-label { "
            "  color: white; "
            "  font-weight: bold; "
            "  font-family: ElysiaOSNew12; "
            "  font-size: 24px; "
            "  text-shadow: 0 1px 0 rgba(255, 255, 255, 0.8); "
            "} "
            "button { "
            "  background: transparent; "
            "  border: none; "
            "  color: white; "
            "  font-family: ElysiaOSNew12; "
            "  font-weight: bold; "
            "  font-size: 16px; "
            "  text-shadow: 0 1px 0 rgba(255, 255, 255, 0.8); "
            "} "
            "button:hover { "
            "  background: transparent; "
            "  border-radius: 8px; "
            "  transition: all 0.3s ease; "
            "} "
            "scrolledwindow { "
            "  background: transparent; "
            "  border: none; "
            "} "
            "scrollbar { "
            "  background: transparent; "
            "} "
            "scrollbar slider { "
            "  background: linear-gradient(145deg, rgba(255, 255, 255, 0.2), rgba(255, 255, 255, 0.1)); "
            "  border-radius: 6px; "
            "  min-width: 8px; "
            "} "
            "scrollbar slider:hover { "
            "  background: linear-gradient(62deg, #fd84cb 20%, #fed0f4 70%); "
            "} ";

        gtk_css_provider_load_from_string(provider, css);
        gtk_style_context_add_provider_for_display(
            gdk_display_get_default(),
            GTK_STYLE_PROVIDER(provider),
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

        g_object_unref(provider);
    }
}

void NetworkManager::setupBackButton() {
    if (!networkContainer) return;
    
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
    
    // Position the back button at top left of the network container
    gtk_fixed_put(GTK_FIXED(networkContainer), backButton, 0, 0);
    
    // Create callback data using smart pointer for automatic cleanup
    std::unique_ptr<NetworkBackButtonCallbackData> callbackData = std::make_unique<NetworkBackButtonCallbackData>();
    callbackData->networkManager = this;
    
    // Add click gesture
    GtkGesture* clickGesture = gtk_gesture_click_new();
    if (clickGesture) {
        gtk_widget_add_controller(backButton, GTK_EVENT_CONTROLLER(clickGesture));
        
        // Store the callback data in a way that GTK can manage it
        NetworkBackButtonCallbackData* dataPtr = callbackData.release(); // Release ownership to GTK
        
        g_signal_connect_data(clickGesture, "pressed", 
            G_CALLBACK(onBackButtonClicked), 
            dataPtr, 
            [](gpointer data, GClosure*) {
                // Cleanup callback data
                delete static_cast<NetworkBackButtonCallbackData*>(data);
            }, 
            G_CONNECT_DEFAULT);
    }
}

void NetworkManager::setupNetworkManager() {
    // Initialize NetworkManager client
    nmClient = nm_client_new(nullptr, nullptr);
    if (nmClient) {
        std::cout << "NetworkManager client initialized successfully" << std::endl;
        
        // Initialize network state
        networkingEnabled = checkNetworkingEnabled();
        hasEthernetConnection = checkEthernetConnection();
        
        std::cout << "Initial network state: networking=" << (networkingEnabled ? "enabled" : "disabled")
                  << ", ethernet=" << (hasEthernetConnection ? "connected" : "disconnected") << std::endl;
        
        // Connect to NetworkManager state change signals
        g_signal_connect(nmClient, "notify::wireless-enabled", 
                        G_CALLBACK(onNetworkManagerChanged), this);
        g_signal_connect(nmClient, "notify::wireless-hardware-enabled", 
                        G_CALLBACK(onNetworkManagerChanged), this);
        g_signal_connect(nmClient, "notify::networking-enabled", 
                        G_CALLBACK(onNetworkManagerChanged), this);
        
        // Connect to Wi-Fi device signals
        NMDeviceWifi* wifi = getPrimaryWifiDevice();
        if (wifi) {
            std::cout << "Found Wi-Fi device: " << nm_device_get_iface(NM_DEVICE(wifi)) << std::endl;
            g_signal_connect(wifi, "notify::active-access-point", 
                           G_CALLBACK(onWifiDeviceChanged), this);
            g_signal_connect(wifi, "notify::access-points", 
                           G_CALLBACK(onWifiDeviceChanged), this);
        } else {
            std::cout << "No Wi-Fi device found" << std::endl;
        }
        
        // Initial state setup
        reflectWifiSwitchState();
        populateWifiList();
        updateConnectionStatus();
        
        // Auto-enable networking if disabled
        if (!networkingEnabled) {
            std::cout << "Networking is disabled, auto-enabling..." << std::endl;
            enableNetworking();
        }
        
    } else {
        std::cout << "Failed to initialize NetworkManager client" << std::endl;
        
        // Show error in the list
        if (wifiListBox) {
            GtkWidget* row = gtk_list_box_row_new();
            GtkWidget* label = gtk_label_new(TR(TranslationKeys::NETWORKMANAGER_NOT_AVAILABLE));
            gtk_widget_add_css_class(label, "message-label");
            gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), label);
            gtk_list_box_append(GTK_LIST_BOX(wifiListBox), row);
        }
        
        // Disable Wi-Fi controls
        if (wifiSwitch) gtk_widget_set_sensitive(wifiSwitch, FALSE);
        if (refreshButton) gtk_widget_set_sensitive(refreshButton, FALSE);
    }
}

void NetworkManager::refreshNetworks() {
    scanWifiNetworks();
    updateConnectionStatus();
}

void NetworkManager::populateWifiList() {
    if (!wifiListBox) return;
    
    // Clear existing list - GTK will handle cleanup of g_object_set_data_full callback data
    GtkWidget* child = gtk_widget_get_first_child(wifiListBox);
    while (child) {
        GtkWidget* next = gtk_widget_get_next_sibling(child);
        gtk_list_box_remove(GTK_LIST_BOX(wifiListBox), child);
        child = next;
    }
    networkWidgets.clear();
    
    // Check if networking is enabled
    if (!networkingEnabled) {
        GtkWidget* row = gtk_list_box_row_new();
        
        GtkWidget* rowBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        gtk_widget_set_margin_top(rowBox, 20);
        gtk_widget_set_margin_bottom(rowBox, 20);
        gtk_widget_set_margin_start(rowBox, 20);
        gtk_widget_set_margin_end(rowBox, 20);
        
        GtkWidget* label = gtk_label_new(TR(TranslationKeys::NETWORKING_DISABLED));
        gtk_widget_add_css_class(label, "message-label");
        gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
        gtk_box_append(GTK_BOX(rowBox), label);
        
        GtkWidget* enableBtn = gtk_button_new_with_label(TR(TranslationKeys::ENABLE_NETWORKING));
        gtk_widget_add_css_class(enableBtn, "enable-networking-button");
        gtk_widget_set_halign(enableBtn, GTK_ALIGN_CENTER);
        g_signal_connect(enableBtn, "clicked", G_CALLBACK(onEnableNetworkingClicked), this);
        gtk_box_append(GTK_BOX(rowBox), enableBtn);
        
        gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), rowBox);
        gtk_list_box_append(GTK_LIST_BOX(wifiListBox), row);
        networkWidgets.push_back(row);
        return;
    }
    
    // Check if already connected via ethernet
    if (hasEthernetConnection) {
        GtkWidget* row = gtk_list_box_row_new();
        GtkWidget* label = gtk_label_new(TR(TranslationKeys::CONNECTED_VIA_ETHERNET));
        gtk_widget_add_css_class(label, "message-label");
        gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
        gtk_widget_set_margin_top(label, 20);
        gtk_widget_set_margin_bottom(label, 20);
        gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), label);
        gtk_list_box_append(GTK_LIST_BOX(wifiListBox), row);
        networkWidgets.push_back(row);
        return;
    }
    
    // Check if Wi-Fi is enabled
    if (!nmClient) return;
    
    gboolean hwEnabled = nm_client_wireless_hardware_get_enabled(nmClient);
    gboolean swEnabled = nm_client_wireless_get_enabled(nmClient);
    
    if (!hwEnabled) {
        GtkWidget* row = gtk_list_box_row_new();
        GtkWidget* label = gtk_label_new(TR(TranslationKeys::WIFI_HARDWARE_DISABLED));
        gtk_widget_add_css_class(label, "message-label");
        gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
        gtk_widget_set_margin_top(label, 20);
        gtk_widget_set_margin_bottom(label, 20);
        gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), label);
        gtk_list_box_append(GTK_LIST_BOX(wifiListBox), row);
        networkWidgets.push_back(row);
        return;
    }
    
    if (!swEnabled) {
        GtkWidget* row = gtk_list_box_row_new();
        GtkWidget* label = gtk_label_new(TR(TranslationKeys::WIFI_DISABLED));
        gtk_widget_add_css_class(label, "message-label");
        gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
        gtk_widget_set_margin_top(label, 20);
        gtk_widget_set_margin_bottom(label, 20);
        gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), label);
        gtk_list_box_append(GTK_LIST_BOX(wifiListBox), row);
        networkWidgets.push_back(row);
        return;
    }
    
    // Get available networks
    std::vector<WifiNetwork> networks = getAvailableNetworks();
    
    if (networks.empty()) {
        GtkWidget* row = gtk_list_box_row_new();
        GtkWidget* label = gtk_label_new(TR(TranslationKeys::NO_NETWORKS_FOUND));
        gtk_widget_add_css_class(label, "message-label");
        gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
        gtk_widget_set_margin_top(label, 20);
        gtk_widget_set_margin_bottom(label, 20);
        gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), label);
        gtk_list_box_append(GTK_LIST_BOX(wifiListBox), row);
        networkWidgets.push_back(row);
        return;
    }
    
    // Build network rows
    for (const WifiNetwork& network : networks) {
        buildNetworkRow(network);
    }
}

void NetworkManager::buildNetworkRow(const WifiNetwork& network) {
    if (!wifiListBox) return;
    
    GtkWidget* row = gtk_list_box_row_new();
    gtk_widget_set_size_request(row, -1, 70);
    
    GtkWidget* rowBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 15);
    gtk_widget_set_margin_top(rowBox, 12);
    gtk_widget_set_margin_bottom(rowBox, 12);
    gtk_widget_set_margin_start(rowBox, 20);
    gtk_widget_set_margin_end(rowBox, 20);
    
    // Signal strength icon
    const char* iconName;
    if (network.strength > 75) iconName = "network-wireless-signal-excellent-symbolic";
    else if (network.strength > 50) iconName = "network-wireless-signal-good-symbolic";
    else if (network.strength > 25) iconName = "network-wireless-signal-ok-symbolic";
    else iconName = "network-wireless-signal-weak-symbolic";
    
    GtkWidget* signalIcon = gtk_image_new_from_icon_name(iconName);
    gtk_image_set_pixel_size(GTK_IMAGE(signalIcon), 24);
    gtk_box_append(GTK_BOX(rowBox), signalIcon);
    
    // Network info box
    GtkWidget* infoBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    
    // Network name
    GtkWidget* nameLabel = gtk_label_new(network.ssid.c_str());
    gtk_widget_add_css_class(nameLabel, "network-name");
    gtk_widget_set_halign(nameLabel, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(infoBox), nameLabel);
    
    // Network status
    if (!network.status.empty()) {
        GtkWidget* statusLabel = gtk_label_new(network.status.c_str());
        gtk_widget_add_css_class(statusLabel, "network-status");
        if (network.isActive) {
            gtk_widget_add_css_class(statusLabel, "connected");
        } else if (network.isSaved) {
            gtk_widget_add_css_class(statusLabel, "saved");
        } else if (network.isSecured) {
            gtk_widget_add_css_class(statusLabel, "secured");
        }
        gtk_widget_set_halign(statusLabel, GTK_ALIGN_START);
        gtk_box_append(GTK_BOX(infoBox), statusLabel);
    }
    
    gtk_widget_set_hexpand(infoBox, TRUE);
    gtk_box_append(GTK_BOX(rowBox), infoBox);
    
    // Security icon
    if (network.isSecured) {
        GtkWidget* lockIcon = gtk_image_new_from_icon_name("network-wireless-encrypted-symbolic");
        gtk_image_set_pixel_size(GTK_IMAGE(lockIcon), 16);
        gtk_box_append(GTK_BOX(rowBox), lockIcon);
    }
    
        // Settings button for connected networks
    if (network.isActive) {
        GtkWidget* settingsBtn = gtk_button_new();
        GtkWidget* settingsIcon = gtk_image_new_from_icon_name("preferences-system-symbolic");
        gtk_image_set_pixel_size(GTK_IMAGE(settingsIcon), 16);
        gtk_button_set_child(GTK_BUTTON(settingsBtn), settingsIcon);
        gtk_widget_add_css_class(settingsBtn, "settings-button");
        gtk_widget_add_css_class(settingsBtn, "flat");
        gtk_widget_add_css_class(settingsBtn, "circular");
        gtk_widget_set_tooltip_text(settingsBtn, TR(TranslationKeys::NETWORK_SETTINGS));
        
        // Find active connection for this network
        NMActiveConnection* activeConn = nullptr;
        const GPtrArray* activeConnections = nm_client_get_active_connections(nmClient);
        if (activeConnections) {
            for (guint i = 0; i < activeConnections->len; i++) {
                NMActiveConnection* conn = static_cast<NMActiveConnection*>(g_ptr_array_index(activeConnections, i));
                if (conn) {
                    const char* connId = nm_active_connection_get_id(conn);
                    if (connId && network.ssid == std::string(connId)) {
                        activeConn = conn;
                        break;
                    }
                }
            }
        }
        
        // Store settings data with the button
        NetworkSettingsData* settingsData = new NetworkSettingsData();
        settingsData->networkManager = this;
        settingsData->ssid = network.ssid;
        settingsData->activeConnection = activeConn; // Don't ref here to avoid double-free
        
        g_object_set_data_full(G_OBJECT(settingsBtn), "settings-data", settingsData,
                              [](gpointer data) {
                                  NetworkSettingsData* d = static_cast<NetworkSettingsData*>(data);
                                  delete d;
                              });
        
        g_signal_connect(settingsBtn, "clicked", G_CALLBACK(onNetworkSettingsClicked), settingsData);
        gtk_box_append(GTK_BOX(rowBox), settingsBtn);
    }
    
    // Connect button
    GtkWidget* connectBtn = gtk_button_new();
    GtkWidget* connectIcon = gtk_image_new_from_icon_name("go-next-symbolic");
    gtk_image_set_pixel_size(GTK_IMAGE(connectIcon), 16);
    gtk_button_set_child(GTK_BUTTON(connectBtn), connectIcon);
    gtk_widget_add_css_class(connectBtn, "connect-button");
    gtk_widget_add_css_class(connectBtn, "flat");
    gtk_widget_add_css_class(connectBtn, "circular");
    
    // Store network data with the button
    ConnectButtonData* btnData = new ConnectButtonData();
    btnData->networkManager = this;
    btnData->ssid = network.ssid;
    btnData->ap = nullptr; // Find AP dynamically to avoid reference issues
    btnData->isSecured = network.isSecured;
    
    g_object_set_data_full(G_OBJECT(connectBtn), "connect-data", btnData,
                          [](gpointer data) {
                              ConnectButtonData* d = static_cast<ConnectButtonData*>(data);
                              delete d;
                          });
    
    g_signal_connect(connectBtn, "clicked", G_CALLBACK(onConnectButtonClicked), btnData);
    gtk_box_append(GTK_BOX(rowBox), connectBtn);
    
    gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), rowBox);
    gtk_list_box_append(GTK_LIST_BOX(wifiListBox), row);
    networkWidgets.push_back(row);
}

std::vector<WifiNetwork> NetworkManager::getAvailableNetworks() {
    std::vector<WifiNetwork> networks;
    
    if (!nmClient) return networks;
    
    NMDeviceWifi* wifi = getPrimaryWifiDevice();
    if (!wifi) return networks;
    
    const GPtrArray* aps = nm_device_wifi_get_access_points(wifi);
    if (!aps || aps->len == 0) return networks;
    
    NMAccessPoint* activeAp = nm_device_wifi_get_active_access_point(wifi);
    
    for (guint i = 0; i < aps->len; ++i) {
        NMAccessPoint* ap = static_cast<NMAccessPoint*>(g_ptr_array_index(aps, i));
        if (!ap) continue;
        
        std::string ssid = ssidFromBytes(nm_access_point_get_ssid(ap));
        if (ssid.empty()) ssid = "<hidden>";
        
        WifiNetwork network;
        network.ssid = ssid;
        network.strength = nm_access_point_get_strength(ap);
        network.isSecured = isAccessPointSecured(ap);
        network.ap = nullptr; // Don't store raw pointers to avoid double-free
        
        // Check if this is the active connection
        network.isActive = false;
        if (activeAp) {
            const char* activePath = nm_object_get_path(NM_OBJECT(activeAp));
            const char* apPath = nm_object_get_path(NM_OBJECT(ap));
            if (activePath && apPath && g_strcmp0(activePath, apPath) == 0) {
                network.isActive = true;
            }
        }
        
        // Check if this network is saved
        NMRemoteConnection* saved = findSavedConnection(ssid);
        network.isSaved = (saved != nullptr);
        if (saved) g_object_unref(saved);
        
        // Set status
        if (network.isActive) {
            network.status = "Connected";
        } else if (network.isSaved) {
            network.status = "Saved";
        } else if (network.isSecured) {
            network.status = "Secured";
        } else {
            network.status = "Open";
        }
        
        networks.push_back(network);
    }
    
    return networks;
}

void NetworkManager::updateConnectionStatus() {
    if (!connectionStatusLabel || !nmClient) return;
    
    // Remove existing CSS classes
    gtk_widget_remove_css_class(connectionStatusLabel, "disconnected");
    gtk_widget_remove_css_class(connectionStatusLabel, "checking");
    
    std::string statusText = "";
    std::string cssClass = "";
    
    // Check ethernet connection first
    if (hasEthernetConnection) {
        // Find the active ethernet connection name
        const GPtrArray* devices = nm_client_get_devices(nmClient);
        if (devices) {
            for (guint i = 0; i < devices->len; ++i) {
                NMDevice* dev = static_cast<NMDevice*>(g_ptr_array_index(devices, i));
                if (NM_IS_DEVICE_ETHERNET(dev)) {
                    NMDeviceState state = nm_device_get_state(dev);
                    if (state == NM_DEVICE_STATE_ACTIVATED) {
                        NMActiveConnection* activeConn = nm_device_get_active_connection(dev);
                        if (activeConn) {
                            const char* connName = nm_active_connection_get_id(activeConn);
                            statusText = TR(TranslationKeys::CONNECTED_TO_PREFIX) + std::string(connName ? connName : TR(TranslationKeys::ETHERNET));
                        } else {
                            statusText = std::string(TR(TranslationKeys::CONNECTED_TO_PREFIX)) + std::string(TR(TranslationKeys::ETHERNET));
                        }
                        break;
                    }
                }
            }
        }
        
        if (statusText.empty()) {
            statusText = std::string(TR(TranslationKeys::CONNECTED_TO_PREFIX)) + std::string(TR(TranslationKeys::ETHERNET));
        }
    } else {
        // Check Wi-Fi connection
        NMDeviceWifi* wifi = getPrimaryWifiDevice();
        if (wifi) {
            NMDeviceState state = nm_device_get_state(NM_DEVICE(wifi));
            if (state == NM_DEVICE_STATE_ACTIVATED) {
                NMAccessPoint* activeAp = nm_device_wifi_get_active_access_point(wifi);
                if (activeAp) {
                    std::string ssid = ssidFromBytes(nm_access_point_get_ssid(activeAp));
                    if (!ssid.empty()) {
                        statusText = "Connected to: " + ssid;
                    } else {
                        statusText = "Connected to: Wi-Fi Network";
                    }
                } else {
                    statusText = "Connected to: Wi-Fi Network";
                }
            } else if (state == NM_DEVICE_STATE_PREPARE || 
                       state == NM_DEVICE_STATE_CONFIG || 
                       state == NM_DEVICE_STATE_NEED_AUTH ||
                       state == NM_DEVICE_STATE_IP_CONFIG ||
                       state == NM_DEVICE_STATE_IP_CHECK) {
                statusText = TR(TranslationKeys::CONNECTING_STATUS);
                cssClass = "checking";
            } else {
                // Check if networking is enabled
                if (!networkingEnabled) {
                    statusText = TR(TranslationKeys::NETWORKING_DISABLED_STATUS);
                    cssClass = "disconnected";
                } else if (!nm_client_wireless_get_enabled(nmClient)) {
                    statusText = TR(TranslationKeys::WIFI_DISABLED_STATUS);
                    cssClass = "disconnected";
                } else {
                    statusText = TR(TranslationKeys::NOT_CONNECTED_STATUS);
                    cssClass = "disconnected";
                }
            }
        } else {
            statusText = "No network device found";
            cssClass = "disconnected";
        }
    }
    
    // Update the label
    gtk_label_set_text(GTK_LABEL(connectionStatusLabel), statusText.c_str());
    
    // Apply CSS class if needed
    if (!cssClass.empty()) {
        gtk_widget_add_css_class(connectionStatusLabel, cssClass.c_str());
    }
}


// Network utility functions implementation
NMDeviceWifi* NetworkManager::getPrimaryWifiDevice() {
    if (!nmClient) return nullptr;
    
    const GPtrArray* devices = nm_client_get_devices(nmClient);
    if (!devices) return nullptr;
    
    for (guint i = 0; i < devices->len; ++i) {
        NMDevice* dev = static_cast<NMDevice*>(g_ptr_array_index(devices, i));
        if (NM_IS_DEVICE_WIFI(dev)) {
            return NM_DEVICE_WIFI(dev);
        }
    }
    return nullptr;
}

std::string NetworkManager::ssidFromBytes(GBytes* ssidBytes) {
    if (!ssidBytes) return "";
    
    gsize len = 0;
    const guint8* data = static_cast<const guint8*>(g_bytes_get_data(ssidBytes, &len));
    if (!data || len == 0) return "";
    
    return std::string(reinterpret_cast<const char*>(data), len);
}

bool NetworkManager::isAccessPointSecured(NMAccessPoint* ap) {
    if (!ap) return false;
    
    return (nm_access_point_get_flags(ap) != NM_802_11_AP_FLAGS_NONE) ||
           (nm_access_point_get_wpa_flags(ap) != NM_802_11_AP_SEC_NONE) ||
           (nm_access_point_get_rsn_flags(ap) != NM_802_11_AP_SEC_NONE);
}

NMRemoteConnection* NetworkManager::findSavedConnection(const std::string& ssid) {
    if (!nmClient || ssid.empty()) return nullptr;
    
    const GPtrArray* conns = nm_client_get_connections(nmClient);
    if (!conns) return nullptr;
    
    for (guint i = 0; i < conns->len; ++i) {
        NMRemoteConnection* rc = static_cast<NMRemoteConnection*>(g_ptr_array_index(conns, i));
        if (!rc) continue;
        
        NMConnection* c = NM_CONNECTION(rc);
        NMSettingWireless* sWifi = nm_connection_get_setting_wireless(c);
        if (!sWifi) continue;
        
        GBytes* bytes = nm_setting_wireless_get_ssid(sWifi);
        std::string connSsid = ssidFromBytes(bytes);
        
        if (connSsid == ssid) {
            return NM_REMOTE_CONNECTION(g_object_ref(rc));
        }
    }
    return nullptr;
}

bool NetworkManager::checkNetworkingEnabled() {
    if (!nmClient) return false;
    return nm_client_networking_get_enabled(nmClient);
}

bool NetworkManager::checkEthernetConnection() {
    if (!nmClient) return false;
    
    const GPtrArray* devices = nm_client_get_devices(nmClient);
    if (!devices) return false;
    
    for (guint i = 0; i < devices->len; ++i) {
        NMDevice* dev = static_cast<NMDevice*>(g_ptr_array_index(devices, i));
        if (NM_IS_DEVICE_ETHERNET(dev)) {
            NMDeviceState state = nm_device_get_state(dev);
            if (state == NM_DEVICE_STATE_ACTIVATED) {
                return true;
            }
        }
    }
    return false;
}

void NetworkManager::updateNetworkState() {
    if (!nmClient) return;
    
    bool newNetworkingEnabled = checkNetworkingEnabled();
    bool newEthernetConnection = checkEthernetConnection();
    
    bool stateChanged = (networkingEnabled != newNetworkingEnabled) ||
                       (hasEthernetConnection != newEthernetConnection);
    
    networkingEnabled = newNetworkingEnabled;
    hasEthernetConnection = newEthernetConnection;
    
    if (stateChanged) {
        std::cout << "Network state changed: networking=" << (newNetworkingEnabled ? "enabled" : "disabled")
                  << ", ethernet=" << (newEthernetConnection ? "connected" : "disconnected") << std::endl;
        
        // Update UI with debounced timeout
        if (updateTimeoutId > 0) {
            g_source_remove(updateTimeoutId);
        }
        updateTimeoutId = g_timeout_add(300, updateNetworkStateTimeout, this);
    }
}

void NetworkManager::enableNetworking() {
    if (!nmClient) return;
    
    std::cout << "Enabling networking..." << std::endl;
    GError* error = nullptr;
    
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    if (!nm_client_networking_set_enabled(nmClient, TRUE, &error)) {
        std::cerr << "Failed to enable networking: " << (error ? error->message : "Unknown error") << std::endl;
        if (error) g_error_free(error);
    }
    #pragma GCC diagnostic pop
}

void NetworkManager::reflectWifiSwitchState() {
    if (!nmClient || !wifiSwitch || updatingWifiSwitch) return;
    
    updatingWifiSwitch = true;
    
    gboolean hwEnabled = nm_client_wireless_hardware_get_enabled(nmClient);
    gboolean swEnabled = nm_client_wireless_get_enabled(nmClient);
    
    std::cout << "Wi-Fi Hardware enabled: " << (hwEnabled ? "YES" : "NO")
              << ", Software enabled: " << (swEnabled ? "YES" : "NO")
              << ", Networking enabled: " << (networkingEnabled ? "YES" : "NO") << std::endl;
    
    // Set switch state to reflect software Wi-Fi state only if hardware is enabled and networking is enabled
    if (hwEnabled && networkingEnabled) {
        gtk_switch_set_active(GTK_SWITCH(wifiSwitch), swEnabled);
        gtk_widget_set_sensitive(wifiSwitch, TRUE);
    } else {
        // Hardware disabled or networking disabled - switch should be off and disabled
        gtk_switch_set_active(GTK_SWITCH(wifiSwitch), FALSE);
        gtk_widget_set_sensitive(wifiSwitch, FALSE);
    }
    
    updatingWifiSwitch = false;
    
    // Update the refresh button sensitivity based on Wi-Fi state
    if (refreshButton) {
        gtk_widget_set_sensitive(refreshButton, hwEnabled && swEnabled && networkingEnabled);
    }
}

void NetworkManager::scanWifiNetworks() {
    if (!nmClient) return;
    
    NMDeviceWifi* wifi = getPrimaryWifiDevice();
    if (!wifi) return;
    
    nm_device_wifi_request_scan_async(wifi, nullptr, nullptr, nullptr);
    g_timeout_add(2000, populateWifiListTimeout, this);
}

void NetworkManager::connectToNetwork(const std::string& ssid, NMAccessPoint* ap, bool isSecured) {
    if (!nmClient || ssid.empty() || !ap) return;
    
    NMDeviceWifi* wifi = getPrimaryWifiDevice();
    if (!wifi) return;
    
    // Check if saved connection exists
    NMRemoteConnection* saved = findSavedConnection(ssid);
    if (saved) {
        // Activate saved connection
        nm_client_activate_connection_async(nmClient, NM_CONNECTION(saved), 
                                          NM_DEVICE(wifi), nullptr, nullptr, nullptr, nullptr);
        g_object_unref(saved);
        return;
    }
    
    if (isSecured) {
        // Show password dialog
        showPasswordDialog(ssid, ap);
    } else {
        // Connect to open network
        NMConnection* c = nm_simple_connection_new();
        
        // Connection setting
        NMSettingConnection* sCon = NM_SETTING_CONNECTION(nm_setting_connection_new());
        g_object_set(G_OBJECT(sCon),
                     NM_SETTING_CONNECTION_ID, ssid.c_str(),
                     NM_SETTING_CONNECTION_TYPE, NM_SETTING_WIRELESS_SETTING_NAME,
                     NM_SETTING_CONNECTION_AUTOCONNECT, TRUE,
                     nullptr);
        nm_connection_add_setting(c, NM_SETTING(sCon));
        
        // Wireless setting
        NMSettingWireless* sWifi = NM_SETTING_WIRELESS(nm_setting_wireless_new());
        GBytes* ssidBytes = g_bytes_new(ssid.c_str(), ssid.length());
        g_object_set(G_OBJECT(sWifi),
                     NM_SETTING_WIRELESS_SSID, ssidBytes,
                     NM_SETTING_WIRELESS_MODE, "infrastructure",
                     nullptr);
        g_bytes_unref(ssidBytes);
        nm_connection_add_setting(c, NM_SETTING(sWifi));
        
        const char* apPath = nm_object_get_path(NM_OBJECT(ap));
        nm_client_add_and_activate_connection_async(nmClient, c, NM_DEVICE(wifi), 
                                                   apPath, nullptr, nullptr, nullptr);
    }
}

void NetworkManager::showPasswordDialog(const std::string& ssid, NMAccessPoint* ap) {
    GtkWidget* dialog = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(dialog), TR(TranslationKeys::ENTER_NETWORK_PASSWORD));
    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(parentWindow));
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_default_size(GTK_WINDOW(dialog), 400, 200);
    
    GtkWidget* mainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_margin_top(mainBox, 20);
    gtk_widget_set_margin_bottom(mainBox, 20);
    gtk_widget_set_margin_start(mainBox, 20);
    gtk_widget_set_margin_end(mainBox, 20);
    
    std::string prompt = "Enter password for \"" + ssid + "\":";
    GtkWidget* label = gtk_label_new(prompt.c_str());
    gtk_box_append(GTK_BOX(mainBox), label);
    
    GtkWidget* entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(entry), FALSE);
    gtk_entry_set_input_purpose(GTK_ENTRY(entry), GTK_INPUT_PURPOSE_PASSWORD);
    gtk_box_append(GTK_BOX(mainBox), entry);
    
    GtkWidget* buttonBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign(buttonBox, GTK_ALIGN_END);
    
    GtkWidget* cancelBtn = gtk_button_new_with_label(TR(TranslationKeys::CANCEL));
    GtkWidget* connectBtn = gtk_button_new_with_label(TR(TranslationKeys::CONNECT));
    gtk_widget_add_css_class(connectBtn, "suggested-action");
    
    gtk_box_append(GTK_BOX(buttonBox), cancelBtn);
    gtk_box_append(GTK_BOX(buttonBox), connectBtn);
    gtk_box_append(GTK_BOX(mainBox), buttonBox);
    
    gtk_window_set_child(GTK_WINDOW(dialog), mainBox);
    
    // Package data for dialog response handler
    PasswordDialogData* d = new PasswordDialogData();
    d->networkManager = this;
    d->wifiDev = getPrimaryWifiDevice();
    if (d->wifiDev) g_object_ref(d->wifiDev);
    d->ap = NM_ACCESS_POINT(g_object_ref(ap));
    d->ssid = ssid;
    d->entry = entry;
    d->dialog = dialog;
    
    // Connect button handlers
    g_signal_connect_swapped(cancelBtn, "clicked", G_CALLBACK(gtk_window_destroy), dialog);
    g_signal_connect(connectBtn, "clicked", G_CALLBACK(onPasswordConnectClicked), d);
    g_signal_connect(dialog, "destroy", G_CALLBACK(onPasswordDialogDestroy), d);
    
    gtk_window_present(GTK_WINDOW(dialog));
}

// Password dialog callback functions
void NetworkManager::onPasswordConnectClicked(GtkButton* button, gpointer user_data) {
    (void)button;
    
    PasswordDialogData* data = static_cast<PasswordDialogData*>(user_data);
    if (data && data->ssid.length() > 0 && data->entry && data->networkManager && data->networkManager->nmClient) {
        const gchar* psk = gtk_editable_get_text(GTK_EDITABLE(data->entry));
        if (psk && *psk) {
            // Add and activate PSK connection
            NMConnection* c = nm_simple_connection_new();
            
            // Connection setting
            NMSettingConnection* sCon = NM_SETTING_CONNECTION(nm_setting_connection_new());
            g_object_set(G_OBJECT(sCon),
                         NM_SETTING_CONNECTION_ID, data->ssid.c_str(),
                         NM_SETTING_CONNECTION_TYPE, NM_SETTING_WIRELESS_SETTING_NAME,
                         NM_SETTING_CONNECTION_AUTOCONNECT, TRUE,
                         nullptr);
            nm_connection_add_setting(c, NM_SETTING(sCon));
            
            // Wireless setting
            NMSettingWireless* sWifi = NM_SETTING_WIRELESS(nm_setting_wireless_new());
            GBytes* ssidBytes = g_bytes_new(data->ssid.c_str(), data->ssid.length());
            g_object_set(G_OBJECT(sWifi),
                         NM_SETTING_WIRELESS_SSID, ssidBytes,
                         NM_SETTING_WIRELESS_MODE, "infrastructure",
                         nullptr);
            g_bytes_unref(ssidBytes);
            nm_connection_add_setting(c, NM_SETTING(sWifi));
            
            // Security (WPA-PSK)
            NMSettingWirelessSecurity* sWsec = NM_SETTING_WIRELESS_SECURITY(nm_setting_wireless_security_new());
            g_object_set(G_OBJECT(sWsec),
                         NM_SETTING_WIRELESS_SECURITY_KEY_MGMT, "wpa-psk",
                         NM_SETTING_WIRELESS_SECURITY_PSK, psk,
                         nullptr);
            nm_connection_add_setting(c, NM_SETTING(sWsec));
            
            // AP object path as specific_object
            const char* apPath = nm_object_get_path(NM_OBJECT(data->ap));
            
            nm_client_add_and_activate_connection_async(
                data->networkManager->nmClient,
                c,
                NM_DEVICE(data->wifiDev),
                apPath,
                nullptr, nullptr, nullptr);
        }
    }
    
    if (data && data->dialog) {
        gtk_window_destroy(GTK_WINDOW(data->dialog));
    }
}

void NetworkManager::onPasswordDialogDestroy(GtkWidget* dialog, gpointer user_data) {
    (void)dialog;
    
    PasswordDialogData* data = static_cast<PasswordDialogData*>(user_data);
    if (data) {
        if (data->ap) g_object_unref(data->ap);
        if (data->wifiDev) g_object_unref(data->wifiDev);
        delete data;
    }
}

// Event handlers implementation
void NetworkManager::onBackButtonClicked(GtkGestureClick* gesture, int n_press, double x, double y, gpointer user_data) {
    (void)gesture; (void)n_press; (void)x; (void)y;
    
    NetworkBackButtonCallbackData* data = static_cast<NetworkBackButtonCallbackData*>(user_data);
    if (data && data->networkManager) {
        // Hide network page
        data->networkManager->hide();
        
        // Show main menu
        if (data->networkManager->mainWindow) {
            data->networkManager->mainWindow->showMainMenu();
        }
    }
}

gboolean NetworkManager::onWifiSwitchStateSet(GtkSwitch* sw, gboolean state, gpointer user_data) {
    (void)sw;
    
    NetworkManager* netMgr = static_cast<NetworkManager*>(user_data);
    if (!netMgr || !netMgr->nmClient || netMgr->updatingWifiSwitch) return TRUE;
    
    gboolean hwEnabled = nm_client_wireless_hardware_get_enabled(netMgr->nmClient);
    gboolean currentSwEnabled = nm_client_wireless_get_enabled(netMgr->nmClient);
    
    std::cout << "Switch toggle requested: " << (state ? "ON" : "OFF")
              << " (HW: " << (hwEnabled ? "enabled" : "disabled")
              << ", Current SW: " << (currentSwEnabled ? "enabled" : "disabled")
              << ", Networking: " << (netMgr->networkingEnabled ? "enabled" : "disabled") << ")" << std::endl;
    
    if (!hwEnabled || !netMgr->networkingEnabled) {
        std::cout << "Cannot enable Wi-Fi: Hardware or networking is disabled" << std::endl;
        netMgr->reflectWifiSwitchState();
        return TRUE;
    }
    
    // Only proceed if the requested state is different from current state
    if (state != currentSwEnabled) {
        std::cout << "Setting Wi-Fi software state to: " << (state ? "enabled" : "disabled") << std::endl;
        
        // Set software Wi-Fi state
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
        nm_client_wireless_set_enabled(netMgr->nmClient, state);
        #pragma GCC diagnostic pop
        
        // Give some time for the change to take effect before updating UI
        if (state) {
            // Enabling Wi-Fi - scan after a delay
            g_timeout_add(1000, scanWifiNetworksTimeout, netMgr);
        } else {
            // Disabling Wi-Fi - update list immediately
            g_timeout_add(500, populateWifiListTimeout, netMgr);
        }
        
        // Reflect the state change after a short delay
        g_timeout_add(300, reflectWifiSwitchStateTimeout, netMgr);
    }
    
    return TRUE; // Prevent default toggle handling
}

void NetworkManager::onRefreshButtonClicked(GtkButton* button, gpointer user_data) {
    (void)button;
    
    NetworkManager* netMgr = static_cast<NetworkManager*>(user_data);
    if (netMgr) {
        netMgr->scanWifiNetworks();
    }
}

void NetworkManager::onConnectButtonClicked(GtkButton* button, gpointer user_data) {
    (void)button;
    
    ConnectButtonData* data = static_cast<ConnectButtonData*>(user_data);
    if (data && data->networkManager && data->networkManager->nmClient) {
        // Find the AP dynamically by SSID to avoid stale pointer issues
        NMAccessPoint* ap = nullptr;
        NMDeviceWifi* wifi = data->networkManager->getPrimaryWifiDevice();
        if (wifi) {
            const GPtrArray* aps = nm_device_wifi_get_access_points(wifi);
            if (aps) {
                for (guint i = 0; i < aps->len; ++i) {
                    NMAccessPoint* currentAp = static_cast<NMAccessPoint*>(g_ptr_array_index(aps, i));
                    if (currentAp) {
                        std::string ssid = data->networkManager->ssidFromBytes(nm_access_point_get_ssid(currentAp));
                        if (ssid == data->ssid) {
                            ap = currentAp;
                            break;
                        }
                    }
                }
            }
        }
        data->networkManager->connectToNetwork(data->ssid, ap, data->isSecured);
    }
}

void NetworkManager::onEnableNetworkingClicked(GtkButton* button, gpointer user_data) {
    (void)button;
    
    NetworkManager* netMgr = static_cast<NetworkManager*>(user_data);
    if (netMgr) {
        netMgr->enableNetworking();
    }
}

void NetworkManager::onNetworkManagerChanged(NMClient* client, gpointer user_data) {
    (void)client;
    
    NetworkManager* netMgr = static_cast<NetworkManager*>(user_data);
    if (netMgr) {
        netMgr->updateNetworkState();
    }
}

void NetworkManager::onWifiDeviceChanged(GObject* gobj, GParamSpec* pspec, gpointer user_data) {
    (void)gobj; (void)pspec;
    
    NetworkManager* netMgr = static_cast<NetworkManager*>(user_data);
    if (netMgr) {
        netMgr->updateNetworkState();
    }
}

gboolean NetworkManager::updateNetworkStateTimeout(gpointer user_data) {
    NetworkManager* netMgr = static_cast<NetworkManager*>(user_data);
    if (netMgr && netMgr->nmClient) {
        netMgr->updateTimeoutId = 0;
        
        if (!netMgr->updatingWifiSwitch) {
            netMgr->reflectWifiSwitchState();
            netMgr->populateWifiList();
            netMgr->updateConnectionStatus();
        }
    }
    
    return G_SOURCE_REMOVE;
}

gboolean NetworkManager::populateWifiListTimeout(gpointer user_data) {
    NetworkManager* netMgr = static_cast<NetworkManager*>(user_data);
    if (netMgr && netMgr->nmClient) {
        netMgr->populateWifiList();
    }
    return G_SOURCE_REMOVE;
}

gboolean NetworkManager::scanWifiNetworksTimeout(gpointer user_data) {
    NetworkManager* netMgr = static_cast<NetworkManager*>(user_data);
    if (netMgr && netMgr->nmClient) {
        netMgr->scanWifiNetworks();
    }
    return G_SOURCE_REMOVE;
}

gboolean NetworkManager::reflectWifiSwitchStateTimeout(gpointer user_data) {
    NetworkManager* netMgr = static_cast<NetworkManager*>(user_data);
    if (netMgr && netMgr->nmClient) {
        netMgr->reflectWifiSwitchState();
    }
    return G_SOURCE_REMOVE;
}

void NetworkManager::onNetworkSettingsClicked(GtkButton* button, gpointer user_data) {
    (void)button;
    NetworkSettingsData* data = static_cast<NetworkSettingsData*>(user_data);
    if (data && data->networkManager && data->networkManager->nmClient) {
        data->networkManager->showNetworkSettings(data->ssid, data->activeConnection);
    }
}

void NetworkManager::showNetworkSettings(const std::string& ssid, NMActiveConnection* activeConnection) {
    if (!activeConnection) return;
    
    // Get the connection settings
    NMRemoteConnection* connection = nm_active_connection_get_connection(activeConnection);
    if (!connection) return;
    
    NMConnection* nmConnection = NM_CONNECTION(connection);
    NMSettingIPConfig* s_ip4 = NM_SETTING_IP_CONFIG(nm_connection_get_setting_ip4_config(nmConnection));
    NMSettingIPConfig* s_ip6 = NM_SETTING_IP_CONFIG(nm_connection_get_setting_ip6_config(nmConnection));
    
    // Create the settings dialog
    GtkWidget* dialog = gtk_dialog_new_with_buttons(
        ("Network Settings - " + ssid).c_str(),
        GTK_WINDOW(parentWindow),
        GTK_DIALOG_MODAL,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Apply", GTK_RESPONSE_ACCEPT,
        nullptr
    );
    
    gtk_window_set_default_size(GTK_WINDOW(dialog), 500, 600);
    
    // Add CSS styling for the dialog
    GtkCssProvider* dialogCssProvider = gtk_css_provider_new();
    if (dialogCssProvider) {
        const char* dialogCss = 
            "window.dialog { "
            "  background: rgba(255, 255, 255, 0.4); "
            "  color: black; "
            "} "
            "window.dialog label { "
            "  color: black; "
            "} "
            "window.dialog entry { "
            "  background: white; "
            "  color: black; "
            "  border: 1px solid #ccc; "
            "} "
            "window.dialog combobox { "
            "  background: white; "
            "  color: black; "
            "} "
            "window.dialog combobox button { "
            "  background: white; "
            "  color: black; "
            "} "
            "window.dialog checkbutton { "
            "  color: black; "
            "} "
            "window.dialog checkbutton label { "
            "  color: black; "
            "} "
            "window.dialog notebook { "
            "  background: rgba(255, 255, 255, 0.6); "
            "} "
            "window.dialog notebook tab { "
            "  background: rgba(255, 255, 255, 0.8); "
            "  color: black; "
            "} "
            "window.dialog notebook tab:checked { "
            "  background: white; "
            "  color: black; "
            "} ";
        
        gtk_css_provider_load_from_string(dialogCssProvider, dialogCss);
        gtk_style_context_add_provider_for_display(
            gdk_display_get_default(),
            GTK_STYLE_PROVIDER(dialogCssProvider),
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION + 1);
        
        g_object_unref(dialogCssProvider);
    }
    
    GtkWidget* content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_widget_set_margin_top(content_area, 20);
    gtk_widget_set_margin_bottom(content_area, 20);
    gtk_widget_set_margin_start(content_area, 20);
    gtk_widget_set_margin_end(content_area, 20);
    
    // Create notebook for tabs
   // GtkWidget* notebook = gtk_notebook_new();
   // gtk_box_append(GTK_BOX(content_area), notebook);
    
    // IPv4 Tab
    GtkWidget* ipv4_page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_margin_top(ipv4_page, 20);
    gtk_widget_set_margin_bottom(ipv4_page, 20);
    gtk_widget_set_margin_start(ipv4_page, 20);
    gtk_widget_set_margin_end(ipv4_page, 20);
    
    // IPv4 Method
    GtkWidget* ipv4_method_label = gtk_label_new("IPv4 Method:");
    gtk_widget_set_halign(ipv4_method_label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(ipv4_page), ipv4_method_label);
    
    GtkWidget* ipv4_method_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(ipv4_method_combo), "Automatic (DHCP)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(ipv4_method_combo), "Manual");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(ipv4_method_combo), "Link-Local Only");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(ipv4_method_combo), "Disabled");
    
    // Set current method
    const char* method = s_ip4 ? nm_setting_ip_config_get_method(s_ip4) : NM_SETTING_IP4_CONFIG_METHOD_AUTO;
    if (g_strcmp0(method, NM_SETTING_IP4_CONFIG_METHOD_AUTO) == 0) {
        gtk_combo_box_set_active(GTK_COMBO_BOX(ipv4_method_combo), 0);
    } else if (g_strcmp0(method, NM_SETTING_IP4_CONFIG_METHOD_MANUAL) == 0) {
        gtk_combo_box_set_active(GTK_COMBO_BOX(ipv4_method_combo), 1);
    } else if (g_strcmp0(method, NM_SETTING_IP4_CONFIG_METHOD_LINK_LOCAL) == 0) {
        gtk_combo_box_set_active(GTK_COMBO_BOX(ipv4_method_combo), 2);
    } else {
        gtk_combo_box_set_active(GTK_COMBO_BOX(ipv4_method_combo), 3);
    }
    
    gtk_box_append(GTK_BOX(ipv4_page), ipv4_method_combo);
    
    // Manual IP settings (initially hidden)
    GtkWidget* manual_ip_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    
    GtkWidget* ip_label = gtk_label_new("IP Address:");
    gtk_widget_set_halign(ip_label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(manual_ip_box), ip_label);
    
    GtkWidget* ip_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(ip_entry), "192.168.1.100");
    gtk_box_append(GTK_BOX(manual_ip_box), ip_entry);
    
    GtkWidget* netmask_label = gtk_label_new("Netmask:");
    gtk_widget_set_halign(netmask_label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(manual_ip_box), netmask_label);
    
    GtkWidget* netmask_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(netmask_entry), "255.255.255.0");
    gtk_box_append(GTK_BOX(manual_ip_box), netmask_entry);
    
    GtkWidget* gateway_label = gtk_label_new("Gateway:");
    gtk_widget_set_halign(gateway_label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(manual_ip_box), gateway_label);
    
    GtkWidget* gateway_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(gateway_entry), "192.168.1.1");
    gtk_box_append(GTK_BOX(manual_ip_box), gateway_entry);
    
    // Fill current manual settings if available
    if (s_ip4 && nm_setting_ip_config_get_num_addresses(s_ip4) > 0) {
        NMIPAddress* addr = nm_setting_ip_config_get_address(s_ip4, 0);
        if (addr) {
            gtk_editable_set_text(GTK_EDITABLE(ip_entry), nm_ip_address_get_address(addr));
            
            // Convert prefix to netmask
            guint prefix = nm_ip_address_get_prefix(addr);
            guint32 netmask = htonl(~((1 << (32 - prefix)) - 1));
            char netmask_str[16];
            inet_ntop(AF_INET, &netmask, netmask_str, sizeof(netmask_str));
            gtk_editable_set_text(GTK_EDITABLE(netmask_entry), netmask_str);
        }
        
        if (nm_setting_ip_config_get_gateway(s_ip4)) {
            gtk_editable_set_text(GTK_EDITABLE(gateway_entry), nm_setting_ip_config_get_gateway(s_ip4));
        }
    }
    
    gtk_box_append(GTK_BOX(ipv4_page), manual_ip_box);
    
    // DNS Settings
    GtkWidget* dns_separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_append(GTK_BOX(ipv4_page), dns_separator);
    
    GtkWidget* dns_label = gtk_label_new("DNS Servers:");
    gtk_widget_set_halign(dns_label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(ipv4_page), dns_label);
    
    GtkWidget* dns_auto_check = gtk_check_button_new_with_label("Obtain DNS server addresses automatically");
    gtk_box_append(GTK_BOX(ipv4_page), dns_auto_check);
    
    GtkWidget* dns_manual_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    
    GtkWidget* dns1_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(dns1_entry), "Primary DNS (e.g., 8.8.8.8)");
    gtk_box_append(GTK_BOX(dns_manual_box), dns1_entry);
    
    GtkWidget* dns2_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(dns2_entry), "Secondary DNS (e.g., 8.8.4.4)");
    gtk_box_append(GTK_BOX(dns_manual_box), dns2_entry);
    
    // Fill current DNS settings
    bool auto_dns = true;
    if (s_ip4) {
        guint num_dns = nm_setting_ip_config_get_num_dns(s_ip4);
        if (num_dns > 0) {
            auto_dns = false;
            gtk_editable_set_text(GTK_EDITABLE(dns1_entry), nm_setting_ip_config_get_dns(s_ip4, 0));
            if (num_dns > 1) {
                gtk_editable_set_text(GTK_EDITABLE(dns2_entry), nm_setting_ip_config_get_dns(s_ip4, 1));
            }
        }
    }
    
    gtk_check_button_set_active(GTK_CHECK_BUTTON(dns_auto_check), auto_dns);
    gtk_widget_set_sensitive(dns_manual_box, !auto_dns);
    
    gtk_box_append(GTK_BOX(ipv4_page), dns_manual_box);
    
    // Connect signals for dynamic UI updates
    g_signal_connect(ipv4_method_combo, "changed", G_CALLBACK(onIPv4MethodChanged), manual_ip_box);
    g_signal_connect(dns_auto_check, "toggled", G_CALLBACK(onDNSAutoToggled), dns_manual_box);
    
    // Set initial sensitivity
    gtk_widget_set_sensitive(manual_ip_box, gtk_combo_box_get_active(GTK_COMBO_BOX(ipv4_method_combo)) == 1);
    
    // IPv6 Tab (simplified for now)
    GtkWidget* ipv6_page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_margin_top(ipv6_page, 20);
    gtk_widget_set_margin_bottom(ipv6_page, 20);
    gtk_widget_set_margin_start(ipv6_page, 20);
    gtk_widget_set_margin_end(ipv6_page, 20);
    
    GtkWidget* ipv6_method_label = gtk_label_new("IPv6 Method:");
    gtk_widget_set_halign(ipv6_method_label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(ipv6_page), ipv6_method_label);
    
    GtkWidget* ipv6_method_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(ipv6_method_combo), "Automatic");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(ipv6_method_combo), "Manual");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(ipv6_method_combo), "Link-Local Only");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(ipv6_method_combo), "Disabled");
    
    // Set current IPv6 method
    const char* ipv6_method = s_ip6 ? nm_setting_ip_config_get_method(s_ip6) : NM_SETTING_IP6_CONFIG_METHOD_AUTO;
    if (g_strcmp0(ipv6_method, NM_SETTING_IP6_CONFIG_METHOD_AUTO) == 0) {
        gtk_combo_box_set_active(GTK_COMBO_BOX(ipv6_method_combo), 0);
    } else if (g_strcmp0(ipv6_method, NM_SETTING_IP6_CONFIG_METHOD_MANUAL) == 0) {
        gtk_combo_box_set_active(GTK_COMBO_BOX(ipv6_method_combo), 1);
    } else if (g_strcmp0(ipv6_method, NM_SETTING_IP6_CONFIG_METHOD_LINK_LOCAL) == 0) {
        gtk_combo_box_set_active(GTK_COMBO_BOX(ipv6_method_combo), 2);
    } else {
        gtk_combo_box_set_active(GTK_COMBO_BOX(ipv6_method_combo), 3);
    }
    
    gtk_box_append(GTK_BOX(ipv6_page), ipv6_method_combo);
    
    // IPv6 Manual Settings
    GtkWidget* ipv6_separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_append(GTK_BOX(ipv6_page), ipv6_separator);
    
    GtkWidget* manual_ipv6_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    
    GtkWidget* ipv6_address_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(ipv6_address_entry), "IPv6 Address (e.g., 2001:db8::1)");
    gtk_box_append(GTK_BOX(manual_ipv6_box), ipv6_address_entry);
    
    GtkWidget* ipv6_prefix_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(ipv6_prefix_entry), "Prefix Length (e.g., 64)");
    gtk_box_append(GTK_BOX(manual_ipv6_box), ipv6_prefix_entry);
    
    GtkWidget* ipv6_gateway_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(ipv6_gateway_entry), "Gateway (e.g., 2001:db8::1)");
    gtk_box_append(GTK_BOX(manual_ipv6_box), ipv6_gateway_entry);
    
    // Fill current IPv6 settings if manual
    if (s_ip6 && g_strcmp0(nm_setting_ip_config_get_method(s_ip6), NM_SETTING_IP6_CONFIG_METHOD_MANUAL) == 0) {
        guint num_addresses = nm_setting_ip_config_get_num_addresses(s_ip6);
        if (num_addresses > 0) {
            NMIPAddress* addr = nm_setting_ip_config_get_address(s_ip6, 0);
            if (addr) {
                gtk_editable_set_text(GTK_EDITABLE(ipv6_address_entry), nm_ip_address_get_address(addr));
                gtk_editable_set_text(GTK_EDITABLE(ipv6_prefix_entry), std::to_string(nm_ip_address_get_prefix(addr)).c_str());
            }
        }
        
        const char* gateway = nm_setting_ip_config_get_gateway(s_ip6);
        if (gateway) {
            gtk_editable_set_text(GTK_EDITABLE(ipv6_gateway_entry), gateway);
        }
    }
    
    gtk_box_append(GTK_BOX(ipv6_page), manual_ipv6_box);
    
    // IPv6 DNS Settings
    GtkWidget* ipv6_dns_separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_append(GTK_BOX(ipv6_page), ipv6_dns_separator);
    
    GtkWidget* ipv6_dns_label = gtk_label_new("IPv6 DNS Servers:");
    gtk_widget_set_halign(ipv6_dns_label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(ipv6_page), ipv6_dns_label);
    
    GtkWidget* ipv6_dns_auto_check = gtk_check_button_new_with_label("Obtain IPv6 DNS server addresses automatically");
    gtk_box_append(GTK_BOX(ipv6_page), ipv6_dns_auto_check);
    
    GtkWidget* ipv6_dns_manual_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    
    GtkWidget* ipv6_dns1_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(ipv6_dns1_entry), "Primary IPv6 DNS (e.g., 2001:4860:4860::8888)");
    gtk_box_append(GTK_BOX(ipv6_dns_manual_box), ipv6_dns1_entry);
    
    GtkWidget* ipv6_dns2_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(ipv6_dns2_entry), "Secondary IPv6 DNS (e.g., 2001:4860:4860::8844)");
    gtk_box_append(GTK_BOX(ipv6_dns_manual_box), ipv6_dns2_entry);
    
    // Fill current IPv6 DNS settings
    bool ipv6_auto_dns = true;
    if (s_ip6) {
        guint num_dns = nm_setting_ip_config_get_num_dns(s_ip6);
        if (num_dns > 0) {
            ipv6_auto_dns = false;
            gtk_editable_set_text(GTK_EDITABLE(ipv6_dns1_entry), nm_setting_ip_config_get_dns(s_ip6, 0));
            if (num_dns > 1) {
                gtk_editable_set_text(GTK_EDITABLE(ipv6_dns2_entry), nm_setting_ip_config_get_dns(s_ip6, 1));
            }
        }
    }
    
    gtk_check_button_set_active(GTK_CHECK_BUTTON(ipv6_dns_auto_check), ipv6_auto_dns);
    gtk_widget_set_sensitive(ipv6_dns_manual_box, !ipv6_auto_dns);
    
    gtk_box_append(GTK_BOX(ipv6_page), ipv6_dns_manual_box);
    
    // Connect IPv6 signals for dynamic UI updates
    g_signal_connect(ipv6_method_combo, "changed", G_CALLBACK(onIPv6MethodChanged), manual_ipv6_box);
    g_signal_connect(ipv6_dns_auto_check, "toggled", G_CALLBACK(onIPv6DNSAutoToggled), ipv6_dns_manual_box);
    
    // Set initial IPv6 sensitivity
    gtk_widget_set_sensitive(manual_ipv6_box, gtk_combo_box_get_active(GTK_COMBO_BOX(ipv6_method_combo)) == 1);
    
    // Advanced Tab (for ports and other settings)
    GtkWidget* advanced_page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_margin_top(advanced_page, 20);
    gtk_widget_set_margin_bottom(advanced_page, 20);
    gtk_widget_set_margin_start(advanced_page, 20);
    gtk_widget_set_margin_end(advanced_page, 20);
    
    // Port Settings Section
    GtkWidget* port_label = gtk_label_new("Port Settings:");
    gtk_widget_set_halign(port_label, GTK_ALIGN_START);
    PangoAttrList* portAttrList = pango_attr_list_new();
    if (portAttrList) {
        PangoAttribute* portSizeAttr = pango_attr_size_new(16 * PANGO_SCALE);
        PangoAttribute* portWeightAttr = pango_attr_weight_new(PANGO_WEIGHT_BOLD);
        if (portSizeAttr) pango_attr_list_insert(portAttrList, portSizeAttr);
        if (portWeightAttr) pango_attr_list_insert(portAttrList, portWeightAttr);
        gtk_label_set_attributes(GTK_LABEL(port_label), portAttrList);
        pango_attr_list_unref(portAttrList);
    }
    gtk_box_append(GTK_BOX(advanced_page), port_label);
    
    // HTTP Proxy Port
    GtkWidget* http_port_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget* http_port_label = gtk_label_new("HTTP Proxy Port:");
    gtk_widget_set_size_request(http_port_label, 150, -1);
    gtk_widget_set_halign(http_port_label, GTK_ALIGN_START);
    GtkWidget* http_port_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(http_port_entry), "8080");
    gtk_widget_set_size_request(http_port_entry, 100, -1);
    gtk_box_append(GTK_BOX(http_port_box), http_port_label);
    gtk_box_append(GTK_BOX(http_port_box), http_port_entry);
    gtk_box_append(GTK_BOX(advanced_page), http_port_box);
    
    // HTTPS Proxy Port
    GtkWidget* https_port_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget* https_port_label = gtk_label_new("HTTPS Proxy Port:");
    gtk_widget_set_size_request(https_port_label, 150, -1);
    gtk_widget_set_halign(https_port_label, GTK_ALIGN_START);
    GtkWidget* https_port_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(https_port_entry), "8443");
    gtk_widget_set_size_request(https_port_entry, 100, -1);
    gtk_box_append(GTK_BOX(https_port_box), https_port_label);
    gtk_box_append(GTK_BOX(https_port_box), https_port_entry);
    gtk_box_append(GTK_BOX(advanced_page), https_port_box);
    
    // FTP Proxy Port
    GtkWidget* ftp_port_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget* ftp_port_label = gtk_label_new("FTP Proxy Port:");
    gtk_widget_set_size_request(ftp_port_label, 150, -1);
    gtk_widget_set_halign(ftp_port_label, GTK_ALIGN_START);
    GtkWidget* ftp_port_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(ftp_port_entry), "21");
    gtk_widget_set_size_request(ftp_port_entry, 100, -1);
    gtk_box_append(GTK_BOX(ftp_port_box), ftp_port_label);
    gtk_box_append(GTK_BOX(ftp_port_box), ftp_port_entry);
    gtk_box_append(GTK_BOX(advanced_page), ftp_port_box);
    
    // SOCKS Proxy Port
    GtkWidget* socks_port_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget* socks_port_label = gtk_label_new("SOCKS Proxy Port:");
    gtk_widget_set_size_request(socks_port_label, 150, -1);
    gtk_widget_set_halign(socks_port_label, GTK_ALIGN_START);
    GtkWidget* socks_port_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(socks_port_entry), "1080");
    gtk_widget_set_size_request(socks_port_entry, 100, -1);
    gtk_box_append(GTK_BOX(socks_port_box), socks_port_label);
    gtk_box_append(GTK_BOX(socks_port_box), socks_port_entry);
    gtk_box_append(GTK_BOX(advanced_page), socks_port_box);
    
    // Separator
    GtkWidget* port_separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_append(GTK_BOX(advanced_page), port_separator);
    
    // Custom Port Range
    GtkWidget* custom_port_label = gtk_label_new("Custom Port Range:");
    gtk_widget_set_halign(custom_port_label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(advanced_page), custom_port_label);
    
    GtkWidget* port_range_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget* port_start_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(port_start_entry), "Start Port (e.g., 1024)");
    gtk_widget_set_size_request(port_start_entry, 150, -1);
    
    GtkWidget* port_dash_label = gtk_label_new(" - ");
    
    GtkWidget* port_end_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(port_end_entry), "End Port (e.g., 65535)");
    gtk_widget_set_size_request(port_end_entry, 150, -1);
    
    gtk_box_append(GTK_BOX(port_range_box), port_start_entry);
    gtk_box_append(GTK_BOX(port_range_box), port_dash_label);
    gtk_box_append(GTK_BOX(port_range_box), port_end_entry);
    gtk_box_append(GTK_BOX(advanced_page), port_range_box);
    
    // Add tabs to notebook
   // gtk_notebook_append_page(GTK_NOTEBOOK(notebook), ipv4_page, gtk_label_new("IPv4"));
   // gtk_notebook_append_page(GTK_NOTEBOOK(notebook), ipv6_page, gtk_label_new("IPv6"));
   // gtk_notebook_append_page(GTK_NOTEBOOK(notebook), advanced_page, gtk_label_new("Advanced"));
    
    // Store widgets for later access
    g_object_set_data(G_OBJECT(dialog), "ipv4_method", ipv4_method_combo);
    g_object_set_data(G_OBJECT(dialog), "ip_entry", ip_entry);
    g_object_set_data(G_OBJECT(dialog), "netmask_entry", netmask_entry);
    g_object_set_data(G_OBJECT(dialog), "gateway_entry", gateway_entry);
    g_object_set_data(G_OBJECT(dialog), "dns_auto", dns_auto_check);
    g_object_set_data(G_OBJECT(dialog), "dns1_entry", dns1_entry);
    g_object_set_data(G_OBJECT(dialog), "dns2_entry", dns2_entry);
    g_object_set_data(G_OBJECT(dialog), "ipv6_method", ipv6_method_combo);
    g_object_set_data(G_OBJECT(dialog), "ipv6_address_entry", ipv6_address_entry);
    g_object_set_data(G_OBJECT(dialog), "ipv6_prefix_entry", ipv6_prefix_entry);
    g_object_set_data(G_OBJECT(dialog), "ipv6_gateway_entry", ipv6_gateway_entry);
    g_object_set_data(G_OBJECT(dialog), "ipv6_dns_auto", ipv6_dns_auto_check);
    g_object_set_data(G_OBJECT(dialog), "ipv6_dns1_entry", ipv6_dns1_entry);
    g_object_set_data(G_OBJECT(dialog), "ipv6_dns2_entry", ipv6_dns2_entry);
    g_object_set_data(G_OBJECT(dialog), "http_port_entry", http_port_entry);
    g_object_set_data(G_OBJECT(dialog), "https_port_entry", https_port_entry);
    g_object_set_data(G_OBJECT(dialog), "ftp_port_entry", ftp_port_entry);
    g_object_set_data(G_OBJECT(dialog), "socks_port_entry", socks_port_entry);
    g_object_set_data(G_OBJECT(dialog), "port_start_entry", port_start_entry);
    g_object_set_data(G_OBJECT(dialog), "port_end_entry", port_end_entry);
    g_object_set_data(G_OBJECT(dialog), "connection", connection);
    g_object_set_data(G_OBJECT(dialog), "network_manager", this);
    
    g_signal_connect(dialog, "response", G_CALLBACK(onSettingsDialogResponse), nullptr);
    
    gtk_window_present(GTK_WINDOW(dialog));
}

void NetworkManager::onSettingsDialogResponse(GtkDialog* dialog, gint response_id, gpointer user_data) {
    (void)user_data;
    
    if (response_id == GTK_RESPONSE_ACCEPT) {
        // Apply settings
        NetworkManager* netMgr = static_cast<NetworkManager*>(g_object_get_data(G_OBJECT(dialog), "network_manager"));
        NMRemoteConnection* connection = static_cast<NMRemoteConnection*>(g_object_get_data(G_OBJECT(dialog), "connection"));
        
        if (!netMgr || !connection) {
            gtk_window_destroy(GTK_WINDOW(dialog));
            return;
        }
        
        // Get widgets
        GtkComboBox* ipv4_method = GTK_COMBO_BOX(g_object_get_data(G_OBJECT(dialog), "ipv4_method"));
        GtkEntry* ip_entry = GTK_ENTRY(g_object_get_data(G_OBJECT(dialog), "ip_entry"));
        GtkEntry* netmask_entry = GTK_ENTRY(g_object_get_data(G_OBJECT(dialog), "netmask_entry"));
        GtkEntry* gateway_entry = GTK_ENTRY(g_object_get_data(G_OBJECT(dialog), "gateway_entry"));
        GtkCheckButton* dns_auto = GTK_CHECK_BUTTON(g_object_get_data(G_OBJECT(dialog), "dns_auto"));
        GtkEntry* dns1_entry = GTK_ENTRY(g_object_get_data(G_OBJECT(dialog), "dns1_entry"));
        GtkEntry* dns2_entry = GTK_ENTRY(g_object_get_data(G_OBJECT(dialog), "dns2_entry"));
        GtkComboBox* ipv6_method = GTK_COMBO_BOX(g_object_get_data(G_OBJECT(dialog), "ipv6_method"));
        GtkEntry* ipv6_address_entry = GTK_ENTRY(g_object_get_data(G_OBJECT(dialog), "ipv6_address_entry"));
        GtkEntry* ipv6_prefix_entry = GTK_ENTRY(g_object_get_data(G_OBJECT(dialog), "ipv6_prefix_entry"));
        GtkEntry* ipv6_gateway_entry = GTK_ENTRY(g_object_get_data(G_OBJECT(dialog), "ipv6_gateway_entry"));
        GtkCheckButton* ipv6_dns_auto = GTK_CHECK_BUTTON(g_object_get_data(G_OBJECT(dialog), "ipv6_dns_auto"));
        GtkEntry* ipv6_dns1_entry = GTK_ENTRY(g_object_get_data(G_OBJECT(dialog), "ipv6_dns1_entry"));
        GtkEntry* ipv6_dns2_entry = GTK_ENTRY(g_object_get_data(G_OBJECT(dialog), "ipv6_dns2_entry"));
        GtkEntry* http_port_entry = GTK_ENTRY(g_object_get_data(G_OBJECT(dialog), "http_port_entry"));
        GtkEntry* https_port_entry = GTK_ENTRY(g_object_get_data(G_OBJECT(dialog), "https_port_entry"));
        GtkEntry* ftp_port_entry = GTK_ENTRY(g_object_get_data(G_OBJECT(dialog), "ftp_port_entry"));
        GtkEntry* socks_port_entry = GTK_ENTRY(g_object_get_data(G_OBJECT(dialog), "socks_port_entry"));
        GtkEntry* port_start_entry = GTK_ENTRY(g_object_get_data(G_OBJECT(dialog), "port_start_entry"));
        GtkEntry* port_end_entry = GTK_ENTRY(g_object_get_data(G_OBJECT(dialog), "port_end_entry"));
        
        // Create a new connection with updated settings
        NMConnection* new_connection = nm_simple_connection_new_clone(NM_CONNECTION(connection));
        
        // Update IPv4 settings
        NMSettingIPConfig* s_ip4 = NM_SETTING_IP_CONFIG(nm_connection_get_setting_ip4_config(new_connection));
        if (!s_ip4) {
            s_ip4 = NM_SETTING_IP_CONFIG(nm_setting_ip4_config_new());
            nm_connection_add_setting(new_connection, NM_SETTING(s_ip4));
        }
        
        int ipv4_method_idx = gtk_combo_box_get_active(ipv4_method);
        switch (ipv4_method_idx) {
            case 0: // Auto
                g_object_set(s_ip4, NM_SETTING_IP_CONFIG_METHOD, NM_SETTING_IP4_CONFIG_METHOD_AUTO, NULL);
                break;
            case 1: { // Manual
                g_object_set(s_ip4, NM_SETTING_IP_CONFIG_METHOD, NM_SETTING_IP4_CONFIG_METHOD_MANUAL, NULL);
                
                // Clear existing addresses
                nm_setting_ip_config_clear_addresses(s_ip4);
                
                // Add manual address if provided
                const char* ip_text = gtk_editable_get_text(GTK_EDITABLE(ip_entry));
                const char* netmask_text = gtk_editable_get_text(GTK_EDITABLE(netmask_entry));
                const char* gateway_text = gtk_editable_get_text(GTK_EDITABLE(gateway_entry));
                
                if (ip_text && strlen(ip_text) > 0) {
                    // Convert netmask to prefix
                    guint prefix = 24; // default
                    if (netmask_text && strlen(netmask_text) > 0) {
                        struct in_addr netmask_addr;
                        if (inet_aton(netmask_text, &netmask_addr)) {
                            guint32 mask = ntohl(netmask_addr.s_addr);
                            prefix = __builtin_popcount(mask);
                        }
                    }
                    
                    NMIPAddress* addr = nm_ip_address_new(AF_INET, ip_text, prefix, nullptr);
                    if (addr) {
                        nm_setting_ip_config_add_address(s_ip4, addr);
                        nm_ip_address_unref(addr);
                    }
                    
                    if (gateway_text && strlen(gateway_text) > 0) {
                        g_object_set(s_ip4, NM_SETTING_IP_CONFIG_GATEWAY, gateway_text, NULL);
                    }
                }
                break;
            }
            case 2: // Link-Local
                g_object_set(s_ip4, NM_SETTING_IP_CONFIG_METHOD, NM_SETTING_IP4_CONFIG_METHOD_LINK_LOCAL, NULL);
                break;
            case 3: // Disabled
                g_object_set(s_ip4, NM_SETTING_IP_CONFIG_METHOD, NM_SETTING_IP4_CONFIG_METHOD_DISABLED, NULL);
                break;
        }
        
        // Update DNS settings
        nm_setting_ip_config_clear_dns(s_ip4);
        if (!gtk_check_button_get_active(dns_auto)) {
            const char* dns1_text = gtk_editable_get_text(GTK_EDITABLE(dns1_entry));
            const char* dns2_text = gtk_editable_get_text(GTK_EDITABLE(dns2_entry));
            
            if (dns1_text && strlen(dns1_text) > 0) {
                nm_setting_ip_config_add_dns(s_ip4, dns1_text);
            }
            if (dns2_text && strlen(dns2_text) > 0) {
                nm_setting_ip_config_add_dns(s_ip4, dns2_text);
            }
        }
        
        // Update IPv6 settings
        NMSettingIPConfig* s_ip6 = NM_SETTING_IP_CONFIG(nm_connection_get_setting_ip6_config(new_connection));
        if (!s_ip6) {
            s_ip6 = NM_SETTING_IP_CONFIG(nm_setting_ip6_config_new());
            nm_connection_add_setting(new_connection, NM_SETTING(s_ip6));
        }
        
        int ipv6_method_idx = gtk_combo_box_get_active(ipv6_method);
        switch (ipv6_method_idx) {
            case 0: // Auto
                g_object_set(s_ip6, NM_SETTING_IP_CONFIG_METHOD, NM_SETTING_IP6_CONFIG_METHOD_AUTO, NULL);
                break;
            case 1: { // Manual
                g_object_set(s_ip6, NM_SETTING_IP_CONFIG_METHOD, NM_SETTING_IP6_CONFIG_METHOD_MANUAL, NULL);
                
                // Clear existing addresses
                nm_setting_ip_config_clear_addresses(s_ip6);
                
                const char* ipv6_address_text = gtk_editable_get_text(GTK_EDITABLE(ipv6_address_entry));
                const char* ipv6_prefix_text = gtk_editable_get_text(GTK_EDITABLE(ipv6_prefix_entry));
                const char* ipv6_gateway_text = gtk_editable_get_text(GTK_EDITABLE(ipv6_gateway_entry));
                
                if (ipv6_address_text && strlen(ipv6_address_text) > 0 && 
                    ipv6_prefix_text && strlen(ipv6_prefix_text) > 0) {
                    
                    int prefix = std::stoi(ipv6_prefix_text);
                    if (prefix < 0 || prefix > 128) prefix = 64; // Default IPv6 prefix
                    
                    GError* error = nullptr;
                    NMIPAddress* addr = nm_ip_address_new(AF_INET6, ipv6_address_text, prefix, &error);
                    if (addr && !error) {
                        nm_setting_ip_config_add_address(s_ip6, addr);
                        nm_ip_address_unref(addr);
                    } else if (error) {
                        std::cout << "IPv6 address error: " << error->message << std::endl;
                        g_error_free(error);
                    }
                }
                
                // Set IPv6 gateway
                if (ipv6_gateway_text && strlen(ipv6_gateway_text) > 0) {
                    g_object_set(s_ip6, NM_SETTING_IP_CONFIG_GATEWAY, ipv6_gateway_text, NULL);
                }
                break;
            }
            case 2: // Link-Local
                g_object_set(s_ip6, NM_SETTING_IP_CONFIG_METHOD, NM_SETTING_IP6_CONFIG_METHOD_LINK_LOCAL, NULL);
                break;
            case 3: // Disabled
                g_object_set(s_ip6, NM_SETTING_IP_CONFIG_METHOD, NM_SETTING_IP6_CONFIG_METHOD_DISABLED, NULL);
                break;
        }
        
        // Handle IPv6 DNS settings
        nm_setting_ip_config_clear_dns(s_ip6);
        if (!gtk_check_button_get_active(ipv6_dns_auto)) {
            const char* ipv6_dns1_text = gtk_editable_get_text(GTK_EDITABLE(ipv6_dns1_entry));
            const char* ipv6_dns2_text = gtk_editable_get_text(GTK_EDITABLE(ipv6_dns2_entry));
            
            if (ipv6_dns1_text && strlen(ipv6_dns1_text) > 0) {
                nm_setting_ip_config_add_dns(s_ip6, ipv6_dns1_text);
            }
            if (ipv6_dns2_text && strlen(ipv6_dns2_text) > 0) {
                nm_setting_ip_config_add_dns(s_ip6, ipv6_dns2_text);
            }
        }
        
        // Handle port settings (stored as connection data for reference)
        const char* http_port_text = gtk_editable_get_text(GTK_EDITABLE(http_port_entry));
        const char* https_port_text = gtk_editable_get_text(GTK_EDITABLE(https_port_entry));
        const char* ftp_port_text = gtk_editable_get_text(GTK_EDITABLE(ftp_port_entry));
        const char* socks_port_text = gtk_editable_get_text(GTK_EDITABLE(socks_port_entry));
        const char* port_start_text = gtk_editable_get_text(GTK_EDITABLE(port_start_entry));
        const char* port_end_text = gtk_editable_get_text(GTK_EDITABLE(port_end_entry));
        
        // Log port settings (in a real implementation, these would be saved to proxy settings)
        if (http_port_text && strlen(http_port_text) > 0) {
            std::cout << "HTTP Proxy Port set to: " << http_port_text << std::endl;
        }
        if (https_port_text && strlen(https_port_text) > 0) {
            std::cout << "HTTPS Proxy Port set to: " << https_port_text << std::endl;
        }
        if (ftp_port_text && strlen(ftp_port_text) > 0) {
            std::cout << "FTP Proxy Port set to: " << ftp_port_text << std::endl;
        }
        if (socks_port_text && strlen(socks_port_text) > 0) {
            std::cout << "SOCKS Proxy Port set to: " << socks_port_text << std::endl;
        }
        if (port_start_text && strlen(port_start_text) > 0 && port_end_text && strlen(port_end_text) > 0) {
            std::cout << "Custom Port Range set to: " << port_start_text << "-" << port_end_text << std::endl;
        }
        
        // Save the connection
        nm_remote_connection_commit_changes_async(connection, TRUE, nullptr, nullptr, nullptr);
        
        g_object_unref(new_connection);
        
        std::cout << "Network settings updated successfully" << std::endl;
    }
    
    gtk_window_destroy(GTK_WINDOW(dialog));
}

void NetworkManager::onIPv4MethodChanged(GtkComboBox* combo, gpointer user_data) {
    GtkWidget* manual_box = static_cast<GtkWidget*>(user_data);
    int active = gtk_combo_box_get_active(combo);
    gtk_widget_set_sensitive(manual_box, active == 1); // Manual method
}

void NetworkManager::onDNSAutoToggled(GtkCheckButton* check, gpointer user_data) {
    GtkWidget* dns_box = static_cast<GtkWidget*>(user_data);
    gboolean active = gtk_check_button_get_active(check);
    gtk_widget_set_sensitive(dns_box, !active);
}

void NetworkManager::onIPv6MethodChanged(GtkComboBox* combo, gpointer user_data) {
    GtkWidget* manual_box = static_cast<GtkWidget*>(user_data);
    int active = gtk_combo_box_get_active(combo);
    gtk_widget_set_sensitive(manual_box, active == 1); // Manual method
}

void NetworkManager::onIPv6DNSAutoToggled(GtkCheckButton* check, gpointer user_data) {
    GtkWidget* dns_box = static_cast<GtkWidget*>(user_data);
    gboolean active = gtk_check_button_get_active(check);
    gtk_widget_set_sensitive(dns_box, !active);
}

std::string NetworkManager::getAssetPath(const std::string& filename) {
    const char* home = std::getenv("HOME");
    if (!home) {
        std::cerr << "HOME environment variable not set" << std::endl;
        return filename;
    }
    return std::string(home) + "/.config/Elysia/assets/assets/" + filename;
}
