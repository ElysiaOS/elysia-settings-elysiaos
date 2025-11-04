#include "MainWindow.h"
#include <iostream>
#include <cstdlib>
#include <glib.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <unistd.h>
#include <map>
#include <utility>
#include <string>
#include <random>
#include <cmath>
#include <algorithm>
#include <fstream>

MainWindow::MainWindow(GtkApplication* app) : window(nullptr), overlay(nullptr), fixed(nullptr), 
    background(nullptr), exitButton(nullptr), themeButton(nullptr), hoverInfoOverlay(nullptr), aboutManager(nullptr), storageManager(nullptr), networkManager(nullptr),
    bluetoothManager(nullptr), soundManager(nullptr), appearanceManager(nullptr), 
          batteryManager(nullptr), displayManager(nullptr), powerManager(nullptr), applicationsManager(nullptr), languageManager(nullptr), isAnimating(false), currentTheme(getInitialTheme()) {
    
    // Initialize translation system
    initializeTranslations();
    
    if (app) {
        window = gtk_application_window_new(app);
        if (window) {
            setupWindow();
            loadBackground();
            setupTiles();
            setupExitButton();
            setupThemeButton();
            
            // Create about manager after overlay is set up
            aboutManager = new AboutManager(this, window, overlay);
            
            // Create storage manager after overlay is set up
            storageManager = new StorageManager(this, window, overlay);
            
            // Create network manager after overlay is set up
            networkManager = new NetworkManager(this, window, overlay);
            
            // Create bluetooth manager after overlay is set up
            bluetoothManager = new BluetoothManager(this, window, overlay);
            
            // Create sound manager after overlay is set up
            soundManager = new SoundManager(this, GTK_WINDOW(window), overlay);
            
            // Create appearance manager after overlay is set up
            appearanceManager = new AppearanceManager(this, GTK_WINDOW(window), overlay);
            
            // Create battery manager after overlay is set up
            batteryManager = new BatteryManager(this, GTK_WINDOW(window), overlay);
            
            // Create display manager after overlay is set up
            displayManager = new DisplayManager(this, GTK_WINDOW(window), overlay);
            
            // Create power manager after overlay is set up
            powerManager = new PowerManager(this, GTK_WINDOW(window), overlay);
            
            // Create applications manager after overlay is set up
            applicationsManager = new ApplicationsManager(this, window, overlay);
            
            // Create language manager after overlay is set up
            languageManager = new LanguageManager(this, GTK_WINDOW(window), overlay);
        }
    }
}

MainWindow::MainWindow(GtkApplication* app, const std::string& section) : window(nullptr), overlay(nullptr), fixed(nullptr), 
    background(nullptr), exitButton(nullptr), themeButton(nullptr), hoverInfoOverlay(nullptr), aboutManager(nullptr), storageManager(nullptr), networkManager(nullptr),
    bluetoothManager(nullptr), soundManager(nullptr), appearanceManager(nullptr), 
          batteryManager(nullptr), displayManager(nullptr), powerManager(nullptr), applicationsManager(nullptr), languageManager(nullptr), isAnimating(false), currentTheme(getInitialTheme()) {
    
    // Initialize translation system
    initializeTranslations();
    
    if (app) {
        window = gtk_application_window_new(app);
        if (window) {
            setupWindow();
            loadBackground();
            setupTiles();
            setupExitButton();
            setupThemeButton();
            
            // Create about manager after overlay is set up
            aboutManager = new AboutManager(this, window, overlay);
            
            // Create storage manager after overlay is set up
            storageManager = new StorageManager(this, window, overlay);
            
            // Create network manager after overlay is set up
            networkManager = new NetworkManager(this, window, overlay);
            
            // Create bluetooth manager after overlay is set up
            bluetoothManager = new BluetoothManager(this, window, overlay);
            
            // Create sound manager after overlay is set up
            soundManager = new SoundManager(this, GTK_WINDOW(window), overlay);
            
            // Create appearance manager after overlay is set up
            appearanceManager = new AppearanceManager(this, GTK_WINDOW(window), overlay);
            
            // Create battery manager after overlay is set up
            batteryManager = new BatteryManager(this, GTK_WINDOW(window), overlay);
            
            // Create display manager after overlay is set up
            displayManager = new DisplayManager(this, GTK_WINDOW(window), overlay);
            
            // Create power manager after overlay is set up
            powerManager = new PowerManager(this, GTK_WINDOW(window), overlay);
            
            // Create applications manager after overlay is set up
            applicationsManager = new ApplicationsManager(this, window, overlay);
            
            // Create language manager after overlay is set up
            languageManager = new LanguageManager(this, GTK_WINDOW(window), overlay);
            
            // Open the specified section after a short delay to ensure everything is initialized
            g_timeout_add(100, [](gpointer user_data) -> gboolean {
                auto* data_pair = static_cast<std::pair<MainWindow*, std::string>*>(user_data);
                MainWindow* mainWindow = data_pair->first;
                std::string section = data_pair->second;
                
                mainWindow->openSection(section);
                
                delete data_pair;
                return G_SOURCE_REMOVE;
            }, new std::pair<MainWindow*, std::string>(this, section));
        }
    }
}

MainWindow::~MainWindow() {
    // Clean up managers properly to prevent memory corruption
    // Order matters: clean up in reverse order of creation
    if (applicationsManager) {
        delete applicationsManager;
        applicationsManager = nullptr;
    }
    if (languageManager) {
        delete languageManager;
        languageManager = nullptr;
    }
    if (powerManager) {
        delete powerManager;
        powerManager = nullptr;
    }
    if (displayManager) {
        delete displayManager;
        displayManager = nullptr;
    }
    if (batteryManager) {
        delete batteryManager;
        batteryManager = nullptr;
    }
    if (appearanceManager) {
        delete appearanceManager;
        appearanceManager = nullptr;
    }
    if (soundManager) {
        delete soundManager;
        soundManager = nullptr;
    }
    if (bluetoothManager) {
        delete bluetoothManager;
        bluetoothManager = nullptr;
    }
    if (networkManager) {
        delete networkManager;
        networkManager = nullptr;
    }
    if (storageManager) {
        delete storageManager;
        storageManager = nullptr;
    }
    if (aboutManager) {
        delete aboutManager;
        aboutManager = nullptr;
    }
    
    // Clear tile widgets vector
    tileWidgets.clear();
    
    // Clean up bouncing animation timers
    for (auto& tile : bouncingTiles) {
        if (tile.bounceTimerId > 0) {
            g_source_remove(tile.bounceTimerId);
        }
    }
    bouncingTiles.clear();
    
    // Clean up image bouncing animation timers
    for (auto& tile : imageBouncingTiles) {
        if (tile.imageBounceTimerId > 0) {
            g_source_remove(tile.imageBounceTimerId);
        }
    }
    imageBouncingTiles.clear();
    
    // GTK handles widget cleanup automatically
}

void MainWindow::show() {
    if (window) {
        gtk_widget_set_visible(window, TRUE);
    }
}

void MainWindow::showMainMenu() {
    if (fixed) {
        gtk_widget_set_visible(fixed, TRUE);
    }
    // Switch back to the main background for the current theme
    if (currentTheme == Theme::Amphoreus) {
        switchToBackground("background-mem.png");
    } else {
        switchToBackground("background.png");
    }
    
    // Reset animation flag when returning to main menu
    isAnimating = false;
}

void MainWindow::hideMainMenu() {
    if (fixed) {
        gtk_widget_set_visible(fixed, FALSE);
    }
    // Ensure any hover info overlay is not visible while inside a settings page
    hideHoverInfo();
}

