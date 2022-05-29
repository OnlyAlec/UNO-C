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

/* TODO:
  * - Ver la manera de evitar que la ventana se espere al video asi evitando el error de GTS
  * - Cuando se lance el evento "DESTROY" evitar que termine la ventana si es que fue por un CALLBACK
  *      -> Si se cierra una funcion con la X, entonces si validar el gtk_main_quit()
  * - Las variables "eventHuman, eventBot" no se deben declarar como globales, deberian ser locales.
  *   Sin embargo, como en la funcion donde se ocupa el show(), pierde la direccion de builder tambien
  *   se pierden las direcciones de estos bototenes.
  * - En algunas funciones se pierde la direccion de builder.
*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <gst/gst.h>
#include <gst/video/videooverlay.h>
#include <gdk/gdkwin32.h>

#define MAX_CARDS 30
static char *image_names[] = {
  "assets\\MainCards\\small\\blue_0.png",
  "assets\\MainCards\\small\\blue_1.png",
  "assets\\MainCards\\small\\blue_2.png",
  "assets\\MainCards\\small\\blue_3.png",
  "assets\\MainCards\\small\\blue_4.png",
  "assets\\MainCards\\small\\blue_5.png",
  "assets\\MainCards\\small\\blue_6.png",
  "assets\\MainCards\\small\\blue_7.png",
  "assets\\MainCards\\small\\blue_8.png",
  "assets\\MainCards\\small\\blue_9.png",
  "assets\\MainCards\\small\\blue_picker.png",
  "assets\\MainCards\\small\\blue_reverse.png",
  "assets\\MainCards\\small\\blue_skip.png",
  "assets\\MainCards\\small\\green_0.png",
  "assets\\MainCards\\small\\green_1.png",
  "assets\\MainCards\\small\\green_2.png",
  "assets\\MainCards\\small\\green_3.png",
  "assets\\MainCards\\small\\green_4.png",
  "assets\\MainCards\\small\\green_5.png",
  "assets\\MainCards\\small\\green_6.png",
  "assets\\MainCards\\small\\green_7.png",
  "assets\\MainCards\\small\\green_8.png",
  "assets\\MainCards\\small\\green_9.png",
  "assets\\MainCards\\small\\green_picker.png",
  "assets\\MainCards\\small\\green_reverse.png",
  "assets\\MainCards\\small\\green_skip.png",
  "assets\\MainCards\\small\\red_0.png",
  "assets\\MainCards\\small\\red_1.png",
  "assets\\MainCards\\small\\red_2.png",
  "assets\\MainCards\\small\\red_3.png",
  "assets\\MainCards\\small\\red_4.png",
  "assets\\MainCards\\small\\red_5.png",
  "assets\\MainCards\\small\\red_6.png",
  "assets\\MainCards\\small\\red_7.png",
  "assets\\MainCards\\small\\red_8.png",
  "assets\\MainCards\\small\\red_9.png",
  "assets\\MainCards\\small\\red_picker.png",
  "assets\\MainCards\\small\\red_reverse.png",
  "assets\\MainCards\\small\\red_skip.png",
  "assets\\MainCards\\small\\yellow_0.png",
  "assets\\MainCards\\small\\yellow_1.png",
  "assets\\MainCards\\small\\yellow_2.png",
  "assets\\MainCards\\small\\yellow_3.png",
  "assets\\MainCards\\small\\yellow_4.png",
  "assets\\MainCards\\small\\yellow_5.png",
  "assets\\MainCards\\small\\yellow_6.png",
  "assets\\MainCards\\small\\yellow_7.png",
  "assets\\MainCards\\small\\yellow_8.png",
  "assets\\MainCards\\small\\yellow_9.png",
  "assets\\MainCards\\small\\yellow_picker.png",
  "assets\\MainCards\\small\\yellow_reverse.png",
  "assets\\MainCards\\small\\yellow_skip.png",
  "assets\\MainCards\\small\\changer_wild_color.png",
  "assets\\MainCards\\small\\four_wild_pick.png",
};
/*Estructuras*/
typedef struct{
  char apodo[20];
  char ID[20];
  char src[MAX_PATH];
} perfil;
typedef struct{
  char color[15];
  int numero; // 0-9 : Numeors, 10-14 : Acciones
  char srcCompleto[MAX_PATH]; //Variable para identificar la direcccion de la carta en el arreglo
  char srcSimple[50]; //Variable para identificar la direcccion de la carta en el arreglo
} cartas;
typedef struct {
  int numJugadores;
  perfil jugadores[4];
  char modJuego[20];
} Menu;

typedef struct{
  cartas mazo[54];
  perfil juga[4];
}Juego;

/*   Globales   */
GObject *windowMain, *eventBot, *eventHuman, *Global;
static GstElement *playbin, *play, *sink;
static int registrados = 0, *apRegistrados = &registrados, goGame = 0;
Menu DB_Menu;
Juego DB_Juego;

/*   Globales para el juego   */
int cardSelected=0;
cartas jugador1[MAX_CARDS] = {0};
cartas jugador2[MAX_CARDS] = {0};
cartas jugador3[MAX_CARDS] = {0};
cartas jugador4[MAX_CARDS] = {0};
cartas cardInit = {0};

