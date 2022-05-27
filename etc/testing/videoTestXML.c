#include <gtk/gtk.h>
#include <gst/gst.h>
#include <gst/video/videooverlay.h>
#include <gdk/gdkwin32.h>

static GstElement *playbin, *play, *sink;

void TESTING();
void motionCardMovement();
void menuhowPlay();


char* fullPath( char * partialPath ){
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
  GstElement *plays = GST_ELEMENT(data);
  switch (GST_MESSAGE_TYPE(msg))
  {
  case GST_MESSAGE_EOS:
      /* Repite si llega al final con un EOF */
      g_print("Reiniciando LOOP!\n");
      if (!gst_element_seek(plays,
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
    GError *error = NULL;
    GtkBuilder *builder = gtk_builder_new();
    GObject *window, *button, *overlayArea;
    GtkWidget *video_drawing_area = gtk_drawing_area_new();

    /*   Integracion de XML    */
    builder = gtk_builder_new ();
    if (gtk_builder_add_from_file (builder, "XML/testing.glade", &error) == 0) {
      g_printerr ("Error loading file: %s\n", error->message);
      g_clear_error (&error);
      system("pause");
    }

    /*   Ventana Principal    */
    window = gtk_builder_get_object (builder, "main");
    // g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    overlayArea = gtk_builder_get_object (builder, "Overlay");
    g_object_get (sink, "widget", &video_drawing_area, NULL);
    gtk_widget_set_size_request(video_drawing_area, 1280, 720);
    gtk_container_add(GTK_CONTAINER(overlayArea), video_drawing_area);

    button = gtk_builder_get_object (builder, "CentralMotion");
    g_signal_connect(button, "button-release-event", G_CALLBACK(TESTING), NULL);
    g_signal_connect (button, "motion-notify-event", G_CALLBACK (motionCardMovement), NULL);

    button = gtk_builder_get_object (builder, "RightMotion");
    g_signal_connect (button, "motion-notify-event", G_CALLBACK (motionCardMovement), NULL);
    g_signal_connect (button, "button-release-event", G_CALLBACK (gtk_main_quit), NULL);

    button = gtk_builder_get_object (builder, "LeftMotion");
    g_signal_connect (button, "motion-notify-event", G_CALLBACK (TESTING), NULL);
    // g_signal_connect (button, "button-release-event", G_CALLBACK (gtk_main_quit), NULL);

    gst_element_set_state (play, GST_STATE_PLAYING);
    gtk_widget_show_all(GTK_WIDGET(window));
}

int main(int argc, char **argv) {

    // GMainLoop *loop;
    GstBus *bus;
    char file[_MAX_PATH] = "file:///";
    strcat(file, fullPath("assets\\backgrounds\\blue.mp4"));
    gtk_init (&argc, &argv);
    gst_init (&argc, &argv);
    // loop = g_main_loop_new (NULL, FALSE);

    play = gst_element_factory_make ("playbin", "play");
    g_object_set (G_OBJECT (play), "uri", file, NULL);

    sink = gst_element_factory_make ("gtksink", NULL);
    g_object_set(G_OBJECT(play), "video-sink", sink, NULL);

    bus = gst_pipeline_get_bus (GST_PIPELINE (play));
    gst_bus_add_watch (bus, bus_callback, play);
    gst_object_unref (bus);

    activate(NULL, NULL);
    gtk_main();

    gst_element_set_state (play, GST_STATE_NULL);
    gst_object_unref (GST_OBJECT (play));
    system("pause");
    return 0;
}


void TESTING(){
  g_print("\nDISCULPE DULCE DAMISELA, SERIA TAN AMABLE DE USAR MI ROSTRO COMO OBJETO DE REPOSO PARA SU ABUNDANTE RETAGUARIDA :)\n");
}

void motionCardMovement(){
  g_print("Motion on Card Triggered!");
}


void menuhowPlay(GObject *buttonInit, gpointer user_data) {
  /*   Cierre de ventana   */
  GtkWindow *windowClose;
  windowClose = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(buttonInit)));
  if(GTK_IS_WINDOW(windowClose)){
    g_print("Se cierra: %s", gtk_window_get_title (GTK_WINDOW (windowClose)));
    gtk_widget_destroy(GTK_WIDGET(windowClose));
  }
  /*   Integracion del XML   */
  GtkBuilder *builder;
  GError *error = NULL;
  builder = gtk_builder_new ();
  if (gtk_builder_add_from_file (builder, "XML/ComoJugar.glade", &error) == 0) {
    g_printerr ("Error loading file: %s\n", error->message);
    g_clear_error (&error);
    system("pause");
  }
  /*   Ventana   */
  GtkWidget *window;
  window = GTK_WIDGET(gtk_builder_get_object (builder, "ComoJugarW"));
  // g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
  gtk_widget_show_all(GTK_WIDGET(window));
}