void MainWindow::openSection(const std::string& section) {
    std::cout << "Opening section: " << section << std::endl;
    
    if (section == "about") {
        hideMainMenu();
        if (aboutManager) {
            aboutManager->show();
        }
        if (currentTheme == Theme::Amphoreus) {
            switchToBackground("background-mem.png");
        }
    } else if (section == "storage") {
        hideMainMenu();
        if (storageManager) {
            storageManager->show();
        }
        if (currentTheme == Theme::Amphoreus) {
            switchToBackground("background-mem.png");
        }
    } else if (section == "network") {
        hideMainMenu();
        if (networkManager) {
            networkManager->show();
        }
        if (currentTheme == Theme::Amphoreus) {
            switchToBackground("background-mem.png");
        }
    } else if (section == "bluetooth") {
        hideMainMenu();
        if (bluetoothManager) {
            bluetoothManager->show();
        }
        if (currentTheme == Theme::Amphoreus) {
            switchToBackground("background-mem.png");
        }
    } else if (section == "sound") {
        hideMainMenu();
        if (soundManager) {
            soundManager->show();
        }
        if (currentTheme == Theme::Amphoreus) {
            switchToBackground("background-mem.png");
        }
    } else if (section == "appearance") {
        hideMainMenu();
        if (appearanceManager) {
            appearanceManager->show();
        }
        if (currentTheme == Theme::Amphoreus) {
            switchToBackground("background-mem.png");
        }
    } else if (section == "battery") {
        hideMainMenu();
        if (batteryManager) {
            batteryManager->show();
        }
        if (currentTheme == Theme::Amphoreus) {
            switchToBackground("background-mem.png");
        }
    } else if (section == "display") {
        hideMainMenu();
        if (displayManager) {
            displayManager->show();
        }
        if (currentTheme == Theme::Amphoreus) {
            switchToBackground("background-mem.png");
        }
    } else if (section == "power") {
        hideMainMenu();
        if (powerManager) {
            powerManager->show();
        }
        if (currentTheme == Theme::Amphoreus) {
            switchToBackground("background-mem.png");
        }
    } else if (section == "hyperland") {
        hideMainMenu();
        if (applicationsManager) {
            applicationsManager->show();
        }
        if (currentTheme == Theme::Amphoreus) {
            switchToBackground("background-mem.png");
        }
    } else if (section == "car") {
        hideMainMenu();
        if (languageManager) {
            languageManager->show();
        }
        if (currentTheme == Theme::Amphoreus) {
            switchToBackground("background-mem.png");
        }
    } else if (section == "updates") {
        // Launch the Elysia updater script
        std::cout << "Launching Elysia updater..." << std::endl;
        std::string command = "/usr/bin/elysia-updater.sh &";
        int result = system(command.c_str());
        if (result == 0) {
            std::cout << "Elysia updater launched successfully" << std::endl;
        } else {
            std::cout << "Failed to launch Elysia updater (exit code: " << result << ")" << std::endl;
        }
    } else if (section == "support") {
        // Open support website
        std::cout << "Opening support website..." << std::endl;
        std::string command = "xdg-open https://ko-fi.com/matsuko3 &";
        int result = system(command.c_str());
        if (result == 0) {
            std::cout << "Support website opened successfully" << std::endl;
        } else {
            std::cout << "Failed to open support website (exit code: " << result << ")" << std::endl;
        }
    } else {
        std::cout << "Settings page for " << section << " not implemented yet" << std::endl;
    }
}

void MainWindow::switchToBackground(const std::string& backgroundName) {
    if (!overlay) return;
    
    std::string bgPath = getAssetPath(backgroundName);
    
    // Check if file exists before trying to load it
    if (access(bgPath.c_str(), F_OK) != 0) {
        std::cerr << "Background file not found: " << bgPath << std::endl;
        return;
    }
    
    GtkWidget* picture = gtk_picture_new_for_filename(bgPath.c_str());
    if (!picture) {
        std::cerr << "Failed to load background image: " << bgPath << std::endl;
        return;
    }
    
    // Make it fill the entire window
    gtk_widget_set_hexpand(picture, TRUE);
    gtk_widget_set_vexpand(picture, TRUE);
    gtk_picture_set_can_shrink(GTK_PICTURE(picture), TRUE);
    gtk_picture_set_content_fit(GTK_PICTURE(picture), GTK_CONTENT_FIT_FILL);
    
    // Put picture as background of overlay
    gtk_overlay_set_child(GTK_OVERLAY(overlay), picture);
    
    background = picture;
}

void MainWindow::setupWindow() {
    if (!window) return;
    
    gtk_window_set_title(GTK_WINDOW(window), TR(TranslationKeys::WINDOW_TITLE));
    gtk_window_set_default_size(GTK_WINDOW(window), 1600, 800);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    // Create overlay so we can stack background and tiles
    overlay = gtk_overlay_new();
    if (overlay) {
        gtk_window_set_child(GTK_WINDOW(window), overlay);

        // Create fixed container for tiles
        fixed = gtk_fixed_new();
        if (fixed) {
            gtk_overlay_add_overlay(GTK_OVERLAY(overlay), fixed);
        }
    }
}

void MainWindow::loadBackground() {
    if (!overlay) return;
    
    // Load background based on current theme
    if (currentTheme == Theme::Amphoreus) {
        switchToBackground("background-mem.png");
    } else {
        switchToBackground("background.png");
    }
    return;
    
    // Legacy code below (kept for reference, but shouldn't be reached)
    std::string bgPath = getAssetPath("background.png");
    
    // Check if file exists
    if (access(bgPath.c_str(), F_OK) != 0) {
        std::cerr << "Background file not found: " << bgPath << std::endl;
        return;
    }

    GtkWidget* picture = gtk_picture_new_for_filename(bgPath.c_str());
    if (!picture) {
        std::cerr << "Failed to load background image: " << bgPath << std::endl;
        return;
    }

    // Make it fill the entire window
    gtk_widget_set_hexpand(picture, TRUE);
    gtk_widget_set_vexpand(picture, TRUE);
    gtk_picture_set_can_shrink(GTK_PICTURE(picture), TRUE);
    gtk_picture_set_content_fit(GTK_PICTURE(picture), GTK_CONTENT_FIT_FILL);

    // Put picture as background of overlay
    gtk_overlay_set_child(GTK_OVERLAY(overlay), picture);
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), fixed);

    background = picture;
}

void MainWindow::setupTiles() {
    if (!fixed) return;
    
    // Use theme data setup instead
    setupThemeData();
}

void MainWindow::setupExitButton() {
    if (!overlay) return;
    
    std::string exitPath = getAssetPath("exit.png");
    
    // Create a button - EXACTLY like other managers
    exitButton = gtk_button_new();
    if (!exitButton) return;
    
    // Create a fixed container to position the label on top of the image
    GtkWidget* fixedContainer = gtk_fixed_new();
    if (!fixedContainer) return;
    
    gtk_widget_set_size_request(fixedContainer, 120, 60);
    
    // Load the exit image
    GtkWidget* exitImage = gtk_picture_new_for_filename(exitPath.c_str());
    if (!exitImage) {
        exitImage = gtk_image_new_from_icon_name("application-exit");
    } else {
        gtk_widget_set_size_request(exitImage, 159, 66);
        gtk_picture_set_can_shrink(GTK_PICTURE(exitImage), TRUE);
        gtk_picture_set_content_fit(GTK_PICTURE(exitImage), GTK_CONTENT_FIT_CONTAIN);
    }
    
    // Create label
    GtkWidget* label = gtk_label_new(TR(TranslationKeys::EXIT));
    if (label) {
        gtk_widget_add_css_class(label, "back-button-label");
    }
    
    // Position image and label in the fixed container
    if (exitImage) {
        gtk_fixed_put(GTK_FIXED(fixedContainer), exitImage, 0, 0);
    }
    if (label) {
        gtk_fixed_put(GTK_FIXED(fixedContainer), label, 40, 20);
    }
    
    // Add fixed container to button
    gtk_button_set_child(GTK_BUTTON(exitButton), fixedContainer);
    
    // Style the button to remove borders and background
    gtk_widget_set_size_request(exitButton, 120, 60);
    gtk_widget_add_css_class(exitButton, "flat");
    
    // Position the exit button at top left - exactly like back buttons in managers
    gtk_fixed_put(GTK_FIXED(fixed), exitButton, 0, 0);
    
    // Connect the callback
    g_signal_connect(exitButton, "clicked", G_CALLBACK(onExitButtonClicked), this);
}