/*   Funciones   */
// GTS
gboolean bus_callback(GstBus *, GstMessage *, gpointer);
static void activateVideo(char *);
char* fullPath(char *);
// GTK
static void activate(GtkApplication *, gpointer);
void motionCard(GObject *, GdkEventConfigure *, gpointer );
void returnMain(GObject*, gpointer*);
// static void destroyWindow(GtkWidget *, GdkEventConfigure *, gpointer *);
// Menus
void menuPerfiles(GObject *, GtkBuilder* );
void menuLocalOnline(GObject *, gpointer);
void menuComoJugar(GObject *, gpointer);
void menuInstrucciones(GObject*, GtkBuilder* );
void menuObjetivo(GObject*, GtkBuilder* );
void menuSeleccionJugadores(GObject *, GtkBuilder* );
//PreJuego
void numJugadores(GObject * );
void modoJuego(GObject *, GtkBuilder*);
int restartMenuPerfiles (GObject *);
void guardarApodo(GObject *);
void guardarPFP(GObject *);
// Juego Principal
void startGame(GObject *);
void ocultarValidar(GObject *);

int main (int argc, char *argv[]) {
  g_print("( ▀ v ▀ ) CORRIENDO!\n");
  /*   init GTK && GTS   */
  gtk_init (&argc, &argv);
  gst_init (&argc, &argv);
  // loop = g_main_loop_new (NULL, FALSE);
  /* Activacion de la aplicacion */
  activateVideo("assets\\backgrounds\\Blue.mp4");
  activate(NULL,NULL);
  /*  GTK Set Up   */
  gtk_main();
  /*   Limpieza   */
  gst_element_set_state (play, GST_STATE_NULL);
  gst_object_unref (GST_OBJECT (play));
  return 0;
}

