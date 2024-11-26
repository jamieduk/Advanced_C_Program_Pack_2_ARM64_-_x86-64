// Made By Jay @ J~Net 2024
//
// Image Compressor with Preview
#include <gtk/gtk.h>
#include <jpeglib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function prototypes
void compress_image(const char *input_file, const char *output_file, int quality);
void on_compress_button_clicked(GtkWidget *widget, gpointer data);
void on_about_button_clicked(GtkWidget *widget, gpointer data);
void on_file_dropped(GtkWidget *widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *data, guint info, guint time, gpointer user_data);

// Global variables for GUI elements
GtkWidget *image_preview;
GtkWidget *compression_slider;
GtkListStore *file_list;

typedef struct {
    char **file_paths;
    int file_count;
} FileData;

FileData files={NULL, 0};

// Function to compress an image
void compress_image(const char *input_file, const char *output_file, int quality) {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_compress_struct jinfo;
    struct jpeg_error_mgr jerr;

    FILE *infile, *outfile;
    JSAMPARRAY buffer;
    int row_stride;

    // Open the input file
    if ((infile=fopen(input_file, "rb")) == NULL) {
        fprintf(stderr, "Error: Cannot open %s\n", input_file);
        return;
    }

    cinfo.err=jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    // Set up the compression object
    jinfo.err=jpeg_std_error(&jerr);
    jpeg_create_compress(&jinfo);

    if ((outfile=fopen(output_file, "wb")) == NULL) {
        fprintf(stderr, "Error: Cannot open %s\n", output_file);
        fclose(infile);
        return;
    }
    jpeg_stdio_dest(&jinfo, outfile);

    jinfo.image_width=cinfo.image_width;
    jinfo.image_height=cinfo.image_height;
    jinfo.input_components=cinfo.num_components;
    jinfo.in_color_space=cinfo.jpeg_color_space;

    jpeg_set_defaults(&jinfo);
    jpeg_set_quality(&jinfo, quality, TRUE);
    jpeg_start_compress(&jinfo, TRUE);

    row_stride=cinfo.output_width * cinfo.output_components;
    buffer=(*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, buffer, 1);
        jpeg_write_scanlines(&jinfo, buffer, 1);
    }

    jpeg_finish_compress(&jinfo);
    jpeg_destroy_compress(&jinfo);
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    fclose(infile);
    fclose(outfile);
}

// Callback for the About button
void on_about_button_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog=gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                               "Author: Jay Mee\nImage Compressor");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Callback for file drag-and-drop
void on_file_dropped(GtkWidget *widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *data, guint info, guint time, gpointer user_data) {
    gchar **uris=gtk_selection_data_get_uris(data);
    for (int i=0; uris[i] != NULL; i++) {
        gchar *file_path=g_filename_from_uri(uris[i], NULL, NULL);
        GtkTreeIter iter;
        gtk_list_store_append(file_list, &iter);
        gtk_list_store_set(file_list, &iter, 0, file_path, -1);

        files.file_paths=realloc(files.file_paths, sizeof(char *) * (files.file_count + 1));
        files.file_paths[files.file_count]=strdup(file_path);
        files.file_count++;
        g_free(file_path);
    }
}

// Callback for the Compress button
void on_compress_button_clicked(GtkWidget *widget, gpointer data) {
    int quality=(int)gtk_range_get_value(GTK_RANGE(compression_slider));
    for (int i=0; i < files.file_count; i++) {
        char output_file[256];
        snprintf(output_file, sizeof(output_file), "%s_compressed.jpg", files.file_paths[i]);
        compress_image(files.file_paths[i], output_file, quality);
    }
    GtkWidget *dialog=gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                               "Compression complete!");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Function to create the GUI
void create_gui(int argc, char *argv[]) {
    GtkWidget *window, *vbox, *hbox, *compress_button, *about_button, *scroll_window;
    GtkWidget *tree_view;
    GtkCellRenderer *renderer;

    gtk_init(&argc, &argv);

    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Image Compressor");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

    vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    file_list=gtk_list_store_new(1, G_TYPE_STRING);
    tree_view=gtk_tree_view_new_with_model(GTK_TREE_MODEL(file_list));
    renderer=gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "File", renderer, "text", 0, NULL);

    scroll_window=gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(scroll_window, -1, 200);
    gtk_container_add(GTK_CONTAINER(scroll_window), tree_view);
    gtk_box_pack_start(GTK_BOX(vbox), scroll_window, TRUE, TRUE, 0);

    image_preview=gtk_image_new();
    gtk_box_pack_start(GTK_BOX(vbox), image_preview, TRUE, TRUE, 0);

    compression_slider=gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 10, 100, 1);
    gtk_box_pack_start(GTK_BOX(vbox), compression_slider, FALSE, FALSE, 0);

    hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    compress_button=gtk_button_new_with_label("Compress");
    g_signal_connect(compress_button, "clicked", G_CALLBACK(on_compress_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), compress_button, TRUE, TRUE, 0);

    about_button=gtk_button_new_with_label("About");
    g_signal_connect(about_button, "clicked", G_CALLBACK(on_about_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), about_button, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
    gtk_main();
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        // CLI Mode
        if (strcmp(argv[1], "-c") == 0 && argc == 4) {
            int quality=atoi(argv[3]);
            compress_image(argv[2], "compressed_image.jpg", quality);
            printf("Image compressed successfully.\n");
        } else {
            printf("Usage: %s -c <input_file> <quality>\n", argv[0]);
        }
    } else {
        // GUI Mode
        create_gui(argc, argv);
    }
    return 0;
}