void MainWindow::setupThemeButton() {
    if (!overlay) return;
    
    // Add CSS for theme button
    GtkCssProvider* provider = gtk_css_provider_new();
    if (provider) {
        const char* css =
            ".theme-button-label { "
            "  color: white; "
            "  font-family: ElysiaOSNew12; "
            "  font-size: 18px; "
            "  font-weight: bold; "
            "  text-shadow: 0 1px 2px rgba(0, 0, 0, 0.5); "
            "  background: transparent; "
            "  border: none; "
            "  margin: 0px; "
            "  padding: 0px; "
            "} "
            ".theme-button { "
            "  background: rgba(0, 0, 0, 0.3); "
            "  border: 2px solid rgba(255, 255, 255, 0.5); "
            "  border-radius: 8px; "
            "} "
            ".theme-button:hover { "
            "  background: rgba(0, 0, 0, 0.5); "
            "  border-color: rgba(255, 255, 255, 0.8); "
            "} ";

        gtk_css_provider_load_from_string(provider, css);
        gtk_style_context_add_provider_for_display(
            gdk_display_get_default(),
            GTK_STYLE_PROVIDER(provider),
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        g_object_unref(provider);
    }
    
    // Create a simple button
    themeButton = gtk_button_new();
    if (!themeButton) return;
    
    // Create label
    std::string themeText = (currentTheme == Theme::ElysianRealm) ? "Elysian Realm" : "Amphoreus";
    GtkWidget* label = gtk_label_new(themeText.c_str());
    if (label) {
        gtk_widget_add_css_class(label, "theme-button-label");
    }
    
    // Add label to button
    gtk_button_set_child(GTK_BUTTON(themeButton), label);
    
    // Style the button
    gtk_widget_set_size_request(themeButton, 200, 50);
    gtk_widget_add_css_class(themeButton, "flat");
    gtk_widget_add_css_class(themeButton, "theme-button");
    
    // Position the theme button at top right, horizontally aligned with exit button
    gtk_fixed_put(GTK_FIXED(fixed), themeButton, 1380, 5);
    
    // Connect the callback
    g_signal_connect(themeButton, "clicked", G_CALLBACK(onThemeButtonClicked), this);
}

void MainWindow::createTile(const SettingsTile& tile, const std::pair<int, int>& labelPosition) {
    if (!fixed) return;
    
    std::cout << "Loading tile: " << tile.name << " from: " << tile.imagePath << std::endl;
    
    // Check if file exists
    if (access(tile.imagePath.c_str(), F_OK) != 0) {
        std::cerr << "Tile image file not found: " << tile.imagePath << std::endl;
    }

    GtkWidget* image = gtk_picture_new_for_filename(tile.imagePath.c_str());
    if (!image) {
        std::cerr << "Failed to load tile image: " << tile.imagePath << std::endl;
        image = gtk_drawing_area_new();
        if (image) {
            gtk_widget_set_size_request(image, tile.width, tile.height);
        }
    } else {
        gtk_widget_set_size_request(image, tile.width, tile.height);
        gtk_picture_set_can_shrink(GTK_PICTURE(image), TRUE);
        gtk_picture_set_content_fit(GTK_PICTURE(image), GTK_CONTENT_FIT_CONTAIN);
    }

    // Click gesture
    GtkGesture* clickGesture = gtk_gesture_click_new();
    if (clickGesture) {
        gtk_widget_add_controller(image, GTK_EVENT_CONTROLLER(clickGesture));
    }

    // Hover effects
    GtkEventController* motionController = gtk_event_controller_motion_new();
    if (motionController) {
        gtk_widget_add_controller(image, motionController);
    }

    // Store tile name
    char* tileNameCopy = g_strdup(tile.name.c_str());
    if (tileNameCopy) {
        g_object_set_data_full(G_OBJECT(image), "tile-name", tileNameCopy, g_free);
        g_object_set_data(G_OBJECT(image), "main-window", this);
    }

    if (clickGesture) {
        g_signal_connect(clickGesture, "pressed", G_CALLBACK(onTileClicked), this);
    }
    if (motionController) {
        g_signal_connect(motionController, "enter", G_CALLBACK(onTileEnter), nullptr);
        g_signal_connect(motionController, "leave", G_CALLBACK(onTileLeave), nullptr);
    }

    GtkCssProvider* provider = gtk_css_provider_new();
    if (provider) {
        const char* css =
            ".tile-hover { "
            "  transition: all 0.3s cubic-bezier(0.68, -0.55, 0.265, 1.55); "
            "  filter: brightness(1.2) drop-shadow(0 0 30px rgba(254, 208, 244, 0.6)); "
            "  transform: scale(1.08); "
            "} "
            ".tile-normal { "
            "  transition: all 0.3s cubic-bezier(0.68, -0.55, 0.265, 1.55); "
            "  filter: brightness(1.0); "
            "  transform: scale(1.0); "
            "} "
            ".title-label, .info-label { "
            "  color: white; "
            "  background: transparent; "
            "  border: none; "
            "  margin: 0px; "
            "  padding: 0px; "
            "} "
            ".tile-label { "
            "  color: white; "
            "  font-family: ElysiaOSNew12; "
            "  font-size: 16px; "
            "  font-weight: bold; "
            "  text-shadow: 0 1px 2px rgba(0, 0, 0, 0.5); "
            "  background: transparent; "
            "  border: none; "
            "  margin: 0px; "
            "  padding: 0px; "
            "}";

        gtk_css_provider_load_from_string(provider, css);
        gtk_style_context_add_provider_for_display(
            gdk_display_get_default(),
            GTK_STYLE_PROVIDER(provider),
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

        g_object_unref(provider);
    }

    if (image) {
        gtk_widget_add_css_class(image, "tile-normal");
        gtk_fixed_put(GTK_FIXED(fixed), image, tile.x, tile.y);
        tileWidgets.push_back(image);
        
        // Create label for the tile using translations
        std::string labelText;
        if (tile.name == "car") labelText = TR(TranslationKeys::TILE_CAR);
        else if (tile.name == "battery") labelText = TR(TranslationKeys::TILE_BATTERY);
        else if (tile.name == "appearance") labelText = TR(TranslationKeys::TILE_APPEARANCE);
        else if (tile.name == "power") labelText = TR(TranslationKeys::TILE_POWER);
        else if (tile.name == "bluetooth") labelText = TR(TranslationKeys::TILE_BLUETOOTH);
        else if (tile.name == "sound") labelText = TR(TranslationKeys::TILE_SOUND);
        else if (tile.name == "about") labelText = TR(TranslationKeys::TILE_ABOUT);
        else if (tile.name == "display") labelText = TR(TranslationKeys::TILE_DISPLAY);
        else if (tile.name == "network") labelText = TR(TranslationKeys::TILE_NETWORK);
        else if (tile.name == "updates") labelText = TR(TranslationKeys::TILE_UPDATES);
        else if (tile.name == "hyperland") labelText = TR(TranslationKeys::TILE_HYPERLAND);
        else if (tile.name == "storage") labelText = TR(TranslationKeys::TILE_STORAGE);
        else if (tile.name == "support") labelText = TR(TranslationKeys::TILE_SUPPORT);
        else {
            labelText = tile.name;
            if (!labelText.empty()) {
                labelText[0] = std::toupper(labelText[0]);
            }
        }
        
        GtkWidget* label = gtk_label_new(labelText.c_str());
        gtk_widget_add_css_class(label, "tile-label");
        // Position label with custom position
        gtk_fixed_put(GTK_FIXED(fixed), label, labelPosition.first, labelPosition.second);
        tileWidgets.push_back(label);
    }
}

void MainWindow::onTileClicked(GtkGestureClick* gesture, int, double, double, gpointer data) {
    if (!gesture) return;
    
    GtkWidget* widget = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));
    if (!widget) return;
    
    const char* tileName = (const char*)g_object_get_data(G_OBJECT(widget), "tile-name");
    MainWindow* mainWindow = static_cast<MainWindow*>(data);

    if (tileName && mainWindow) {
        // Prevent multiple simultaneous clicks during animation
        if (mainWindow->isAnimating) {
            std::cout << "Animation in progress, ignoring click on: " << tileName << std::endl;
            return;
        }
        
        // Set animation flag IMMEDIATELY to prevent multiple clicks
        mainWindow->isAnimating = true;
        // Hide any hover info overlay immediately to prevent it from lingering over pages
        mainWindow->hideHoverInfo();
        
        std::cout << "Clicked on: " << tileName << " settings" << std::endl;
        
        // Find the tile data for shatter effect
        SettingsTile* clickedTile = nullptr;
        for (auto& tile : mainWindow->tiles) {
            if (tile.name == std::string(tileName)) {
                clickedTile = &tile;
                break;
            }
        }
        
        if (clickedTile) {
            // Create and start shatter effect
            mainWindow->currentShatterEffect = std::make_unique<MainWindow::ShatterEffect>(mainWindow, widget, *clickedTile);
            mainWindow->currentShatterEffect->startShatter();
            
            // Add much shorter delay before showing the page to let shatter effect play
            g_timeout_add(100, [](gpointer user_data) -> gboolean {
                auto* data_pair = static_cast<std::pair<MainWindow*, std::string>*>(user_data);
                MainWindow* mainWindow = data_pair->first;
                std::string tileName = data_pair->second;
                
                // Debug output to see what tile name we're getting
                std::cout << "Clicked tile: '" << tileName << "'" << std::endl;
                
                if (tileName == "about") {
                    mainWindow->hideMainMenu();
                    if (mainWindow->aboutManager) {
                        mainWindow->aboutManager->show();
                    }
                    if (mainWindow->currentTheme == Theme::Amphoreus) {
                        mainWindow->switchToBackground("background-mem.png");
                    }
                } else if (tileName == "storage") {
                    mainWindow->hideMainMenu();
                    if (mainWindow->storageManager) {
                        mainWindow->storageManager->show();
                    }
                    if (mainWindow->currentTheme == Theme::Amphoreus) {
                        mainWindow->switchToBackground("background-mem.png");
                    }
                } else if (tileName == "network") {
                    mainWindow->hideMainMenu();
                    if (mainWindow->networkManager) {
                        mainWindow->networkManager->show();
                    }
                    if (mainWindow->currentTheme == Theme::Amphoreus) {
                        mainWindow->switchToBackground("background-mem.png");
                    }
                } else if (tileName == "bluetooth") {
                    mainWindow->hideMainMenu();
                    if (mainWindow->bluetoothManager) {
                        mainWindow->bluetoothManager->show();
                    }
                    if (mainWindow->currentTheme == Theme::Amphoreus) {
                        mainWindow->switchToBackground("background-mem.png");
                    }
                } else if (tileName == "sound") {
                    mainWindow->hideMainMenu();
                    if (mainWindow->soundManager) {
                        mainWindow->soundManager->show();
                    }
                    if (mainWindow->currentTheme == Theme::Amphoreus) {
                        mainWindow->switchToBackground("background-mem.png");
                    }
                } else if (tileName == "appearance") {
                    mainWindow->hideMainMenu();
                    if (mainWindow->appearanceManager) {
                        mainWindow->appearanceManager->show();
                    }
                    if (mainWindow->currentTheme == Theme::Amphoreus) {
                        mainWindow->switchToBackground("background-mem.png");
                    }
                } else if (tileName == "battery") {
                    mainWindow->hideMainMenu();
                    if (mainWindow->batteryManager) {
                        mainWindow->batteryManager->show();
                    }
                    if (mainWindow->currentTheme == Theme::Amphoreus) {
                        mainWindow->switchToBackground("background-mem.png");
                    }
                } else if (tileName == "display") {
                    mainWindow->hideMainMenu();
                    if (mainWindow->displayManager) {
                        mainWindow->displayManager->show();
                    }
                    if (mainWindow->currentTheme == Theme::Amphoreus) {
                        mainWindow->switchToBackground("background-mem.png");
                    }
                } else if (tileName == "power") {
                    mainWindow->hideMainMenu();
                    if (mainWindow->powerManager) {
                        mainWindow->powerManager->show();
                    }
                    if (mainWindow->currentTheme == Theme::Amphoreus) {
                        mainWindow->switchToBackground("background-mem.png");
                    }
                } else if (tileName == "hyperland") {
                    mainWindow->hideMainMenu();
                    if (mainWindow->applicationsManager) {
                        mainWindow->applicationsManager->show();
                    }
                    if (mainWindow->currentTheme == Theme::Amphoreus) {
                        mainWindow->switchToBackground("background-mem.png");
                    }
                } else if (tileName == "car") {
                    mainWindow->hideMainMenu();
                    if (mainWindow->languageManager) {
                        mainWindow->languageManager->show();
                    }
                    if (mainWindow->currentTheme == Theme::Amphoreus) {
                        mainWindow->switchToBackground("background-mem.png");
                    }
                } else if (tileName == "updates") {
                    // Launch the Elysia updater script
                    std::cout << "Launching Elysia updater..." << std::endl;
                    std::string command = "/usr/bin/elysia-updater.sh &";
                    int result = system(command.c_str());
                    if (result == 0) {
                        std::cout << "Elysia updater launched successfully" << std::endl;
                    } else {
                        std::cout << "Failed to launch Elysia updater (exit code: " << result << ")" << std::endl;
                    }
                    // Reset animation flag since we're not opening a manager
                    mainWindow->isAnimating = false;
                } else if (tileName == "support") {
                    // Open support website
                    std::cout << "Opening support website..." << std::endl;
                    std::string command = "xdg-open https://ko-fi.com/matsuko3 &";
                    int result = system(command.c_str());
                    if (result == 0) {
                        std::cout << "Support website opened successfully" << std::endl;
                    } else {
                        std::cout << "Failed to open support website (exit code: " << result << ")" << std::endl;
                    }
                    // Reset animation flag since we're not opening a manager
                    mainWindow->isAnimating = false;
                } else {
                    std::cout << "Settings page for " << tileName << " not implemented yet" << std::endl;
                    // Reset animation flag for unimplemented tiles too
                    mainWindow->isAnimating = false;
                }
                
                // Don't reset animation flag here - only reset when returning to main menu
                
                delete data_pair;
                return G_SOURCE_REMOVE;
            }, new std::pair<MainWindow*, std::string>(mainWindow, std::string(tileName)));
        } else {
            // If tile data not found, reset the flag to prevent it being stuck
            std::cout << "Tile data not found for: " << tileName << std::endl;
            mainWindow->isAnimating = false;
        }
    }
}

