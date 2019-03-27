#include <gtk/gtk.h>
#include<string.h>
#include<stdio.h>
typedef struct
{
    GtkWidget* menu;
    GtkWidget* menuLabel;
    GtkWidget* new;
    GtkWidget* open;
    GtkWidget* save;
    GtkWidget* saveAs;
    GtkWidget* close;
}fileMenu;

typedef struct PageUI
{
    GtkWidget* box;
    GtkWidget* text_view;
    GtkTextBuffer* buffer;
    char* tabLabel;
    char* filePath;
    struct PageUI* next;
}pageUI;

typedef struct 
{
    GtkWidget* window;
    GtkWidget* menubar;
    fileMenu* filemenu;
    GtkWidget* button;
    GtkWidget* button_box;
    GtkWidget* notebook;
    GtkWidget* box;
}TextEditorUI;

pageUI* pageBegin=NULL;
pageUI* pageEnd=NULL;
int numberOfPages=0;

static void initaliseTextEditor(TextEditorUI*);
static void addPage();
static void removePage();
static void openFile();
static void saveFile();
static void createMenu(TextEditorUI*);
pageUI* obtainCurrentPage();
static void saveFileAs();
char* getFileContents(char*);
void setFileName(char*,pageUI*);


TextEditorUI app;

int main (int argc, char **argv)
{
    gtk_init(&argc,&argv);

    initaliseTextEditor(&app);

    // Display the window with all its child elements
    gtk_widget_show_all (app.window);
    // The activate function is called when the activate event is triggered, displaying the texEditor
    gtk_main();
    
    return 0;
} 


static void initaliseTextEditor(TextEditorUI* app)
{
    // Set Window
    app->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (app->window), "Window");
    gtk_window_set_default_size (GTK_WINDOW (app->window), 500, 500);

    // Set Box Container
    app->box=gtk_box_new(TRUE,20);

    // Set MnuBar
    app->menubar=gtk_menu_bar_new();
    createMenu(app);
    gtk_box_pack_start(GTK_BOX(app->box),app->menubar,0,0,0);


    #if 0
    // Set button widget and assign functions - events
    app->button = gtk_button_new_with_label (":D");
    g_signal_connect (app->button, "clicked", G_CALLBACK (print_hello), NULL);
    g_signal_connect_swapped (app->button, "clicked", G_CALLBACK (gtk_widget_destroy), app->window);
    #endif
    

    // Set notebook widget - for tabbed window
    app->notebook=gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(app->box),app->notebook,0,0,0);
    
    

    // Add notebook to the window.
    gtk_container_add(GTK_CONTAINER (app->window),GTK_WIDGET(app->box));
}
static void createMenu(TextEditorUI* app)
{
    fileMenu* file;

    app->filemenu=(fileMenu*)malloc(sizeof(fileMenu));
    file=app->filemenu;

    file->menu=gtk_menu_new();
    
    file->menuLabel=gtk_menu_item_new_with_label("File");

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file->menuLabel),file->menu);

    gtk_menu_shell_append(GTK_MENU_SHELL(app->menubar),file->menuLabel);

    file->new=gtk_menu_item_new_with_label("New");
    file->open=gtk_menu_item_new_with_label("Open");
    file->save=gtk_menu_item_new_with_label("Save");
    file->saveAs=gtk_menu_item_new_with_label("Save As");
    file->close=gtk_menu_item_new_with_label("Close");

    gtk_menu_shell_append(GTK_MENU_SHELL(file->menu),file->new);
    gtk_menu_shell_append(GTK_MENU_SHELL(file->menu),file->open);
    gtk_menu_shell_append(GTK_MENU_SHELL(file->menu),file->save);
    gtk_menu_shell_append(GTK_MENU_SHELL(file->menu),file->saveAs);
    gtk_menu_shell_append(GTK_MENU_SHELL(file->menu),file->close);

    g_signal_connect(file->new, "activate", G_CALLBACK(addPage),NULL);
    g_signal_connect(file->close, "activate", G_CALLBACK(removePage),NULL);
    g_signal_connect(file->open, "activate", G_CALLBACK(openFile),NULL);
    g_signal_connect(file->save, "activate", G_CALLBACK(saveFile),NULL);
    g_signal_connect(file->saveAs, "activate", G_CALLBACK(saveFileAs),NULL);
}
static void addPage()
{
    pageUI* page=(pageUI*)malloc(sizeof(pageUI));
    page->box=gtk_box_new(TRUE,20);
    
    // Set TextWidget
    page->text_view=gtk_text_view_new();
    gtk_box_pack_start(GTK_BOX(page->box),page->text_view,1,1,0); 
    
    page->buffer=gtk_text_view_get_buffer (GTK_TEXT_VIEW (page->text_view)); // Sets buffer to textView

    page->tabLabel=NULL;
    page->filePath=NULL;

    // Add box containing the textWidget as the first tab of the notebook
    gtk_notebook_append_page(GTK_NOTEBOOK(app.notebook),page->box,NULL);
    gtk_widget_show_all (app.window);

    
    // Append to LinkedList
    if(pageBegin==NULL)
        pageBegin=page;
    else
        pageEnd->next=page;
    pageEnd=page;
    pageEnd->next=NULL;
    ++numberOfPages;
}

