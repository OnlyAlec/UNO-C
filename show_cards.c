#include <gtk/gtk.h>
/*   Global   */
int cardNum=0;
/*    Directorio de Imagenes   */
static const gchar *image_names[] = {
  "assets\\small\\blue_0.png",
  "assets\\small\\blue_1.png",
  "assets\\small\\blue_2.png",
  "assets\\small\\blue_3.png",
  "assets\\small\\blue_4.png",
  "assets\\small\\blue_5.png",
  "assets\\small\\blue_6.png",
  "assets\\small\\blue_7.png",
  "assets\\small\\blue_8.png",
  "assets\\small\\blue_9.png",
  "assets\\small\\blue_picker.png",
  "assets\\small\\blue_reverse.png",
  "assets\\small\\blue_skip.png",
  "assets\\small\\green_0.png",
  "assets\\small\\green_1.png",
  "assets\\small\\green_2.png",
  "assets\\small\\green_3.png",
  "assets\\small\\green_4.png",
  "assets\\small\\green_5.png",
  "assets\\small\\green_6.png",
  "assets\\small\\green_7.png",
  "assets\\small\\green_8.png",
  "assets\\small\\green_9.png",
  "assets\\small\\green_picker.png",
  "assets\\small\\green_reverse.png",
  "assets\\small\\green_skip.png",
  "assets\\small\\red_0.png",
  "assets\\small\\red_1.png",
  "assets\\small\\red_2.png",
  "assets\\small\\red_3.png",
  "assets\\small\\red_4.png",
  "assets\\small\\red_5.png",
  "assets\\small\\red_6.png",
  "assets\\small\\red_7.png",
  "assets\\small\\red_8.png",
  "assets\\small\\red_9.png",
  "assets\\small\\red_picker.png",
  "assets\\small\\red_reverse.png",
  "assets\\small\\red_skip.png",
  "assets\\small\\yellow_0.png",
  "assets\\small\\yellow_1.png",
  "assets\\small\\yellow_2.png",
  "assets\\small\\yellow_3.png",
  "assets\\small\\yellow_4.png",
  "assets\\small\\yellow_5.png",
  "assets\\small\\yellow_6.png",
  "assets\\small\\yellow_7.png",
  "assets\\small\\yellow_8.png",
  "assets\\small\\yellow_9.png",
  "assets\\small\\yellow_picker.png",
  "assets\\small\\yellow_reverse.png",
  "assets\\small\\yellow_skip.png",
  "assets\\small\\wild_color_changer.png",
  "assets\\small\\wild_pick_four.png",
  "assets\\small\\card_back_alt.png",
  "assets\\small\\card_back.png"
};


static void
change_c (GtkWidget *widget,
          gpointer   data)
{
  GtkBuilder* builder = gtk_builder_new ();
  gtk_builder_add_from_file (builder, "Glade/UI-Card.glade", NULL);
  GObject* window = gtk_builder_get_object (builder, "window");

  const gchar* accionName = gtk_widget_get_name (widget);
  g_print ("Cambiando Carta - %s\n", accionName);

  if (strcmp(accionName, "Despues") == 0 && cardNum >= 0) {
    cardNum++;
    gtk_image_set_from_file(GTK_IMAGE(data), image_names[cardNum]);
  } else if (!(cardNum == 0)) {
    cardNum--;
    gtk_image_set_from_file(GTK_IMAGE(data), image_names[cardNum]);
  } else {
    g_print ("No hay mas cartas!\n");
    gtk_image_set_from_file(GTK_IMAGE(data), NULL);
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    GtkWidget* dialog = gtk_message_dialog_new (GTK_WINDOW(window),
                                  flags,
                                  GTK_MESSAGE_ERROR,
                                  GTK_BUTTONS_CLOSE,
                                  "No hay mas cartas!");

 // Destroy the dialog when the user responds to it
 // (e.g. clicks a button)
 g_signal_connect_swapped (dialog, "response",
                           G_CALLBACK (gtk_widget_destroy),
                           dialog);
  }
}

int main (int   argc,
      char *argv[])
{
  GtkBuilder *builder;
  GObject *window, *button, *img;
  GError *error = NULL;
  gchar *filename;

  gtk_init (&argc, &argv);

  /*   Integracion de XML    */
  builder = gtk_builder_new ();
  if (gtk_builder_add_from_file (builder, "Glade/UI-Card.glade", &error) == 0)
    {
      g_printerr ("Error loading file: %s\n", error->message);
      g_clear_error (&error);
      system("pause");
      return 1;
    }

  /*   Conexion de señales    */
  img = gtk_builder_get_object (builder, "image1");
  // g_signal_connect (img, "draw", G_CALLBACK (get_file), &filename);


  window = gtk_builder_get_object (builder, "window");
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

  button = gtk_builder_get_object (builder, "button1");
  g_signal_connect (button, "clicked", G_CALLBACK (change_c), img);
  gtk_widget_set_name (GTK_WIDGET(button), "Antes");

  button = gtk_builder_get_object (builder, "button2");
  g_signal_connect (button, "clicked", G_CALLBACK (change_c), img);
  gtk_widget_set_name (GTK_WIDGET(button), "Despues");

  gtk_main ();

  return 0;
}