void MainWindow::onTileEnter(GtkEventControllerMotion* controller, double, double, gpointer) {
    if (!controller) return;
    
    GtkWidget* widget = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(controller));
    if (widget) {
        gtk_widget_remove_css_class(widget, "tile-normal");
        gtk_widget_add_css_class(widget, "tile-hover");
        
        // Start both bouncing animations
        MainWindow* mainWindow = static_cast<MainWindow*>(g_object_get_data(G_OBJECT(widget), "main-window"));
        if (mainWindow) {
            // Get the tile's base position from the SettingsTile data
            const char* tileName = static_cast<const char*>(g_object_get_data(G_OBJECT(widget), "tile-name"));
            if (tileName) {
                for (const auto& tile : mainWindow->tiles) {
                    if (tile.name == tileName) {
                        mainWindow->startBounceAnimation(widget, tile.x, tile.y);
                        mainWindow->startImageBounceAnimation(widget);
                        break;
                    }
                }
                // Show hover info for Amphoreus theme
                mainWindow->showHoverInfo(std::string(tileName));
            }
        }
    }
}

void MainWindow::onTileLeave(GtkEventControllerMotion* controller, gpointer) {
    if (!controller) return;
    
    GtkWidget* widget = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(controller));
    if (widget) {
        gtk_widget_remove_css_class(widget, "tile-hover");
        gtk_widget_add_css_class(widget, "tile-normal");
        // Stop both bouncing animations
        MainWindow* mainWindow = static_cast<MainWindow*>(g_object_get_data(G_OBJECT(widget), "main-window"));
        if (mainWindow) {
            mainWindow->stopBounceAnimation(widget);
            mainWindow->stopImageBounceAnimation(widget);
            // Hide hover info
            mainWindow->hideHoverInfo();
        }
    }
}

