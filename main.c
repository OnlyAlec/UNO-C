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
#include <gst/video/videooverlay.h>
#include <gdk/gdkwin32.h>

/*   Globales   */
GObject *windowMain;
static GstElement *playbin, *play, *sink;

/*   Funciones   */
void motionCardMovement(GObject *, GdkEventConfigure *, gpointer );
void profiles(GObject *, GtkBuilder* );
void playerSelectFn(GObject *, GtkBuilder* );
void SelectVSFn(GObject *, GtkBuilder* );
char* fullPath(char *);
gboolean bus_callback(GstBus *, GstMessage *, gpointer);
void returnMain(GObject*, gpointer*);

// Menus
void menuPlay(GObject *, gpointer);
void menuhowPlay(GObject *, gpointer);
void menuInstruc(GObject*, GtkBuilder* );
void menuObj(GObject*, GtkBuilder* );
void menuLocal(GObject *, GtkBuilder* );

static void activate(GtkApplication *app, gpointer user_data) {
  GtkBuilder *builder;
  GObject *button, *box, *event, *overlayArea, *img;
  GError *error = NULL;
  GtkWidget *video_drawing_area = gtk_drawing_area_new();


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
  //g_signal_connect (windowMain, "motion-notify-event", G_CALLBACK (motionCardMovement), NULL);
  //gtk_widget_add_events(GTK_WIDGET(windowMain), GDK_POINTER_MOTION_MASK);


  // g_signal_connect (windowMain, "destroy", G_CALLBACK (gtk_main_quit), NULL);
  //     /* Generales */
  gtk_builder_connect_signals(builder, NULL);
    /*   Video Area   */
    overlayArea = gtk_builder_get_object (builder, "Overlay");
    g_object_get (sink, "widget", &video_drawing_area, NULL);
    gtk_widget_set_size_request(video_drawing_area, 1280, 720);
    gtk_container_add(GTK_CONTAINER(overlayArea), video_drawing_area);

  box = gtk_builder_get_object (builder, "Profile");
    /* Individuakes Botones */
  button = gtk_builder_get_object (builder, "Menu-Btn1");
  button = gtk_builder_get_object (builder, "Acerca");
  button = gtk_builder_get_object (builder, "SFX-Btn1");
  button = gtk_builder_get_object (builder, "Music-Btn1");

    /* Individuakes Imagenes */
  event = gtk_builder_get_object (builder, "CentralMotion");
  img = gtk_builder_get_object (builder, "Central1");
  g_signal_connect (event, "button-release-event", G_CALLBACK (menuPlay), NULL);
  g_signal_connect (event, "enter_notify_event", G_CALLBACK (motionCardMovement), NULL);
  g_signal_connect (event, "leave_notify_event", G_CALLBACK (motionCardMovement), NULL);

  event = gtk_builder_get_object (builder, "LeftMotion");
  img = gtk_builder_get_object (builder, "Left1");
  g_signal_connect (event, "button-release-event", G_CALLBACK (menuhowPlay), NULL);
  g_signal_connect (event, "enter_notify_event", G_CALLBACK (motionCardMovement), NULL);
  g_signal_connect (event, "leave_notify_event", G_CALLBACK (motionCardMovement), NULL);

  event = gtk_builder_get_object (builder, "RightMotion");
  img = gtk_builder_get_object (builder, "Right1");
  g_signal_connect (event, "enter_notify_event", G_CALLBACK (motionCardMovement), NULL);
  g_signal_connect (event, "leave_notify_event", G_CALLBACK (motionCardMovement), NULL);


  gst_element_set_state (play, GST_STATE_PLAYING);
  gtk_widget_show_all(GTK_WIDGET(windowMain));
  g_object_unref (builder);
}

