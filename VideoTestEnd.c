#include <gtk/gtk.h>
#include <gst/gst.h>
#include <gst/video/videooverlay.h>
#include <gdk/gdkwin32.h>


static GstElement *playbin, *sink, *play;
static guintptr window_handle = 0;

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

static void videoDrawingArea(GtkWidget *video_drawing_area, gpointer user_data) {
    GdkWindow *video_window = gtk_widget_get_window(video_drawing_area);
    if (!gdk_window_ensure_native(video_window)) {
        g_error("Couldn't create native window needed for GstVideoOverlay!");
    }
    window_handle = (guintptr) GDK_WINDOW_HWND(video_window);
}

static void activate(GtkApplication *app, gpointer user_data){
   GtkBuilder *builder;
   GObject *event;
   
    GtkWidget *window = gtk_application_window_new(app);
    GtkWidget *root_pane = gtk_overlay_new();
    GtkWidget *videoArea  = gtk_drawing_area_new();
    GtkWidget *Logo = gtk_image_new_from_file("RetroUno_B.png");

    event = gtk_builder_get_object (builder, "CentralMotion");

    gtk_window_set_default_size(GTK_WINDOW(window), 1280, 720);
    gtk_container_add(GTK_CONTAINER(window), root_pane);
    gtk_container_add(GTK_CONTAINER(root_pane), videoArea);
    gtk_widget_set_size_request(videoArea, 1280, 720);

    g_signal_connect(videoArea, "realize", G_CALLBACK(videoDrawingArea), NULL);

    gtk_overlay_add_overlay(GTK_CONTAINER(root_pane), Logo);

    gtk_widget_set_halign(Logo, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(Logo, GTK_ALIGN_START);
    
    g_signal_connect(event, "clicked", G_CALLBACK(SEXOO), NULL);

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

int main(int argc, char **argv){
    char file[_MAX_PATH]="file:///";
    strcat(file, fullPath("assets\\backgrounds\\blue.mp4"));

    gst_init(&argc, &argv);
    playbin = gst_element_factory_make("playbin", NULL);
    g_object_set(playbin, "uri", file, NULL);
    
    
    GtkApplication *app = gtk_application_new("com.techn.videotest", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    return g_application_run(G_APPLICATION(app), argc, argv);

}

void SEXOO(){
    g_print("HOLA, SEXO?");
}