std::string MainWindow::getAssetPath(const std::string& filename) {
    const char* home = std::getenv("HOME");
    if (!home) {
        std::cerr << "HOME environment variable not set" << std::endl;
        return filename;
    }
    return std::string(home) + "/.config/Elysia/assets/assets/" + filename;
}

MainWindow::Theme MainWindow::getInitialTheme() {
    const char* home = std::getenv("HOME");
    if (!home) {
        std::cerr << "HOME environment variable not set, defaulting to ElysianRealm theme" << std::endl;
        return Theme::ElysianRealm;
    }
    
    std::string lightFilePath = std::string(home) + "/.config/hypr/Light.txt";
    std::ifstream file(lightFilePath);
    
    if (!file.is_open()) {
        std::cout << "Light.txt not found, defaulting to ElysianRealm theme" << std::endl;
        return Theme::ElysianRealm;
    }
    
    std::string content;
    std::getline(file, content);
    file.close();
    
    // Trim whitespace
    content.erase(0, content.find_first_not_of(" \t\n\r"));
    content.erase(content.find_last_not_of(" \t\n\r") + 1);
    
    if (content.empty()) {
        std::cout << "Light.txt is empty, defaulting to ElysianRealm theme" << std::endl;
        return Theme::ElysianRealm;
    }
    
    // Convert to lowercase for case-insensitive comparison
    std::string contentLower = content;
    std::transform(contentLower.begin(), contentLower.end(), contentLower.begin(), ::tolower);
    
    if (contentLower.find("ely") != std::string::npos) {
        std::cout << "Light.txt contains 'ely', starting with ElysianRealm theme" << std::endl;
        return Theme::ElysianRealm;
    } else if (contentLower.find("cyrene") != std::string::npos) {
        std::cout << "Light.txt contains 'cyrene', starting with Amphoreus theme" << std::endl;
        return Theme::Amphoreus;
    } else {
        std::cout << "Light.txt doesn't contain 'ely' or 'cyrene', defaulting to ElysianRealm theme" << std::endl;
        return Theme::ElysianRealm;
    }
}

void MainWindow::initializeTranslations() {
    // Try to detect current system language using the same method as LanguageManager
    std::string currentLang = "en_US"; // Default fallback
    
    // First, try to read from /etc/locale.conf (same as LanguageManager::getCurrentLanguage)
    std::ifstream file("/etc/locale.conf");
    std::string line;
    
    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (line.find("LANG=") == 0) {
                currentLang = line.substr(5); // Remove "LANG=" prefix
                // Remove quotes if present
                if (currentLang.front() == '"' && currentLang.back() == '"') {
                    currentLang = currentLang.substr(1, currentLang.length() - 2);
                }
                break;
            }
        }
        file.close();
    }
    
    // If not found in file, try environment variable as fallback
    if (currentLang == "en_US") {
        const char* lang = std::getenv("LANG");
        if (lang) {
            currentLang = std::string(lang);
        }
    }
    
    // Extract base language code (remove encoding suffix like .UTF-8)
    size_t dotPos = currentLang.find('.');
    if (dotPos != std::string::npos) {
        currentLang = currentLang.substr(0, dotPos);
    }
    
    // Map to supported languages
    std::string mappedLang = "en_US"; // Default fallback
    if (currentLang.find("zh_CN") != std::string::npos) {
        mappedLang = "zh_CN";
    } else if (currentLang.find("ja_JP") != std::string::npos) {
        mappedLang = "ja_JP";
    } else if (currentLang.find("ko_KR") != std::string::npos) {
        mappedLang = "ko_KR";
    } else if (currentLang.find("ru_RU") != std::string::npos) {
        mappedLang = "ru_RU";
    } else if (currentLang.find("de_DE") != std::string::npos) {
        mappedLang = "de_DE";
    } else if (currentLang.find("fr_FR") != std::string::npos) {
        mappedLang = "fr_FR";
    } else if (currentLang.find("vi_VN") != std::string::npos) {
        mappedLang = "vi_VN";
    } else if (currentLang.find("id_ID") != std::string::npos) {
        mappedLang = "id_ID";
    } else if (currentLang.find("es_ES") != std::string::npos) {
        mappedLang = "es_ES";
    }
    
    // Set the language in the translation manager
    TranslationManager::getInstance().setLanguage(mappedLang);
    std::cout << "Initialized translations with language: " << mappedLang << " (detected from: " << currentLang << ")" << std::endl;
}

void MainWindow::showHoverInfo(const std::string& tileName) {
    // Only show hover info for Amphoreus theme
    if (currentTheme != Theme::Amphoreus) {
        return;
    }
    
    if (!overlay) return;
    
    // Determine which info image to show based on tile name
    // Pattern: <tile>-info1.png for all tiles (e.g., about-info1.png, battery-info1.png)
    std::string infoImageName = std::string(tileName) + "-info1.png";
    
    std::string infoPath = getAssetPath(infoImageName);
    
    // Check if file exists
    if (access(infoPath.c_str(), F_OK) != 0) {
        std::cerr << "Hover info image file not found: " << infoPath << std::endl;
        return;
    }
    
    // If overlay already exists, just update the image (no assumptions about old child type)
    if (hoverInfoOverlay) {
        GtkWidget* picture = gtk_picture_new_for_filename(infoPath.c_str());
        if (picture) {
            gtk_widget_set_size_request(picture, 1600, 800);
            gtk_picture_set_can_shrink(GTK_PICTURE(picture), TRUE);
            gtk_picture_set_content_fit(GTK_PICTURE(picture), GTK_CONTENT_FIT_FILL);
            gtk_button_set_child(GTK_BUTTON(hoverInfoOverlay), picture);
        }
        // Ensure overlay remains non-targetable
        gtk_widget_set_can_target(hoverInfoOverlay, FALSE);
        gtk_widget_set_visible(hoverInfoOverlay, TRUE);
        return;
    }
    
    // Create new overlay button with the info image
    hoverInfoOverlay = gtk_button_new();
    if (!hoverInfoOverlay) return;
    
    GtkWidget* picture = gtk_picture_new_for_filename(infoPath.c_str());
    if (!picture) {
        std::cerr << "Failed to load hover info image: " << infoPath << std::endl;
        return;
    }
    
    // Make it fill the entire window (same as background)
    gtk_widget_set_size_request(picture, 1600, 800);
    gtk_picture_set_can_shrink(GTK_PICTURE(picture), TRUE);
    gtk_picture_set_content_fit(GTK_PICTURE(picture), GTK_CONTENT_FIT_FILL);
    
    gtk_button_set_child(GTK_BUTTON(hoverInfoOverlay), picture);
    gtk_widget_add_css_class(hoverInfoOverlay, "flat");
    gtk_widget_add_css_class(hoverInfoOverlay, "hover-info-overlay");
    
    // Make it non-interactive and not a target for events so it doesn't steal hover
    gtk_widget_set_sensitive(hoverInfoOverlay, FALSE);
    gtk_widget_set_can_target(hoverInfoOverlay, FALSE);
    
    // Add to overlay and position at (0, 0) to cover entire window
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), hoverInfoOverlay);
    gtk_widget_set_visible(hoverInfoOverlay, TRUE);
}

