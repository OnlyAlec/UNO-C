#include <gtk/gtk.h>
// callback function which is called when button is clicked
static void on_button_clicked(GtkButton *btn, gpointer data)
{
    // change button label when it's clicked
    gtk_button_set_label(btn, "Hello World");
}
// callback function which is called when application is first started
static void on_app_activate(GApplication *app, gpointer data)
 {
    // create a new application window for the application
    // GtkApplication is sub-class of GApplication
    // downcast GApplication* to GtkApplication* with GTK_APPLICATION() macro
    GtkWidget *window = gtk_application_window_new(GTK_APPLICATION(app));
    // a simple push button
    GtkWidget *btn = gtk_button_new_with_label("Click Me!");
    // connect the event-handler for "clicked" signal of button
    g_signal_connect(btn, "clicked", G_CALLBACK(on_button_clicked), NULL);
    // add the button to the window
    gtk_container_add(GTK_CONTAINER(window), btn);
    // display the window
    gtk_widget_show_all(GTK_WIDGET(window));
}
int main(int argc, char *argv[]) 
{
    // create new GtkApplication with an unique application ID
    GtkApplication *app = gtk_application_new(
        "org.gtkmm.example.HelloApp",
        G_APPLICATION_FLAGS_NONE
    );
    // connect the event-handler for "activate" signal of GApplication
    // G_CALLBACK() macro is used to cast the callback function pointer
    // to generic void pointer
    g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), NULL);
    // start the application, terminate by closing the window
    // GtkApplication* is upcast to GApplication* with G_APPLICATION() macro
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    // deallocate the application object
    g_object_unref(app);
// Codigo cargar imagen
    GtkWidget *window, *image;
    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 250, 250);
    gtk_window_set_title(GTK_WINDOW(window), "Image");
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(window),2);
    image = gtk_image_new_from_file("blue_0_large.png");
   
    gtk_container_add(GTK_CONTAINER(window), image);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(window);
    gtk_main();
    
     gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 250, 250);
    gtk_window_set_title(GTK_WINDOW(window), "Image");
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(window),2);
    image = gtk_image_new_from_file("card_back_large.png");
   
    gtk_container_add(GTK_CONTAINER(window), image);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(window);
    gtk_main();

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 250, 250);
    gtk_window_set_title(GTK_WINDOW(window), "Image");
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    gtk_container_set_border_width(GTK_CONTAINER(window),2);

    image = gtk_image_new_from_file("wild_colora_changer_large.png");

    gtk_container_add(GTK_CONTAINER(window), image);

    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(window);
    gtk_main();




    return status;
} 
