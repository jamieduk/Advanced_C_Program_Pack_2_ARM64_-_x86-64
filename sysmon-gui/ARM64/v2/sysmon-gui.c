// Made By Jay @ J~Net 2024
//
//
// sudo apt-get install -y libgtk-3-dev libcairo2-dev
//
// gcc -o sysmon-gui sysmon-gui.c `pkg-config --cflags --libs gtk+-3.0`
//
//
#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

gdouble cpu_usage=0.0;
gdouble ram_usage=0.0;
gdouble disk_usage=0.0;

void read_system_stats() {
    // CPU Usage
    FILE *fp=popen("top -bn1 | grep 'Cpu(s)' | sed 's/^.*([0-9.]+)%* id.*/\\1/'", "r");
    if (fp) {
        fscanf(fp, "%lf", &cpu_usage);
        cpu_usage=100.0 - cpu_usage; // Invert to get usage percentage
        pclose(fp);
    }

    // RAM Usage
    fp=popen("free | grep Mem | awk '{print $3/$2 * 100.0}'", "r");
    if (fp) {
        fscanf(fp, "%lf", &ram_usage);
        pclose(fp);
    }

    // Disk Usage
    fp=popen("df / | grep / | awk '{ print $5 }' | sed 's/%//g'", "r");
    if (fp) {
        fscanf(fp, "%lf", &disk_usage);
        pclose(fp);
    }
}

static void draw_graph(GtkWidget *widget, cairo_t *cr) {
    read_system_stats();

    // Clear background (Dark theme)
    cairo_set_source_rgb(cr, 0.1, 0.1, 0.1); // Dark grey
    cairo_paint(cr);

    // Draw CPU usage
    cairo_set_source_rgb(cr, 1, 0.2, 0.2); // Red for CPU
    cairo_rectangle(cr, 50, 50, 200 * (cpu_usage / 100.0), 30);
    cairo_fill(cr);

    // Draw RAM usage (Purple)
    cairo_set_source_rgb(cr, 0.5, 0.0, 0.5); // Purple for RAM
    cairo_rectangle(cr, 50, 100, 200 * (ram_usage / 100.0), 30);
    cairo_fill(cr);

    // Draw Disk usage
    cairo_set_source_rgb(cr, 0.2, 0.2, 1); // Blue for Disk
    cairo_rectangle(cr, 50, 150, 200 * (disk_usage / 100.0), 30);
    cairo_fill(cr);

    // Set text color and font size
    cairo_set_source_rgb(cr, 1, 1, 1); // White for text
    cairo_set_font_size(cr, 12);
    cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD); // Bold text

    // Prepare text buffers
    char text[50];

    // Draw labels
    sprintf(text, "CPU Usage: %.1f%%", cpu_usage);
    cairo_move_to(cr, 10, 70);
    cairo_show_text(cr, text);

    sprintf(text, "RAM Usage: %.1f%%", ram_usage);
    cairo_move_to(cr, 10, 120);
    cairo_show_text(cr, text);

    sprintf(text, "Disk Usage: %.1f%%", disk_usage);
    cairo_move_to(cr, 10, 170);
    cairo_show_text(cr, text);
}

void on_about_menu_item_selected(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    dialog=gtk_message_dialog_new(GTK_WINDOW(data),
                                     GTK_DIALOG_DESTROY_WITH_PARENT,
                                     GTK_MESSAGE_INFO,
                                     GTK_BUTTONS_OK,
                                     "Author: Jay Mee");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "System Resource Visualizer");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 250);

    // Creating a box for layout
    GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    GtkWidget *about_button=gtk_button_new_with_label("About");
    gtk_box_pack_start(GTK_BOX(vbox), about_button, FALSE, FALSE, 5);

    g_signal_connect(about_button, "clicked", G_CALLBACK(on_about_menu_item_selected), window);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *drawing_area=gtk_drawing_area_new();
    gtk_box_pack_start(GTK_BOX(vbox), drawing_area, TRUE, TRUE, 5);
    g_signal_connect(G_OBJECT(drawing_area), "draw", G_CALLBACK(draw_graph), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

