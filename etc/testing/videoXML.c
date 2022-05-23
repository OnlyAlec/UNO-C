/**
 * @file videoXML.c
 * @author Alec Chacon (a.ct@lasallistas.org.mx)
 * @brief ULSA - Ingenieria Cibernetica
 * @version 2.1
 * @date 2022-05-16
 *
 * @copyright Copyright (c) 2022
 *
 */

/*   Archivo de prueba donde solo se coloca un video con GTK por medio de XML
     *  Se esta usando el archivo testing.glade como XML
     *  FIXME: Por el momento el problema es que el video se queda hasta adelante dejando atras los demas elementos.
*/

#include <gtk/gtk.h>
#include <gst/gst.h>
#include <gst/video/videooverlay.h>

/*   Hacerlo multi-plataforma   */
#include <gdk/gdk.h>
#if defined (GDK_WINDOWING_X11)
#include <gdk/gdkx.h>
#elif defined (GDK_WINDOWING_WIN32)
#include <gdk/gdkwin32.h>
#elif defined (GDK_WINDOWING_QUARTZ)
#include <gdk/gdkquartz.h>
#endif

/*   Data Manegment   */
typedef struct _CustomData {
  GstElement *playbin;           /* Our one and only pipeline */

  GtkWidget *slider;              /* Slider widget to keep track of current position */
  GtkWidget *streams_list;        /* Text widget to display info about the streams */
  gulong slider_update_signal_id; /* Signal ID for the slider update signal */

  GstState state;                 /* Current state of the pipeline */
  gint64 duration;                /* Duration of the clip, in nanoseconds */
} CustomData;

/*   Fn Propias   */
char* fullPath( char * partialPath ) {
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

/*   GST Fn   */
static void realize_cb (GtkWidget *widget, CustomData *data) {
  GdkWindow *window = gtk_widget_get_window (widget);
  guintptr window_handle;

  if (!gdk_window_ensure_native (window))
    g_error ("Couldn't create native window needed for GstVideoOverlay!");

  /* Retrieve window handler from GDK */
#if defined (GDK_WINDOWING_WIN32)
  window_handle = (guintptr)GDK_WINDOW_HWND (window);
#elif defined (GDK_WINDOWING_QUARTZ)
  window_handle = gdk_quartz_window_get_nsview (window);
#elif defined (GDK_WINDOWING_X11)
  window_handle = GDK_WINDOW_XID (window);
#endif
  /* Pass it to playbin, which implements VideoOverlay and will forward it to the video sink */
  gst_video_overlay_set_window_handle (GST_VIDEO_OVERLAY (data->playbin), window_handle);
}


static void create_ui (CustomData *data) {
  GtkWidget *main_window;  /* The uppermost window, containing all other windows */
  GtkWidget *video_window; /* The drawing area where the video will be shown */
//   GtkWidget *main_box;     /* VBox to hold main_hbox and the controls */
//   GtkWidget *main_hbox;    /* HBox to hold the video_window and the stream info text widget */
//   GtkWidget *controls;     /* HBox to hold the buttons and the slider */
//   GtkWidget *play_button, *pause_button, *stop_button; /* Buttons */


  /* Al obtener un objeto del XML me regresa un GObjecr, necesto castearlo a un GtkWidget para tenerlo absoluto, asi ocupo el cast GTK_WIDGET() */
  GtkBuilder *builder;
  GError *error = NULL;

  builder = gtk_builder_new ();
  if (gtk_builder_add_from_file (builder, "../../XML/testing.glade", &error) == 0) {
    g_printerr ("Error loading file: %s\n", error->message);
    g_clear_error (&error);
    system("pause");
  }
  main_window = GTK_WIDGET(gtk_builder_get_object (builder, "main"));
  g_signal_connect (G_OBJECT (main_window), "destroy", G_CALLBACK (gtk_main_quit), data);

  video_window = GTK_WIDGET(gtk_builder_get_object (builder, "BG"));
  g_signal_connect (G_OBJECT(video_window), "realize", G_CALLBACK (realize_cb), data);
//   g_signal_connect (G_OBJECT(video_window), "draw", G_CALLBACK (draw_cb), data);
  gtk_window_set_default_size (GTK_WINDOW (main_window), 970, 577);
  gtk_widget_show_all (main_window);

}

/* This function is called when an error message is posted on the bus */
static void error_cb (GstBus *bus, GstMessage *msg, CustomData *data) {
  GError *err;
  gchar *debug_info;

  /* Print error details on the screen */
  gst_message_parse_error (msg, &err, &debug_info);
  g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
  g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
  g_clear_error (&err);
  g_free (debug_info);

  /* Set the pipeline to READY (which stops playback) */
  gst_element_set_state (data->playbin, GST_STATE_READY);
}

/* La funcion se activa cuando el archivo llega a un  EOF, por lo tanto lo ponemos en
 * pausa para mover el apuntador del  video hasta el inicio y al final reproducirlo de
 * nuevo. Simulando un loop */
static void eos_cb (GstBus *bus, GstMessage *msg, CustomData *data) {
  g_print ("End-Of-Stream reached.\n");
  gst_element_set_state (data->playbin, GST_STATE_PAUSED);
  /*   Loop Video   */
  if (!gst_element_seek(data->playbin,
              1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
              GST_SEEK_TYPE_SET,  0000000000, //2 seg
              GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE)) {
      g_print("Seek failed!\n");
  }
  g_print ("Start-Of-Stream reached.\n");
  gst_element_set_state (data->playbin, GST_STATE_PLAYING);
}

int main(int argc, char *argv[]) {
  CustomData data;
  GstStateChangeReturn ret;
  GstBus *bus;
  char file[_MAX_PATH] = "file:///";
  strcat(file, fullPath("..\\..\\assets\\backgrounds\\Blue.mp4"));

  /* Initialize GTK  && GStreamer */
  gtk_init (&argc, &argv);
  gst_init (&argc, &argv);

  /* Initialize our data structure */
  memset (&data, 0, sizeof (data));
  data.duration = GST_CLOCK_TIME_NONE;

  /* Create the elements */
  data.playbin = gst_element_factory_make ("playbin", "playbin");

  if (!data.playbin) {
    g_printerr ("Not all elements could be created.\n");
    return -1;
  }

  /* Set the URI to play */
  g_object_set (data.playbin, "uri", file, NULL);

  /* Create the GUI */
  create_ui (&data);

  /* Instruct the bus to emit signals for each received message, and connect to the interesting signals */
  bus = gst_element_get_bus (data.playbin);
  gst_bus_add_signal_watch (bus);
  g_signal_connect (G_OBJECT (bus), "message::error", (GCallback)error_cb, &data);
  g_signal_connect (G_OBJECT (bus), "message::eos", (GCallback)eos_cb, &data);
  gst_object_unref (bus);

  /* Start playing */
  ret = gst_element_set_state (data.playbin, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr ("Unable to set the pipeline to the playing state.\n");
    gst_object_unref (data.playbin);
    return -1;
  }

  /* Start the GTK main loop. We will not regain control until gtk_main_quit is called. */
  gtk_main ();

  /*   Clean    */
  gst_element_set_state (data.playbin, GST_STATE_NULL);
  gst_object_unref (data.playbin);
  return 0;
}