/**
 * @file main.c
 * @author Alec Chacon (a.ct@lasallistas.org.mx)
 * @brief Ingenieria Cibernetica
 * @version 0.1
 * @date 2022-05-15
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdlib.h>
#include <gtk/gtk.h>
#include <gst/gst.h>

/*   Globales   */
GObject *windowMain;
GstElement *sink;

/*   Funciones   */
void motionCardMovement();
void menuhowPlay();
void menuPlay();
void menuLocal(GObject *, GtkBuilder* );
void profiles(GObject *, GtkBuilder* );
void playerSelectFn(GObject *, GtkBuilder* );
void SelectVSFn(GObject *, GtkBuilder* );

static void activate(GtkApplication *app, gpointer user_data) {
  GtkBuilder *builder;
  GObject *button, *img, *box, *event, *fixed;
  GtkWidget *draw;
  GError *error = NULL;

  /*   Integracion de XML    */
  builder = gtk_builder_new ();
  if (gtk_builder_add_from_file (builder, "XML/PrincipalPlus.glade", &error) == 0) {
    g_printerr ("Error loading file: %s\n", error->message);
    g_clear_error (&error);
    system("pause");
  }

  /*   Conexion de señales    */
    /* Ventana Principal */
  windowMain = gtk_builder_get_object (builder, "main");
  // g_signal_connect (windowMain, "destroy", G_CALLBACK (gtk_main_quit), NULL);
  //     /* Generales */
  gtk_builder_connect_signals(builder, NULL);
    /*   Individuales Cajas   */
  draw = GTK_WIDGET(gtk_builder_get_object (builder, "bg-video"));
  g_object_get (sink, "widget", &draw, NULL);

  box = gtk_builder_get_object (builder, "Profile");
  fixed = gtk_builder_get_object (builder, "mainFixed");

    /* Individuakes Botones */
  button = gtk_builder_get_object (builder, "Menu-Btn1");
  button = gtk_builder_get_object (builder, "Acerca");
  button = gtk_builder_get_object (builder, "SFX-Btn1");
  button = gtk_builder_get_object (builder, "Music-Btn1");

    /* Individuakes Imagenes */
  event = gtk_builder_get_object (builder, "CentralMotion");
  g_signal_connect (event, "motion-notify-event", G_CALLBACK (motionCardMovement), NULL);
  g_signal_connect (event, "button-release-event", G_CALLBACK (menuPlay), NULL);

  event = gtk_builder_get_object (builder, "RightMotion");
  g_signal_connect (event, "motion-notify-event", G_CALLBACK (motionCardMovement), NULL);

  event = gtk_builder_get_object (builder, "LeftMotion");
  g_signal_connect (event, "motion-notify-event", G_CALLBACK (motionCardMovement), NULL);
  g_signal_connect (event, "button-release-event", G_CALLBACK (menuhowPlay), NULL);

  gtk_widget_show_all(GTK_WIDGET(windowMain));
  // El comando no existe?
  // gtk_object_unref (builder);
}

char* fullPath(char *);
gboolean bus_callback(GstBus *, GstMessage *, gpointer);
// void startVideoBG(GObject *, gpointer);


int main (int argc, char *argv[]) {
  GMainLoop *loop;
  GstElement *play;
  GstBus *bus;

  char file[_MAX_PATH] = "file:///";
  strcat(file, fullPath("..\\..\\assets\\backgrounds\\blue.mp4"));
  /*   init GTK && GTS   */
  gtk_init (&argc, &argv);
  gst_init (&argc, &argv);
  loop = g_main_loop_new (NULL, FALSE);

  play = gst_element_factory_make ("playbin", NULL);
  g_object_set (G_OBJECT (play), "uri", file, NULL);

  sink = gst_element_factory_make ("gtksink", NULL);
  g_object_set(play, "video-sink", sink, NULL);

  /*   GST Set Up   */
  bus = gst_pipeline_get_bus (GST_PIPELINE (play));
  gst_bus_add_watch (bus, bus_callback, play);
  gst_object_unref (bus);

  /*  GTK Set Up   */
  // FIXME: Intentar no correr el programa como una aplicaccion, correrlo con un main()
  // FIXME: No iniciar el loop de la aplicacion (mas bien ocupar el del programa) y no iniciar el loop del GTS si lo incertas en un overley/layer/etc
  // TODO: Quitar las cajas de eventos de la ventana principal, activar el motion en las imagenes
  activate(NULL,NULL);

  gst_element_set_state (play, GST_STATE_PLAYING);
  gtk_main();
  /*   Limpieza   */
  gst_element_set_state (play, GST_STATE_NULL);
  gst_object_unref (GST_OBJECT (play));
  return 0;
}

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

gboolean bus_callback(GstBus *bus, GstMessage *msg, gpointer data) {
  GstElement *play = GST_ELEMENT(data);
  switch (GST_MESSAGE_TYPE(msg)) {
  case GST_MESSAGE_EOS:
      if (!gst_element_seek(play,
                  1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
                  GST_SEEK_TYPE_SET,  2000000000, //2 seg
                  GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE)) {
          g_print("Fallo del Seek!\n");
      }
      break;
  default:
      break;
  }
  return TRUE;
}

