// Made By Jay @ J~Net 2024
//
//
//
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

// Function prototypes
void on_about_clicked(GtkWidget *widget, gpointer data);
void on_screenshot_clicked(GtkWidget *widget, gpointer data);
void on_quit_clicked(GtkWidget *widget, gpointer data);

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Main window
    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Screenshot Editor");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Main layout
    GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Toolbar
    GtkWidget *toolbar=gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);

    // Screenshot button
    GtkWidget *screenshot_button=gtk_button_new_with_label("Take Screenshot");
    g_signal_connect(screenshot_button, "clicked", G_CALLBACK(on_screenshot_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(toolbar), screenshot_button, TRUE, TRUE, 0);

    // About button
    GtkWidget *about_button=gtk_button_new_with_label("About");
    g_signal_connect(about_button, "clicked", G_CALLBACK(on_about_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(toolbar), about_button, TRUE, TRUE, 0);

    // Quit button
    GtkWidget *quit_button=gtk_button_new_with_label("Quit");
    g_signal_connect(quit_button, "clicked", G_CALLBACK(on_quit_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(toolbar), quit_button, TRUE, TRUE, 0);

    // Annotation area placeholder
    GtkWidget *annotation_label=gtk_label_new("Annotate your screenshots here.");
    gtk_box_pack_start(GTK_BOX(vbox), annotation_label, TRUE, TRUE, 0);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

// Function to handle the About button
void on_about_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog=gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Author: Jay Mee\nScreenshot Editor\n");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Function to take a screenshot (placeholder for now)
void on_screenshot_clicked(GtkWidget *widget, gpointer data) {
    system("gnome-screenshot -i"); // Open GNOME screenshot tool (modify as needed)
}

// Function to handle Quit button
void on_quit_clicked(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

