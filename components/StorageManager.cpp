#include "StorageManager.h"
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

// Forward declaration of callback data
struct StorageBackButtonCallbackData {
    StorageManager* storageManager;
};

StorageManager::StorageManager(MainWindow* mainWindow, GtkWidget* parentWindow, GtkWidget* overlay) 
    : mainWindow(mainWindow), parentWindow(parentWindow), overlay(overlay), 
      storageContainer(nullptr), backButton(nullptr), drivesContainer(nullptr) {
    setupUI();
}

StorageManager::~StorageManager() {
    // Clear widget tracking
    driveWidgets.clear();
    
    // All GTK widgets are managed by GTK and will be cleaned up automatically
}

void StorageManager::show() {
    if (storageContainer) {
        gtk_widget_set_visible(storageContainer, TRUE);
        // Switch to the storage background
        if (mainWindow) {
            mainWindow->switchToBackground("background4.png");
        }
        updateDisks();
    }
}

void StorageManager::hide() {
    if (storageContainer) {
        gtk_widget_set_visible(storageContainer, FALSE);
    }
}

void StorageManager::setupUI() {
    // Create container for storage page
    storageContainer = gtk_fixed_new();
    if (overlay && storageContainer) {
        gtk_overlay_add_overlay(GTK_OVERLAY(overlay), storageContainer);
        gtk_widget_set_visible(storageContainer, FALSE);
        
        initUI();
        setupBackButton();
    }
}

