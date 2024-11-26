//
// Audio Cutter and Converter
// Made By Jay @ J~Net 2024
// Audio Cutter and Converter
// Made By Jay @ J~Net 2024
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILES 100

static char *input_files[MAX_FILES];
static int file_count=0;
static char *output_folder=NULL;
static char *output_format="wav";
static int start_time=0;
static int end_time=0;

// Callback to select multiple audio files
void on_file_select(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    dialog=gtk_file_chooser_dialog_new("Select Audio Files", GTK_WINDOW(data),
                                       GTK_FILE_CHOOSER_ACTION_OPEN,
                                       "_Cancel", GTK_RESPONSE_CANCEL,
                                       "_Open", GTK_RESPONSE_ACCEPT,
                                       NULL);
    gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog), TRUE);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        GSList *files=gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(dialog));
        for (GSList *iter=files; iter != NULL && file_count < MAX_FILES; iter=iter->next) {
            input_files[file_count]=g_strdup((char *)iter->data);
            g_print("Added file: %s\n", input_files[file_count]);
            file_count++;
        }
        g_slist_free_full(files, g_free);
    }
    gtk_widget_destroy(dialog);
}

// Callback to set the output folder
void on_output_folder_select(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    dialog=gtk_file_chooser_dialog_new("Select Output Folder", GTK_WINDOW(data),
                                       GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                       "_Cancel", GTK_RESPONSE_CANCEL,
                                       "_Select", GTK_RESPONSE_ACCEPT,
                                       NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        output_folder=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        g_print("Selected output folder: %s\n", output_folder);
    }
    gtk_widget_destroy(dialog);
}

// Callback to update the output format
void on_format_changed(GtkWidget *widget, gpointer data) {
    output_format=(char *)gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
    g_print("Selected output format: %s\n", output_format);
}

// Callback for start time entry
void on_start_time_changed(GtkWidget *widget, gpointer data) {
    start_time=atoi(gtk_entry_get_text(GTK_ENTRY(widget)));
}

// Callback for end time entry
void on_end_time_changed(GtkWidget *widget, gpointer data) {
    end_time=atoi(gtk_entry_get_text(GTK_ENTRY(widget)));
}

// Callback to start conversion
void on_convert_clicked(GtkWidget *widget, gpointer data) {
    if (file_count == 0) {
        g_print("No files to process.\n");
        return;
    }
    if (!output_folder) {
        g_print("No output folder selected.\n");
        return;
    }

    for (int i=0; i < file_count; i++) {
        char output_file[512];
        char command[1024];
        snprintf(output_file, sizeof(output_file), "%s/output_%d.%s", output_folder, i + 1, output_format);
        snprintf(command, sizeof(command), "ffmpeg -i \"%s\" -ss %d -to %d \"%s\"",
                 input_files[i], start_time, end_time, output_file);
        g_print("Executing: %s\n", command);
        int result=system(command);
        if (result == 0) {
            g_print("Conversion complete: %s\n", output_file);
        } else {
            g_print("Error processing file: %s\n", input_files[i]);
        }
    }
}

// Callback for "About" button
void on_about_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    dialog=gtk_message_dialog_new(GTK_WINDOW(data),
                                   GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                   GTK_MESSAGE_INFO,
                                   GTK_BUTTONS_OK,
                                   "Audio Cutter and Converter\nAuthor: Jay Mee\nJ~Net 2024");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Main function
int main(int argc, char *argv[]) {
    GtkWidget *window, *vbox;
    GtkWidget *file_button, *output_button, *convert_button, *about_button;
    GtkWidget *start_time_entry, *end_time_entry;
    GtkWidget *format_combo;

    gtk_init(&argc, &argv);

    // Create main window
    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Audio Cutter and Converter");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Vertical box layout
    vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // File selection button
    file_button=gtk_button_new_with_label("Select Audio Files");
    g_signal_connect(file_button, "clicked", G_CALLBACK(on_file_select), window);
    gtk_box_pack_start(GTK_BOX(vbox), file_button, FALSE, FALSE, 0);

    // Output folder selection button
    output_button=gtk_button_new_with_label("Select Output Folder");
    g_signal_connect(output_button, "clicked", G_CALLBACK(on_output_folder_select), window);
    gtk_box_pack_start(GTK_BOX(vbox), output_button, FALSE, FALSE, 0);

    // Start time entry
    start_time_entry=gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(start_time_entry), "Start Time (seconds)");
    g_signal_connect(start_time_entry, "changed", G_CALLBACK(on_start_time_changed), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), start_time_entry, FALSE, FALSE, 0);

    // End time entry
    end_time_entry=gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(end_time_entry), "End Time (seconds)");
    g_signal_connect(end_time_entry, "changed", G_CALLBACK(on_end_time_changed), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), end_time_entry, FALSE, FALSE, 0);

    // Format selection dropdown
    format_combo=gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(format_combo), "wav");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(format_combo), "mp3");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(format_combo), "flac");
    gtk_combo_box_set_active(GTK_COMBO_BOX(format_combo), 0);
    g_signal_connect(format_combo, "changed", G_CALLBACK(on_format_changed), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), format_combo, FALSE, FALSE, 0);

    // Convert button
    convert_button=gtk_button_new_with_label("Start Conversion");
    g_signal_connect(convert_button, "clicked", G_CALLBACK(on_convert_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), convert_button, FALSE, FALSE, 0);

    // About button
    about_button=gtk_button_new_with_label("About");
    g_signal_connect(about_button, "clicked", G_CALLBACK(on_about_clicked), window);
    gtk_box_pack_start(GTK_BOX(vbox), about_button, FALSE, FALSE, 0);

    // Show everything
    gtk_widget_show_all(window);

    gtk_main();

    // Cleanup
    for (int i=0; i < file_count; i++) {
        if (input_files[i]) g_free(input_files[i]);
    }
    if (output_folder) g_free(output_folder);

    return 0;
}

