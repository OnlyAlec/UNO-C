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
  * - Corregir el levantamiento de cartas en el MENU PRINCIPAL
  * - Ver la manera de evitar que la ventana se espere al video asi evitando el error de GTS
  * - Cuando se lance el evento "DESTROY" evitar que termine la ventana si es que fue por un CALLBACK
  * - Las variables "eventHuman, eventBot" no se deben declarar como globales, deberian ser locales.
  *   Sin embargo, como en la funcion donde se ocupa el show(), pierde la direccion de builder tambien
  *   se pierden las direcciones de estos bototenes.
  * - En algunas funciones se pierde la direccion de builder.
  * - Una vez el usuario seleccione un avatar, desaparecer las demas img y solo quedar el avatar seleccionado
*/

#include <stdlib.h>
#include <gtk/gtk.h>
#include <gst/gst.h>
#include <gst/video/videooverlay.h>
#include <gdk/gdkwin32.h>

static const gchar *image_names[] = {
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
  "assets\\MainCards\\small\\wild_color_changer.png",
  "assets\\MainCards\\small\\wild_pick_four.png",
  "assets\\MainCards\\small\\card_back_alt.png",
  "assets\\MainCards\\small\\card_back.png"
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
  char src[MAX_PATH]; //Variable para identificar la direcccion de la carta en el arreglo
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
GObject *windowMain, *eventBot, *eventHuman;
static GstElement *playbin, *play, *sink;
static int registrados = 0, *apRegistrados = &registrados;
Menu DB_Menu;
Juego DB_Juego;


/*   Funciones   */
// GTS
gboolean bus_callback(GstBus *, GstMessage *, gpointer);
static void activateVideo(char *);
char* fullPath(char *);
// GTK
static void activate(GtkApplication *, gpointer);
void motionCard(GObject *, GdkEventConfigure *, gpointer );
void returnMain(GObject*, gpointer*);
// Menus
void menuPerfiles(GObject *, GtkBuilder* );
void menuLocalOnline(GObject *, gpointer);
void menuComoJugar(GObject *, gpointer);
void menuInstrucciones(GObject*, GtkBuilder* );
void menuObjetivo(GObject*, GtkBuilder* );
void menuSeleccionJugadores(GObject *, GtkBuilder* );
//PreJuego
void numJugadores(GObject * );
void modoJuego(GObject *, GtkBuilder* );
int restartMenuPerfiles(GObject * );
void guardarApodo(GObject *);
void guardarPFP(GObject *);
// Juego Principal
void startGame();
void ocultar();

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
  system("pause");
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

  /*   Testing   */
  button = gtk_builder_get_object (builder, "Testing");
  g_signal_connect (button, "clicked", G_CALLBACK (startGame), NULL);

  gst_element_set_state (play, GST_STATE_PLAYING);
  gtk_widget_show_all(GTK_WIDGET(windowMain));
  g_object_unref (builder);
  g_print("Menu Principal (^_^) \n");
}
void motionCard(GObject *eventBox, GdkEventConfigure *event, gpointer data){
  int time = 1000; // Tiempo en milisegundos
  GdkEventType eventType = event->type;
  if(event->type == GDK_ENTER_NOTIFY){
    for (int i = 0; i < 10; i++) {
      gtk_widget_set_margin_bottom(GTK_WIDGET(eventBox), 10*i);
    }
  } else if (event->type == GDK_LEAVE_NOTIFY){
    for (int i = 10; i > 1; i--) {
      gtk_widget_set_margin_bottom(GTK_WIDGET(eventBox), 10*i);
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
    // g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

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

    button = gtk_builder_get_object (builder, "Next");
    g_signal_connect (button, "clicked", G_CALLBACK(restartMenuPerfiles), builder);

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
  GObject *button;
  const gchar* data = gtk_widget_get_name(GTK_WIDGET(vsImg));
  g_print("VS = %s!\n", data);
  if (strcmp(data, "Bot") == 0)
    strcpy(DB_Menu.modJuego, "Bot");
  else if (strcmp(data, "Human") == 0)
    strcpy(DB_Menu.modJuego, "Human");

  if (!restartMenuPerfiles(vsImg)){
    g_object_unref(builder);
    startGame();
  }
}
int restartMenuPerfiles (GObject *object){
  // Cuando builder llega a esta funcion la dirrecion se pierde, entonces la redefino
  GtkBuilder *builder = gtk_builder_new ();
  gtk_builder_add_from_file (builder, "XML/PreJuego.glade", NULL);

  if (GTK_IS_BUTTON(object)) {
      if (*apRegistrados <= DB_Menu.numJugadores) {
        menuPerfiles(object, builder);
        *apRegistrados=+1;
      } else
        return 1;
  } else{
    menuPerfiles(object, builder);
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
}

/*   Funciones para el JUEGO   */
void startGame(){
  GtkBuilder *builder;
  GObject *boxA, *boxAB, *boxI, *boxD, *event,*event2,*event3,*event4,*event5,*event6,*event7, *img;
  GError *error = NULL;
  GtkWidget *window;
  g_print("Inicio del juego\n");
  builder = gtk_builder_new ();
  if (gtk_builder_add_from_file (builder, "XML/menuplay.glade", &error) == 0) {
    g_printerr ("Error loading file: %s\n", error->message);
    g_clear_error (&error);
  }

  windowMain = gtk_builder_get_object (builder, "playmenu");
  // g_signal_connect (windowMain, "destroy", G_CALLBACK (gtk_main_quit), NULL);
    /* Generales */
  gtk_builder_connect_signals(builder, NULL);

  boxA = gtk_builder_get_object(builder, "CartasArriba");
  boxAB = gtk_builder_get_object(builder, "CartasAbajo");
  boxI = gtk_builder_get_object(builder, "CartaDerecha");
  boxD = gtk_builder_get_object(builder, "CartasIzquierda");

  event = gtk_builder_get_object(builder, "CardI1");
  g_signal_connect(event, "button-release-event", G_CALLBACK(ocultar), NULL);
  g_signal_connect (event, "enter_notify_event", G_CALLBACK (motionCard), NULL);
  g_signal_connect (event, "leave_notify_event", G_CALLBACK (motionCard), NULL);

  event = gtk_builder_get_object(builder, "CardI2");
  g_signal_connect(event, "button-release-event", G_CALLBACK(ocultar), NULL);


  event = gtk_builder_get_object(builder, "CardI3");
  g_signal_connect(event, "button-release-event", G_CALLBACK(ocultar), NULL);


  event = gtk_builder_get_object(builder, "CardI4");
  g_signal_connect(event, "button-release-event", G_CALLBACK(ocultar), NULL);

  event = gtk_builder_get_object(builder, "CardI5");
  g_signal_connect(event, "button-release-event", G_CALLBACK(ocultar), NULL);

  event = gtk_builder_get_object(builder, "CardI6");
  g_signal_connect(event, "button-release-event", G_CALLBACK(ocultar), NULL);

  event = gtk_builder_get_object(builder, "CardI7");
  g_signal_connect(event, "button-release-event", G_CALLBACK(ocultar), NULL);


  gtk_widget_show_all(GTK_WIDGET(windowMain));
}

void ocultar(GObject*event){
  gtk_widget_hide(GTK_WIDGET(event));
}