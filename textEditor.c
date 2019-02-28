#include <gtk/gtk.h>

static void
print_hello (GtkWidget *widget, gpointer data)
{
    g_print ("Hello World\n");
}

static void activate (GtkApplication *app, gpointer user_data)
{
    GtkWidget* window;
    GtkWidget* button;
    GtkWidget* button_box;
    void* notebook;
    GtkWidget* box;
    GtkWidget* text_view;
    GtkTextBuffer *buffer; 

    
    // Set Window
    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "Window");
    gtk_window_set_default_size (GTK_WINDOW (window), 500, 500);

    /*button_box = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);*/
    /*
    button = gtk_button_new_with_label ("Hello World");
    g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);
    g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy), window);
    gtk_container_add (GTK_CONTAINER (button_box), button); */
    
    // Set Box Container
    box=gtk_box_new(TRUE,20);

    // Set TextWidget
    text_view=gtk_text_view_new();
    gtk_box_pack_start(GTK_BOX(box),text_view,1,1,0); // Pack text widget into box
    // Still need to implement the buffer part

    // Set button widget and assign functions - events
    button = gtk_button_new_with_label (":D");
    g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);
    g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy), window);
    
    // Pack button into box
    gtk_box_pack_start(GTK_BOX(box),button,1,1,0);

    // Set notebook widget - for tabbed window
    notebook=gtk_notebook_new();

    // Add box containing the textWidget and button as the first tab of the notebook
    gtk_notebook_append_page(notebook,box,NULL);

    // Add notebook to the window.
    gtk_container_add(GTK_CONTAINER (window),GTK_WIDGET(notebook));

    // Display the window with all its child elements
    gtk_widget_show_all (window);
}

int main (int argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new ("simple.textEditor", G_APPLICATION_FLAGS_NONE);

    // The activate function is called when the activate event is triggered, displaying the texEditor
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);

    // Status has the error code associated with the application
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    // Error code returned
    return status;
} 

//Things to implement - SaveFile, Add tab, Open File. This's probably enough for the project.
// Other things we could do, search for words, syntax highlighting based on language, automatic language detection,
// Find and replace words, maybe compile button for C? Line numbering.