// Made By Jay @ J~Net 2024
//
// gcc -o search-gui search-gui.c `pkg-config --cflags --libs gtk+-3.0`
//
//
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>

static GtkWidget *name_entry;
static GtkWidget *type_entry;
static GtkWidget *exclude_entry;
static GtkWidget *recursive_check;
static GtkWidget *case_check;
static GtkWidget *log_check;
static GtkTreeView *result_view;
static GtkTreeView *search_dirs_view;
static GtkListStore *result_store;
static GtkListStore *dir_store;

void search_directory(const char *dir_path, const char *file_name, const char *file_type, const char *exclude, int recursive, int case_insensitive);
int match_file(const char *file_name, const char *pattern, int case_insensitive);
int should_exclude(const char *name, const char *exclude, int case_insensitive);
void to_lowercase(char *str);
void open_containing_folder(const char *file_path);
void start_search(GtkWidget *widget, gpointer data);
void stop_search();
void clear_results();
gboolean on_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data);
void on_context_menu_action(GtkWidget *menu_item, gpointer action);
void add_search_dir(GtkWidget *widget, gpointer data);
void remove_search_dir(GtkWidget *widget, gpointer data);

void start_search(GtkWidget *widget, gpointer data) {
    // Clear previous results
    clear_results();

    GtkTreeIter iter;
    gchar *dir_path;
    GtkTreeModel *model=gtk_tree_view_get_model(search_dirs_view);

    if (gtk_tree_model_get_iter_first(model, &iter)) {
        do {
            gtk_tree_model_get(model, &iter, 0, &dir_path, -1);
            const gchar *file_name=gtk_entry_get_text(GTK_ENTRY(name_entry));
            const gchar *file_type=gtk_entry_get_text(GTK_ENTRY(type_entry));
            const gchar *exclude=gtk_entry_get_text(GTK_ENTRY(exclude_entry));
            int recursive=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(recursive_check));
            int case_insensitive=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(case_check));

            // Search directory
            search_directory(dir_path, file_name, file_type, exclude, recursive, case_insensitive);
            g_free(dir_path);
        } while (gtk_tree_model_iter_next(model, &iter));
    }
}

void stop_search() {
    // Here you can implement logic (e.g., using a flag to stop a threaded search operation)
}

void clear_results() {
    gtk_list_store_clear(result_store);
}

void on_about_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog=gtk_message_dialog_new(GTK_WINDOW(data),
                        GTK_DIALOG_MODAL,
                        GTK_MESSAGE_INFO,
                        GTK_BUTTONS_OK,
                        "Made by Jay Mee - J~Net 2024");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void add_search_dir(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog=gtk_file_chooser_dialog_new("Select Directory",
                        GTK_WINDOW(data),
                        GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                        "_Cancel", GTK_RESPONSE_CANCEL,
                        "_Select", GTK_RESPONSE_ACCEPT,
                        NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *foldername=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        GtkTreeIter iter;
        gtk_list_store_append(dir_store, &iter);
        gtk_list_store_set(dir_store, &iter, 0, foldername, -1);
        g_free(foldername);
    }

    gtk_widget_destroy(dialog);
}

void remove_search_dir(GtkWidget *widget, gpointer data) {
    GtkTreeSelection *selection=gtk_tree_view_get_selection(search_dirs_view);
    GtkTreeIter iter;
    GtkTreeModel *model;
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
    }
}

gboolean on_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    GtkTreeSelection *selection;
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkWidget *menu, *menu_item;
    
    if (event->type == GDK_BUTTON_PRESS && event->button == 3) {
        // Right-click event
        selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(widget));

        if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
            
            menu=gtk_menu_new();

            menu_item=gtk_menu_item_new_with_label("Open Containing Folder");
            g_signal_connect(menu_item, "activate", G_CALLBACK(on_context_menu_action), "open");
            gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

            menu_item=gtk_menu_item_new_with_label("Copy File");
            g_signal_connect(menu_item, "activate", G_CALLBACK(on_context_menu_action), "copy");
            gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

            menu_item=gtk_menu_item_new_with_label("Cut File");
            g_signal_connect(menu_item, "activate", G_CALLBACK(on_context_menu_action), "cut");
            gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

            menu_item=gtk_menu_item_new_with_label("Remove File");
            g_signal_connect(menu_item, "activate", G_CALLBACK(on_context_menu_action), "remove");
            gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

            menu_item=gtk_menu_item_new_with_label("Rename File");
            g_signal_connect(menu_item, "activate", G_CALLBACK(on_context_menu_action), "rename");
            gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

            gtk_widget_show_all(menu);
            gtk_menu_popup_at_pointer(GTK_MENU(menu), (GdkEvent *) event);
            return TRUE;
        }
    }

    return FALSE;
}

