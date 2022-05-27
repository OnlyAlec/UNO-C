#include <gtk/gtk.h>
#include <gst/gst.h>
#include <gst/video/videooverlay.h>
#include <gdk/gdkwin32.h>

static GstElement *playbin, *play, *sink;
GObject *windowMain;

void TESTING();
void motionCardMovement();
void menuhowPlay();
void move();

char* fullPath( char * partialPath )
{
  int i=0;
  char temp[_MAX_PATH], *full = &temp[0];
  if( _fullpath( full, partialPath, _MAX_PATH ) != NULL ){
    g_print( "Ruta completa Windows: %s\n", full);
      while (full[i]!='\0') {
        if(full[i]=='\\')
          full[i]='/';
        i++;
      }
    g_print( "Ruta completa Linux: %s\n", full);
    return full;
  }
  else{
     g_print( "Ruta Invalida\n" );
     return NULL;
  }
}

gboolean bus_callback(GstBus *bus, GstMessage *msg, gpointer data) {
  GstElement *play = GST_ELEMENT(data);
  switch (GST_MESSAGE_TYPE(msg))
  {
  case GST_MESSAGE_EOS:
      /* Repite si llega al final con un EOF */
      if (!gst_element_seek(play,
                  1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
                  GST_SEEK_TYPE_SET,  2000000000, //2 seg
                  GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE)) {
          g_print("Seek failed!\n");
      }
      break;
  default:
      break;
  }
  return TRUE;
}


static void activate(GtkApplication *app, gpointer user_data) {

   GtkBuilder *builder;
  GObject *boxA, *boxAB, *boxI, *boxD, *event, *img;
  GError *error = NULL;
  GtkWidget *window;
  g_print("Inicio del juego\n");
 
   builder = gtk_builder_new ();
  if (gtk_builder_add_from_file (builder, "XML/menuplay.glade", &error) == 0) {
    g_printerr ("Error loading file: %s\n", error->message);
    g_clear_error (&error);
    system("pause");
  }

  windowMain = gtk_builder_get_object (builder, "playmenu");
  // g_signal_connect (windowMain, "destroy", G_CALLBACK (gtk_main_quit), NULL);
    /* Generales */
  gtk_builder_connect_signals(builder, NULL);

  boxA = gtk_builder_get_object(builder, "CartasArriba");
  boxAB = gtk_builder_get_object(builder, "CartasAbajo");
  boxI = gtk_builder_get_object(builder, "CartaDerecha");
  boxD = gtk_builder_get_object(builder, "CartasIzquierda");

  event = gtk_builder_get_object(builder, "Card4");

  g_signal_connect(event, "button-release-event", G_CALLBACK(move), NULL);
  
  gtk_widget_show_all(GTK_WIDGET(windowMain));
  g_object_unref (builder);

}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("com.bck.UNO", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    g_application_run(G_APPLICATION(app), argc, argv);
    return 0;
}



void move(GObject*Button, GObject*img){
    gtk_widget_set_halign(Button, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(Button, GTK_ALIGN_CENTER);
    
    gtk_widget_set_halign(img, GTK_ALIGN_START);
    gtk_widget_set_valign(img, GTK_ALIGN_START);

    Sleep(10);
    gtk_widget_hide(Button);
}
