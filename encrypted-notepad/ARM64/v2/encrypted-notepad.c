// Made By Jay @ J~Net 2024
//
// sudo apt install -y libgtk-3-dev libssl-dev
//
// gcc -o encrypted-notepad encrypted-notepad.c `pkg-config --cflags --libs gtk+-3.0` -lssl -lcrypto
//
#include <gtk/gtk.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define KEY_LENGTH 32 // AES key length (256 bits)
#define BLOCK_SIZE 16 // AES block size (128 bits)
#define NOTES_FILE "notes.enc"
#define IV_SIZE BLOCK_SIZE

// Function prototypes
void on_save_button_clicked(GtkButton *button, gpointer user_data);
void on_open_button_clicked(GtkButton *button, gpointer user_data);
void on_about_button_clicked(GtkButton *button, gpointer user_data);

int encrypt(const unsigned char *plaintext, int plaintext_len, unsigned char *ciphertext, const unsigned char *key, unsigned char *iv);
int decrypt(const unsigned char *ciphertext, int ciphertext_len, unsigned char *plaintext, const unsigned char *key, const unsigned char *iv);
void prompt_for_password(char *password, size_t length);

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

// Prompt user for a password
void prompt_for_password(char *password, size_t length) {
    GtkWidget *dialog=gtk_dialog_new_with_buttons("Enter Password", NULL, GTK_DIALOG_MODAL,
                                                  "_OK", GTK_RESPONSE_OK,
                                                  "_Cancel", GTK_RESPONSE_CANCEL,
                                                  NULL);
    GtkWidget *entry=gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(entry), FALSE);
    gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), entry, TRUE, TRUE, 0);
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        strncpy(password, gtk_entry_get_text(GTK_ENTRY(entry)), length - 1);
    } else {
        password[0]='\0'; // Cancelled
    }
    gtk_widget_destroy(dialog);
}

// Save button callback
void on_save_button_clicked(GtkButton *button, gpointer user_data) {
    GtkTextBuffer *text_buffer=GTK_TEXT_BUFFER(user_data);
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(text_buffer, &start, &end);
    gchar *text=gtk_text_buffer_get_text(text_buffer, &start, &end, FALSE);

    char password[KEY_LENGTH]={0};
    prompt_for_password(password, sizeof(password));
    if (strlen(password) == 0) return;

    unsigned char key[KEY_LENGTH]={0}, iv[IV_SIZE];
    RAND_bytes(iv, IV_SIZE); // Generate a random IV

    strncpy((char *)key, password, KEY_LENGTH);

    size_t ciphertext_len=strlen(text) + BLOCK_SIZE; // Allocate enough space for ciphertext
    unsigned char *ciphertext=malloc(ciphertext_len);
    ciphertext_len=encrypt((unsigned char *)text, strlen(text), ciphertext, key, iv);

    FILE *file=fopen(NOTES_FILE, "wb");
    if (file) {
        fwrite(iv, 1, IV_SIZE, file); // Save IV
        fwrite(ciphertext, 1, ciphertext_len, file);
        fclose(file);
        gtk_text_buffer_set_text(text_buffer, "Saved encrypted note successfully.", -1);
    } else {
        gtk_text_buffer_set_text(text_buffer, "Failed to save note.", -1);
    }

    g_free(text);
    free(ciphertext);
}

// Open button callback
void on_open_button_clicked(GtkButton *button, gpointer user_data) {
    GtkTextBuffer *text_buffer=GTK_TEXT_BUFFER(user_data);
    unsigned char iv[IV_SIZE], *ciphertext;
    size_t ciphertext_len;

    FILE *file=fopen(NOTES_FILE, "rb");
    if (!file) {
        gtk_text_buffer_set_text(text_buffer, "Failed to open note.", -1);
        return;
    }

    fread(iv, 1, IV_SIZE, file); // Read IV
    fseek(file, 0, SEEK_END);
    ciphertext_len=ftell(file) - IV_SIZE;
    fseek(file, IV_SIZE, SEEK_SET);

    ciphertext=malloc(ciphertext_len);
    fread(ciphertext, 1, ciphertext_len, file);
    fclose(file);

    char password[KEY_LENGTH]={0};
    prompt_for_password(password, sizeof(password));
    if (strlen(password) == 0) {
        free(ciphertext);
        return;
    }

    unsigned char key[KEY_LENGTH]={0};
    strncpy((char *)key, password, KEY_LENGTH);

    unsigned char *plaintext=malloc(ciphertext_len + 1);
    int plaintext_len=decrypt(ciphertext, ciphertext_len, plaintext, key, iv);

    if (plaintext_len >= 0) {
        plaintext[plaintext_len]='\0';
        gtk_text_buffer_set_text(text_buffer, (const char *)plaintext, -1);
    } else {
        gtk_text_buffer_set_text(text_buffer, "Decryption failed.", -1);
    }

    free(ciphertext);
    free(plaintext);
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
int encrypt(const unsigned char *plaintext, int plaintext_len, unsigned char *ciphertext, const unsigned char *key, unsigned char *iv) {
    EVP_CIPHER_CTX *ctx;
    int len, ciphertext_len;

    if (!(ctx=EVP_CIPHER_CTX_new())) return -1;
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) return -1;
    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len)) return -1;
    ciphertext_len=len;
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) return -1;
    ciphertext_len += len;
    EVP_CIPHER_CTX_free(ctx);
    return ciphertext_len;
}

// AES decryption function using EVP
int decrypt(const unsigned char *ciphertext, int ciphertext_len, unsigned char *plaintext, const unsigned char *key, const unsigned char *iv) {
    EVP_CIPHER_CTX *ctx;
    int len, plaintext_len;

    if (!(ctx=EVP_CIPHER_CTX_new())) return -1;
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) return -1;
    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len)) return -1;
    plaintext_len=len;
    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) return -1;
    plaintext_len += len;
    EVP_CIPHER_CTX_free(ctx);
    return plaintext_len;
}

