/**
 * @file video.c
 * @author Alec Chacon (a.ct@lasallistas.org.mx)
 * @brief ULSA - Ingenieria Cibernetica
 * @version 0.1
 * @date 2022-05-13
 *
 * @copyright Copyright (c) 2022
 *
 */

/*   Archivo de prueba donde solo se coloca un video con GTK   */
#include <stdlib.h>
#include <gtk/gtk.h>
#include <gst/gst.h>

void SEXOO();

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

/*   Globales   */
GstElement *sink, *play;




static void activate(GtkApplication *app, gpointer user_data) {
  GtkWidget *window = gtk_application_window_new(app);
  gtk_window_set_default_size(GTK_WINDOW(window), 1280, 720);
  GtkWidget *root_pane = gtk_layout_new(NULL, NULL);
  gtk_container_add(GTK_CONTAINER(window), root_pane);
  GtkWidget *video_drawing_area = gtk_drawing_area_new();

    GtkWidget *Logo = gtk_image_new_from_file ("RetroUno_W.png");


    gtk_overlay_add_overlay(GTK_CONTAINER(root_pane), Logo);
    gtk_widget_set_halign(Logo, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(Logo, GTK_ALIGN_START);
    ///g_signal_connect(G_OBJECT(Logo), "clicked", G_CALLBACK(SEXOO), NULL);
  
  
  g_object_get (sink, "widget", &video_drawing_area, NULL);
  gtk_widget_set_size_request(video_drawing_area, 1280, 720);
  gtk_container_add(GTK_CONTAINER(root_pane), video_drawing_area);
  
  gst_element_set_state (play, GST_STATE_PLAYING);
  gtk_widget_show_all(window);
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





int main(int argc, char **argv) {
  GMainLoop *loop;
  GstBus *bus;

  char file[_MAX_PATH] = "file:///";
  strcat(file, fullPath("..\\..\\assets\\backgrounds\\Blue.mp4"));

  /* init GStreamer */
  gtk_init (&argc, &argv);
  gst_init (&argc, &argv);
  loop = g_main_loop_new (NULL, FALSE);

  play = gst_element_factory_make ("playbin", "play");
  g_object_set (G_OBJECT (play), "uri", file, NULL);

  sink = gst_element_factory_make ("gtksink", NULL);
  g_object_set(play, "video-sink", sink, NULL);

  /*   GST Set Up   */
  bus = gst_pipeline_get_bus (GST_PIPELINE (play));
  gst_bus_add_watch (bus, bus_callback, play);
  gst_object_unref (bus);

  /*  GTK Set Up   */
  GtkApplication *app = gtk_application_new("com.bck.videogtktest",G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  // g_main_loop_run (loop);
  g_application_run(G_APPLICATION(app), argc, argv);

  /*   Clean    */
  gst_element_set_state (play, GST_STATE_NULL);
  gst_object_unref (GST_OBJECT (play));
  system("pause");
  return 0;
}



void SEXOO(){
  g_print("\nDISCULPE DULCE DAMISELA, SERIA TAN AMABLE DE USAR MI ROSTRO COMO OBJETO DE REPOSO PARA SU ABUNDANTE RETAGUARIDA :)\n");
}