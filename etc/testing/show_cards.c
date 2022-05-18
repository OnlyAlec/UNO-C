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
  GError *error = NULL;
  GtkBuilder* builder = gtk_builder_new ();
  if (gtk_builder_add_from_file (builder, "XML/UI-Cardv.2.glade", &error) == 0)
    {
      g_printerr ("Error loading file: %s\n", error->message);
      g_clear_error (&error);
      system("pause");
    }

  GObject* dialog = gtk_builder_get_object (builder, "NoCards");
  gtk_builder_connect_signals(builder, NULL);

  const gchar* accionName = gtk_widget_get_name (widget);
  g_print ("Cambiando Carta - %s - #%d\n", accionName, cardNum);

  if (strcmp(accionName, "Despues") == 0 && cardNum < 55) {
    cardNum++;
    gtk_image_set_from_file(GTK_IMAGE(data), image_names[cardNum]);
  } else if (strcmp(accionName, "Antes") == 0 && cardNum > 0) {
    cardNum--;
    gtk_image_set_from_file(GTK_IMAGE(data), image_names[cardNum]);
  } else {
    g_print ("No hay mas cartas!\n");

    gtk_widget_show_all (GTK_WIDGET(dialog));
  }
}

int main (int   argc,
      char *argv[])
{
  GtkBuilder *builder;
  GObject *window, *button, *img, *dialog;
  GError *error = NULL;
  gchar *filename;

  gtk_init (&argc, &argv);

  /*   Integracion de XML    */
  builder = gtk_builder_new ();
  if (gtk_builder_add_from_file (builder, "XML/UI-Cardv.2.glade", &error) == 0)
    {
      g_printerr ("Error loading file: %s\n", error->message);
      g_clear_error (&error);
      system("pause");
      return 1;
    }

  /*   Conexion de señales    */
    /* Ventana Principal*/
  window = gtk_builder_get_object (builder, "main");
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
      /* Generales */
  gtk_builder_connect_signals(builder, NULL);

    /* Individuakes */
  img = gtk_builder_get_object (builder, "img_card");
  button = gtk_builder_get_object (builder, "c_antes");
  g_signal_connect (button, "clicked", G_CALLBACK (change_c), img);
  // gtk_widget_set_name (GTK_WIDGET(button), "Antes");

  button = gtk_builder_get_object (builder, "c_despues");
  g_signal_connect (button, "clicked", G_CALLBACK (change_c), img);
  // gtk_widget_set_name (GTK_WIDGET(button), "Despues");

  gtk_widget_show(GTK_WIDGET(window));
  gtk_main ();

  return 0;
}
