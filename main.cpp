#include "MainWindow.h"
#include <gtk/gtk.h>
#include <locale.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cctype>

static void activate(GtkApplication* app, gpointer user_data) {
    // Set locale to avoid issues
    setlocale(LC_ALL, "");
    
    if (app) {
        std::string* section = static_cast<std::string*>(user_data);
        MainWindow* window;
        
        if (section && !section->empty()) {
            window = new MainWindow(app, *section);
        } else {
            window = new MainWindow(app);
        }
        
        if (window) {
            window->show();
            
            // Store window pointer in app data so we can clean it up later
            g_object_set_data_full(G_OBJECT(app), "main_window", window, 
                                  [](gpointer data) {
                                      delete static_cast<MainWindow*>(data);
                                  });
        }
        
        // Clean up the section string if it was allocated
        if (section) {
            delete section;
        }
    }
}

int main(int argc, char** argv) {
    // Set locale to avoid issues
    setlocale(LC_ALL, "");
    
    // Parse command line arguments
    std::string section = "";
    if (argc > 1) {
        std::string arg = argv[1];
        
        // Convert to lowercase for consistency
        std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);
        
        // Handle help requests
        if (arg == "-h" || arg == "--help" || arg == "help") {
            std::cout << "Usage: " << argv[0] << " [section]" << std::endl;
            std::cout << "Launch Elysia Settings application" << std::endl;
            std::cout << "" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  [no arguments]    Launch the main settings interface" << std::endl;
            std::cout << "  section           Launch directly to a specific section" << std::endl;
            std::cout << "" << std::endl;
            std::cout << "Valid sections:" << std::endl;
            std::cout << "  about             About system information" << std::endl;
            std::cout << "  display           Display and screen settings" << std::endl;
            std::cout << "  network           Network configuration" << std::endl;
            std::cout << "  sound             Audio settings" << std::endl;
            std::cout << "  applications      Application management (aliases: apps, hyprland)" << std::endl;
            std::cout << "  wallpaper         Wallpaper and appearance (alias: appearance)" << std::endl;
            std::cout << "  bluetooth         Bluetooth settings" << std::endl;
            std::cout << "  battery           Battery information" << std::endl;
            std::cout << "  power             Power management" << std::endl;
            std::cout << "  storage           Storage and disk information" << std::endl;
            std::cout << "  support           Support and donations" << std::endl;
            std::cout << "  update            System updates (alias: updates)" << std::endl;
            std::cout << "" << std::endl;
            std::cout << "Examples:" << std::endl;
            std::cout << "  " << argv[0] << "                Launch main settings" << std::endl;
            std::cout << "  " << argv[0] << " about          Launch About section" << std::endl;
            std::cout << "  " << argv[0] << " display        Launch Display settings" << std::endl;
            std::cout << "  " << argv[0] << " --help         Show this help" << std::endl;
            return 0;
        }
        
        // Map aliases to actual section names
        std::map<std::string, std::string> sectionMap = {
            {"apps", "hyperland"},
            {"hyprland", "hyperland"},
            {"wallpaper", "appearance"},
            {"appearance", "appearance"},
            {"updates", "updates"},
            {"update", "updates"}
        };
        
        // Check if it's a known alias or section
        if (sectionMap.find(arg) != sectionMap.end()) {
            section = sectionMap[arg];
        } else {
            // List of valid sections
            std::vector<std::string> validSections = {
                "about", "display", "network", "sound", "hyperland", 
                "appearance", "bluetooth", "battery", "power", "storage", 
                "support", "updates"
            };
            
            if (std::find(validSections.begin(), validSections.end(), arg) != validSections.end()) {
                section = arg;
            } else {
                std::cerr << "Error: Unknown section '" << argv[1] << "'" << std::endl;
                std::cerr << "Use '" << argv[0] << " --help' for usage information." << std::endl;
                return 1;
            }
        }
    }
    
    GtkApplication* app = gtk_application_new("com.elysia.settings", G_APPLICATION_DEFAULT_FLAGS);
    if (app) {
        // Pass the section to the activate callback
        std::string* sectionPtr = section.empty() ? nullptr : new std::string(section);
        g_signal_connect(app, "activate", G_CALLBACK(activate), sectionPtr);
        
        // Create a minimal argv array without the section argument to avoid GIO file handling issues
        char* app_argv[] = {argv[0], nullptr};
        int app_argc = 1;
        int status = g_application_run(G_APPLICATION(app), app_argc, app_argv);
        g_object_unref(app);
        
        return status;
    }
    
    return 1;
}