static void removePage()
{
    if(numberOfPages)
    {
        gint currentPageNumber=gtk_notebook_get_current_page(GTK_NOTEBOOK(app.notebook));
        printf("%d",currentPageNumber);
        gtk_notebook_remove_page(GTK_NOTEBOOK(app.notebook),currentPageNumber);

        // Remove from LinkedList
        pageUI* temp=pageBegin;
        if(currentPageNumber==0) // If it's the first node
        {
            pageBegin=pageBegin->next;
            temp->next=0;
            free(temp);
            temp=0;
        }
        else if(currentPageNumber==numberOfPages-1) // If it's the last node
        {
            while(temp->next!=pageEnd)
                temp=temp->next;
            temp->next=0;            
            free(pageEnd);
            pageEnd=temp;
        }
        else // If the node's in between
        {
            int pageCount=0;
            while(pageCount!=currentPageNumber-1)
            {
                ++pageCount;
                temp=temp->next;
            }
            pageUI* tempNext=temp->next; // Node to be deleted
            temp->next=tempNext->next;
            tempNext->next=0;
            free(tempNext);
        }
        --numberOfPages;
    }
}
static void openFile()
{
    addPage(); // A new tab must be created
    GtkWidget *dialog; // The open file dialog box
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN; // Either to open/save. Open in this case.
    gint res; // The response
    dialog=gtk_file_chooser_dialog_new("Open File",
                                        GTK_WINDOW(app.window),
                                        action,("_Cancel"),
                                        GTK_RESPONSE_CANCEL,
                                        ("_Open"),
                                        GTK_RESPONSE_ACCEPT,
                                        NULL);

    res = gtk_dialog_run (GTK_DIALOG (dialog));
    if (res == GTK_RESPONSE_ACCEPT) // If user has selected a file
    {
        GtkFileChooser* chooser = GTK_FILE_CHOOSER (dialog);
        pageEnd->filePath = gtk_file_chooser_get_filename (chooser); // Obtain chosen file path
        char* fileContents=getFileContents(pageEnd->filePath); // Obtain file contents
        setFileName(pageEnd->filePath,pageEnd); // Set file name from file path
        gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(app.notebook),GTK_WIDGET(pageEnd->box), pageEnd->tabLabel); 
        gtk_text_buffer_set_text(pageEnd->buffer, fileContents, -1);
        free(fileContents);
    }
    gtk_widget_destroy (dialog);
}
void setFileName(char* filePath,pageUI* page) // Function to find fileName given the file path
{
    while(filePath[0]) // Go to end of string
        ++filePath;
    while(filePath[0]!='/') // Find the last occurence of '/'
        --filePath;
    page->tabLabel=filePath+1;
}
char* getFileContents(char* filePath)
{
    FILE* f=fopen(filePath,"r");
    long long int fileLength;
    if(f)
    {
        fseek(f,0,SEEK_END); // Move pointer to end of file.
        fileLength=ftell(f); // Count number of bytes.
        fseek(f,0,SEEK_SET); // Move pointer back to start of file.
        char* fileContents=(char*)malloc(fileLength);
        fread(fileContents,1,fileLength,f);
        fclose(f);
        return fileContents;
    }
}
pageUI* obtainCurrentPage()
{
    gint currentPageNumber=gtk_notebook_get_current_page(GTK_NOTEBOOK(app.notebook));
    int pageCount=0;
    pageUI* tempPage=pageBegin;
    for(;pageCount<currentPageNumber;++pageCount) // Find the current page in the linked list.
        tempPage=tempPage->next; 
    return tempPage;
}
void saveFile()
{   
    pageUI* page=obtainCurrentPage();

    if(page->filePath) // If the file has already been saved, then save to known filePath
    {
        FILE* outputFile=fopen(page->filePath,"w"); // Open known file
        // Obtain text_view buffer
        GtkTextIter start,end;
        gchar* fileContent;
        gtk_text_buffer_get_start_iter(page->buffer,&start);
        gtk_text_buffer_get_end_iter(page->buffer,&end);
        fileContent=gtk_text_buffer_get_text(page->buffer,&start,&end,FALSE);

        fprintf(outputFile,"%s",fileContent); // Write to file
        fclose(outputFile);
    }
    else
    {
        saveFileAs();
    }
}
void saveFileAs()
{
    pageUI* page=obtainCurrentPage();

    // Obtain text_view buffer
    GtkTextIter start,end;
    gchar* fileContent;
    gtk_text_buffer_get_start_iter(page->buffer,&start);
    gtk_text_buffer_get_end_iter(page->buffer,&end);
    fileContent=gtk_text_buffer_get_text(page->buffer,&start,&end,FALSE);

    GtkWidget *dialog;
    GtkFileChooser *chooser;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    gint res;
    dialog=gtk_file_chooser_dialog_new("Save File",
                                        GTK_WINDOW(app.window),
                                        action,
                                        ("_Cancel"),
                                        GTK_RESPONSE_CANCEL,
                                        ("_Save"),
                                        GTK_RESPONSE_ACCEPT,
                                        NULL);
    chooser = GTK_FILE_CHOOSER (dialog);
    gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);

    if (page->tabLabel==NULL) // If new file
        gtk_file_chooser_set_current_name (chooser,"Untitled document"); 
    else // File exists already
        gtk_file_chooser_set_filename (chooser,page->tabLabel);

    res = gtk_dialog_run (GTK_DIALOG (dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        page->filePath = gtk_file_chooser_get_filename (chooser); // Get path of save file
        FILE* outputFile=fopen(page->filePath,"w"); // Get file pointer to chosen file
        fprintf(outputFile,"%s",fileContent); // Save contents to file
        setFileName(page->filePath,page); // Set file name from file path
        gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(app.notebook),GTK_WIDGET(page->box), page->tabLabel);
        fclose(outputFile);
    }
    gtk_widget_destroy (dialog);
}


// Other things to could do, search for words, syntax highlighting based on language, automatic language detection,
// Find and replace words, maybe compile button for C? Line numbering.