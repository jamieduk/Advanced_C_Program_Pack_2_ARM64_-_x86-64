// Made By Jay @ J~Net 2024
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

// Global variables for backup settings
char source_dir[256]="";
char dest_dir[256]="";
int is_incremental=1; // Default to incremental backups

// Function prototypes
void select_source(GtkWidget *widget, gpointer data);
void select_dest(GtkWidget *widget, gpointer data);
void start_backup(GtkWidget *widget, gpointer data);
void toggle_backup_type(GtkWidget *widget, gpointer data);
void show_about(GtkWidget *widget, gpointer data);
void schedule_backup(GtkWidget *widget, gpointer data);

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Main window
    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Simple Backup Tool");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Layout container
    GtkWidget *box=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5); // Vertical layout with minimal spacing
    gtk_container_set_border_width(GTK_CONTAINER(window), 5);  // Reduce border width
    gtk_container_add(GTK_CONTAINER(window), box);

    // Source directory button
    GtkWidget *btn_source=gtk_button_new_with_label("Select Source Directory");
    g_signal_connect(btn_source, "clicked", G_CALLBACK(select_source), NULL);
    gtk_box_pack_start(GTK_BOX(box), btn_source, FALSE, FALSE, 0);

    // Destination directory button
    GtkWidget *btn_dest=gtk_button_new_with_label("Select Destination Directory");
    g_signal_connect(btn_dest, "clicked", G_CALLBACK(select_dest), NULL);
    gtk_box_pack_start(GTK_BOX(box), btn_dest, FALSE, FALSE, 0);

    // Backup type toggle
    GtkWidget *btn_toggle=gtk_button_new_with_label("Toggle Backup Type (Incremental)");
    g_signal_connect(btn_toggle, "clicked", G_CALLBACK(toggle_backup_type), btn_toggle);
    gtk_box_pack_start(GTK_BOX(box), btn_toggle, FALSE, FALSE, 0);

    // Start backup button
    GtkWidget *btn_backup=gtk_button_new_with_label("Start Backup");
    g_signal_connect(btn_backup, "clicked", G_CALLBACK(start_backup), NULL);
    gtk_box_pack_start(GTK_BOX(box), btn_backup, FALSE, FALSE, 0);

    // Schedule backup button
    GtkWidget *btn_schedule=gtk_button_new_with_label("Schedule Backup");
    g_signal_connect(btn_schedule, "clicked", G_CALLBACK(schedule_backup), NULL);
    gtk_box_pack_start(GTK_BOX(box), btn_schedule, FALSE, FALSE, 0);

    // About button
    GtkWidget *btn_about=gtk_button_new_with_label("About");
    g_signal_connect(btn_about, "clicked", G_CALLBACK(show_about), NULL);
    gtk_box_pack_start(GTK_BOX(box), btn_about, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}

// Function to select source directory
void select_source(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog=gtk_file_chooser_dialog_new("Select Source Directory", NULL,
                                                    GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                                    "Cancel", GTK_RESPONSE_CANCEL,
                                                    "Select", GTK_RESPONSE_ACCEPT,
                                                    NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        GtkFileChooser *chooser=GTK_FILE_CHOOSER(dialog);
        strncpy(source_dir, gtk_file_chooser_get_filename(chooser), sizeof(source_dir) - 1);
    }
    gtk_widget_destroy(dialog);
}

// Function to select destination directory
void select_dest(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog=gtk_file_chooser_dialog_new("Select Destination Directory", NULL,
                                                    GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                                    "Cancel", GTK_RESPONSE_CANCEL,
                                                    "Select", GTK_RESPONSE_ACCEPT,
                                                    NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        GtkFileChooser *chooser=GTK_FILE_CHOOSER(dialog);
        strncpy(dest_dir, gtk_file_chooser_get_filename(chooser), sizeof(dest_dir) - 1);
    }
    gtk_widget_destroy(dialog);
}

// Function to toggle backup type
void toggle_backup_type(GtkWidget *widget, gpointer data) {
    is_incremental=!is_incremental;
    const char *label=is_incremental ? "Toggle Backup Type (Incremental)" : "Toggle Backup Type (Full)";
    gtk_button_set_label(GTK_BUTTON(widget), label);
}

// Function to start backup
void start_backup(GtkWidget *widget, gpointer data) {
    if (strlen(source_dir) == 0 || strlen(dest_dir) == 0) {
        GtkWidget *dialog=gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR,
                                                   GTK_BUTTONS_OK, "Please select both source and destination directories.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    char command[1024];
    snprintf(command, sizeof(command), "rsync -av%s %s/ %s/",
             is_incremental ? "" : " --delete",
             source_dir, dest_dir);

    int result=system(command);
    const char *message=(result == 0) ? "Backup completed successfully." : "Backup failed.";
    GtkWidget *dialog=gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK, "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Function to schedule backup
void schedule_backup(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog=gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK,
                                               "Scheduling feature is not yet implemented.");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Function to show About dialog
void show_about(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog=gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK,
                                               "Simple Backup Tool\nAuthor: Jay Mee\nVersion: 1.0");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