/*  Funciones para la activacion del VIDEO   */
gboolean bus_callback(GstBus *bus, GstMessage *msg, gpointer data) {
  GstElement *plays = GST_ELEMENT(data);
  switch (GST_MESSAGE_TYPE(msg))  {
  case GST_MESSAGE_EOS:
      g_print(" ► Reiniciando LOOP!\n");
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
static void activateVideo(char * path ){
  GstBus *bus;

  char file[_MAX_PATH] = "file:///";
  strcat(file, fullPath(path));

  play = gst_element_factory_make ("playbin", NULL);
  g_object_set (G_OBJECT (play), "uri", file, NULL);

  sink = gst_element_factory_make ("gtksink", NULL);
  g_object_set(play, "video-sink", sink, NULL);

  /*   GST Set Up   */
  bus = gst_pipeline_get_bus (GST_PIPELINE (play));
  gst_bus_add_watch (bus, bus_callback, play);
  gst_object_unref (bus);
}
char* fullPath( char * partialPath ) {
  int i=0;
  char temp[_MAX_PATH], *full = &temp[0];
  if( _fullpath( full, partialPath, _MAX_PATH ) != NULL ){
      while (full[i]!='\0') {
        if(full[i]=='\\')
          full[i]='/';
        i++;
      }
    return full;
  }
  else{
     g_print( "Ruta Invalida\n" );
     return NULL;
  }
}

/*  Funciones de animacion o GTK   */
static void activate(GtkApplication *app, gpointer user_data) {
  GtkBuilder *builder;
  GObject *button, *box, *event, *overlayArea, *img;
  GError *error = NULL;
  GtkWidget *video_drawing_area = gtk_drawing_area_new();

  /*   Integracion de XML    */
  builder = gtk_builder_new ();
  if (gtk_builder_add_from_file (builder, "XML/menuPrincipal.glade", &error) == 0) {
    g_printerr ("Error loading file: %s\n", error->message);
    g_clear_error (&error);
    system("pause");
  }
  /*   Conexion de señales    */
    /* Ventana Principal */
  windowMain = gtk_builder_get_object (builder, "main");
  // g_signal_connect (windowMain, "destroy", G_CALLBACK (gtk_main_quit), NULL);
    /* Generales */
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
  g_signal_connect (event, "button-release-event", G_CALLBACK (menuLocalOnline), NULL);
  g_signal_connect (event, "enter_notify_event", G_CALLBACK (motionCard), NULL);
  g_signal_connect (event, "leave_notify_event", G_CALLBACK (motionCard), NULL);

  event = gtk_builder_get_object (builder, "LeftMotion");
  img = gtk_builder_get_object (builder, "Left1");
  g_signal_connect (event, "button-release-event", G_CALLBACK (menuComoJugar), NULL);
  g_signal_connect (event, "enter_notify_event", G_CALLBACK (motionCard), NULL);
  g_signal_connect (event, "leave_notify_event", G_CALLBACK (motionCard), NULL);

  event = gtk_builder_get_object (builder, "RightMotion");
  img = gtk_builder_get_object (builder, "Right1");
  g_signal_connect (event, "enter_notify_event", G_CALLBACK (motionCard), NULL);
  g_signal_connect (event, "leave_notify_event", G_CALLBACK (motionCard), NULL);

  // /*   Testing   */
  button = gtk_builder_get_object (builder, "Testing");
  // g_signal_connect (button, "clicked", G_CALLBACK (startGame), NULL);

  gst_element_set_state (play, GST_STATE_PLAYING);
  gtk_widget_show_all(GTK_WIDGET(windowMain));
  gtk_widget_hide(GTK_WIDGET(button));
  g_object_unref (builder);
  g_print("Menu Principal (^_^) \n");
}
void motionCard(GObject *eventBox, GdkEventConfigure *event, gpointer data){
  // Move "eventBox" upper when the event is GDK_ENTER_NOTIFY, if not move it down

  int time = 60; // Tiempo en milisegundos
  GdkEventType eventType = event->type;
  if(event->type == GDK_ENTER_NOTIFY){
    for (int i = 0; i < 5000; i++) {
      gtk_widget_set_margin_bottom(GTK_WIDGET(eventBox), i/time);
    }
  } else if (event->type == GDK_LEAVE_NOTIFY){
    for (int i = 5000; i > 1; i--) {
      gtk_widget_set_margin_bottom(GTK_WIDGET(eventBox), i/time);
    }
  }
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
// static void destroyWindow(GtkWidget *window, GdkEventConfigure *eventConfigure, gpointer *data){
//   if (closeAutoWindow == 0){
//     gtk_widget_destroy(window);
//     gtk_main_quit();
//   }
// }

/*  Funciones de Menus  */
void menuPerfiles(GObject *buttonInit, GtkBuilder* builder){
    g_print("Menu Perfiles\n");
    /*   Cierre de ventana   */
    GtkWindow *windowClose;
    windowClose = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(buttonInit)));
    if(GTK_IS_WINDOW(windowClose)){
      g_print("Se cierra: %s\n", gtk_window_get_title (GTK_WINDOW (windowClose)));
      gtk_widget_destroy(GTK_WIDGET(windowClose));
    }

    GtkWidget *window;
    window = GTK_WIDGET(gtk_builder_get_object (builder, "ProfileBuild"));
    // g_signal_connect (window, "destroy", G_CALLBACK (destroyWindow), NULL);

    /*   Obtencion de Objetos   */
    GObject *text, *event, *button;
    text = gtk_builder_get_object (builder, "NamePlayer");
    g_signal_connect (text, "focus_out_event", G_CALLBACK(guardarApodo), NULL);

    event = gtk_builder_get_object (builder, "AvatarM1");
    gtk_widget_set_name(GTK_WIDGET(event), "avatarW1");

    g_signal_connect (GTK_WIDGET(event), "button-release-event", G_CALLBACK(guardarPFP), NULL);
    event = gtk_builder_get_object (builder, "AvatarM2");
    gtk_widget_set_name(GTK_WIDGET(event), "avatarW2");

    g_signal_connect (GTK_WIDGET(event), "button-release-event", G_CALLBACK(guardarPFP), NULL);
    event = gtk_builder_get_object (builder, "AvatarM3");
    gtk_widget_set_name(GTK_WIDGET(event), "avatarW3");

    g_signal_connect (GTK_WIDGET(event), "button-release-event", G_CALLBACK(guardarPFP), NULL);
    event = gtk_builder_get_object (builder, "AvatarM4");
    gtk_widget_set_name(GTK_WIDGET(event), "avatarW4");

    g_signal_connect (GTK_WIDGET(event), "button-release-event", G_CALLBACK(guardarPFP), NULL);
    event = gtk_builder_get_object (builder, "AvatarM5");
    gtk_widget_set_name(GTK_WIDGET(event), "avatarW5");

    g_signal_connect (GTK_WIDGET(event), "button-release-event", G_CALLBACK(guardarPFP), NULL);

    event = gtk_builder_get_object (builder, "AvatarH1");
    g_signal_connect (GTK_WIDGET(event), "button-release-event", G_CALLBACK(guardarPFP), NULL);
    gtk_widget_set_name(GTK_WIDGET(event), "avatarM1");

    event = gtk_builder_get_object (builder, "AvatarH2");
    g_signal_connect (GTK_WIDGET(event), "button-release-event", G_CALLBACK(guardarPFP), NULL);
    gtk_widget_set_name(GTK_WIDGET(event), "avatarM2");

    event = gtk_builder_get_object (builder, "AvatarH3");
    g_signal_connect (GTK_WIDGET(event), "button-release-event", G_CALLBACK(guardarPFP), NULL);
    gtk_widget_set_name(GTK_WIDGET(event), "avatarM3");

    event = gtk_builder_get_object (builder, "AvatarH4");
    g_signal_connect (GTK_WIDGET(event), "button-release-event", G_CALLBACK(guardarPFP), NULL);
    gtk_widget_set_name(GTK_WIDGET(event), "avatarM4");

    event = gtk_builder_get_object (builder, "AvatarH5");
    g_signal_connect (GTK_WIDGET(event), "button-release-event", G_CALLBACK(guardarPFP), NULL);
    gtk_widget_set_name(GTK_WIDGET(event), "avatarM5");

    Global = gtk_builder_get_object (builder, "Next");
    g_signal_connect (Global, "clicked", G_CALLBACK(restartMenuPerfiles), builder);

    gtk_widget_show_all(GTK_WIDGET(window)) ;
}
void menuLocalOnline(GObject *buttonInit, gpointer user_data){
  g_print("Menu Local o Multijugador\n");
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
  g_signal_connect (button, "clicked", G_CALLBACK (menuSeleccionJugadores), builder);
  button = gtk_builder_get_object (builder, "GameMult");
  // g_signal_connect (button, "clicked", G_CALLBACK (menuMult), builder);

  gtk_widget_show_all(GTK_WIDGET(window));
}
void menuComoJugar(GObject *buttonInit, gpointer user_data) {
  g_print("Menu Como Juagar (Instrucciones o Objetivo)\n");
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
  if (gtk_builder_add_from_file (builder, "XML/comoJugar.glade", &error) == 0) {
    g_printerr ("Error loading file: %s\n", error->message);
    g_clear_error (&error);
    system("pause");
  }
  /*   Activacion Video   */
  activateVideo("assets\\backgrounds\\VideoInstrucciones.mp4");
  /*   Obtencion de Objetos   */
  GtkWidget *window;
  GObject *button, *overlayArea;
  GtkWidget *video_drawing_area = gtk_drawing_area_new();

  /*   Ventanas   */
  window = GTK_WIDGET(gtk_builder_get_object (builder, "ComoJugarW"));
  // g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
    /*   Video Area   */
  overlayArea = gtk_builder_get_object (builder, "Overlay");
  g_object_get (sink, "widget", &video_drawing_area, NULL);
  gtk_widget_set_size_request(video_drawing_area, 1280, 720);
  gtk_container_add(GTK_CONTAINER(overlayArea), video_drawing_area);
  /*   Botones   */
  button = gtk_builder_get_object (builder, "ObjetivoBtn");
  g_signal_connect (button, "clicked", G_CALLBACK (menuObjetivo), builder);
  button = gtk_builder_get_object (builder, "InstruccionesBtn");
  g_signal_connect (button, "clicked", G_CALLBACK (menuInstrucciones), builder);

  gst_element_set_state (play, GST_STATE_PLAYING);
  gtk_widget_show_all(GTK_WIDGET(window));
}
void menuInstrucciones(GObject *buttonInit, GtkBuilder* builder){
  g_print("Menu Instrucciones\n");
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
void menuObjetivo(GObject *buttonInit, GtkBuilder* builder) {
  g_print("Menu Objetivos\n");
  GObject *window, *button;

  /*   Cierre de ventana   */
  GtkWindow *windowClose;
  windowClose = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(buttonInit)));
  if(GTK_IS_WINDOW(windowClose)){
    g_print("Se cierra: %s\n", gtk_window_get_title (GTK_WINDOW (windowClose)));
    gtk_widget_destroy(GTK_WIDGET(windowClose));
  }

  window = gtk_builder_get_object (builder, "menuObjetivo");
  button = gtk_builder_get_object (builder, "back-btn");
  g_signal_connect (button, "clicked", G_CALLBACK (returnMain), NULL);

  gtk_widget_show_all(GTK_WIDGET(window));
  // g_object_unref(builder);
}
void menuSeleccionJugadores(GObject *buttonInit, GtkBuilder* builder){
  // FIXME: Enviar 2 widgets con una struct para poder enviar los datos
  g_print("Menu Cuantos Jugadores\n");
    /*   Cierre de ventana   */
  GtkWindow *windowClose;
  windowClose = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(buttonInit)));
  if(GTK_IS_WINDOW(windowClose)){
    g_print("Se cierra: %s\n", gtk_window_get_title (GTK_WINDOW (windowClose)));
    gtk_widget_destroy(GTK_WIDGET(windowClose));
  }
  /*   Obtencion de Objetos   */
  GObject *Eventimg;
  GtkWidget *window;

  window = GTK_WIDGET(gtk_builder_get_object (builder, "SelectPlayer"));
  // g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

  Eventimg = gtk_builder_get_object (builder, "1P");
  gtk_widget_set_name(GTK_WIDGET(Eventimg), "1P");
  g_signal_connect (GTK_WIDGET(Eventimg), "button-release-event", G_CALLBACK(numJugadores), eventBot);
  Eventimg = gtk_builder_get_object (builder, "2P");
  gtk_widget_set_name(GTK_WIDGET(Eventimg), "2P");
  g_signal_connect (GTK_WIDGET(Eventimg), "button-release-event", G_CALLBACK(numJugadores), window);
  Eventimg = gtk_builder_get_object (builder, "3P");
  gtk_widget_set_name(GTK_WIDGET(Eventimg), "3P");
  g_signal_connect (GTK_WIDGET(Eventimg), "button-release-event",  G_CALLBACK(numJugadores), window);
  Eventimg = gtk_builder_get_object (builder, "4P");
  gtk_widget_set_name(GTK_WIDGET(Eventimg), "4P");
  g_signal_connect (GTK_WIDGET(Eventimg), "button-release-event", G_CALLBACK(numJugadores), window);

  eventBot = gtk_builder_get_object (builder, "Bot");
  gtk_widget_set_name(GTK_WIDGET(eventBot), "Bot");
  g_signal_connect (eventBot, "button-release-event", G_CALLBACK(modoJuego), window);
  eventHuman = gtk_builder_get_object (builder, "Human");
  gtk_widget_set_name(GTK_WIDGET(eventHuman), "Human");
  g_signal_connect (eventHuman, "button-release-event", G_CALLBACK(modoJuego), window);

  gtk_widget_show_all(GTK_WIDGET(window));
  gtk_widget_hide(GTK_WIDGET(eventBot));
  gtk_widget_hide(GTK_WIDGET(eventHuman));
  g_object_unref(builder);
}

