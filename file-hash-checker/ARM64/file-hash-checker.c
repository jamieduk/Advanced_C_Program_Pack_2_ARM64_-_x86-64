// Made By Jay @ J~Net 2024
//
// gcc -o file-hash-checker file-hash-checker.c $(pkg-config --cflags --libs gtk4) -lssl -lcrypto
//
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <openssl/evp.h>

typedef struct {
    GtkWidget *md5_label;
    GtkWidget *sha256_label;
    GtkWidget *filename_label;
} AppWidgets;

// Generic hash calculation function using EVP
static char* calculate_hash(const char* filename, const char* algorithm) {
    FILE *file=fopen(filename, "rb");
    if (!file) return NULL;

    unsigned char buffer[4096];
    size_t bytes;
    
    EVP_MD_CTX *mdctx=EVP_MD_CTX_new();
    const EVP_MD *md=EVP_get_digestbyname(algorithm);
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digest_len;

    EVP_DigestInit_ex(mdctx, md, NULL);
    while ((bytes=fread(buffer, 1, sizeof(buffer), file)) != 0) {
        EVP_DigestUpdate(mdctx, buffer, bytes);
    }
    EVP_DigestFinal_ex(mdctx, digest, &digest_len);
    EVP_MD_CTX_free(mdctx);
    fclose(file);

    char *hash_string=malloc(digest_len * 2 + 1);
    for(unsigned int i=0; i < digest_len; i++) {
        sprintf(&hash_string[i * 2], "%02x", digest[i]);
    }

    return hash_string;
}

static void process_file(const char *filepath, AppWidgets *widgets) {
    // Update filename label
    char *filename=g_path_get_basename(filepath);
    char *filename_text=g_strdup_printf("File: %s", filename);
    gtk_label_set_text(GTK_LABEL(widgets->filename_label), filename_text);
    g_free(filename_text);
    g_free(filename);

    // Calculate and display MD5
    char *md5=calculate_hash(filepath, "MD5");
    if (md5) {
        char *md5_text=g_strdup_printf("MD5: %s", md5);
        gtk_label_set_text(GTK_LABEL(widgets->md5_label), md5_text);
        g_free(md5_text);
        free(md5);
    }

    // Calculate and display SHA-256
    char *sha256=calculate_hash(filepath, "SHA256");
    if (sha256) {
        char *sha256_text=g_strdup_printf("SHA-256: %s", sha256);
        gtk_label_set_text(GTK_LABEL(widgets->sha256_label), sha256_text);
        g_free(sha256_text);
        free(sha256);
    }
}

static void file_dialog_response(GObject *source_object, GAsyncResult *res, gpointer user_data) {
    GtkFileDialog *dialog=GTK_FILE_DIALOG(source_object);
    GError *error=NULL;
    AppWidgets *widgets=(AppWidgets *)user_data;
    
    GFile *file=gtk_file_dialog_open_finish(dialog, res, &error);
    if (file) {
        char *filepath=g_file_get_path(file);
        process_file(filepath, widgets);
        g_free(filepath);
        g_object_unref(file);
    } else if (error) {
        g_warning("Error opening file: %s", error->message);
        g_error_free(error);
    }
}

static void on_open_file(GtkWidget *widget, gpointer data) {
    AppWidgets *widgets=(AppWidgets *)data;
    GtkWindow *window=GTK_WINDOW(gtk_widget_get_root(widget));
    GtkFileDialog *dialog=gtk_file_dialog_new();
    
    gtk_file_dialog_open(dialog,
                        window,
                        NULL,
                        file_dialog_response,
                        widgets);
    
    g_object_unref(dialog);
}

static void on_about(GtkWidget *widget, gpointer data) {
    GtkWindow *window=GTK_WINDOW(gtk_widget_get_root(widget));
    GtkWidget *about_window, *box, *label;
    
    about_window=gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(about_window), "About");
    gtk_window_set_transient_for(GTK_WINDOW(about_window), GTK_WINDOW(window));
    
    box=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_window_set_child(GTK_WINDOW(about_window), box);
    
    label=gtk_label_new("File Hash Checker\n\nAuthor: Jay Mee");
    gtk_box_append(GTK_BOX(box), label);
    
    gtk_window_present(GTK_WINDOW(about_window));
}

static gboolean on_drop(GtkDropTarget *target,
                       const GValue *value,
                       double x,
                       double y,
                       gpointer user_data) {
    AppWidgets *widgets=(AppWidgets *)user_data;
    
    if (G_VALUE_HOLDS(value, G_TYPE_FILE)) {
        GFile *file=g_value_get_object(value);
        char *filepath=g_file_get_path(file);
        if (filepath) {
            process_file(filepath, widgets);
            g_free(filepath);
            return TRUE;
        }
    }
    return FALSE;
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *box;
    GtkWidget *button_box;
    GtkWidget *open_button;
    GtkWidget *about_button;
    GtkDropTarget *drop_target;
    AppWidgets *widgets=g_new(AppWidgets, 1);
    
    // Create main window
    window=gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "File Hash Checker");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 200);
    
    // Create main vertical box
    box=gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start(box, 10);
    gtk_widget_set_margin_end(box, 10);
    gtk_widget_set_margin_top(box, 10);
    gtk_widget_set_margin_bottom(box, 10);
    gtk_window_set_child(GTK_WINDOW(window), box);
    
    // Create labels
    widgets->filename_label=gtk_label_new("Drop a file here or use Open button");
    widgets->md5_label=gtk_label_new("MD5: ");
    widgets->sha256_label=gtk_label_new("SHA-256: ");
    
    // Make labels selectable
    gtk_label_set_selectable(GTK_LABEL(widgets->md5_label), TRUE);
    gtk_label_set_selectable(GTK_LABEL(widgets->sha256_label), TRUE);
    
    // Create button box
    button_box=gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    open_button=gtk_button_new_with_label("Open File");
    about_button=gtk_button_new_with_label("About");
    
    // Add buttons to button box
    gtk_box_append(GTK_BOX(button_box), open_button);
    gtk_box_append(GTK_BOX(button_box), about_button);
    
    // Add everything to main box
    gtk_box_append(GTK_BOX(box), button_box);
    gtk_box_append(GTK_BOX(box), widgets->filename_label);
    gtk_box_append(GTK_BOX(box), widgets->md5_label);
    gtk_box_append(GTK_BOX(box), widgets->sha256_label);
    
    // Connect signals
    g_signal_connect(open_button, "clicked", G_CALLBACK(on_open_file), widgets);
    g_signal_connect(about_button, "clicked", G_CALLBACK(on_about), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_window_destroy), NULL);
    
    // Set up drag and drop
    drop_target=gtk_drop_target_new(G_TYPE_FILE, GDK_ACTION_COPY);
    g_signal_connect(drop_target, "drop", G_CALLBACK(on_drop), widgets);
    gtk_widget_add_controller(window, GTK_EVENT_CONTROLLER(drop_target));
    
    // Show window
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char *argv[]) {
    GtkApplication *app;
    int status;
    
    app=gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status=g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    
    return status;
}