void MainWindow::hideHoverInfo() {
    if (hoverInfoOverlay) {
        gtk_widget_set_visible(hoverInfoOverlay, FALSE);
    }
}

// ShatterEffect Implementation
MainWindow::ShatterEffect::ShatterEffect(MainWindow* mainWindow, GtkWidget* tileWidget, const SettingsTile& tile)
    : mainWindow(mainWindow), originalTile(tileWidget), tileData(tile), animationTimeoutId(0), effectStartTime(0) {
}

MainWindow::ShatterEffect::~ShatterEffect() {
    cleanupEffect();
}

void MainWindow::ShatterEffect::startShatter() {
    if (!mainWindow || !originalTile) return;
    
    // Hide the original tile
    gtk_widget_set_visible(originalTile, FALSE);
    
    // Create fragments
    createFragments();
    
    // Start animation
    effectStartTime = g_get_monotonic_time();
    animationTimeoutId = g_timeout_add(8, animateFragments, this); // ~120 FPS for smoother animation
}

void MainWindow::ShatterEffect::createFragments() {
    if (!mainWindow->fixed) return;
    
    // Load the original image to create fragments from
    GdkPixbuf* originalPixbuf = gdk_pixbuf_new_from_file(tileData.imagePath.c_str(), nullptr);
    if (!originalPixbuf) {
        std::cerr << "Failed to load image for shatter effect: " << tileData.imagePath << std::endl;
        return;
    }
    
    // Scale the pixbuf to match the tile size
    GdkPixbuf* scaledPixbuf = gdk_pixbuf_scale_simple(originalPixbuf, tileData.width, tileData.height, GDK_INTERP_BILINEAR);
    g_object_unref(originalPixbuf);
    
    if (!scaledPixbuf) {
        std::cerr << "Failed to scale image for shatter effect" << std::endl;
        return;
    }
    
    // Create irregular fragments (more realistic glass shatter pattern)
    const int fragmentsX = 5;
    const int fragmentsY = 4;
    const double baseFragmentWidth = tileData.width / (double)fragmentsX;
    const double baseFragmentHeight = tileData.height / (double)fragmentsY;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> velocityDist(-500.0, 500.0);
    std::uniform_real_distribution<double> rotationDist(-15.0, 15.0);
    std::uniform_real_distribution<double> gravityDist(800.0, 1200.0);
    std::uniform_real_distribution<double> sizeDist(0.7, 1.3); // Vary fragment sizes
    std::uniform_real_distribution<double> offsetDist(-10.0, 10.0); // Random offsets for irregular shapes
    
    for (int y = 0; y < fragmentsY; y++) {
        for (int x = 0; x < fragmentsX; x++) {
            // Add some randomness to fragment size and position for irregular shatter
            double sizeMultiplier = sizeDist(gen);
            double fragmentWidth = baseFragmentWidth * sizeMultiplier;
            double fragmentHeight = baseFragmentHeight * sizeMultiplier;
            
            // Calculate source rectangle (what part of the original image to crop)
            int srcX = (int)(x * baseFragmentWidth + offsetDist(gen));
            int srcY = (int)(y * baseFragmentHeight + offsetDist(gen));
            int srcWidth = (int)fragmentWidth;
            int srcHeight = (int)fragmentHeight;
            
            // Clamp to image bounds
            srcX = std::max(0, std::min(srcX, tileData.width - 1));
            srcY = std::max(0, std::min(srcY, tileData.height - 1));
            srcWidth = std::min(srcWidth, tileData.width - srcX);
            srcHeight = std::min(srcHeight, tileData.height - srcY);
            
            if (srcWidth <= 0 || srcHeight <= 0) continue;
            
            // Create a cropped pixbuf for this fragment
            GdkPixbuf* fragmentPixbuf = gdk_pixbuf_new_subpixbuf(scaledPixbuf, srcX, srcY, srcWidth, srcHeight);
            if (!fragmentPixbuf) continue;
            
            // Create fragment widget using the cropped image
            GdkTexture* texture = gdk_texture_new_for_pixbuf(fragmentPixbuf);
            GtkWidget* fragmentWidget = gtk_picture_new_for_paintable(GDK_PAINTABLE(texture));
            if (!fragmentWidget) {
                g_object_unref(fragmentPixbuf);
                g_object_unref(texture);
                continue;
            }
            g_object_unref(texture); // Picture widget takes ownership
            
            // Set fragment size
            gtk_widget_set_size_request(fragmentWidget, srcWidth, srcHeight);
            gtk_picture_set_can_shrink(GTK_PICTURE(fragmentWidget), FALSE);
            gtk_picture_set_content_fit(GTK_PICTURE(fragmentWidget), GTK_CONTENT_FIT_FILL);
            
            // Calculate fragment position (where it starts on screen)
            double fragX = tileData.x + srcX;
            double fragY = tileData.y + srcY;
            
            // Add CSS for realistic glass fragment effect
            GtkCssProvider* provider = gtk_css_provider_new();
            if (provider) {
                // Create jagged, glass-like borders and effects
                std::string css = 
                    ".glass-fragment-" + std::to_string(x) + "-" + std::to_string(y) + " { "
                    "  border: 2px solid rgba(255,255,255,0.6); "
                    "  border-radius: " + std::to_string(2 + (int)(offsetDist(gen) * 0.3)) + "px; "
                    "  box-shadow: "
                    "    0 0 10px rgba(255,255,255,0.4), "
                    "    2px 2px 8px rgba(0,0,0,0.5), "
                    "    inset 0 0 3px rgba(255,255,255,0.3); "
                    "  background-clip: padding-box; "
                    "  transform-origin: center; "
                    "  filter: brightness(1.1) contrast(1.1); "
                    "} ";
                
                gtk_css_provider_load_from_string(provider, css.c_str());
                gtk_style_context_add_provider_for_display(
                    gdk_display_get_default(),
                    GTK_STYLE_PROVIDER(provider),
                    GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
                g_object_unref(provider);
            }
            
            std::string fragmentClass = "glass-fragment-" + std::to_string(x) + "-" + std::to_string(y);
            gtk_widget_add_css_class(fragmentWidget, fragmentClass.c_str());
            
            // Position fragment
            gtk_fixed_put(GTK_FIXED(mainWindow->fixed), fragmentWidget, (int)fragX, (int)fragY);
            
            // Create fragment data with more realistic physics
            ShatterFragment fragment;
            fragment.widget = fragmentWidget;
            fragment.startX = fragX;
            fragment.startY = fragY;
            
            // Calculate explosion direction from click center
            double centerX = tileData.x + tileData.width / 2.0;
            double centerY = tileData.y + tileData.height / 2.0;
            double dirX = (fragX + srcWidth/2.0) - centerX;
            double dirY = (fragY + srcHeight/2.0) - centerY;
            double distance = std::sqrt(dirX * dirX + dirY * dirY);
            
            if (distance > 0) {
                dirX /= distance;
                dirY /= distance;
            } else {
                dirX = velocityDist(gen) * 0.01;
                dirY = velocityDist(gen) * 0.01;
            }
            
            // More realistic shatter physics with faster explosion
            double explosionForce = 400.0 + distance * 5.0;
            fragment.velocityX = dirX * explosionForce + velocityDist(gen) * 0.3;
            fragment.velocityY = dirY * explosionForce + velocityDist(gen) * 0.3 - 100.0; // Slight upward bias
            
            fragment.rotation = 0.0;
            fragment.rotationSpeed = rotationDist(gen) * (1.0 + distance * 0.02); // More rotation for outer pieces
            fragment.gravity = gravityDist(gen);
            fragment.startTime = g_get_monotonic_time();
            
            fragments.push_back(fragment);
            
            // Clean up the fragment pixbuf
            g_object_unref(fragmentPixbuf);
        }
    }
    
    // Clean up the scaled pixbuf
    g_object_unref(scaledPixbuf);
}

gboolean MainWindow::ShatterEffect::animateFragments(gpointer user_data) {
    ShatterEffect* effect = static_cast<ShatterEffect*>(user_data);
    if (!effect || !effect->mainWindow) return G_SOURCE_REMOVE;
    
    guint64 currentTime = g_get_monotonic_time();
    double elapsedSeconds = (currentTime - effect->effectStartTime) / 1000000.0;
    double deltaTime = 0.008; // ~120 FPS
    
    // Animation duration: 0.25 seconds for faster, more responsive feel
    if (elapsedSeconds > 0.25) {
        effect->cleanupEffect();
        return G_SOURCE_REMOVE;
    }
    
    // Update each fragment with realistic physics
    for (auto& fragment : effect->fragments) {
        if (!fragment.widget) continue;
        
        // Apply gravity to velocity
        fragment.velocityY += fragment.gravity * deltaTime;
        
        // Apply air resistance (slight deceleration)
        fragment.velocityX *= 0.995;
        fragment.velocityY *= 0.998;
        
        // Update positions based on velocity
        fragment.startX += fragment.velocityX * deltaTime;
        fragment.startY += fragment.velocityY * deltaTime;
        
        // Update rotation
        fragment.rotation += fragment.rotationSpeed * deltaTime;
        
        // Apply position
        gtk_fixed_move(GTK_FIXED(effect->mainWindow->fixed), fragment.widget, (int)fragment.startX, (int)fragment.startY);
        
        // Apply realistic fade out effect (slower at first, faster at end)
        double fadeProgress = elapsedSeconds / 0.25;
        double alpha = 1.0 - (fadeProgress * fadeProgress); // Quadratic fade for more natural look
        alpha = std::max(0.0, alpha);
        gtk_widget_set_opacity(fragment.widget, alpha);
        
        // Apply scaling effect (fragments get slightly smaller as they fall)
        double scale = 1.0 - (fadeProgress * 0.2); // Shrink by up to 20%
        scale = std::max(0.8, scale);
        
        // Get the fragment's CSS class name
        const char* const* classes = gtk_widget_get_css_classes(fragment.widget);
        std::string fragmentClass = "";
        if (classes && classes[0]) {
            fragmentClass = std::string(classes[0]);
        }
        
        // Apply dynamic CSS with rotation, scaling, and enhanced glass effects
        GtkCssProvider* provider = gtk_css_provider_new();
        if (provider && !fragmentClass.empty()) {
            std::string css = 
                "." + fragmentClass + " { "
                "  transform: rotate(" + std::to_string(fragment.rotation) + "deg) scale(" + std::to_string(scale) + "); "
                "  border: 2px solid rgba(255,255,255," + std::to_string(alpha * 0.6) + "); "
                "  box-shadow: "
                "    0 0 " + std::to_string(10 * alpha) + "px rgba(255,255,255," + std::to_string(alpha * 0.4) + "), "
                "    " + std::to_string(2 * scale) + "px " + std::to_string(2 * scale) + "px " + std::to_string(8 * alpha) + "px rgba(0,0,0," + std::to_string(alpha * 0.5) + "), "
                "    inset 0 0 3px rgba(255,255,255," + std::to_string(alpha * 0.3) + "); "
                "  filter: brightness(" + std::to_string(1.1 - fadeProgress * 0.3) + ") contrast(" + std::to_string(1.1 - fadeProgress * 0.2) + "); "
                "} ";
            
            gtk_css_provider_load_from_string(provider, css.c_str());
            gtk_style_context_add_provider_for_display(
                gdk_display_get_default(),
                GTK_STYLE_PROVIDER(provider),
                GTK_STYLE_PROVIDER_PRIORITY_APPLICATION + 2);
            g_object_unref(provider);
        }
    }
    
    return G_SOURCE_CONTINUE;
}

void MainWindow::ShatterEffect::cleanupEffect() {
    if (animationTimeoutId > 0) {
        g_source_remove(animationTimeoutId);
        animationTimeoutId = 0;
    }
    
    // Remove all fragment widgets
    for (auto& fragment : fragments) {
        if (fragment.widget && mainWindow && mainWindow->fixed) {
            gtk_fixed_remove(GTK_FIXED(mainWindow->fixed), fragment.widget);
        }
    }
    fragments.clear();
    
    // Show the original tile again (it will be hidden when page changes)
    if (originalTile) {
        gtk_widget_set_visible(originalTile, TRUE);
    }
    
    // Don't reset animation flag here - only reset when returning to main menu
}

// Bouncing Animation Implementation
void MainWindow::startBounceAnimation(GtkWidget* tileWidget, int baseX, int baseY) {
    // Check if already bouncing
    for (auto& tile : bouncingTiles) {
        if (tile.widget == tileWidget) {
            return; // Already bouncing
        }
    }
    
    // Create new bouncing tile
    BouncingTile bouncingTile;
    bouncingTile.widget = tileWidget;
    bouncingTile.baseX = baseX;
    bouncingTile.baseY = baseY;
    bouncingTile.bounceOffset = 0.0;
    bouncingTile.bounceDir = 1;
    bouncingTile.bounceTimerId = g_timeout_add(16, updateBounceAnimation, this); // 60 FPS like QT6
    
    bouncingTiles.push_back(bouncingTile);
}

void MainWindow::stopBounceAnimation(GtkWidget* tileWidget) {
    for (auto it = bouncingTiles.begin(); it != bouncingTiles.end(); ++it) {
        if (it->widget == tileWidget) {
            // Remove timer
            if (it->bounceTimerId > 0) {
                g_source_remove(it->bounceTimerId);
            }
            
            // Reset position to base
            gtk_fixed_move(GTK_FIXED(fixed), tileWidget, it->baseX, it->baseY);
            
            // Remove from list
            bouncingTiles.erase(it);
            break;
        }
    }
}

gboolean MainWindow::updateBounceAnimation(gpointer user_data) {
    MainWindow* mainWindow = static_cast<MainWindow*>(user_data);
    if (!mainWindow || !mainWindow->fixed) return G_SOURCE_REMOVE;
    
    for (auto& tile : mainWindow->bouncingTiles) {
        if (!tile.widget) continue;
        
        // Update bounce offset like QT6 version
        tile.bounceOffset += tile.bounceDir * 0.7;
        
        // Reverse direction at boundaries like QT6 version
        if (tile.bounceOffset > 8 || tile.bounceOffset < -8) {
            tile.bounceDir *= -1;
        }
        
        // Calculate new position
        int newY = tile.baseY + (int)tile.bounceOffset;
        
        // Move the tile
        gtk_fixed_move(GTK_FIXED(mainWindow->fixed), tile.widget, tile.baseX, newY);
    }
    
    return G_SOURCE_CONTINUE;
}

void MainWindow::onExitButtonClicked(GtkButton*, gpointer user_data) {
    MainWindow* mainWindow = static_cast<MainWindow*>(user_data);
    if (mainWindow && mainWindow->window) {
        // Close the application
        GtkApplication* app = gtk_window_get_application(GTK_WINDOW(mainWindow->window));
        if (app) {
            g_application_quit(G_APPLICATION(app));
        }
    }
}

// Image Bouncing Animation Implementation
void MainWindow::startImageBounceAnimation(GtkWidget* tileWidget) {
    // Disable dynamic hover scaling to avoid cumulative shrink issues.
    // Static hover scale is provided via the .tile-hover CSS rule.
    (void)tileWidget;
}

void MainWindow::stopImageBounceAnimation(GtkWidget* tileWidget) {
    // Nothing to do since dynamic hover scaling is disabled.
    (void)tileWidget;
}

gboolean MainWindow::updateImageBounceAnimation(gpointer user_data) {
    // Dynamic hover scaling disabled; ensure we don't reschedule.
    return G_SOURCE_REMOVE;
}

void MainWindow::onThemeButtonClicked(GtkButton*, gpointer user_data) {
    MainWindow* mainWindow = static_cast<MainWindow*>(user_data);
    if (mainWindow) {
        mainWindow->switchTheme();
    }
}

void MainWindow::switchTheme() {
    // Toggle theme
    if (currentTheme == Theme::ElysianRealm) {
        currentTheme = Theme::Amphoreus;
    } else {
        currentTheme = Theme::ElysianRealm;
    }
    
    // Rebuild everything
    rebuildForTheme();
}

void MainWindow::setupThemeData() {
    if (!fixed) return;
    
    // Stop any active animations FIRST to avoid timers accessing removed widgets
    for (auto& tile : bouncingTiles) {
        if (tile.bounceTimerId > 0) {
            g_source_remove(tile.bounceTimerId);
        }
    }
    bouncingTiles.clear();
    
    for (auto& tile : imageBouncingTiles) {
        if (tile.imageBounceTimerId > 0) {
            g_source_remove(tile.imageBounceTimerId);
        }
    }
    imageBouncingTiles.clear();
    
    // Now remove existing tile widgets from the fixed container
    for (auto* widget : tileWidgets) {
        if (widget) {
            gtk_fixed_remove(GTK_FIXED(fixed), widget);
        }
    }
    tileWidgets.clear();
    tiles.clear();
    
    // Setup tiles based on current theme
    if (currentTheme == Theme::ElysianRealm) {
        setupElysianRealmTheme();
    } else {
        setupAmphoreusTheme();
    }
}

void MainWindow::rebuildForTheme() {
    // Update background
    if (currentTheme == Theme::ElysianRealm) {
        switchToBackground("background.png");
    } else {
        switchToBackground("background-mem.png");
    }
    
    // Ensure any hover overlay is hidden when switching themes
    hideHoverInfo();

    // Rebuild tiles
    setupThemeData();
    
    // Update theme button text (replace child to avoid type assumptions)
    if (themeButton) {
        std::string themeText = (currentTheme == Theme::ElysianRealm) ? "Elysian Realm" : "Amphoreus";
        GtkWidget* newLabel = gtk_label_new(themeText.c_str());
        if (newLabel) {
            gtk_widget_add_css_class(newLabel, "theme-button-label");
            gtk_button_set_child(GTK_BUTTON(themeButton), newLabel);
        }
    }
}

void MainWindow::setupElysianRealmTheme() {
    // Current theme 1 - Elysian Realm
    tiles = {
        {"car", getAssetPath("car.png"), 190, 290, 120, 260},
        {"battery", getAssetPath("battery.png"), 325, 105, 120, 260},
        {"appearance", getAssetPath("wallpaper.png"), 366, 355, 120, 260},
        {"power", getAssetPath("power.png"), 480, 210, 120, 260},
        {"bluetooth", getAssetPath("bluetooth.png"), 630, 95, 120, 260},
        {"sound", getAssetPath("sound.png"), 610, 395, 120, 260},
        {"about", getAssetPath("about.png"), 750, 240, 120, 260},
        {"display", getAssetPath("display.png"), 880, 130, 120, 260},
        {"network", getAssetPath("network.png"), 940, 380, 120, 260},
        {"updates", getAssetPath("update.png"), 1080, 183, 120, 260},
        {"hyperland", getAssetPath("apps.png"), 1290, 162, 120, 260},
        {"storage", getAssetPath("snake.png"), 1195, 340, 120, 260},
        {"support", getAssetPath("support.png"), 1340, 410, 120, 260}
    };
    
    // Label positions for Elysian Realm
    std::map<std::string, std::pair<int, int>> labelPositions = {
        {"car", {240, 290}},
        {"battery", {355, 100}},
        {"appearance", {386, 345}},
        {"power", {515, 210}},
        {"bluetooth", {650, 105}},
        {"sound", {645, 385}},
        {"about", {780, 215}},
        {"display", {905, 120}},
        {"network", {960, 370}},
        {"updates", {1100, 190}},
        {"hyperland", {1300, 160}},
        {"storage", {1215, 340}},
        {"support", {1355, 410}}
    };
    
    for (const auto& tile : tiles) {
        createTile(tile, labelPositions[tile.name]);
    }
}

void MainWindow::setupAmphoreusTheme() {
    // Theme 2 - Amphoreus (using car2.png, battery2.png, etc.)
    // Positions based on the image concept layout
    tiles = {
        {"car", getAssetPath("car2.png"), 20, 530, 150, 260},
        {"battery", getAssetPath("battery2.png"), 180, 430, 150, 260},
        {"appearance", getAssetPath("wallpaper2.png"), 1430, 50, 150, 260},
        {"power", getAssetPath("power2.png"), 180, 100, 150, 260},
        {"bluetooth", getAssetPath("bluetooth2.png"), 180, 250, 150, 260},
        {"sound", getAssetPath("sound2.png"), 1430, 220, 150, 260},
        {"about", getAssetPath("about2.png"), 20, 50, 150, 260},
        {"display", getAssetPath("display2.png"), 20, 220, 150, 260},
        {"network", getAssetPath("network2.png"), 1270, 100, 150, 260},
        {"updates", getAssetPath("update2.png"), 20, 380, 150, 260},
        {"hyperland", getAssetPath("apps2.png"), 1270, 250, 150, 260},
        {"storage", getAssetPath("snake2.png"), 1270, 430, 150, 260},
        {"support", getAssetPath("support2.png"), 1430, 400, 150, 260}
    };
    
    // Label positions for Amphoreus (centered on colored bar at bottom of tile)
    std::map<std::string, std::pair<int, int>> labelPositions = {
        {"car", {80, 690}},
        {"battery", {220, 595}},
        {"appearance", {1460, 215}},
        {"power", {225, 263}},
        {"bluetooth", {209, 410}},
        {"sound", {1480, 383}},
        {"about", {70, 210}},
        {"display", {65, 380}},
        {"network", {1305, 263}},
        {"updates", {60, 545}},
        {"hyperland", {1305, 415}},
        {"storage", {1310, 595}},
        {"support", {1470, 565}}
    };
    
    for (const auto& tile : tiles) {
        createTile(tile, labelPositions[tile.name]);
    }
}