/*   Funciones para el PRE-JUEGO   */
void numJugadores(GObject *buttonInit){
  // Cuando builder llega a esta funcion la dirrecion se pierde, entonces la redefino
  GtkBuilder *builder = gtk_builder_new ();
  gtk_builder_add_from_file (builder, "XML/PreJuego.glade", NULL);

  GObject *eventImg, *box, *img;
  const gchar* data = gtk_widget_get_name(GTK_WIDGET(buttonInit));
  DB_Menu.numJugadores = atoi(&data[0]);
  g_print("Numero de Jugadores = %d!\n", DB_Menu.numJugadores);

  /* Si solo es un jugador, solo puede jugar con BOTS. De lo contrario puede jugar con HUMAN o BOTS */
  if (DB_Menu.numJugadores >= 2){
    gtk_widget_show(GTK_WIDGET(eventHuman));
  }
  gtk_widget_show(GTK_WIDGET(eventBot));
  g_object_unref(builder);
}
void modoJuego(GObject *vsImg, GtkBuilder* builder){
  const gchar* data = gtk_widget_get_name(GTK_WIDGET(vsImg));
  g_print("VS = %s!\n", data);
  if (strcmp(data, "Bot") == 0)
    strcpy(DB_Menu.modJuego, "Bot");
  else if (strcmp(data, "Human") == 0){
    strcpy(DB_Menu.modJuego, "Human");
  }
  restartMenuPerfiles(vsImg);
}
int restartMenuPerfiles (GObject *object){
  // Cuando builder llega a esta funcion la dirrecion se pierde, entonces la redefino
  GtkBuilder *builder = gtk_builder_new ();
  gtk_builder_add_from_file (builder, "XML/PreJuego.glade", NULL);

  if (GTK_IS_BUTTON(object)) {
      if (*apRegistrados < DB_Menu.numJugadores) {
        menuPerfiles(object, builder);
        *apRegistrados+=1;
      } else
        goGame = 1;
  } else{
    menuPerfiles(object, builder);
    *apRegistrados+=1;
  }
  if (goGame == 1) {
    g_object_unref(builder);
    startGame(Global);
  }
}
void guardarApodo(GObject *entryText){
  const gchar *apodo =  gtk_entry_get_text  (GTK_ENTRY(entryText));
  g_print("Apodo Jugador %d : %s\n", *apRegistrados, apodo);
  strcpy(DB_Menu.jugadores[*apRegistrados].apodo, apodo);
}
void guardarPFP(GObject *playerImg){
  g_print("Player %d\n", *apRegistrados);
  const gchar* nameImg = gtk_widget_get_name(GTK_WIDGET(playerImg));
  char path[MAX_PATH] = "..\\assets\\profiles\\";
  strcat(path, nameImg);
  strcat(path, ".png");
  strcpy(DB_Menu.jugadores[*apRegistrados].src, path);

  GObject *box;
  GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(playerImg));
  GtkWidget *parent  = gtk_widget_get_parent(GTK_WIDGET(playerImg));
  parent = gtk_widget_get_parent(GTK_WIDGET(parent));

  GList *children = gtk_container_get_children(GTK_CONTAINER(parent));
  GList *l = NULL,  *k = NULL;
  for (l = children; l != NULL; l = l->next) {
    if (GTK_IS_BOX(l->data)) {
        GList *childrenBox = gtk_container_get_children(GTK_CONTAINER(l->data));
        for (k = childrenBox; k != NULL; k = k->next) {
          if (GTK_IS_EVENT_BOX(k->data)) {
            if (GTK_WIDGET(k->data) != GTK_WIDGET(playerImg)) {
              // gtk_widget_hide(GTK_WIDGET(k->data));
              gtk_widget_set_opacity(GTK_WIDGET(k->data), 0.7);
              gtk_widget_set_sensitive(GTK_WIDGET(k->data), FALSE);
            }
          }
        }
    }
  }
  g_list_free(children);
}

