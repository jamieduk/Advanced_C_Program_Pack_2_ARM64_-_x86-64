// Made By Jay @ J~Net 2024
//
// gcc -o eth-converter eth-converter.c `pkg-config --cflags --libs gtk+-3.0` -lcurl
//
// Example value eth to gbp convert amount 0.00725
//
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h> // Added for the strcpy function

// Callback function for writing data to buffer
size_t write_callback(void *ptr, size_t size, size_t nmemb, char *data) {
    strcat(data, (char*)ptr); // Append the response data to the buffer
    return size * nmemb; // Return the number of bytes processed
}

// Fetch the current ETH to GBP rate
double get_eth_to_gbp() {
    CURL *curl;
    CURLcode res;
    char buffer[1024]=""; // Initialize buffer to store API response
    double rate=0.0;

    // Initialize curl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl=curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.coingecko.com/api/v3/simple/price?ids=ethereum&vs_currencies=gbp");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback); // Set the callback
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer); // Buffer to store the response
        res=curl_easy_perform(curl); // Perform the HTTP request

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            // Parse the JSON response to extract the conversion rate
            sscanf(buffer, "{\"ethereum\":{\"gbp\":%lf}}", &rate);
        }

        curl_easy_cleanup(curl); // Clean up curl handle
    }
    curl_global_cleanup(); // Clean up global curl state

    return rate;
}

// Fetch the current GBP to ETH rate
double get_gbp_to_eth() {
    double eth_rate=get_eth_to_gbp();
    if (eth_rate == 0.0) {
        return 0.0; // Return 0 if conversion rate failed
    }
    return 1.0 / eth_rate; // GBP to ETH is the inverse of ETH to GBP
}

// Callback function for the conversion button
void on_convert_button_clicked(GtkWidget *widget, gpointer data) {
    const gchar *input_text=gtk_entry_get_text(GTK_ENTRY(data));
    double amount=atof(input_text);
    GtkWidget *result_label=GTK_WIDGET(g_object_get_data(G_OBJECT(widget), "result_label"));
    GtkComboBox *combo_box=GTK_COMBO_BOX(g_object_get_data(G_OBJECT(widget), "combo_box"));
    gint active_index=gtk_combo_box_get_active(combo_box);
    
    if (active_index == 0) {
        // Convert ETH to GBP
        double rate=get_eth_to_gbp();
        double gbp=amount * rate;
        char result[128];
        snprintf(result, sizeof(result), "%f ETH=£%.2f", amount, gbp); // Rounded to 2 decimal places
        gtk_label_set_text(GTK_LABEL(result_label), result); // Update result label
    } else {
        // Convert GBP to ETH
        double rate=get_gbp_to_eth();
        double eth=amount * rate;
        char result[128];
        snprintf(result, sizeof(result), "£%.2f=%f ETH", amount, eth); // GBP rounded to 2 decimal places
        gtk_label_set_text(GTK_LABEL(result_label), result); // Update result label
    }
}

// Function to copy the result to the clipboard
void on_copy_button_clicked(GtkWidget *widget, gpointer data) {
    GtkClipboard *clipboard=gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    GtkWidget *result_label=GTK_WIDGET(data);
    const gchar *result_text=gtk_label_get_text(GTK_LABEL(result_label));
    gtk_clipboard_set_text(clipboard, result_text, -1); // Copy the result to clipboard
}

// Show About dialog with author information
void on_about_button_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *about_dialog=gtk_about_dialog_new();
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about_dialog), "ETH to GBP Converter");
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about_dialog), "Made by Jay Mee @ J~Net 2024");
    gtk_dialog_run(GTK_DIALOG(about_dialog));
    gtk_widget_destroy(about_dialog);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv); // Initialize GTK

    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL); // Create window
    GtkWidget *box=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5); // Create box for layout
    GtkWidget *entry=gtk_entry_new(); // Entry widget for ETH amount or GBP
    GtkWidget *button=gtk_button_new_with_label("Convert"); // Button to trigger conversion
    GtkWidget *label=gtk_label_new("Enter ETH or GBP to convert"); // Label to display result
    GtkWidget *about_button=gtk_button_new_with_label("About"); // About button
    GtkWidget *copy_button=gtk_button_new_with_label("Copy to Clipboard"); // Copy button

    // Create ComboBox for conversion selection (ETH to GBP or GBP to ETH)
    GtkWidget *combo_box=gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "ETH to GBP");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "GBP to ETH");
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_box), 0); // Default to ETH to GBP

    // Connect the button click to the conversion callback function
    g_object_set_data(G_OBJECT(button), "result_label", label); // Store label for result display
    g_object_set_data(G_OBJECT(button), "combo_box", combo_box); // Store ComboBox for conversion selection
    g_signal_connect(button, "clicked", G_CALLBACK(on_convert_button_clicked), entry);
    g_signal_connect(about_button, "clicked", G_CALLBACK(on_about_button_clicked), NULL);
    g_signal_connect(copy_button, "clicked", G_CALLBACK(on_copy_button_clicked), label); // Connect copy button

    // Pack widgets into the box container
    gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), combo_box, TRUE, TRUE, 0); // Add ComboBox to layout
    gtk_box_pack_start(GTK_BOX(box), button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), copy_button, TRUE, TRUE, 0); // Add Copy button
    gtk_box_pack_start(GTK_BOX(box), about_button, TRUE, TRUE, 0); // Add About button

    gtk_container_add(GTK_CONTAINER(window), box); // Add box to window
    gtk_widget_show_all(window); // Show all widgets
    gtk_main(); // Start GTK main loop

    return 0;
}