void StorageManager::initUI() {
    if (!storageContainer) return;
    
    // Add title label
    GtkWidget* titleLabel = gtk_label_new(TR(TranslationKeys::STORAGE_TITLE));
    if (titleLabel) {
        gtk_widget_set_size_request(titleLabel, 300, 50);
        
        PangoAttrList* attrList = pango_attr_list_new();
        if (attrList) {
            PangoAttribute* sizeAttr = pango_attr_size_new(22 * PANGO_SCALE);
            PangoAttribute* weightAttr = pango_attr_weight_new(PANGO_WEIGHT_BOLD);
            
            if (sizeAttr) pango_attr_list_insert(attrList, sizeAttr);
            if (weightAttr) pango_attr_list_insert(attrList, weightAttr);
            gtk_label_set_attributes(GTK_LABEL(titleLabel), attrList);
            
            pango_attr_list_unref(attrList);
        }
        
        gtk_label_set_justify(GTK_LABEL(titleLabel), GTK_JUSTIFY_CENTER);
        gtk_widget_add_css_class(titleLabel, "title-label");
        
        gtk_fixed_put(GTK_FIXED(storageContainer), titleLabel, 650, 170);
    }
    
    // Create scrollable container for drives
    GtkWidget* scrolledWindow = gtk_scrolled_window_new();
    if (scrolledWindow) {
        gtk_widget_set_size_request(scrolledWindow, 620, 520);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow), 
                                      GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
        
        // Create container for drives
        drivesContainer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
        if (drivesContainer) {
            gtk_widget_set_margin_start(drivesContainer, 12);
            gtk_widget_set_margin_end(drivesContainer, 12);
            gtk_widget_set_margin_top(drivesContainer, 12);
            gtk_widget_set_margin_bottom(drivesContainer, 12);
            
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolledWindow), drivesContainer);
            gtk_fixed_put(GTK_FIXED(storageContainer), scrolledWindow, 490, 230);
        }
    }
    
    // CSS for storage page with ElysiaOS aesthetic
    GtkCssProvider* provider = gtk_css_provider_new();
    if (provider) {
        const char* css =
            "window { "
            "  background: rgba(255, 255, 255, 0.3); "
            "  border-radius: 21px; "
            "  border: 1px solid #c0c0c0; "
            "  box-shadow: 0 8px 25px rgba(0, 0, 0, 0.25), 0 0 1px rgba(0, 0, 0, 0.8); "
            "} "
            ".title-label { "
            "  font-family: ElysiaOSNew12; "
            "  font-size: 22px; "
            "  font-weight: 600; "
            "  color: white; "
            "  margin: 8px 0px 12px 0px; "
            "  text-shadow: 0 1px 0 rgba(255, 255, 255, 0.8); "
            "} "
            ".drive-frame { "
            "  background: linear-gradient(145deg, rgba(255, 255, 255, 0.2), rgba(255, 255, 255, 0.1)); "
            "  border: 1px solid rgba(192, 192, 192, 0.6); "
            "  border-radius: 14px; "
            "  padding: 10px; "
            "  margin: 4px 0; "
            "  box-shadow: 0 1px 3px rgba(0, 0, 0, 0.15); "
            "} "
            ".drive-label { "
            "  color: white; "
            "  font-family: ElysiaOSNew12; "
            "  font-size: 14px; "
            "  font-weight: 600; "
            "  text-shadow: 0 1px 0 rgba(255, 255, 255, 0.7); "
            "} "
            ".size-label { "
            "  color: white; "
            "  font-family: ElysiaOSNew12; "
            "  font-size: 12px; "
            "  text-shadow: 0 1px 0 rgba(255, 255, 255, 0.7); "
            "} "
            ".progress-bar { "
            "  background-color: rgba(255, 255, 255, 0.2); "
            "  border: 1px solid rgba(192, 192, 192, 0.6); "
            "  border-radius: 8px; "
            "  min-height: 16px; "
            "  font-family: ElysiaOSNew12; "
            "  font-size: 11px; "
            "  color: white; "
            "  text-shadow: 0 1px 0 rgba(255, 255, 255, 0.7); "
            "} "
            ".progress-bar progress { "
            "  background: linear-gradient(90deg, #e5a7c6 0%, #edcee3 100%); "
            "  border-radius: 6px; "
            "  margin: 1px; "
            "} "
            ".back-button-label { "
            "  color: white; "
            "  font-weight: bold; "
            "  font-family: ElysiaOSNew12; "
            "  font-size: 22px; "
            "  text-shadow: 0 1px 0 rgba(255, 255, 255, 0.8); "
            "} "
            "button { "
            "  background: transparent; "
            "  border: none; "
            "  color: white; "
            "  font-family: ElysiaOSNew12; "
            "  font-weight: bold; "
            "  font-size: 15px; "
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

void StorageManager::setupBackButton() {
    if (!storageContainer) return;
    
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
    
    // Position the back button at top left of the storage container
    gtk_fixed_put(GTK_FIXED(storageContainer), backButton, 0, 0);
    
    // Create callback data using smart pointer for automatic cleanup
    std::unique_ptr<StorageBackButtonCallbackData> callbackData = std::make_unique<StorageBackButtonCallbackData>();
    callbackData->storageManager = this;
    
    // Add click gesture
    GtkGesture* clickGesture = gtk_gesture_click_new();
    if (clickGesture) {
        gtk_widget_add_controller(backButton, GTK_EVENT_CONTROLLER(clickGesture));
        
        // Store the callback data in a way that GTK can manage it
        StorageBackButtonCallbackData* dataPtr = callbackData.release(); // Release ownership to GTK
        
        g_signal_connect_data(clickGesture, "pressed", 
            G_CALLBACK(+[](GtkGestureClick* gesture, int n_press, double x, double y, gpointer user_data) {
                StorageBackButtonCallbackData* data = static_cast<StorageBackButtonCallbackData*>(user_data);
                if (data && data->storageManager) {
                    // Hide storage page
                    data->storageManager->hide();
                    
                    // Show main menu
                    if (data->storageManager->mainWindow) {
                        data->storageManager->mainWindow->showMainMenu();
                    }
                }
            }), 
            dataPtr, 
            [](gpointer data, GClosure*) {
                // Cleanup callback data
                delete static_cast<StorageBackButtonCallbackData*>(data);
            }, 
            G_CONNECT_DEFAULT);
    }
    
    // Add hover effects
    GtkEventController* motionController = gtk_event_controller_motion_new();
    if (motionController) {
        gtk_widget_add_controller(backButton, motionController);
    }
}

void StorageManager::updateDisks() {
    // Clear previous widgets
    for (GtkWidget* widget : driveWidgets) {
        gtk_box_remove(GTK_BOX(drivesContainer), widget);
    }
    driveWidgets.clear();
    
    // Get disk info
    std::vector<DriveInfo> drives = getDiskInfo();
    
    for (const DriveInfo& drive : drives) {
        // Create frame for each drive
        GtkWidget* driveFrame = gtk_frame_new(nullptr);
        gtk_widget_add_css_class(driveFrame, "drive-frame");
        gtk_widget_set_size_request(driveFrame, 580, 65);
        
        GtkWidget* driveBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
        gtk_widget_set_margin_start(driveBox, 12);
        gtk_widget_set_margin_end(driveBox, 12);
        gtk_widget_set_margin_top(driveBox, 10);
        gtk_widget_set_margin_bottom(driveBox, 10);
        
        // Icon
        std::string iconPath = getAssetPath("drive.png");
        GtkWidget* iconImage = gtk_picture_new_for_filename(iconPath.c_str());
        if (!iconImage) {
            iconImage = gtk_label_new("HD");
            gtk_widget_add_css_class(iconImage, "drive-label");
        } else {
            gtk_widget_set_size_request(iconImage, 35, 35);
            gtk_picture_set_can_shrink(GTK_PICTURE(iconImage), TRUE);
            gtk_picture_set_content_fit(GTK_PICTURE(iconImage), GTK_CONTENT_FIT_CONTAIN);
        }
        
        // Info container
        GtkWidget* infoBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
        
        // Drive label
        std::string driveText = drive.device + " (" + drive.mountpoint + ")";
        GtkWidget* driveLabel = gtk_label_new(driveText.c_str());
        gtk_widget_add_css_class(driveLabel, "drive-label");
        gtk_label_set_xalign(GTK_LABEL(driveLabel), 0.0);
        
        // Size label
        std::string sizeText = bytesToGB(drive.used) + " / " + bytesToGB(drive.total);
        GtkWidget* sizeLabel = gtk_label_new(sizeText.c_str());
        gtk_widget_add_css_class(sizeLabel, "size-label");
        gtk_label_set_xalign(GTK_LABEL(sizeLabel), 0.0);
        
        // Progress bar
        GtkWidget* progressBar = gtk_progress_bar_new();
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), drive.percent / 100.0);
        gtk_widget_add_css_class(progressBar, "progress-bar");
        gtk_widget_set_size_request(progressBar, 320, 16);
        
        std::string freeText = bytesToGB(drive.free) + " free";
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressBar), freeText.c_str());
        gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(progressBar), TRUE);
        
        // Pack widgets
        gtk_box_append(GTK_BOX(infoBox), driveLabel);
        gtk_box_append(GTK_BOX(infoBox), sizeLabel);
        gtk_box_append(GTK_BOX(infoBox), progressBar);
        
        gtk_box_append(GTK_BOX(driveBox), iconImage);
        gtk_box_append(GTK_BOX(driveBox), infoBox);
        
        gtk_frame_set_child(GTK_FRAME(driveFrame), driveBox);
        
        gtk_box_append(GTK_BOX(drivesContainer), driveFrame);
        driveWidgets.push_back(driveFrame);
    }
}

