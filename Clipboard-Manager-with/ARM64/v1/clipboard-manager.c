// Created By J~Net (c) 2024
//
// gcc clipboard-manager.c -o clipboard-manager `pkg-config --cflags --libs gtk+-3.0` /usr/lib/aarch64-linux-gnu/libX11.so
//
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HISTORY_FILE_PATH "~/.clipboard_history"

typedef struct {
    GtkWidget *list_box;
    GList *clipboard_history;
} AppWidgets;

// Add clipboard content to history
void add_to_history(AppWidgets *app, const char *content) {
    if (g_list_find_custom(app->clipboard_history, content, (GCompareFunc)strcmp) == NULL) {
        app->clipboard_history=g_list_prepend(app->clipboard_history, g_strdup(content));
        GtkWidget *row=gtk_label_new(content);
        gtk_list_box_insert(GTK_LIST_BOX(app->list_box), row, -1);
        gtk_widget_show(row);
    }
}

// Poll clipboard for new content
gboolean poll_clipboard(gpointer data) {
    AppWidgets *app=(AppWidgets *)data;
    char buffer[1024];
    FILE *pipe=popen("xclip -o -selection clipboard", "r");
    if (pipe && fgets(buffer, sizeof(buffer), pipe)) {
        buffer[strcspn(buffer, "\n")]=0; // Remove newline
        add_to_history(app, buffer);
    }
    if (pipe) pclose(pipe);
    return TRUE; // Keep polling
}

// Copy selected item back to clipboard
void copy_selected(GtkButton *button, AppWidgets *app) {
    GtkListBoxRow *row=gtk_list_box_get_selected_row(GTK_LIST_BOX(app->list_box));
    if (row) {
        const char *content=gtk_label_get_text(GTK_LABEL(gtk_bin_get_child(GTK_BIN(row))));
        char command[1050];
        snprintf(command, sizeof(command), "echo '%s' | xclip -selection clipboard", content);
        system(command);
    }
}

// Show the "About" dialog
void show_about(GtkButton *button, gpointer data) {
    GtkWidget *dialog=gtk_message_dialog_new(GTK_WINDOW(data),
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_CLOSE,
                                               "Clipboard Manager\n\nCreated By: J~Net\n(c) 2024");
    gtk_window_set_title(GTK_WINDOW(dialog), "About");
    g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL);
    gtk_widget_show(dialog);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Create app widgets structure
    AppWidgets app={0};
    app.clipboard_history=NULL;

    // Main window
    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Clipboard Manager");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Main container
    GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // List box for history
    app.list_box=gtk_list_box_new();
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(app.list_box), GTK_SELECTION_SINGLE);
    GtkWidget *scroll=gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scroll), app.list_box);
    gtk_box_pack_start(GTK_BOX(vbox), scroll, TRUE, TRUE, 0);

    // Buttons
    GtkWidget *button_box=gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    GtkWidget *copy_button=gtk_button_new_with_label("Copy");
    GtkWidget *about_button=gtk_button_new_with_label("About");
    gtk_container_add(GTK_CONTAINER(button_box), copy_button);
    gtk_container_add(GTK_CONTAINER(button_box), about_button);
    gtk_box_pack_start(GTK_BOX(vbox), button_box, FALSE, FALSE, 0);

    // Signals
    g_signal_connect(copy_button, "clicked", G_CALLBACK(copy_selected), &app);
    g_signal_connect(about_button, "clicked", G_CALLBACK(show_about), window);

    // Start polling clipboard
    g_timeout_add(1000, poll_clipboard, &app);

    // Show everything
    gtk_widget_show_all(window);
    gtk_main();

    // Cleanup
    g_list_free_full(app.clipboard_history, g_free);
    return 0;
}