void on_context_menu_action(GtkWidget *menu_item, gpointer action) {
    g_print("Action: %s\n", (char *)action);
    // Implement specific logic for each action here
}

void search_directory(const char *dir_path, const char *file_name, const char *file_type, const char *exclude, int recursive, int case_insensitive) {
    DIR *dp=opendir(dir_path);
    if (dp == NULL) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry=readdir(dp))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        if (should_exclude(entry->d_name, exclude, case_insensitive)) {
            continue; // Skip excluded items
        }

        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        struct stat path_stat;
        if (stat(full_path, &path_stat) == -1) {
            continue;
        }

        if (S_ISDIR(path_stat.st_mode)) {
            if (recursive) {
                search_directory(full_path, file_name, file_type, exclude, recursive, case_insensitive);
            }
        } else {
            if (match_file(entry->d_name, file_name, case_insensitive) &&
                (strlen(file_type) == 0 || strstr(entry->d_name, file_type))) {
                
                GtkTreeIter iter;
                gtk_list_store_append(result_store, &iter);
                gtk_list_store_set(result_store, &iter, 0, full_path, -1);
                
                if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(log_check))) {
                    FILE *logfile=fopen("search_log.txt", "a");
                    if (logfile != NULL) {
                        fprintf(logfile, "Found file: %s\n", full_path);
                        fclose(logfile);
                    }
                }
            }
        }
    }

    closedir(dp);
}

int match_file(const char *file_name, const char *pattern, int case_insensitive) {
    char name_copy[256], pattern_copy[256];
    if (case_insensitive) {
        strncpy(name_copy, file_name, sizeof(name_copy) - 1);
        strncpy(pattern_copy, pattern, sizeof(pattern_copy) - 1);
        name_copy[sizeof(name_copy) - 1]='\0';
        pattern_copy[sizeof(pattern_copy) - 1]='\0';
        to_lowercase(name_copy);
        to_lowercase(pattern_copy);
        return strstr(name_copy, pattern_copy) != NULL;
    } else {
        return strstr(file_name, pattern) != NULL;
    }
}

int should_exclude(const char *name, const char *exclude, int case_insensitive) {
    if (strlen(exclude) == 0) {
        return 0;
    }

    char name_copy[256], exclude_copy[256];
    strncpy(name_copy, name, sizeof(name_copy) - 1);
    strncpy(exclude_copy, exclude, sizeof(exclude_copy) - 1);

    name_copy[sizeof(name_copy) - 1]='\0';
    exclude_copy[sizeof(exclude_copy) - 1]='\0';

    if (case_insensitive) {
        to_lowercase(name_copy);
        to_lowercase(exclude_copy);
    }

    return strstr(name_copy, exclude_copy) != NULL;
}

void to_lowercase(char *str) {
    for (int i=0; str[i]; i++) {
        str[i]=tolower(str[i]);
    }
}

