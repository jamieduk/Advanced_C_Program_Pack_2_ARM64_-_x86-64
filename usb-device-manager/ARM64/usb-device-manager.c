// Made By Jay @ J~Net 2024
//
// gcc usb-device-manager.c -o usb-device-manager `pkg-config --cflags --libs gtk+-3.0` -ludev
//
//
#include <gtk/gtk.h>
#include <libudev.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function prototypes
void list_usb_devices(GtkWidget *list_view);
void safely_eject_device(GtkWidget *button, gpointer data);
void mount_unmount_device(GtkWidget *button, gpointer data);
void format_usb_device(GtkWidget *button, gpointer data);
void show_about_dialog(GtkWidget *widget, gpointer data);
void cli_mode();

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "--cli") == 0) {
        cli_mode();
        return 0;
    }

    gtk_init(&argc, &argv);

    // Main window
    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "USB Device Manager");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Layout
    GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // USB device list
    GtkWidget *list_view=gtk_tree_view_new();
    gtk_box_pack_start(GTK_BOX(vbox), list_view, TRUE, TRUE, 0);
    list_usb_devices(list_view);

    // Buttons
    GtkWidget *button_box=gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), button_box, FALSE, FALSE, 0);

    GtkWidget *eject_button=gtk_button_new_with_label("Safely Eject");
    g_signal_connect(eject_button, "clicked", G_CALLBACK(safely_eject_device), NULL);
    gtk_container_add(GTK_CONTAINER(button_box), eject_button);

    GtkWidget *mount_button=gtk_button_new_with_label("Mount/Unmount");
    g_signal_connect(mount_button, "clicked", G_CALLBACK(mount_unmount_device), NULL);
    gtk_container_add(GTK_CONTAINER(button_box), mount_button);

    GtkWidget *format_button=gtk_button_new_with_label("Format");
    g_signal_connect(format_button, "clicked", G_CALLBACK(format_usb_device), NULL);
    gtk_container_add(GTK_CONTAINER(button_box), format_button);

    GtkWidget *about_button=gtk_button_new_with_label("About");
    g_signal_connect(about_button, "clicked", G_CALLBACK(show_about_dialog), NULL);
    gtk_container_add(GTK_CONTAINER(button_box), about_button);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

void list_usb_devices(GtkWidget *list_view) {
    GtkListStore *store=gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(list_view), GTK_TREE_MODEL(store));
    g_object_unref(store);

    GtkCellRenderer *renderer=gtk_cell_renderer_text_new();
    GtkTreeViewColumn *column=gtk_tree_view_column_new_with_attributes("Vendor ID", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(list_view), column);

    column=gtk_tree_view_column_new_with_attributes("Product ID", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(list_view), column);

    column=gtk_tree_view_column_new_with_attributes("File System", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(list_view), column);

    struct udev *udev=udev_new();
    if (!udev) {
        g_printerr("Failed to initialize udev\n");
        return;
    }

    struct udev_enumerate *enumerate=udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "usb");
    udev_enumerate_scan_devices(enumerate);

    struct udev_list_entry *devices=udev_enumerate_get_list_entry(enumerate);
    struct udev_list_entry *entry;

    gtk_list_store_clear(store);

    udev_list_entry_foreach(entry, devices) {
        const char *path=udev_list_entry_get_name(entry);
        struct udev_device *device=udev_device_new_from_syspath(udev, path);

        const char *vendor=udev_device_get_sysattr_value(device, "idVendor");
        const char *product=udev_device_get_sysattr_value(device, "idProduct");

        // Try to fetch file system type for block devices
        const char *fs_type=NULL;
        struct udev_device *block=udev_device_get_parent_with_subsystem_devtype(device, "block", "disk");
        if (block) {
            fs_type=udev_device_get_sysattr_value(block, "devtype");
        }

        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                           0, vendor ? vendor : "Unknown",
                           1, product ? product : "Unknown",
                           2, fs_type ? fs_type : "N/A",
                           -1);

        udev_device_unref(device);
    }

    udev_enumerate_unref(enumerate);
    udev_unref(udev);
}


void safely_eject_device(GtkWidget *button, gpointer data) {
    g_print("Safely ejecting device...\n");
    // Implement safe ejection logic here
}

void mount_unmount_device(GtkWidget *button, gpointer data) {
    g_print("Mounting/Unmounting device...\n");
    // Implement mount/unmount logic here
}

void format_usb_device(GtkWidget *button, gpointer data) {
    g_print("Formatting device...\n");
    // Implement formatting logic here
}

void show_about_dialog(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog=gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                               "USB Device Manager\nAuthor: Jay Mee");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void cli_mode() {
    printf("USB Device Manager CLI Mode\n");
    printf("1. List USB Devices\n");
    printf("2. Safely Eject a Device\n");
    printf("3. Mount/Unmount a Device\n");
    printf("4. Format a USB Device\n");
    printf("5. Exit\n");

    while (1) {
        printf("Choose an option: ");
        int choice;
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Listing USB devices...\n");
                // Implement USB device listing here
                break;
            case 2:
                printf("Enter device to safely eject: ");
                // Implement safe eject logic here
                break;
            case 3:
                printf("Enter device to mount/unmount: ");
                // Implement mount/unmount logic here
                break;
            case 4:
                printf("Enter device to format: ");
                // Implement format logic here
                break;
            case 5:
                printf("Exiting...\n");
                return;
            default:
                printf("Invalid choice. Try again.\n");
        }
    }
}

