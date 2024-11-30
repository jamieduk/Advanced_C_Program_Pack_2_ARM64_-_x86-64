#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>

GtkWidget *output_label;

// Function to generate lottery numbers (uncommon, i.e., numbers > 25)
void generate_lottery_numbers(GtkWidget *widget, gpointer data) {
    int numbers[6]; // 5 main numbers + 1 bonus
    int i, j, unique;
    char result[128];

    srand(time(NULL)); // Seed the random number generator

    for (i=0; i < 6; i++) {
        do {
            unique=1;
            // Prioritize numbers above 25 for a better payout
            numbers[i]=(rand() % 24) + 26; // Numbers between 26 and 49

            for (j=0; j < i; j++) {
                if (numbers[i] == numbers[j]) {
                    unique=0;
                    break;
                }
            }
        } while (!unique);
    }

    snprintf(result, sizeof(result), "Lottery Numbers: %d %d %d %d %d %d",
             numbers[0], numbers[1], numbers[2], numbers[3], numbers[4], numbers[5]);
    gtk_label_set_text(GTK_LABEL(output_label), result);
}

// Activate function for the application
void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window, *grid, *predict_button;

    // Create the main window
    window=gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Lottery Number Generator");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 250);

    // Create a grid layout
    grid=gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Create an output label (initially shows the "About" text)
    output_label=gtk_label_new("Author: Jay Mee (c) 2024");
    gtk_grid_attach(GTK_GRID(grid), output_label, 0, 0, 2, 1);

    // Create a "Predict Lottery" button
    predict_button=gtk_button_new_with_label("Predict Lottery");
    gtk_grid_attach(GTK_GRID(grid), predict_button, 0, 1, 2, 1);
    g_signal_connect(predict_button, "clicked", G_CALLBACK(generate_lottery_numbers), NULL);

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    // Use G_APPLICATION_DEFAULT_FLAGS to avoid deprecation warning
    app=gtk_application_new("com.example.lottery", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    status=g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}