// void startVideoBG(GObject *video_drawing_area, gpointer user_data) {
//   // g_object_get (sink, "widget", &video_drawing_area, NULL);
//   gtk_widget_set_size_request(GTK_WIDGET(video_drawing_area), 970, 577);
// }

void motionCardMovement(){
  g_print("Motion on Card Triggered!");
}
void menuhowPlay(){
  g_print("Ventana de como jugar - como child!");

}
void menuPlay(GObject *buttonInit, gpointer user_data){
  GtkBuilder *builder;
  GdkWindow *windowClose;
  GError *error = NULL;

  /*   Cierre de ventana   */
  // FIXME: No se cierra la ventana anterior
  windowClose = gtk_widget_get_window(GTK_WIDGET(buttonInit));
  gdk_window_destroy (windowClose);
  g_print("Se cerro ventana principal!");

  /*   Integracion del XML   */
  builder = gtk_builder_new ();
  if (gtk_builder_add_from_file (builder, "XML/PreJuego.glade", &error) == 0) {
    g_printerr ("Error loading file: %s\n", error->message);
    g_clear_error (&error);
    system("pause");
  }

  /*   Obtencion de Objetos   */
  GtkWidget *window;
  GObject *button;
  /*   Ventanas   */
  window = GTK_WIDGET(gtk_builder_get_object (builder, "SelectMode"));
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
  /*   Generales   */
  gtk_builder_connect_signals(builder, NULL);
  /*   Botones   */
  button = gtk_builder_get_object (builder, "GameLocal");
  g_signal_connect (button, "clicked", G_CALLBACK (menuLocal), builder);
  button = gtk_builder_get_object (builder, "GameMult");
  // g_signal_connect (button, "clicked", G_CALLBACK (menuMult), builder);

  gtk_widget_show_all(GTK_WIDGET(window));
}

void menuLocal(GObject *buttonInit, GtkBuilder* builder){
  // TODO: Definir en la estructura el modo de juego
  // TODO: Guardar vaiables del numero de jugadores  y contra quien va a jugar
  int vs; // 0 = vs CPU, 1 = vs Player
  GtkWidget *windowClose;
  // FIXME: No se cierra la ventana anterior
    /*   Cierre de ventana   */
  windowClose = GTK_WIDGET(gtk_widget_get_window(GTK_WIDGET(buttonInit)));
  gtk_widget_destroy(windowClose);

  /*   Obtencion de Objetos   */
  GObject *buttonBot, *buttonHuman, *img;
  GtkWidget *window;

  window = GTK_WIDGET(gtk_builder_get_object (builder, "SelectPlayer"));
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

  img = gtk_builder_get_object (builder, "1");
  g_signal_connect (GTK_WIDGET(img), "clicked", G_CALLBACK(playerSelectFn), builder);
  img = gtk_builder_get_object (builder, "2");
  g_signal_connect (GTK_WIDGET(img), "clicked", G_CALLBACK(playerSelectFn), builder);
  img = gtk_builder_get_object (builder, "3");
  g_signal_connect (GTK_WIDGET(img), "clicked",  G_CALLBACK(playerSelectFn), builder);
  img = gtk_builder_get_object (builder, "4");
  g_signal_connect (GTK_WIDGET(img), "clicked", G_CALLBACK(playerSelectFn), builder);

  buttonBot = gtk_builder_get_object (builder, "Bot");
  g_signal_connect (buttonBot, "clicked", G_CALLBACK(SelectVSFn), builder);
  buttonHuman = gtk_builder_get_object (builder, "Human");
  g_signal_connect (buttonHuman, "clicked", G_CALLBACK(SelectVSFn), builder);

  gtk_widget_show_all(GTK_WIDGET(window));
  gtk_widget_hide(GTK_WIDGET(buttonBot));
  gtk_widget_hide(GTK_WIDGET(buttonHuman));
}

void profiles(GObject *buttonInit, GtkBuilder* builder){
      /*   Cierre de ventana   */
  // FIXME: No se cierra la ventana anterior
  gtk_widget_destroy(GTK_WIDGET(gtk_widget_get_window(GTK_WIDGET(buttonInit))));

  GtkWidget *window;
  window = GTK_WIDGET(gtk_builder_get_object (builder, "ProfileBuild"));
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

  /*  Repeticion de ventana   */
  /* Necesito la estructura de datos globales para poder hacer el ciclo de la funcion */
  // menuLocal(buttonInit, builder, players);
}

void playerSelectFn(GObject *playerImg, GtkBuilder* builder){
  GObject *button;
  const gchar* data;
  data = gtk_widget_get_name(GTK_WIDGET(playerImg));
  g_print("Player Select = %s!", data);
  // Meter dato a la estructura de datos globales
  /*   AQUI   */
  if (strcmp(data, "1") == 0){
    button = gtk_builder_get_object (builder, "Human");
    gtk_widget_show(GTK_WIDGET(button));
  }

  button = gtk_builder_get_object (builder, "Bot");
  gtk_widget_show(GTK_WIDGET(button));
}

void SelectVSFn(GObject *vsImg, GtkBuilder* builder){
  GObject *button;
  const gchar* data;

  data = gtk_widget_get_name(GTK_WIDGET(vsImg));
  g_print("VS Select = %s!", data);
    // Meter dato a la estructura de datos globales
    /*   AQUI   */
  playerSelectFn(vsImg,builder);
}