int main (int argc, char *argv[]) {
  GstBus *bus;

  char file[_MAX_PATH] = "file:///";
  strcat(file, fullPath("assets\\backgrounds\\Blue.mp4"));

  /*   init GTK && GTS   */
  gtk_init (&argc, &argv);
  gst_init (&argc, &argv);
  // loop = g_main_loop_new (NULL, FALSE);

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
  activate(NULL,NULL);
  gtk_main();
  /*   Limpieza   */
  gst_element_set_state (play, GST_STATE_NULL);
  gst_object_unref (GST_OBJECT (play));
  system("pause");
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
  GstElement *plays = GST_ELEMENT(data);
  switch (GST_MESSAGE_TYPE(msg))  {
  case GST_MESSAGE_EOS:
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

void motionCardMovement(GObject *eventBox, GdkEventConfigure *event, gpointer data){
  int time = 1000; // Tiempo en milisegundos
  GdkEventType eventType = event->type;
  if(event->type == GDK_ENTER_NOTIFY){
    for (int i = 0; i < 10; i++) {
      gtk_widget_set_margin_bottom(GTK_WIDGET(eventBox), 10*i);
      Sleep(10);
    }
  } else if (event->type == GDK_LEAVE_NOTIFY){
    for (int i = 10; i < 1; i--) {
      gtk_widget_set_margin_bottom(GTK_WIDGET(eventBox), 10*i);
      Sleep(10);
    }
  }
}

void menuhowPlay(GObject *buttonInit, gpointer user_data) {
  /*   Cierre de ventana   */
  GtkWindow *windowClose;
  windowClose = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(buttonInit)));
  if(GTK_IS_WINDOW(windowClose)){
    g_print("Se cierra: %s\n", gtk_window_get_title (GTK_WINDOW (windowClose)));
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
  /*   Obtencion de Objetos   */
  GtkWidget *window;
  GObject *button;
  /*   Ventanas   */
  window = GTK_WIDGET(gtk_builder_get_object (builder, "ComoJugarW"));
  // g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
  /*   Botones   */
  button = gtk_builder_get_object (builder, "Objetivo");
  g_signal_connect (button, "clicked", G_CALLBACK (menuObj), builder);
  button = gtk_builder_get_object (builder, "Instruc");
  g_signal_connect (button, "clicked", G_CALLBACK (menuInstruc), builder);

  gtk_widget_show_all(GTK_WIDGET(window));
}

void menuObj(GObject *buttonInit, GtkBuilder* builder) {
  GObject *window, *button;

  /*   Cierre de ventana   */
  GtkWindow *windowClose;
  windowClose = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(buttonInit)));
  if(GTK_IS_WINDOW(windowClose)){
    g_print("Se cierra: %s\n", gtk_window_get_title (GTK_WINDOW (windowClose)));
    gtk_widget_destroy(GTK_WIDGET(windowClose));
  }
  /*   Nuevo menu   */
  g_print("Menu Objetivos\n");
  window = gtk_builder_get_object (builder, "menuObjetivo");
  button = gtk_builder_get_object (builder, "back-btn");
  g_signal_connect (button, "clicked", G_CALLBACK (returnMain), NULL);

  gtk_widget_show_all(GTK_WIDGET(window));
  // g_object_unref(builder);
}

void menuInstruc(GObject *buttonInit, GtkBuilder* builder){
  /*   Cierre de ventana   */
  GtkWindow *windowClose;
  GObject *window, *button;
  windowClose = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(buttonInit)));
  if(GTK_IS_WINDOW(windowClose)){
    g_print("Se cierra: %s\n", gtk_window_get_title (GTK_WINDOW (windowClose)));
    gtk_widget_destroy(GTK_WIDGET(windowClose));
  }
  /*   Nuevo menu   */
  g_print("Menu instrucciones\n");
  window = gtk_builder_get_object (builder, "menuInstrucciones");
  button = gtk_builder_get_object (builder, "back-btn2");
  g_signal_connect (button, "clicked", G_CALLBACK (returnMain), NULL);

  gtk_widget_show_all(GTK_WIDGET(window));
  // g_object_unref(builder);
}

