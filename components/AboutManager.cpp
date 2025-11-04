#include "AboutManager.h"
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

// Forward declaration of callback data
struct BackButtonCallbackData {
    AboutManager* aboutManager;
};

AboutManager::AboutManager(MainWindow* mainWindow, GtkWidget* parentWindow, GtkWidget* overlay) 
    : mainWindow(mainWindow), parentWindow(parentWindow), overlay(overlay), 
      aboutContainer(nullptr), backButton(nullptr), background(nullptr) {
    setupUI();
}

AboutManager::~AboutManager() {
    // Clear widget tracking
    infoLabels.clear();
    
    // All GTK widgets are managed by GTK and will be cleaned up automatically
}

void AboutManager::show() {
    if (aboutContainer) {
        gtk_widget_set_visible(aboutContainer, TRUE);
        // Switch to the about background
        if (mainWindow) {
            mainWindow->switchToBackground("background4.png");
        }
    }
}

void AboutManager::hide() {
    if (aboutContainer) {
        gtk_widget_set_visible(aboutContainer, FALSE);
    }
}

void AboutManager::setupUI() {
    // Create container for about page
    aboutContainer = gtk_fixed_new();
    if (overlay && aboutContainer) {
        gtk_overlay_add_overlay(GTK_OVERLAY(overlay), aboutContainer);
        gtk_widget_set_visible(aboutContainer, FALSE);
        
        setupBackButton();
        loadSystemInfo();
    }
}