/*   Funciones para el JUEGO   */
void startGame(GObject *buttonInit){
    /*   Cierre de ventana   */
  GtkWindow *windowClose;
  windowClose = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(buttonInit)));
  if(GTK_IS_WINDOW(windowClose)){
    g_print("Se cierra: %s\n", gtk_window_get_title (GTK_WINDOW (windowClose)));
    gtk_widget_destroy(GTK_WIDGET(windowClose));
  }

  int a,b,c,j=0,end=0,l=0;
  char tempString[MAX_PATH];
  GtkBuilder *builder;
  GObject *boxA, *boxAB, *boxI, *boxD, *event, *img;
  GError *error = NULL;
  GtkWidget *window;
  char* srcSimple;

  for(int i=0; i<7; i++){
      end = 0;
      a = rand() % 54;
      strcpy(jugador1[i].srcCompleto, image_names[a]);
      strcpy(tempString, image_names[a]);
       srcSimple = strtok(tempString, "\\");
      srcSimple = strtok(NULL, "\\");
      srcSimple = strtok(NULL, "\\");
      srcSimple = strtok(NULL, "\\");
      strcpy(jugador1[i].srcSimple,srcSimple);

      while(end != 1){
        switch(jugador1[i].srcSimple[j]) {
          case 'r':
           strcpy(jugador1[i].color, "Rojo");
              end=1;
            break;
          case 'b':
            strcpy(jugador1[i].color, "Azul");
            end=1;
            break; 
          case 'y':
            strcpy(jugador1[i].color, "Amarillo");
            end=1;
            break;
          case 'g':
            strcpy(jugador1[i].color, "Verde");
            end=1;
            break;
          case 'f':
            strcpy(jugador1[i].color, "ComodinMas4");
            end=1;
            break;
          case 'c':
            strcpy(jugador1[i].color, "Comodin");
            end=1;
          break;
          default:
            if(jugador1[i].srcSimple[i] != '\0')
              j++;
            else
              end=1;
            break;
        }
        /*Switch que valida el numero de las cartas y si es un +2, reversa, negar turno etc...*/ 
        l=strlen(jugador1[i].srcSimple);//Obtiene la ultima letra de la cadena, sabiendo asi su numero o poder de la carta   
        switch (jugador1[i].srcSimple[l-5]){//Se resta menos cinco ya que se toma en cuenta el .png 
           case '0':
             jugador1[i].numero=0;
             break;
           case '1':
             jugador1[i].numero=1;
             break;
           case '2':
             jugador1[i].numero=2;
             break;
           case '3':
             jugador1[i].numero=3;
             break;
           case '4':
             jugador1[i].numero=4;
             break;
           case '5':
             jugador1[i].numero=5;
             break;
           case '6':
             jugador1[i].numero=6;
             break;
           case '7':
             jugador1[i].numero=7;
             break;
           case '8':
             jugador1[i].numero=8;
             break;
           case '9':
             jugador1[i].numero=9;
             break;
           case 'r':/*Carta + 2*/
             jugador1[i].numero=10;
             break;
           case 'w':/*Carta de +4*/
             jugador1[i].numero=11;
             break;
           case 'e':/*Carta de reversa*/
             jugador1[i].numero=12;
             break;
           case 'p':/*Carta para negar turno*/
             jugador1[i].numero=13;
             break;
           }
      }
      if(jugador1[i].color == NULL)
        g_print("Error en el color\n");
      else
        g_print("Jugador 1 - Carta %d - %s - %d\n",i, jugador1[i].color, jugador1[i].numero);
// ---------------------------------------------------------------------------------------------------------------------
      end = 0;
      b = rand() % 54;
      strcpy(jugador2[i].srcCompleto, image_names[b]);
      strcpy(tempString, image_names[b]);
      srcSimple = strtok(tempString, "\\");
      srcSimple = strtok(NULL, "\\");
      srcSimple = strtok(NULL, "\\");
      srcSimple = strtok(NULL, "\\");
      strcpy(jugador2[i].srcSimple,srcSimple);
      while(end != 1){
        switch(jugador2[i].srcSimple[j]) {
          case 'r':
            strcpy(jugador2[i].color, "Rojo");
              end=1;
            break;
          case 'b':
            strcpy(jugador2[i].color, "Azul");
            end=1;
            break;
          case 'y':
            strcpy(jugador2[i].color, "Amarillo");
            end=1;
            break;
          case 'g':
            strcpy(jugador2[i].color, "Verde");
            end=1;
            break;
          case 'f':
            strcpy(jugador2[i].color, "ComodinMas4");
            end=1;
            break;
          case 'c':
            strcpy(jugador2[i].color, "Comodin");
            end=1;
          break;
          default:
            if(jugador2[i].srcSimple[i] != '\0')
              j++;
            else
              end=1;
            break;
        }
        /*Switch que valida el numero de las cartas y si es un +2, reversa, negar turno etc...*/ 
        l=strlen(jugador2[i].srcSimple);//Obtiene la ultima letra de la cadena, sabiendo asi su numero o poder de la carta   
        switch (jugador2[i].srcSimple[l-5]){
           case '0':
             jugador2[i].numero=0;
             break;
           case '1':
             jugador2[i].numero=1;
             break;
           case '2':
             jugador2[i].numero=2;
             break;
           case '3':
             jugador2[i].numero=3;
             break;
           case '4':
             jugador2[i].numero=4;
             break;
           case '5':
             jugador2[i].numero=5;
             break;
           case '6':
             jugador2[i].numero=6;
             break;
           case '7':
             jugador2[i].numero=7;
             break;
           case '8':
             jugador2[i].numero=8;
             break;
           case '9':
             jugador2[i].numero=9;
             break;
           case 'r':/*Carta + 2*/
             jugador2[i].numero=10;
             break;
            case 'w':/*Carta de +4*/
             jugador2[i].numero=11;
             break;
           case 'e':/*Carta de reversa*/
             jugador2[i].numero=12;
             break;
           case 'p':/*Carta para negar turno*/
             jugador2[i].numero=13;
             break;
           }
      }
      if(jugador2[i].color == NULL)
        g_print("Error en el color\n");
      else
        g_print("Jugador 2 - Carta %d - %s - %d\n",i, jugador2[i].color, jugador2[i].numero);
    }
      end=0;
      c = rand()%54;
      strcpy(cardInit.srcCompleto, image_names[c]);
      strcpy(tempString, image_names[c]);
      srcSimple = strtok(tempString, "\\");
      srcSimple = strtok(NULL, "\\");
      srcSimple = strtok(NULL, "\\");
      srcSimple = strtok(NULL, "\\");
      strcpy(cardInit.srcSimple,srcSimple);
      while(end != 1){
        switch(cardInit.srcSimple[j]) {
          case 'r':
            strcpy(cardInit.color, "Rojo");
              end=1;
            break;
          case 'b':
            strcpy(cardInit.color, "Azul");
            end=1;
            break;
          case 'y':
            strcpy(cardInit.color, "Amarillo");
            end=1;
            break;
          case 'g':
            strcpy(cardInit.color, "Verde");
            end=1;
            break;
          case 'f':
            strcpy(cardInit.color, "ComodinMas4");
            end=1;
            break;
          case 'c':
            strcpy(cardInit.color, "Comodin");
            end=1;
          break;
          default:
            if(cardInit.srcSimple != '\0')
              j++;
            else
              end=1;
            break;
        }
      }
      if(cardInit.color == NULL)
        g_print("Error en el color\n");
      else
        g_print("Carta Inicial - %s\n",cardInit.color);

  g_print("\t> Inicio del juego\n");
  builder = gtk_builder_new ();
  if (gtk_builder_add_from_file (builder, "XML/menuplay2jugadores.glade", &error) == 0) {
    g_printerr ("Error loading file: %s\n", error->message);
    g_clear_error (&error);
  }

  window = GTK_WIDGET(gtk_builder_get_object (builder, "menuplay2jugadores"));
    /* Generales */
  gtk_builder_connect_signals(builder, NULL);

  boxA = gtk_builder_get_object(builder, "Arriba");
  boxAB = gtk_builder_get_object(builder, "Abajo");

  event = gtk_builder_get_object(builder, "Ab1");
  img = gtk_builder_get_object(builder, "IAb1");
  g_signal_connect(event, "button-release-event", G_CALLBACK(ocultarValidar), img);
  g_signal_connect (event, "enter_notify_event", G_CALLBACK (motionCard), img);
  g_signal_connect (event, "leave_notify_event", G_CALLBACK (motionCard), img);
  gtk_image_set_from_file(GTK_IMAGE(img), jugador1[0].srcCompleto);

  event = gtk_builder_get_object(builder, "Ab2");
  img = gtk_builder_get_object(builder, "IAb2");
  g_signal_connect (event, "enter_notify_event", G_CALLBACK (motionCard), img);
  g_signal_connect (event, "leave_notify_event", G_CALLBACK (motionCard), img);
  g_signal_connect(event, "button-release-event", G_CALLBACK(ocultarValidar), img);
  gtk_image_set_from_file(GTK_IMAGE(img), jugador1[1].srcCompleto);


  event = gtk_builder_get_object(builder, "Ab3");
  img = gtk_builder_get_object(builder, "IAb3");
  g_signal_connect (event, "enter_notify_event", G_CALLBACK (motionCard), img);
  g_signal_connect (event, "leave_notify_event", G_CALLBACK (motionCard), img);
  g_signal_connect(event, "button-release-event", G_CALLBACK(ocultarValidar), img);
  gtk_image_set_from_file(GTK_IMAGE(img), jugador1[2].srcCompleto);


  event = gtk_builder_get_object(builder, "Ab4");
  img = gtk_builder_get_object(builder, "IAb4");
  g_signal_connect (event, "enter_notify_event", G_CALLBACK (motionCard), img);
  g_signal_connect (event, "leave_notify_event", G_CALLBACK (motionCard), img);
  g_signal_connect(event, "button-release-event", G_CALLBACK(ocultarValidar), img);
  gtk_image_set_from_file(GTK_IMAGE(img), jugador1[3].srcCompleto);

  event = gtk_builder_get_object(builder, "Ab5");
  img = gtk_builder_get_object(builder, "IAb5");
  g_signal_connect (event, "enter_notify_event", G_CALLBACK (motionCard), img);
  g_signal_connect (event, "leave_notify_event", G_CALLBACK (motionCard), img);
  g_signal_connect(event, "button-release-event", G_CALLBACK(ocultarValidar), img);
  gtk_image_set_from_file(GTK_IMAGE(img), jugador1[4].srcCompleto);

  event = gtk_builder_get_object(builder, "Ab6");
  img = gtk_builder_get_object(builder, "IAb6");
  g_signal_connect (event, "enter_notify_event", G_CALLBACK (motionCard), img);
  g_signal_connect (event, "leave_notify_event", G_CALLBACK (motionCard), img);
  g_signal_connect(event, "button-release-event", G_CALLBACK(ocultarValidar), img);
  gtk_image_set_from_file(GTK_IMAGE(img), jugador1[5].srcCompleto);

  event = gtk_builder_get_object(builder, "Ab7");
  img = gtk_builder_get_object(builder, "IAb7");
  g_signal_connect (event, "enter_notify_event", G_CALLBACK (motionCard), img);
  g_signal_connect (event, "leave_notify_event", G_CALLBACK (motionCard), img);
  g_signal_connect(event, "button-release-event", G_CALLBACK(ocultarValidar), img);
  gtk_image_set_from_file(GTK_IMAGE(img), jugador1[6].srcCompleto);

  img = gtk_builder_get_object(builder, "IJuego");
  gtk_image_set_from_file(GTK_IMAGE(img), cardInit.srcCompleto);

  event = gtk_builder_get_object(builder, "Comida");

  gtk_widget_show_all(GTK_WIDGET(window));
}