void menuPlay(GObject *buttonInit, gpointer user_data){
  /*   Cierre de ventana   */
  GtkWindow *windowClose;
  windowClose = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(buttonInit)));
  if(GTK_IS_WINDOW(windowClose)){
    g_print("Se cierra: %s\n", gtk_window_get_title (GTK_WINDOW (windowClose)));
    gtk_widget_destroy(GTK_WIDGET(windowClose));
  }
  /*   Integracion del XML   */
  GtkBuilder *builder;
  GError *error = NULL;
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
  // g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
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
    /*   Cierre de ventana   */
  GtkWindow *windowClose;
  windowClose = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(buttonInit)));
  if(GTK_IS_WINDOW(windowClose)){
    g_print("Se cierra: %s\n", gtk_window_get_title (GTK_WINDOW (windowClose)));
    gtk_widget_destroy(GTK_WIDGET(windowClose));
  }
  /*   Obtencion de Objetos   */
  GObject *buttonBot, *buttonHuman, *Eventimg;
  GtkWidget *window;

  window = GTK_WIDGET(gtk_builder_get_object (builder, "SelectPlayer"));
  // g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

  // TODO: Pasar los eventos a una caja de eventos
  Eventimg = gtk_builder_get_object (builder, "1P");
  g_signal_connect (GTK_WIDGET(Eventimg), "button-release-event", G_CALLBACK(playerSelectFn), builder);
  gtk_widget_set_name(GTK_WIDGET(Eventimg), "1P");
  Eventimg = gtk_builder_get_object (builder, "2P");
  g_signal_connect (GTK_WIDGET(Eventimg), "button-release-event", G_CALLBACK(playerSelectFn), builder);
  gtk_widget_set_name(GTK_WIDGET(Eventimg), "2P");
  Eventimg = gtk_builder_get_object (builder, "3P");
  g_signal_connect (GTK_WIDGET(Eventimg), "button-release-event",  G_CALLBACK(playerSelectFn), builder);
  gtk_widget_set_name(GTK_WIDGET(Eventimg), "3P");
  Eventimg = gtk_builder_get_object (builder, "4P");
  g_signal_connect (GTK_WIDGET(Eventimg), "button-release-event", G_CALLBACK(playerSelectFn), builder);
  gtk_widget_set_name(GTK_WIDGET(Eventimg), "4P");

  buttonBot = gtk_builder_get_object (builder, "Bot");
  g_signal_connect (buttonBot, "button-release-event", G_CALLBACK(SelectVSFn), builder);
  buttonHuman = gtk_builder_get_object (builder, "Human");
  g_signal_connect (buttonHuman, "button-release-event", G_CALLBACK(SelectVSFn), builder);

  gtk_widget_show_all(GTK_WIDGET(window));
  gtk_widget_hide(GTK_WIDGET(buttonBot));
  gtk_widget_hide(GTK_WIDGET(buttonHuman));
}

void profiles(GObject *buttonInit, GtkBuilder* builder){
      /*   Cierre de ventana   */
  GtkWindow *windowClose;
  windowClose = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(buttonInit)));
  if(GTK_IS_WINDOW(windowClose)){
    g_print("Se cierra: %s\n", gtk_window_get_title (GTK_WINDOW (windowClose)));
    gtk_widget_destroy(GTK_WIDGET(windowClose));
  }

  GtkWidget *window;
  window = GTK_WIDGET(gtk_builder_get_object (builder, "ProfileBuild"));
  // g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

  /*  Repeticion de ventana   */
  /* Necesito la estructura de datos globales para poder hacer el ciclo de la funcion */
  // menuLocal(buttonInit, builder, players);
}

void playerSelectFn(GObject *playerImg, GtkBuilder* builder){
  GObject *button;
  const gchar* data;
  data = gtk_widget_get_name(GTK_WIDGET(playerImg));
  g_print("Player Select = %s!\n", data);
  // Meter dato a la estructura de datos globales
  /*   AQUI   */
  if (strcmp(data, "1") == 0){
    button = gtk_builder_get_object (builder, "Human");
    gtk_widget_show(GTK_WIDGET(button));
  } else {
    button = gtk_builder_get_object (builder, "Bot");
    gtk_widget_show(GTK_WIDGET(button));
  }
  g_object_unref(builder);
}

void SelectVSFn(GObject *vsImg, GtkBuilder* builder){
  GObject *button;
  const gchar* data;

  data = gtk_widget_get_name(GTK_WIDGET(vsImg));
  g_print("VS Select = %s!\n", data);
    // Meter dato a la estructura de datos globales
    /*   AQUI   */
  playerSelectFn(vsImg,builder);
  g_object_unref(builder);
}

void returnMain(GObject *init, gpointer* user_data){
  GtkWindow *windowClose;
  windowClose = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(init)));
  if(GTK_IS_WINDOW(windowClose)){
    g_print("Se cierra: %s\n", gtk_window_get_title (GTK_WINDOW (windowClose)));
    gtk_widget_destroy(GTK_WIDGET(windowClose));
  }
  main(0,NULL);
}