std::vector<DriveInfo> StorageManager::getDiskInfo() {
    std::vector<DriveInfo> drives;
    
    // Using popen to execute df command
    std::string command = "df -T -B1";
    FILE* pipe = popen(command.c_str(), "r");
    
    if (!pipe) {
        std::cerr << "ERROR: Could not execute df command" << std::endl;
        return drives;
    }
    
    char buffer[1024];
    std::vector<std::string> lines;
    
    while (fgets(buffer, sizeof buffer, pipe) != nullptr) {
        std::string line(buffer);
        // Remove trailing newline and carriage return
        while (!line.empty() && (line.back() == '\n' || line.back() == '\r')) {
            line.pop_back();
        }
        lines.push_back(line);
    }
    
    pclose(pipe);
    
    if (lines.size() < 2) return drives;
    
    // Valid filesystem types
    std::vector<std::string> validTypes = {"ext4", "btrfs", "xfs", "vfat", "ntfs", "fuseblk", "exfat"};
    
    // Skip header line and process each line
    for (size_t i = 1; i < lines.size(); ++i) {
        std::string line = lines[i];
        if (line.empty()) continue;
        
        // Parse the line using strtok
        char* lineCopy = strdup(line.c_str());
        char* token = strtok(lineCopy, " \t");
        
        if (!token) {
            free(lineCopy);
            continue;
        }
        
        DriveInfo drive;
        drive.device = std::string(token);
        
        token = strtok(nullptr, " \t");
        if (!token) {
            free(lineCopy);
            continue;
        }
        
        drive.fstype = std::string(token);
        
        // Check if filesystem type is valid
        bool validType = false;
        for (const std::string& type : validTypes) {
            if (drive.fstype == type) {
                validType = true;
                break;
            }
        }
        
        if (!validType) {
            free(lineCopy);
            continue;
        }
        
        token = strtok(nullptr, " \t");
        if (!token) {
            free(lineCopy);
            continue;
        }
        drive.total = std::stoll(std::string(token));
        
        token = strtok(nullptr, " \t");
        if (!token) {
            free(lineCopy);
            continue;
        }
        drive.used = std::stoll(std::string(token));
        
        token = strtok(nullptr, " \t");
        if (!token) {
            free(lineCopy);
            continue;
        }
        drive.free = std::stoll(std::string(token));
        
        // Skip usage percentage
        token = strtok(nullptr, " \t");
        if (!token) {
            free(lineCopy);
            continue;
        }
        
        token = strtok(nullptr, " \t");
        if (!token) {
            free(lineCopy);
            continue;
        }
        drive.mountpoint = std::string(token);
        
        // Calculate percentage
        if (drive.total > 0) {
            drive.percent = (int)((drive.used * 100.0) / drive.total);
        } else {
            drive.percent = 0;
        }
        
        drives.push_back(drive);
        free(lineCopy);
    }
    
    return drives;
}

std::string StorageManager::bytesToGB(long long bytes) {
    double gb = bytes / (1024.0 * 1024 * 1024);
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.1f GB", gb);
    return std::string(buffer);
}

std::string StorageManager::getAssetPath(const std::string& filename) {
    const char* home = std::getenv("HOME");
    if (!home) {
        std::cerr << "HOME environment variable not set" << std::endl;
        return filename;
    }
    return std::string(home) + "/.config/Elysia/assets/assets/" + filename;
}