void ocultarValidar(GObject *event){
  int valid=0;
  GtkWidget *card = gtk_bin_get_child (GTK_WIDGET(event));
  // if(cardSelected == 0){
  //   //FIXME: Validar la carta antes de desaparecer
  //   //FIXME: Hacer que la funcion sea un callback
  //   if (cardInit.color == card.color){
  //     valid = 1;
  //   }
  //   if (valid == 1){
      gtk_image_set_from_file(GTK_IMAGE(card), "assets\\MainCards\\CardInv.png");
      gtk_widget_set_sensitive(GTK_WIDGET(event), FALSE);
      cardSelected = 1;
    // }
  // }
}
void comparacion_num_color() {
  // Sea el jugador que sea,cuando le haga click a la carta ANTES de desaparecerla
  // Comprobamos el numero
  // Que datos ocupamos?
  // NUMERO de carta del usuario - JUGADOR(cartas) -> Numero
  //  Obtener que carta de su baraja es?
  //  Comparamos
  // NUMERO de carta INICIAL - CARTAINIT(cartas) -> Numero
  // Comparamos CON LA DEL USUARIO
  //jugador 1
  if(strcmp(jugador1->color,cardInit.color)==0 || jugador1->numero==cardInit.numero)
  {
     printf("\nTiro de carta");
  }
  else
  {
    do
    {
       printf("\nCOME DEL MAZO");
    } while (jugador1->numero==cardInit.numero || strcmp(jugador1->color,cardInit.color)==0); 
  }
  //Jugador 2
  if(strcmp(jugador2->color,cardInit.color)==0 || jugador2->numero==cardInit.numero)
  {
     printf("\nTiro de carta");
  }
  else
  {
    do
    {
       printf("\nCOME DEL MAZO");
    } while (jugador2->numero==cardInit.numero || strcmp(jugador2->color,cardInit.color)==0); 
  }
   //Jugador 3
   if(strcmp(jugador3->color,cardInit.color)==0 || jugador3->numero==cardInit.numero)
  {
     printf("\nTiro de carta");
  }
  else
  {
    do
    {
       printf("\nCOME DEL MAZO");
    } while (jugador3->numero==cardInit.numero || strcmp(jugador3->color,cardInit.color)==0); 
  }
  //Jugador 4
  if(strcmp(jugador4->color,cardInit.color)==0 || jugador4->numero==cardInit.numero)
  {
     printf("\nTiro de carta");
  }
  else
  {
    do
    {
       printf("\nCOME DEL MAZO");
    } while (jugador4->numero==cardInit.numero || strcmp(jugador4->color,cardInit.color)==0); 
  }
}