void AboutManager::setupBackButton() {
    if (!aboutContainer) return;
    
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
    // Place the image at the top-left corner of the container
    if (backImage) {
        gtk_fixed_put(GTK_FIXED(fixedContainer), backImage, 0, 0);
    }
    // Place the label on top of the image, centered
    if (label) {
        gtk_fixed_put(GTK_FIXED(fixedContainer), label, 40, 20);
    }
    
    // Add fixed container to button
    gtk_button_set_child(GTK_BUTTON(backButton), fixedContainer);
    
    // Style the button to remove borders and background
    gtk_widget_set_size_request(backButton, 120, 60);
    gtk_widget_add_css_class(backButton, "flat");
    
    // Position the back button at top left of the about container
    gtk_fixed_put(GTK_FIXED(aboutContainer), backButton, 0, 0);
    
    // Create callback data using smart pointer for automatic cleanup
    std::unique_ptr<BackButtonCallbackData> callbackData = std::make_unique<BackButtonCallbackData>();
    callbackData->aboutManager = this;
    
    // Add click gesture
    GtkGesture* clickGesture = gtk_gesture_click_new();
    if (clickGesture) {
        gtk_widget_add_controller(backButton, GTK_EVENT_CONTROLLER(clickGesture));
        
        // Store the callback data in a way that GTK can manage it
        BackButtonCallbackData* dataPtr = callbackData.release(); // Release ownership to GTK
        
        g_signal_connect_data(clickGesture, "pressed", 
            G_CALLBACK(+[](GtkGestureClick* gesture, int n_press, double x, double y, gpointer user_data) {
                BackButtonCallbackData* data = static_cast<BackButtonCallbackData*>(user_data);
                if (data && data->aboutManager) {
                    // Hide about page
                    data->aboutManager->hide();
                    
                    // Show main menu
                    if (data->aboutManager->mainWindow) {
                        data->aboutManager->mainWindow->showMainMenu();
                    }
                }
            }), 
            dataPtr, 
            [](gpointer data, GClosure*) {
                // Cleanup callback data
                delete static_cast<BackButtonCallbackData*>(data);
            }, 
            G_CONNECT_DEFAULT);
    }
    
    // Add hover effects
    GtkEventController* motionController = gtk_event_controller_motion_new();
    if (motionController) {
        gtk_widget_add_controller(backButton, motionController);
    }
    
    // CSS for hover effects and label positioning
    GtkCssProvider* provider = gtk_css_provider_new();
    if (provider) {
        const char* css =
            "button { "
            "  background: transparent; "
            "  border: none; "
            "  color: white; "
            "  font-family: ElysiaOSNew12; "
            "  font-weight: bold; "
            "  font-size: 18px; "
            "} "
            "button:hover { "
            "  background: rgba(255, 255, 255, 0.1); "
            "  border-radius: 10px; "
            "  transition: all 0.3s ease; "
            "} "
            ".title-label { "
            "  color: white; "
            "  font-weight: bold; "
            "  font-family: ElysiaOSNew12; "
            "  font-size: 20px; "
            "} "
            ".info-label { "
            "  color: white; "
            "  font-weight: bold; "
            "  font-family: ElysiaOSNew12; "
            "  font-size: 20px; "
            "} "
            ".back-button-label { "
            "  color: white; "
            "  font-weight: bold; "
            "  font-family: ElysiaOSNew12; "
            "  font-size: 26px; "
            "}";

        gtk_css_provider_load_from_string(provider, css);
        gtk_style_context_add_provider_for_display(
            gdk_display_get_default(),
            GTK_STYLE_PROVIDER(provider),
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

        g_object_unref(provider);
    }
}

void AboutManager::loadSystemInfo() {
    if (!aboutContainer) return;
    
    // Load desktop image
    std::string desktopPath = getAssetPath("desktop.png");
    GtkWidget* desktopImage = gtk_picture_new_for_filename(desktopPath.c_str());
    
    if (!desktopImage) {
        desktopImage = gtk_drawing_area_new();
        gtk_widget_set_size_request(desktopImage, 120, 120);
    } else {
        gtk_widget_set_size_request(desktopImage, 120, 120);
        gtk_picture_set_can_shrink(GTK_PICTURE(desktopImage), TRUE);
        gtk_picture_set_content_fit(GTK_PICTURE(desktopImage), GTK_CONTENT_FIT_CONTAIN);
    }
    
    gtk_fixed_put(GTK_FIXED(aboutContainer), desktopImage, 740, 100);
    
    // Add title label
    GtkWidget* titleLabel = gtk_label_new(TR(TranslationKeys::SYSTEM_INFO));
    if (titleLabel) {
        gtk_widget_set_size_request(titleLabel, 300, 50);
        
        PangoAttrList* attrList = pango_attr_list_new();
        if (attrList) {
            PangoAttribute* sizeAttr = pango_attr_size_new(20 * PANGO_SCALE);
            PangoAttribute* weightAttr = pango_attr_weight_new(PANGO_WEIGHT_BOLD);
            
            if (sizeAttr) pango_attr_list_insert(attrList, sizeAttr);
            if (weightAttr) pango_attr_list_insert(attrList, weightAttr);
            gtk_label_set_attributes(GTK_LABEL(titleLabel), attrList);
            
            pango_attr_list_unref(attrList);
        }
        
        gtk_label_set_justify(GTK_LABEL(titleLabel), GTK_JUSTIFY_CENTER);
        gtk_widget_add_css_class(titleLabel, "title-label");
        
        gtk_fixed_put(GTK_FIXED(aboutContainer), titleLabel, 650, 255);
    }
    
    // Load fastfetch output
    std::vector<std::string> fastfetchLines = getFastfetchOutput();
    int y = 250;
    
    for (const std::string& line : fastfetchLines) {
        if (!line.empty() && y < 700) { // Prevent going off-screen
            GtkWidget* label = gtk_label_new(line.c_str());
            if (label) {
                gtk_widget_set_size_request(label, 500, 180);
                gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
                gtk_widget_add_css_class(label, "info-label");
                
                // Set font attributes
                PangoAttrList* labelAttrList = pango_attr_list_new();
                if (labelAttrList) {
                    PangoAttribute* fontSizeAttr = pango_attr_size_new(16 * PANGO_SCALE);
                    if (fontSizeAttr) {
                        pango_attr_list_insert(labelAttrList, fontSizeAttr);
                    }
                    gtk_label_set_attributes(GTK_LABEL(label), labelAttrList);
                    pango_attr_list_unref(labelAttrList);
                }
                
                gtk_fixed_put(GTK_FIXED(aboutContainer), label, 540, y);
                infoLabels.push_back(label);
                y += 35;
            }
        }
    }
}

std::vector<std::string> AboutManager::getFastfetchOutput() {
    std::vector<std::string> result;
    
    // Try to execute fastfetch
    const char* home = std::getenv("HOME");
    if (!home) {
        result.push_back("ERROR: HOME environment variable not set");
        return result;
    }
    
    std::string configPath = std::string(home) + "/.config/fastfetch/sys-config.jsonc";
    
    // First check if the config file exists
    if (access(configPath.c_str(), F_OK) != 0) {
        result.push_back("ERROR: Fastfetch config file not found at: " + configPath);
        return result;
    }
    
    // Using popen to execute fastfetch with timeout for safety
    std::string command = "timeout 3 fastfetch -c \"" + configPath + "\"";
    std::cout << "Executing command: " << command << std::endl;
    FILE* pipe = popen(command.c_str(), "r");
    
    if (!pipe) {
        result.push_back("ERROR: Could not execute fastfetch");
        return result;
    }
    
    char buffer[512];
    int lineCount = 0;
    while (fgets(buffer, sizeof buffer, pipe) != nullptr && lineCount < 25) {
        std::string line(buffer);
        // Remove trailing newline and carriage return
        while (!line.empty() && (line.back() == '\n' || line.back() == '\r')) {
            line.pop_back();
        }
        // Only add non-empty lines
        if (!line.empty()) {
            result.push_back(line);
            lineCount++;
        }
    }
    
    int status = pclose(pipe);
    std::cout << "Fastfetch command exited with status: " << status << std::endl;
    
    if (result.empty()) {
        result.push_back("No system information available");
    }
    
    return result;
}

std::string AboutManager::getAssetPath(const std::string& filename) {
    const char* home = std::getenv("HOME");
    if (!home) {
        std::cerr << "HOME environment variable not set" << std::endl;
        return filename;
    }
    return std::string(home) + "/.config/Elysia/assets/assets/" + filename;
}