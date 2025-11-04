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
    if (!nm_client_networking_set_enabled(nmClient, TRUE, &error)) {
        std::cerr << "Failed to enable networking: " << (error ? error->message : "Unknown error") << std::endl;
        if (error) g_error_free(error);
    }
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
    gtk_window_set_title(GTK_WINDOW(dialog), "Enter Network Password");
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
    
    GtkWidget* cancelBtn = gtk_button_new_with_label("Cancel");
    GtkWidget* connectBtn = gtk_button_new_with_label("Connect");
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
    g_signal_connect(connectBtn, "clicked", G_CALLBACK([](GtkButton* button, gpointer userData) {
        PasswordDialogData* data = static_cast<PasswordDialogData*>(userData);
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
    }), d);
    
    g_signal_connect(dialog, "destroy", G_CALLBACK([](GtkWidget* dialog, gpointer userData) {
        PasswordDialogData* data = static_cast<PasswordDialogData*>(userData);
        if (data) {
            if (data->ap) g_object_unref(data->ap);
            if (data->wifiDev) g_object_unref(data->wifiDev);
            delete data;
        }
    }), d);
    
    gtk_window_present(GTK_WINDOW(dialog));
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
            g_timeout_add(1000, [](gpointer data) -> gboolean {
                NetworkManager* nm = static_cast<NetworkManager*>(data);
                nm->scanWifiNetworks();
                return G_SOURCE_REMOVE;
            }, netMgr);
        } else {
            // Disabling Wi-Fi - update list immediately
            g_timeout_add(500, [](gpointer data) -> gboolean {
                NetworkManager* nm = static_cast<NetworkManager*>(data);
                nm->populateWifiList();
                return G_SOURCE_REMOVE;
            }, netMgr);
        }
        
        // Reflect the state change after a short delay
        g_timeout_add(300, [](gpointer data) -> gboolean {
            NetworkManager* nm = static_cast<NetworkManager*>(data);
            nm->reflectWifiSwitchState();
            return G_SOURCE_REMOVE;
        }, netMgr);
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
    if (data && data->networkManager) {
        data->networkManager->connectToNetwork(data->ssid, data->ap, data->isSecured);
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
    if (netMgr) {
        netMgr->updateTimeoutId = 0;
        
        if (!netMgr->updatingWifiSwitch) {
            netMgr->reflectWifiSwitchState();
            netMgr->populateWifiList();
        }
    }
    
    return G_SOURCE_REMOVE;
}

gboolean NetworkManager::populateWifiListTimeout(gpointer user_data) {
    NetworkManager* netMgr = static_cast<NetworkManager*>(user_data);
    if (netMgr) {
        netMgr->populateWifiList();
    }
    return G_SOURCE_REMOVE;
}

std::string NetworkManager::getAssetPath(const std::string& filename) {
    const char* home = std::getenv("HOME");
    if (!home) {
        std::cerr << "HOME environment variable not set" << std::endl;
        return filename;
    }
    return std::string(home) + "/.config/Elysia/assets/assets/" + filename;
}