/*
void comparacion_num(GObject *event)
{
  int num1=0, num2=0;
  printf("\tComparacion de numeros en cartas: \n");
               printf("\nIngrese dos numeros de cartas:");
                scanf("%d  %d",&num1,&num2);
                if(num1==num2)
                {
                printf("los numeros de la carta de mazo con la de enmedio son iguales \n");
                printf("CARTA CON VALOR DE %d LA CARTA TIRADA TIENE VALOR DE %d son iguales,sigue el juego \n",num1,num2);
                }
                else
                {
                printf("Los numeros no son iguales,tire otra carta con diferente numero\n");
                }
}
void comparacion_colores(GObject* event)
{
  char color1[15],color2[15];
  cartas mazo,jugador;
  printf("\t \tCOLORES DE CARTAS VERDE,AZUL,AMARILLO Y ROJO \n");
            printf("CODIGO DE COLOR: ROJO: #ec6464,Verde: #64bb8e,Azul: #2dbbde,Amarillo: #f7e359 \n");
            printf("ingrese codigo de color: \n");
            fflush(stdin);
            gets(mazo.color);
            printf("Ingrese nuevamente el codigo: \n");
            fflush(stdin);
            gets(jugador.color);
            if(strcmp(jugador.color,mazo.color)==0)
            {

                printf("Los colores son iguales, puede seguir \n");
            }
            else
            {
                printf("Los colores no son iguales por lo que debe intentar con otro \n");

            }
}*/