// Made By Jay @ J~Net 2024
//
// sudo apt install -y libgtk-3-dev libssl-dev
//
// gcc -o encrypted-notepad encrypted-notepad.c `pkg-config --cflags --libs gtk+-3.0` -lssl -lcrypto
//
//
#include <gtk/gtk.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <string.h>
#include <stdio.h>

#define KEY_LENGTH 32 // AES key length (256 bits)
#define BLOCK_SIZE 16 // AES block size (128 bits)
#define NOTES_FILE "notes.enc"

// Function prototypes
void on_save_button_clicked(GtkButton *button, gpointer user_data);
void on_open_button_clicked(GtkButton *button, gpointer user_data);
void on_about_button_clicked(GtkButton *button, gpointer user_data);

int encrypt(const unsigned char *plaintext, int plaintext_len, unsigned char *ciphertext, const unsigned char *key, const unsigned char *iv);
int decrypt(const unsigned char *ciphertext, int ciphertext_len, unsigned char *plaintext, const unsigned char *key, const unsigned char *iv);

// Main function
int main(int argc, char *argv[]) {
    GtkWidget *window, *vbox, *text_view, *save_button, *open_button, *about_button;
    GtkTextBuffer *text_buffer;

    gtk_init(&argc, &argv);

    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Encrypted Notepad");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
    
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    text_view=gtk_text_view_new();
    text_buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_box_pack_start(GTK_BOX(vbox), text_view, TRUE, TRUE, 0);

    save_button=gtk_button_new_with_label("Save");
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_button_clicked), text_buffer);
    gtk_box_pack_start(GTK_BOX(vbox), save_button, FALSE, FALSE, 0);

    open_button=gtk_button_new_with_label("Open");
    g_signal_connect(open_button, "clicked", G_CALLBACK(on_open_button_clicked), text_buffer);
    gtk_box_pack_start(GTK_BOX(vbox), open_button, FALSE, FALSE, 0);

    about_button=gtk_button_new_with_label("About");
    g_signal_connect(about_button, "clicked", G_CALLBACK(on_about_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), about_button, FALSE, FALSE, 0);
    
    gtk_widget_show_all(window);
    gtk_main();
    
    return 0;
}

// Save button callback
void on_save_button_clicked(GtkButton *button, gpointer user_data) {
    GtkTextBuffer *text_buffer=GTK_TEXT_BUFFER(user_data);
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(text_buffer, &start, &end);
    gchar *text=gtk_text_buffer_get_text(text_buffer, &start, &end, FALSE);

    const char *password="your-password"; // Replace with secure password handling
    unsigned char key[KEY_LENGTH]={0};
    const unsigned char iv[BLOCK_SIZE]={0}; // Initialize IV (consider randomness)

    strncpy((char *)key, password, KEY_LENGTH);

    unsigned char ciphertext[256]={0};
    int ciphertext_len=encrypt((unsigned char *)text, strlen((char *)text), ciphertext, key, iv);

    FILE *file=fopen(NOTES_FILE, "wb");
    if (file) {
        fwrite(ciphertext, sizeof(unsigned char), ciphertext_len, file);
        fclose(file);
        gtk_text_buffer_set_text(text_buffer, "Saved encrypted note successfully.", -1);
    } else {
        gtk_text_buffer_set_text(text_buffer, "Failed to save note.", -1);
    }

    g_free(text);
}

// Open button callback
void on_open_button_clicked(GtkButton *button, gpointer user_data) {
    GtkTextBuffer *text_buffer=GTK_TEXT_BUFFER(user_data);
    unsigned char ciphertext[256];
    int ciphertext_len;

    FILE *file=fopen(NOTES_FILE, "rb");
    if (file) {
        ciphertext_len=fread(ciphertext, sizeof(unsigned char), sizeof(ciphertext), file);
        fclose(file);

        const char *password="your-password"; // Replace with secure password handling
        unsigned char key[KEY_LENGTH]={0};
        const unsigned char iv[BLOCK_SIZE]={0}; // Initialize IV (should match the one used in encryption)

        strncpy((char *)key, password, KEY_LENGTH);
        
        unsigned char plaintext[256]={0};
        int plaintext_len=decrypt(ciphertext, ciphertext_len, plaintext, key, iv);

        gtk_text_buffer_set_text(text_buffer, (const char *)plaintext, plaintext_len);
    } else {
        gtk_text_buffer_set_text(text_buffer, "Failed to open note.", -1);
    }
}

// About button callback
void on_about_button_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *dialog=gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                               "Author: Jay Mee");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// AES encryption function using EVP
int encrypt(const unsigned char *plaintext, int plaintext_len, unsigned char *ciphertext, const unsigned char *key, const unsigned char *iv) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;

    // Create and initialize the context
    if (!(ctx=EVP_CIPHER_CTX_new()))
        exit(EXIT_FAILURE);

    // Initialize the encryption operation
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        exit(EXIT_FAILURE);

    // Provide the message to be encrypted
    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        exit(EXIT_FAILURE);
    ciphertext_len=len;

    // Finalize the encryption
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        exit(EXIT_FAILURE);
    ciphertext_len += len;

    // Clean up
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len; // Return the length of the ciphertext
}

// AES decryption function using EVP
int decrypt(const unsigned char *ciphertext, int ciphertext_len, unsigned char *plaintext, const unsigned char *key, const unsigned char *iv) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;

    // Create and initialize the context
    if (!(ctx=EVP_CIPHER_CTX_new()))
        exit(EXIT_FAILURE);

    // Initialize the decryption operation
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        exit(EXIT_FAILURE);

    // Provide the message to be decrypted
    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        exit(EXIT_FAILURE);
    plaintext_len=len;

    // Finalize the decryption
    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        exit(EXIT_FAILURE);
    plaintext_len += len;

    // Clean up
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len; // Return the length of the plaintext
}

