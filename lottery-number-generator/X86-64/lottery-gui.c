// Made By Jay @ J~Net 2024
//
// gcc -o lottery-gui lottery-gui.c `pkg-config --cflags --libs gtk+-3.0`
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <gtk/gtk.h>

#define LOTTERY_SIZE 5
#define LOTTERY_MAX 49
#define COMMON_NUMBERS_SIZE 10

static GtkWidget *output_text;

void show_about(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    GtkWidget *label;
    GtkWidget *hbox;
    GtkWidget *image;

    dialog= gtk_dialog_new_with_buttons("About",
                                         GTK_WINDOW(gtk_widget_get_toplevel(widget)),
                                         GTK_DIALOG_MODAL,
                                         "_OK",
                                         GTK_RESPONSE_OK,
                                         NULL);

    hbox= gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 10);
    gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), hbox, TRUE, TRUE, 0);

    label= gtk_label_new("Lottery Number Generator\n\nAuthor: Jay Mee\n\nMade By Jay @ J~Net 2024");
    gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);

    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

int is_common(int number, int *common_numbers, int common_count) {
    for (int i=0; i < common_count; i++) {
        if (common_numbers[i]== number) {
            return 1; // Number is common
        }
    }
    return 0; // Number is not common
}

void generate_lottery_numbers(int *common_numbers, int common_count) {
    int lottery[LOTTERY_SIZE];
    int bonus;
    int i, j;
    int isUnique;

    srand(time(NULL)); // Seed the random number generator

    // Generate 5 unique lottery numbers
    for (i=0; i < LOTTERY_SIZE; i++) {
        do {
            isUnique=1;
            lottery[i]=rand() % LOTTERY_MAX + 1; // Generate a number between 1 and 49

            // Check for uniqueness
            for (j=0; j < i; j++) {
                if (lottery[i]== lottery[j] || is_common(lottery[i], common_numbers, common_count)) {
                    isUnique=0;
                    break;
                }
            }
        } while (!isUnique);
    }

    // Generate 1 bonus number
    do {
        bonus=rand() % LOTTERY_MAX + 1; // Generate a number between 1 and 49
        isUnique=1;

        // Check if bonus number is unique
        for (i=0; i < LOTTERY_SIZE; i++) {
            if (bonus== lottery[i] || is_common(bonus, common_numbers, common_count)) {
                isUnique=0;
                break;
            }
        }
    } while (!isUnique);

    // Print the generated lottery numbers
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(output_text)), "", -1);
    char result[256];
    snprintf(result, sizeof(result), "Lottery Numbers: ");
    for (i=0; i < LOTTERY_SIZE; i++) {
        snprintf(result + strlen(result), sizeof(result) - strlen(result), "%d ", lottery[i]);
    }
    snprintf(result + strlen(result), sizeof(result) - strlen(result), "\nBonus Number: %d\n", bonus);
    gtk_text_buffer_insert_at_cursor(gtk_text_view_get_buffer(GTK_TEXT_VIEW(output_text)), result, -1);
}

void on_generate_button_clicked(GtkWidget *widget, gpointer data) {
    int common_numbers[COMMON_NUMBERS_SIZE]={0};
    int common_count=0;  // This can be extended to fetch actual common numbers from input if needed
    generate_lottery_numbers(common_numbers, common_count);
}

void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *box;

    window=gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Lottery Number Generator");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

    box=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), box);

    output_text=gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(output_text), FALSE);
    gtk_box_pack_start(GTK_BOX(box), output_text, TRUE, TRUE, 0);

    button=gtk_button_new_with_label("Generate Lottery Numbers");
    g_signal_connect(button, "clicked", G_CALLBACK(on_generate_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(box), button, FALSE, FALSE, 0);
    
    button=gtk_button_new_with_label("About");
    g_signal_connect(button, "clicked", G_CALLBACK(show_about), NULL);
    gtk_box_pack_start(GTK_BOX(box), button, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    // Command-line interface
    if (argc > 1 && strcmp(argv[1], "cli")== 0) {
        // CLI options can be added here
        printf("CLI mode not implemented in this version. Please use the GUI.\n");
        return 1;
    }

    // GUI mode
    GtkApplication *app;
    int status;

    app= gtk_application_new("com.example.GtkLottery", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status= g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}