void open_containing_folder(const char *file_path) {
    char dir_path[512];
    strncpy(dir_path, file_path, sizeof(dir_path) - 1);
    dir_path[sizeof(dir_path) - 1]='\0';

    char *last_slash=strrchr(dir_path, '/');
    if (last_slash != NULL) {
        *last_slash='\0'; // Trim to get directory path
        char command[1024];
        snprintf(command, sizeof(command), "xdg-open \"%s\"", dir_path);
        system(command);
    }
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *search_button, *stop_button, *clear_button;
    GtkWidget *about_button, *add_dir_button, *remove_dir_button;
    GtkWidget *scroll_results, *scroll_dirs;

    gtk_init(&argc, &argv);

    // Create a window
    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "File Search - J~Net 2024");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a grid
    grid=gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Name entry
    name_entry=gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(name_entry), "File Name");
    gtk_grid_attach(GTK_GRID(grid), name_entry, 0, 0, 4, 1);

    // Type entry
    type_entry=gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(type_entry), "File Type");
    gtk_grid_attach(GTK_GRID(grid), type_entry, 0, 1, 4, 1);

    // Exclude entry
    exclude_entry=gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(exclude_entry), "Exclude");
    gtk_grid_attach(GTK_GRID(grid), exclude_entry, 0, 2, 4, 1);

    // Recursive check
    recursive_check=gtk_check_button_new_with_label("Recursive Search");
    gtk_grid_attach(GTK_GRID(grid), recursive_check, 0, 3, 2, 1);

    // Case Insensitive check
    case_check=gtk_check_button_new_with_label("Case Insensitive");
    gtk_grid_attach(GTK_GRID(grid), case_check, 2, 3, 2, 1);

    // Log check
    log_check=gtk_check_button_new_with_label("Enable Search Log");
    gtk_grid_attach(GTK_GRID(grid), log_check, 0, 4, 2, 1);

    // Add directory button
    add_dir_button=gtk_button_new_with_label("Add Search Location");
    g_signal_connect(add_dir_button, "clicked", G_CALLBACK(add_search_dir), window);
    gtk_grid_attach(GTK_GRID(grid), add_dir_button, 0, 5, 2, 1);

    // Remove directory button
    remove_dir_button=gtk_button_new_with_label("Remove Search Location");
    g_signal_connect(remove_dir_button, "clicked", G_CALLBACK(remove_search_dir), window);
    gtk_grid_attach(GTK_GRID(grid), remove_dir_button, 2, 5, 2, 1);

    // Search directories view
    dir_store=gtk_list_store_new(1, G_TYPE_STRING);
    search_dirs_view=GTK_TREE_VIEW(gtk_tree_view_new_with_model(GTK_TREE_MODEL(dir_store)));
    gtk_tree_view_insert_column_with_attributes(search_dirs_view, -1, "Directory",
                                                gtk_cell_renderer_text_new(), "text", 0, NULL);

    scroll_dirs=gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(scroll_dirs, 760, 100);
    gtk_container_add(GTK_CONTAINER(scroll_dirs), GTK_WIDGET(search_dirs_view));
    gtk_grid_attach(GTK_GRID(grid), scroll_dirs, 0, 6, 4, 1);

    // Search button
    search_button=gtk_button_new_with_label("Search");
    g_signal_connect(search_button, "clicked", G_CALLBACK(start_search), NULL);
    gtk_grid_attach(GTK_GRID(grid), search_button, 0, 7, 1, 1);

    // Stop button
    stop_button=gtk_button_new_with_label("Stop Search");
    g_signal_connect(stop_button, "clicked", G_CALLBACK(stop_search), NULL);
    gtk_grid_attach(GTK_GRID(grid), stop_button, 1, 7, 1, 1);

    // Clear results
    clear_button=gtk_button_new_with_label("Clear Results");
    g_signal_connect(clear_button, "clicked", G_CALLBACK(clear_results), NULL);
    gtk_grid_attach(GTK_GRID(grid), clear_button, 2, 7, 1, 1);

    // About button
    about_button=gtk_button_new_with_label("About");
    g_signal_connect(about_button, "clicked", G_CALLBACK(on_about_clicked), window);
    gtk_grid_attach(GTK_GRID(grid), about_button, 3, 7, 1, 1);

    // Result view
    result_store=gtk_list_store_new(1, G_TYPE_STRING);
    result_view=GTK_TREE_VIEW(gtk_tree_view_new_with_model(GTK_TREE_MODEL(result_store)));
    gtk_tree_view_insert_column_with_attributes(result_view, -1, "File Path",
                                                gtk_cell_renderer_text_new(), "text", 0, NULL);
    g_signal_connect(result_view, "button-press-event", G_CALLBACK(on_button_press_event), NULL);

    scroll_results=gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(scroll_results, 760, 300);
    gtk_container_add(GTK_CONTAINER(scroll_results), GTK_WIDGET(result_view));
    gtk_grid_attach(GTK_GRID(grid), scroll_results, 0, 8, 4, 1);

    // Set dark theme
    GtkCssProvider *provider=gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, 
        "* { background-color: #2E2E2E; color: #FFFFFF; }", -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    // Automatically check these
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(case_check), TRUE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(recursive_check), TRUE);

    gtk_widget_show_all(window);

    gtk_main();
    return 0;
}

