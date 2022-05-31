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
  * - Agregar musica
  * - Agregar efectos de sonido
  * - Hacer la ventand de Acerca de Nosotros
  * - Hacer ventana de modificacion de perfil
  *   ->  Agregar una funcion que permita que el usuario pueda cambiar el nombre del jugador
  *   ->  Agregar una funcion que permita que el usuario pueda cambiar el avatar
  * - Integrasion del css para botones y animaciones
  * - Hacer la ventana de configuracion
  *  ->  Configuracion de la diificultad
  *  ->  Configuracion de la pantalla
  *  ->  Configuracion de los colores (Accesibilidad)
  * - Si el usuario ya selecciono el NUMERO DE JUGADORES, bajar la opacidad y desactivarlo img
*/

/*   URGENTE:
  * - Terminar la parte logica, creear las comparaciones de las cartas
  *   -> Como guardamos estos datos, como sabes que mazo tiene el usuario?
  *   -> Como comprobamos cuantas cartas tiene el usuarios?
  *   -> Como sabemos que es un +2,+4, bloqueo, etc?
  *   -> Como sabemos el color de la carta?
  *   -> Como sabemos el numero de la carta?
  *   -> Que datos necesitamos para que termine el juego y no se quede en un loop infinito?
  * - Necesitamos hacer la parte logica de los bots, ser automaticos
*/

//#include <glib.h>
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
  "assets\\MainCards\\small\\changer_wild_colors.png",
  "assets\\MainCards\\small\\four_wild_pick.png",
};
/*Estructuras*/
typedef struct{
  char apodo[20];
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
// Esta estructura es para solo un JUGADOR
typedef struct{
  cartas baraja[MAX_CARDS];
  int numCartasTotales;
  perfil jugador; //FIXME: Enlazar esta estructura con la del menu
} DatosJugador;
typedef struct{
  gint Motion;
  gint Click;
} signalIDs;
typedef struct{
  signalIDs idSignals[MAX_CARDS];
} IDSignalPlayer;
typedef struct{
  GtkWidget *DownBox;
  GtkWidget *UpBox;
  GtkWidget *Fixed;
  int lonDownBox;
  int lonUpBox;
  IDSignalPlayer playerSignal[4];
} ICajas;

/*   Globales   */
GObject *windowMain;
static GstElement *playbin, *play, *sink;
static int registrados = 0, *apRegistrados = &registrados, goGame = 0;
Menu DB_Menu;
ICajas IntefazJuegoCajas;

// Global para que no se pierda el valor del builder. Cada vez que se desocupe le hacemos un "unref"
GtkBuilder *builder;
// Situacion similar con el builder
GList *events = {0};

/*   Globales para el juego   */
int cardSelected=0;
int turno = 0; // Debe de ser de 0 a al numero de Jugadores - 1.
DatosJugador jugador[4] = {0};
cartas cardInit = {0};
DatosJugador bot[1]={0};

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
void menuPerfiles(GtkWidget *);
void menuLocalOnline(GObject *, gpointer);
void menuComoJugar(GObject *, gpointer);
void menuInstrucciones(GObject*);
void menuObjetivo(GObject*);
void menuSeleccionJugadores(GtkWidget *);
//PreJuego
void numJugadores(GtkWidget *);
void modoJuego(GtkWidget *);
void restartMenuPerfiles (GtkWidget *);
void guardarApodo(GtkWidget *);
void guardarPFP(GtkWidget *);
// Juego Principal
void startGame(GtkWidget *);
void repartirCartasInicio();
int comparacion_num_color(GtkWidget*, gpointer);
void ocultarValidar(GObject *);
void colocarCarta(GtkWidget *, cartas);
void vibrarCarta(GtkWidget *); //TODO:
int totalCartasUsuario(int);
void finjuego();
void getInfoCarta(int, int );
void comerCartas(); //TODO:
void colocoCard();
cartas* validarMaso();

  //Interfaz Juego Principal
GtkWidget* interfazJuego();

int main (int argc, char *argv[]) {
  srand(time(NULL));

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
void motionCard(GObject *eventBox, GdkEventConfigure *eventMotion, gpointer data){
  // Move "eventBox" upper when the event is GDK_ENTER_NOTIFY, if not move it down

  int time = 60; // Tiempo en milisegundos
  if(eventMotion->type == GDK_ENTER_NOTIFY){
    for (int i = 0; i < 5000; i++) {
      gtk_widget_set_margin_bottom(GTK_WIDGET(eventBox), i/time);
    }
  } else if (eventMotion->type == GDK_LEAVE_NOTIFY){
    for (int i = 5000; i > 1; i--) {
      gtk_widget_set_margin_bottom(GTK_WIDGET(eventBox), i/time);
    }
  }
}
void returnMain(GObject *init, gpointer* user_data){
  GtkWindow *windowClose;
  windowClose = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(init)));
  if(GTK_IS_WINDOW(windowClose)){
    gtk_widget_destroy(GTK_WIDGET(windowClose));
  }
  g_object_unref (builder);
  main(0,NULL);
}
// static void destroyWindow(GtkWidget *window, GdkEventConfigure *eventConfigure, gpointer *data){
//   if (closeAutoWindow == 0){
//     gtk_widget_destroy(window);
//     gtk_main_quit();
//   }
// }

/*  Funciones de Menus  */
// Menus Pre-Juego
void menuLocalOnline(GObject *buttonInit, gpointer user_data){
  g_print("Menu Local o Multijugador\n");
  /*   Cierre de ventana   */
  GtkWindow *windowClose;
  windowClose = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(buttonInit)));
  if(GTK_IS_WINDOW(windowClose)){
    gtk_widget_destroy(GTK_WIDGET(windowClose));
  }
  /*   Integracion del XML   */
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
void menuPerfiles(GtkWidget *buttonInit){
    /*   Cierre de ventana   */
    GtkWindow *windowClose={0};
    windowClose = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(buttonInit)));
    if(GTK_IS_WINDOW(windowClose)){
      gtk_widget_destroy(GTK_WIDGET(windowClose));
    }

    GtkWidget *window;
    window = GTK_WIDGET(gtk_builder_get_object (builder, "ProfileBuild"));
    // g_signal_connect (window, "destroy", G_CALLBACK (destroyWindow), NULL);

    /*   Obtencion de Objetos   */
    GObject *text, *eventGlade, *button;
    text = gtk_builder_get_object (builder, "NamePlayer");
    g_signal_connect (text, "focus_out_event", G_CALLBACK(guardarApodo), NULL);

    eventGlade = gtk_builder_get_object (builder, "AvatarM1");
    gtk_widget_set_name(GTK_WIDGET(eventGlade), "avatarW1");
    g_signal_connect (eventGlade, "button-release-event", G_CALLBACK(guardarPFP), NULL);

    eventGlade = gtk_builder_get_object (builder, "AvatarM2");
    gtk_widget_set_name(GTK_WIDGET(eventGlade), "avatarW2");
    g_signal_connect (eventGlade, "button-release-event", G_CALLBACK(guardarPFP), NULL);

    eventGlade = gtk_builder_get_object (builder, "AvatarM3");
    gtk_widget_set_name(GTK_WIDGET(eventGlade), "avatarW3");
    g_signal_connect (eventGlade, "button-release-event", G_CALLBACK(guardarPFP), NULL);

    eventGlade = gtk_builder_get_object (builder, "AvatarM4");
    gtk_widget_set_name(GTK_WIDGET(eventGlade), "avatarW4");
    g_signal_connect (eventGlade, "button-release-event", G_CALLBACK(guardarPFP), NULL);

    eventGlade = gtk_builder_get_object (builder, "AvatarM5");
    gtk_widget_set_name(GTK_WIDGET(eventGlade), "avatarW5");
    g_signal_connect (eventGlade, "button-release-event", G_CALLBACK(guardarPFP), NULL);

    eventGlade = gtk_builder_get_object (builder, "AvatarH1");
    gtk_widget_set_name(GTK_WIDGET(eventGlade), "avatarM1");
    g_signal_connect (eventGlade, "button-release-event", G_CALLBACK(guardarPFP), NULL);

    eventGlade = gtk_builder_get_object (builder, "AvatarH2");
    gtk_widget_set_name(GTK_WIDGET(eventGlade), "avatarM2");
    g_signal_connect (eventGlade, "button-release-event", G_CALLBACK(guardarPFP), NULL);

    eventGlade = gtk_builder_get_object (builder, "AvatarH3");
    gtk_widget_set_name(GTK_WIDGET(eventGlade), "avatarM3");
    g_signal_connect (eventGlade, "button-release-event", G_CALLBACK(guardarPFP), NULL);

    eventGlade = gtk_builder_get_object (builder, "AvatarH4");
    gtk_widget_set_name(GTK_WIDGET(eventGlade), "avatarM4");
    g_signal_connect (eventGlade, "button-release-event", G_CALLBACK(guardarPFP), NULL);

    eventGlade = gtk_builder_get_object (builder, "AvatarH5");
    gtk_widget_set_name(GTK_WIDGET(eventGlade), "avatarM5");
    g_signal_connect (eventGlade, "button-release-event", G_CALLBACK(guardarPFP), NULL);

    button = gtk_builder_get_object (builder, "Next");
    g_signal_connect (button, "clicked", G_CALLBACK(restartMenuPerfiles), NULL);

    gtk_widget_show_all(window) ;
}
void menuSeleccionJugadores(GtkWidget *buttonInit){
  g_print("Menu Cuantos Jugadores\n");
    /*   Cierre de ventana   */
  GtkWindow *windowClose;
  windowClose = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(buttonInit)));
  if(GTK_IS_WINDOW(windowClose)){
    gtk_widget_destroy(GTK_WIDGET(windowClose));
  }
  /*   Obtencion de Objetos   */
  GObject *Eventimg,*eventBot, *eventHuman;
  GtkWidget *window;
  window = GTK_WIDGET(gtk_builder_get_object (builder, "SelectPlayer"));
  // g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

  eventBot = gtk_builder_get_object (builder, "Bot");
  gtk_widget_set_name(GTK_WIDGET(eventBot), "Bot");
  g_signal_connect (eventBot, "button-release-event", G_CALLBACK(modoJuego), NULL);
  events = g_list_insert(events, eventBot, 0);

  eventHuman = gtk_builder_get_object (builder, "Human");
  gtk_widget_set_name(GTK_WIDGET(eventHuman), "Human");
  g_signal_connect (eventHuman, "button-release-event", G_CALLBACK(modoJuego), NULL);
  events = g_list_insert(events, eventHuman, 1);

  Eventimg = gtk_builder_get_object (builder, "1P");
  gtk_widget_set_name(GTK_WIDGET(Eventimg), "1P");
  g_signal_connect (Eventimg, "button-release-event", G_CALLBACK(numJugadores), NULL);

  Eventimg = gtk_builder_get_object (builder, "2P");
  gtk_widget_set_name(GTK_WIDGET(Eventimg), "2P");
  g_signal_connect (Eventimg, "button-release-event", G_CALLBACK(numJugadores), NULL);

  Eventimg = gtk_builder_get_object (builder, "3P");
  gtk_widget_set_name(GTK_WIDGET(Eventimg), "3P");
  g_signal_connect (Eventimg, "button-release-event",  G_CALLBACK(numJugadores), NULL);

  Eventimg = gtk_builder_get_object (builder, "4P");
  gtk_widget_set_name(GTK_WIDGET(Eventimg), "4P");
  g_signal_connect (Eventimg, "button-release-event", G_CALLBACK(numJugadores), NULL);

  gtk_widget_show_all(GTK_WIDGET(window));
  gtk_widget_hide(GTK_WIDGET(eventBot));
  gtk_widget_hide(GTK_WIDGET(eventHuman));
}

/*   Funciones para el PRE-JUEGO   */
void numJugadores(GtkWidget *buttonInit){
  GObject *eventHuman=NULL, *eventBot=NULL;
  const gchar* data = gtk_widget_get_name(GTK_WIDGET(buttonInit));

  eventBot = events->data;
  events = events->next;
  eventHuman = events->data;

  DB_Menu.numJugadores = atoi(&data[0]);
  g_print("Numero de Jugadores = %d!\n", DB_Menu.numJugadores);

  /* Si solo es un jugador, solo puede jugar con BOTS. De lo contrario puede jugar con HUMAN o BOTS */
  if (DB_Menu.numJugadores >= 2){
    gtk_widget_show(GTK_WIDGET(eventHuman));
  }
  gtk_widget_show(GTK_WIDGET(eventBot));
}
void modoJuego(GtkWidget *vsImg){
  const gchar* data = gtk_widget_get_name(GTK_WIDGET(vsImg));
  if (strcmp(data, "Bot") == 0)
    strcpy(DB_Menu.modJuego, "Bot");
  else if (strcmp(data, "Human") == 0){
    strcpy(DB_Menu.modJuego, "Human");
  }
  restartMenuPerfiles(vsImg);
}
void restartMenuPerfiles (GtkWidget *object){
  /* Redefino el builder porque al parecer despues de la segunda vuelta esta direccion se pierde o se hace in valida*/
  builder = gtk_builder_new ();
  gtk_builder_add_from_file (builder, "XML/PreJuego.glade", NULL);
  if (GTK_IS_BUTTON(object)) {
      if (*apRegistrados < DB_Menu.numJugadores) {
        menuPerfiles(object);
        *apRegistrados+=1;
      } else
        goGame = 1;
  } else {
    g_print("\nMenu Perfiles\n");
    menuPerfiles(object);
    *apRegistrados+=1;
  }
  if (goGame == 1) {
    // Limpia el constructor de las ventanas anteriores
    g_object_unref(builder);
    startGame(GTK_WIDGET(object));
  }
}
void guardarApodo(GtkWidget *entryText){
  const gchar *apodo =  gtk_entry_get_text  (GTK_ENTRY(entryText));
  g_print("Apodo Jugador %d : %s\n ", *apRegistrados, apodo);
  strcpy(DB_Menu.jugadores[*apRegistrados-1].apodo, apodo);
}
void guardarPFP(GtkWidget *playerImg){
  const gchar* nameImg = gtk_widget_get_name(GTK_WIDGET(playerImg));
  char path[MAX_PATH] = "..\\assets\\profiles\\";
  strcat(path, nameImg);
  strcat(path, ".png");
  strcpy(DB_Menu.jugadores[*apRegistrados-1].src, path);
  g_print("PFP %s\n", DB_Menu.jugadores[*apRegistrados-1].src);

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
              // gtk_widget_set_opacity(GTK_WIDGET(k->data), 0.7);
              gtk_widget_set_sensitive(GTK_WIDGET(k->data), FALSE);
            }
          }
        }
    }
  }
  g_list_free(children);
  g_list_free(l);
  g_list_free(k);
}

// Menus de Como JUgar
void menuComoJugar(GObject *buttonInit, gpointer user_data) {
  g_print("Menu Como Juagar (Instrucciones o Objetivo)\n");
  /*   Cierre de ventana   */
  GtkWindow *windowClose;
  windowClose = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(buttonInit)));
  if(GTK_IS_WINDOW(windowClose)){
    gtk_widget_destroy(GTK_WIDGET(windowClose));
  }
  /*   Integracion del XML   */
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
void menuInstrucciones(GObject *buttonInit){
  /*   Cierre de ventana   */
  GtkWindow *windowClose;
  GObject *window, *button;
  windowClose = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(buttonInit)));
  if(GTK_IS_WINDOW(windowClose)){
    gtk_widget_destroy(GTK_WIDGET(windowClose));
  }
  /*   Nuevo menu   */
  g_print("Menu instrucciones\n");
  window = gtk_builder_get_object (builder, "menuInstrucciones");
  button = gtk_builder_get_object (builder, "back-btn2");
  g_signal_connect (button, "clicked", G_CALLBACK (returnMain), NULL);

  gtk_widget_show_all(GTK_WIDGET(window));
}
void menuObjetivo(GObject *buttonInit) {
  g_print("Menu Objetivos\n");
  GObject *window, *button;

  /*   Cierre de ventana   */
  GtkWindow *windowClose;
  windowClose = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(buttonInit)));
  if(GTK_IS_WINDOW(windowClose)){
    gtk_widget_destroy(GTK_WIDGET(windowClose));
  }

  window = gtk_builder_get_object (builder, "menuObjetivo");
  button = gtk_builder_get_object (builder, "back-btn");
  g_signal_connect (button, "clicked", G_CALLBACK (returnMain), NULL);

  gtk_widget_show_all(GTK_WIDGET(window));
}

/*   Funciones para el JUEGO   */
// UNICAMENTE LOGICO
void startGame(GtkWidget *widget){
  /*
    status 0 - Inicia el Juego
    status 1 - Comprobacion y puesta en mesa
    status 2 - Fin del juego
  */
  int status;
  if(GTK_IS_BUTTON(widget))
    status = 0;
  else {
    if(GTK_IS_EVENT_BOX(widget))
        status = 1;
      else
        status = 2;
  }

  switch (status) {
  case 0:
    repartirCartasInicio();
    interfazJuego(widget);
    break;
  case 1:
    jugador[turno].numCartasTotales = totalCartasUsuario(turno);
    if(jugador[turno].numCartasTotales > 0){
      /* Primero hay que validar la baraja
       * Se valida si hay alguna carta disponibles para tirar          LISTO
       * -> Si las hay, checar si es la misma que clickeo el usuario
       *    - Si si, ponerla en mesa
       *    - Si no, vibrar la carta
       * De lo contrario, se indica que debe de comer del mazo */
      int cont=0;
      cartas *cartasValidas;
      cartasValidas = validarMaso();
      /* Variables para informacion de carta Clickeada */
      char tmp[10];
      const char *nameConst = gtk_widget_get_name(GTK_WIDGET(widget));
      strcpy(tmp, nameConst);
      char *IDChar = strtok(tmp, "_");
      IDChar = strtok(NULL, "_");
      int IDCard = atoi(IDChar);

      if (cartasValidas != 0){ // Valido el mazo
        while ((cartasValidas+cont)->numero >= 0) {
          g_print("Cartas Validas: Numero %d - Color %s\n", (cartasValidas+cont)->numero, (cartasValidas+cont)->color);
          cont++;
        }
        cont = 0;
        while ((cartasValidas+cont)->numero >= 0 && cardSelected == 0) { // Se cicla hasta comprobar todas las cartas que tiene el arreglo "CartasValidas
          // Es la misma que del usuario?
          if ((cartasValidas+cont)->numero == jugador[turno].baraja[IDCard-1].numero && strcmp((cartasValidas+cont)->color, jugador[turno].baraja[IDCard-1].color)==0) {
            jugador[turno].numCartasTotales-=1;
            // Coloca la carta y prende bandera (De que es posible pasar el turno)
            colocarCarta(widget, jugador[turno].baraja[IDCard-1]);
            cardSelected = 1;
          } else {
            cont++;
          }
        }
        if (cardSelected == 0){
          vibrarCarta(widget);
        }
      } else{ // No tiene ninguna carta, entonces come Y PASA TURNO
        g_print("\nNO tiene carta! Come");
        comerCartas();
        cardSelected == 1;
      }
      if (cardSelected == 1) {
        g_print("\n\n\t =>Cambio de TURNO!!!\n\n");
        // cartasValidas = validarMaso();
        // if (cartasValidas != 0) {
        //   while ((cartasValidas+cont)->numero > 0) {
        //     g_print("Cartas Validas: Numero %d - Color %s\n", (cartasValidas+cont)->numero, (cartasValidas+cont)->color);
        //     cont++;
        //   }
        // }
        // FIXME: Si es solo 1 jugador, debe pasar el BOT
        if(DB_Menu.numJugadores != 1){
          if(turno < (DB_Menu.numJugadores-1)){
            turno++;
            cardSelected=0;
          } else{
            turno = 0;
            cardSelected=0;
          }
        }
      }

      //Actualizar Interfaz - 2P
      /* 1. Intercambiar los BOX y renombrar
       * TOP: x-lonBox; y-22
       * DOWN: x-lonBox; y-488
      */

     // Actualizar pos X de la caja de arriba
     // Turno ya es el siguiente (Turno = 0 => 1; Turno = 1 => 0)
      IntefazJuegoCajas.lonUpBox = (1280-((jugador[turno].numCartasTotales*130)+(jugador[turno].numCartasTotales-1*3)))/2;
     // Moviendo la de Abajo -> Arriba
      gtk_fixed_move(GTK_FIXED(IntefazJuegoCajas.Fixed), IntefazJuegoCajas.DownBox, IntefazJuegoCajas.lonDownBox, 22);
      gtk_widget_set_name(IntefazJuegoCajas.DownBox, "UpBox");
     // Moviendo la de Arriba -> Abajo
      gtk_fixed_move(GTK_FIXED(IntefazJuegoCajas.Fixed), IntefazJuegoCajas.UpBox, IntefazJuegoCajas.lonUpBox, 488);
      gtk_widget_set_name(IntefazJuegoCajas.UpBox, "DownBox");
     // Intercalando las direcciones
      GtkWidget *aux = IntefazJuegoCajas.DownBox;
      IntefazJuegoCajas.DownBox = IntefazJuegoCajas.UpBox;
      IntefazJuegoCajas.UpBox = aux;

      /* 2. Actualizamos las imagenes y hacer insensible la caja de arriba y sensible la de abajo */
      // Obtener la ruta de la carta annon
      char file[_MAX_PATH] = {0};
      strcat(file, fullPath("assets\\MainCards\\small\\card_back.png"));

      // Controla que no se salga del arreglo
      int tempturno = turno-1;
      if (turno-1 < 0) {
        tempturno = 1;
      }

      // Ocultar cartas del jugador anterior (UpBox)
      int index=0;
      GList *childrenUp = gtk_container_get_children(GTK_CONTAINER(IntefazJuegoCajas.UpBox));
      GList *l=NULL;
      for (l = childrenUp; l != NULL; l = l->next) {
        // gtk_widget_set_sensitive(GTK_WIDGET(l->data), FALSE);
        g_signal_handler_disconnect(l->data, IntefazJuegoCajas.playerSignal[tempturno].idSignals[index].Click);
        g_signal_handler_disconnect(l->data, IntefazJuegoCajas.playerSignal[tempturno].idSignals[index].Motion);
        index++;
        GList *img = gtk_container_get_children(l->data);
        gtk_image_set_from_file(GTK_IMAGE(img->data), file);
        g_list_free(img);
      }
      g_list_free(childrenUp);
      g_list_free(l);

      // Mostrar las cartas del jugador actual (DownBox)
      index = 0;

      char *stringID = malloc(sizeof(char)+15);
      GList *childrenDown = gtk_container_get_children(GTK_CONTAINER(IntefazJuegoCajas.DownBox));
      GList *k = NULL;
      for (k = childrenDown; k != NULL; k = k->next) {
        GList *img = gtk_container_get_children(k->data);
        gtk_image_set_from_file(GTK_IMAGE(img->data), jugador[turno].baraja[index].srcCompleto);
        // Señales para el eventBOX
        IntefazJuegoCajas.playerSignal[turno].idSignals[index].Motion = g_signal_connect(G_OBJECT(k->data), "enter_notify_event", G_CALLBACK (motionCard), NULL);
        g_signal_connect (G_OBJECT(k->data), "leave_notify_event", G_CALLBACK (motionCard), NULL);
        IntefazJuegoCajas.playerSignal[turno].idSignals[index].Click = g_signal_connect(G_OBJECT(k->data), "button-release-event", G_CALLBACK(startGame), NULL);
        index++;
        sprintf(stringID, "P%d_%d", turno, index);
        gtk_widget_set_name(GTK_WIDGET(k->data), stringID);

        g_list_free(img);
      }
    } else
      startGame(NULL);
    break;
  case 2:
    finjuego();
    break;
  default:
    g_print("\nError! Estoy recibiendo un status... %d", status);
    break;
  }
}

void repartirCartasInicio(){
  int jugadoresRepartidos=0;
  int end, lengSRC,randNum,j;
  char tempString[MAX_PATH];
  char* srcSimple;

  while(jugadoresRepartidos < DB_Menu.numJugadores){
    for(int i=0; i<7; i++){
      randNum = rand() % 54;
      getInfoCarta(jugadoresRepartidos, randNum);
    }
    g_print("\n");
    jugadoresRepartidos++;
  }

// CARTA INICIAL ------------------------------------------------------------------------------------------
    randNum = rand() % 54;
    end=0, lengSRC=0; j=0;

    strcpy(cardInit.srcCompleto, image_names[randNum]);
    strcpy(tempString, image_names[randNum]);
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
          if(cardInit.srcSimple[0] != '\0')
            j++;
          else
            end=1;
          break;
      }
      lengSRC=strlen(cardInit.srcSimple);//Obtiene la ultima letra de la cadena, sabiendo asi su numero o poder de la carta
      switch (cardInit.srcSimple[lengSRC-5]){
          case '0':
            cardInit.numero=0;
            break;
          case '1':
            cardInit.numero=1;
            break;
          case '2':
            cardInit.numero=2;
            break;
          case '3':
            cardInit.numero=3;
            break;
          case '4':
            cardInit.numero=4;
            break;
          case '5':
            cardInit.numero=5;
            break;
          case '6':
            cardInit.numero=6;
            break;
          case '7':
            cardInit.numero=7;
            break;
          case '8':
            cardInit.numero=8;
            break;
          case '9':
            cardInit.numero=9;
            break;
          case 'r':/*Carta + 2*/
            cardInit.numero=10;
            break;
          case 'k':/*Carta de +4*/
            cardInit.numero=11;
            break;
          case 'e':/*Carta de reversa*/
            cardInit.numero=12;
            break;
          case 'p':/*Carta para negar turno*/
            cardInit.numero=13;
            break;
          case 's':/*Carta de Comodin*/
            cardInit.numero=14;
           break;
          }
    if(cardInit.numero == 11 || cardInit.numero == 14)
      end = 0;
    }
    if(cardInit.color == NULL)
      g_print("Error en el color\n");
    else
      g_print("Carta Inicial - %s - %d\n",cardInit.color, cardInit.numero);
}

int comparacion_num_color(GtkWidget *card, gpointer aPturno) {
  int turno = GPOINTER_TO_INT(aPturno);

  // DE ACUERDO A LA SELECCION DEL USUARIO
  // El ID es la carta que selecciono el usuario de su baraja
  // Este ID es proporcionado por el GLADE / INTERFAZ
  // Puedes hacer un GLIST para recuperar los child de la GBOX
  // Y reasignar el ID a las cartas, asi poder compararlas.

  int indexCartaComida;
  const char* nameConst = gtk_widget_get_name(GTK_WIDGET(card));
  char* tmp;
  strcpy(tmp, nameConst);
  char *IDChar = strtok(tmp, "_");
  IDChar = strtok(NULL, "_");

  int ID = atoi(IDChar)-1;

  if(strcmp(jugador[turno].baraja[ID].color, cardInit.color)==0 || jugador[turno].baraja[ID].numero==cardInit.numero) {
     printf("\nTiro de carta");
     return 1;
  } else { // Si no es del NUMERO O No es del COLOR
    /* Si la carta no es correcta entonces no ponerla y poner un motion card donde vibre la carta!*/
    return 0;
  }
  return 0;
}
cartas *validarMaso(){
  int j=0, band=0;
  cartas *arr = malloc(sizeof(cartas)*MAX_CARDS);

  for (int i = 0; i < jugador[turno].numCartasTotales; i++) {
    if(jugador[turno].baraja[i].numero == cardInit.numero || strcmp(jugador[turno].baraja[i].color, cardInit.color)==0) {
      *(arr+j) = jugador[turno].baraja[i];
      j++;
      band = 1;
    } else if (jugador[turno].baraja[i].numero == cardInit.numero && strcmp(jugador[turno].baraja[i].color, cardInit.color)==0) {
      *(arr+j) = jugador[turno].baraja[i];
      j++;
      band = 1;
    } else {
    }
  }
  if (!band)
    return NULL;
  else
    return arr;
}

void colocarCarta(GtkWidget *eventBoxCarta, cartas cartaSeleccionada) {
  /* Debo de remover la carta del BOX y sustituirla por la carta Init */
  GtkWidget *Box = gtk_widget_get_parent(eventBoxCarta);
  GtkContainer *fixed = GTK_CONTAINER(gtk_widget_get_parent(Box));
  GError *error = NULL;

  /* 1. Quitar el hijo de la caja y centra la caja */
  /* Centrado de la caja:
   * Sumar cada carta + 3 de espacio
   * El resutlado lo restamos con 1280
   * Lo dividimos en 2 y es "X"
  */
  IntefazJuegoCajas.Fixed = GTK_WIDGET(fixed);
  gtk_widget_destroy(eventBoxCarta);
  IntefazJuegoCajas.lonDownBox = (1280-((jugador[turno].numCartasTotales*130)+(jugador[turno].numCartasTotales-1*3)))/2;
  gtk_fixed_move(GTK_FIXED(fixed), Box, IntefazJuegoCajas.lonDownBox, 488);

  /* 2. Susituir */
  int cardIndex=1;
  if (GTK_IS_CONTAINER(fixed)) {
    GList *children = gtk_container_get_children(fixed);
    GList *l = NULL, *k = NULL, *j = NULL;
    for (l = children; l != NULL; l = l->next) {
      const char *nameChildren = gtk_widget_get_name(l->data);
      if (strcmp(nameChildren, "CentroBox") == 0){
        GList *boxChildren = gtk_container_get_children(l->data);
        for (k = boxChildren; k != NULL; k = k->next) {
          const char *nameChildrenBox = gtk_widget_get_name(k->data);
          if (strcmp(nameChildrenBox, "CentroEventBox") == 0){
            GList *img = gtk_container_get_children(k->data);
            const char *nameChildrenBoxImg = gtk_widget_get_name(img->data);
            gtk_image_set_from_file(GTK_IMAGE(img->data), cartaSeleccionada.srcCompleto);
          }
        }
      }
      /* Para setear el ID:
      * Obtener todos los hijos (EventBox) del box
      * Usar la funcion para renombrar
      */
      if(strcmp(nameChildren, "AbajoBox") == 0){
        IntefazJuegoCajas.DownBox = GTK_WIDGET(l->data);
        GList *abajoBoxChildren = gtk_container_get_children(l->data);
        for (j = abajoBoxChildren; j != NULL; j = j->next){
          if (GTK_IS_EVENT_BOX(j->data)){
            /* Construir el string (P*TURNO+1*_*NUMCARD*) */
            char *stringID = malloc(sizeof(char)+15);
            sprintf(stringID, "P%d_%d", turno+1, cardIndex);
            gtk_widget_set_name(GTK_WIDGET(j->data), stringID);
            cardIndex++;
          }
        }
      }
      if (strcmp(nameChildren, "UpBox")==0) {
        IntefazJuegoCajas.UpBox = GTK_WIDGET(l->data);
      }

    }
  }
  /* 3. Recorrer las cartas */
  /* Para el arreglo:
   * Buscar la carta que colocamos
   * Elimirar su campo
   * Recorrer todos los que estan a la derecha del elemento
  */
  for(int i = 0; i < jugador[turno].numCartasTotales; i++){
      if(cartaSeleccionada.numero == jugador[turno].baraja[i].numero && strcmp(cartaSeleccionada.color, jugador[turno].baraja[i].color) == 0 ){
          memset(jugador[turno].baraja[i].color, 0, sizeof(cartas));
          IntefazJuegoCajas.playerSignal[turno].idSignals[i].Click = 0;
          IntefazJuegoCajas.playerSignal[turno].idSignals[i].Motion = 0;
      }
  }
  int j,k;
  cartas aux={0};
  signalIDs auxID;

  for (j = 0; j < jugador[turno].numCartasTotales; j++) {
    for (k = 0; k <= (jugador[turno].numCartasTotales-1); k++) {
      // Quiero comprobar cual carta es la 0
      if(jugador[turno].baraja[k].color[0] == 0 && jugador[turno].baraja[k].numero==0){
        aux=jugador[turno].baraja[k];
        auxID=IntefazJuegoCajas.playerSignal[turno].idSignals[k];
        jugador[turno].baraja[k]=jugador[turno].baraja[k+1];
        IntefazJuegoCajas.playerSignal[turno].idSignals[k] = IntefazJuegoCajas.playerSignal[turno].idSignals[k+1];
        jugador[turno].baraja[k+1]=aux;
        IntefazJuegoCajas.playerSignal[turno].idSignals[k+1] = auxID;
      }
    }
  }
  /* 4. Actualzar cardInit */
  cardInit = cartaSeleccionada;

}
void vibrarCarta(GtkWidget *carta) {
  /*Usando la funcion motioin card hay que hacer que se mueva menos y
  a mayor tiempo*/
  // Move "eventBox" upper when the event is GDK_ENTER_NOTIFY, if not move it down
  int time = 30, i, j=5000; // FPS
    for (i = 0; i < 5000; i++) {
      gtk_widget_set_margin_bottom(GTK_WIDGET(carta), i/time);//Esta fuincion hace que suba la carta
      gtk_widget_set_margin_bottom(GTK_WIDGET(carta), j/time);//Esta funcion hacer que baje la carta  tomando comom valor la j
      j--;
    }
    /*La idea de lo que hace el for es que suba y baje la carta en un tiempo tan coro que solo prezca quie vibra*/

  g_print("Se desabilita la carta osea vibra asi bien chido :)\n");

}
int totalCartasUsuario(int turno){
   int i=0;
    while (jugador[turno].baraja[i].color[0]!='\0') {
          i++;
    }
    return i;
}
void comerCartas(){
  system("pause");
}
void finjuego(){
  g_print("Pasar a Menu de Victoria/Derrota/Puntuaciones\n");
}

void getInfoCarta(int numJugador, int cartaRandom){
      char tempString[MAX_PATH];
      char* srcSimple;
      int end=0, lengSRC=0, j=0;

      /* Obtiene la direccion de la imagen de la carta */
      strcpy(jugador[numJugador].baraja[jugador[numJugador].numCartasTotales].srcCompleto, image_names[cartaRandom]);
      strcpy(tempString, image_names[cartaRandom]);
      srcSimple = strtok(tempString, "\\");
      srcSimple = strtok(NULL, "\\");
      srcSimple = strtok(NULL, "\\");
      srcSimple = strtok(NULL, "\\");
      strcpy(jugador[numJugador].baraja[jugador[numJugador].numCartasTotales].srcSimple,srcSimple);

      /* Obtiene que color es y lo guarda en la estructura del jugador */
      while(end != 1){
        switch(jugador[numJugador].baraja[jugador[numJugador].numCartasTotales].srcSimple[j]) {
          case 'r':
           strcpy(jugador[numJugador].baraja[jugador[numJugador].numCartasTotales].color, "Rojo");
              end=1;
            break;
          case 'b':
            strcpy(jugador[numJugador].baraja[jugador[numJugador].numCartasTotales].color, "Azul");
            end=1;
            break;
          case 'y':
            strcpy(jugador[numJugador].baraja[jugador[numJugador].numCartasTotales].color, "Amarillo");
            end=1;
            break;
          case 'g':
            strcpy(jugador[numJugador].baraja[jugador[numJugador].numCartasTotales].color, "Verde");
            end=1;
            break;
          case 'f':
            strcpy(jugador[numJugador].baraja[jugador[numJugador].numCartasTotales].color, "ComodinMas4");
            end=1;
            break;
          case 'c':
            strcpy(jugador[numJugador].baraja[jugador[numJugador].numCartasTotales].color, "Comodin");
            end=1;
          break;
          case 'p':
            strcpy(jugador[numJugador].baraja[jugador[numJugador].numCartasTotales].color, "ComodinMas2");
            end=1;
            break;
          default:
            if(jugador[numJugador].baraja[jugador[numJugador].numCartasTotales].srcSimple[jugador[numJugador].numCartasTotales] != '\0')
              j++;
            else
              end=1;
            break;
        }

        /* Switch que valida el numero de las cartas y si es un +2, reversa, negar turno etc... */
        lengSRC=strlen(jugador[numJugador].baraja[jugador[numJugador].numCartasTotales].srcSimple);
        switch (jugador[numJugador].baraja[jugador[numJugador].numCartasTotales].srcSimple[lengSRC-5]){
           case '0':
             jugador[numJugador].baraja[jugador[numJugador].numCartasTotales].numero=0;
             break;
           case '1':
             jugador[numJugador].baraja[jugador[numJugador].numCartasTotales].numero=1;
             break;
           case '2':
             jugador[numJugador].baraja[jugador[numJugador].numCartasTotales].numero=2;
             break;
           case '3':
             jugador[numJugador].baraja[jugador[numJugador].numCartasTotales].numero=3;
             break;
           case '4':
             jugador[numJugador].baraja[jugador[numJugador].numCartasTotales].numero=4;
             break;
           case '5':
             jugador[numJugador].baraja[jugador[numJugador].numCartasTotales].numero=5;
             break;
           case '6':
             jugador[numJugador].baraja[jugador[numJugador].numCartasTotales].numero=6;
             break;
           case '7':
             jugador[numJugador].baraja[jugador[numJugador].numCartasTotales].numero=7;
             break;
           case '8':
             jugador[numJugador].baraja[jugador[numJugador].numCartasTotales].numero=8;
             break;
           case '9':
             jugador[numJugador].baraja[jugador[numJugador].numCartasTotales].numero=9;
             break;
           case 'r':/*Carta + 2*/
             jugador[numJugador].baraja[jugador[numJugador].numCartasTotales].numero=10;
             break;
           case 'w':/*Carta de +4*/
             jugador[numJugador].baraja[jugador[numJugador].numCartasTotales].numero=11;
             break;
           case 'e':/*Carta de reversa*/
             jugador[numJugador].baraja[jugador[numJugador].numCartasTotales].numero=12;
             break;
           case 'p':/*Carta para negar turno*/
             jugador[numJugador].baraja[jugador[numJugador].numCartasTotales].numero=13;
             break;
           }
      }
      if(jugador[numJugador].baraja[jugador[numJugador].numCartasTotales].color == NULL)
        g_print("Error en el color\n");
      else{
        g_print("Jugador %d - Carta %d - %s - %d\n", numJugador, jugador[numJugador].numCartasTotales, jugador[numJugador].baraja[jugador[numJugador].numCartasTotales].color, jugador[numJugador].baraja[jugador[numJugador].numCartasTotales].numero);
        jugador[numJugador].numCartasTotales+=1;
      }
}

// UNICAMENTE INTERFAZ
GtkWidget* interfazJuego(GtkWidget *widget){

  GtkWidget *windowClose = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(widget)));
  if(GTK_IS_WINDOW(windowClose)){
    gtk_widget_destroy(GTK_WIDGET(windowClose));
  }

  g_print("\n\t> Inicio del juego - INTERFAZ\n");
  GObject *boxA, *boxAB, *boxC,  *event, *img;
  GtkWidget *window;
  GError *error = NULL;

  builder = gtk_builder_new ();
  if (gtk_builder_add_from_file (builder, "XML/menuplay2jugadores.glade", &error) == 0) {
    g_printerr ("Error loading file: %s\n", error->message);
    g_clear_error (&error);
  }

  window = GTK_WIDGET(gtk_builder_get_object (builder, "menuplay2jugadores"));

  boxA = gtk_builder_get_object(builder, "Arriba");
  gtk_widget_set_name(GTK_WIDGET(boxA), "UpBox");

  boxAB = gtk_builder_get_object(builder, "Abajo");
  gtk_widget_set_name(GTK_WIDGET(boxAB), "AbajoBox");


  /* Lo que debe de enviar de la click de la carta:
   * - El objeto (EventBox)
   * - Que estatus mandar, en todo caso es el 2
   *  -> No pude mandar un puntero entonces hago la comprobacion por el widget
  */

  event = gtk_builder_get_object(builder, "Ab1");
  img = gtk_builder_get_object(builder, "IAb1");
  gtk_widget_set_name(GTK_WIDGET(event), "P1_1");
  IntefazJuegoCajas.playerSignal[0].idSignals[0].Click = g_signal_connect(event, "button-release-event", G_CALLBACK(startGame), NULL);
  IntefazJuegoCajas.playerSignal[0].idSignals[0].Motion = g_signal_connect (event, "enter_notify_event", G_CALLBACK (motionCard), img);
  g_signal_connect (event, "leave_notify_event", G_CALLBACK (motionCard), img);
  gtk_image_set_from_file(GTK_IMAGE(img), jugador[0].baraja[0].srcCompleto);

  event = gtk_builder_get_object(builder, "Ab2");
  img = gtk_builder_get_object(builder, "IAb2");
  gtk_widget_set_name(GTK_WIDGET(event), "P1_2");
  IntefazJuegoCajas.playerSignal[0].idSignals[1].Click = g_signal_connect(event, "button-release-event", G_CALLBACK(startGame), NULL);
  IntefazJuegoCajas.playerSignal[0].idSignals[1].Motion = g_signal_connect (event, "enter_notify_event", G_CALLBACK (motionCard), img);
  g_signal_connect (event, "leave_notify_event", G_CALLBACK (motionCard), img);
  gtk_image_set_from_file(GTK_IMAGE(img), jugador[0].baraja[1].srcCompleto);


  event = gtk_builder_get_object(builder, "Ab3");
  img = gtk_builder_get_object(builder, "IAb3");
  gtk_widget_set_name(GTK_WIDGET(event), "P1_3");
  IntefazJuegoCajas.playerSignal[0].idSignals[2].Click = g_signal_connect(event, "button-release-event", G_CALLBACK(startGame), NULL);
  IntefazJuegoCajas.playerSignal[0].idSignals[2].Motion = g_signal_connect (event, "enter_notify_event", G_CALLBACK (motionCard), img);
  g_signal_connect (event, "leave_notify_event", G_CALLBACK (motionCard), img);
  gtk_image_set_from_file(GTK_IMAGE(img), jugador[0].baraja[2].srcCompleto);


  event = gtk_builder_get_object(builder, "Ab4");
  img = gtk_builder_get_object(builder, "IAb4");
  gtk_widget_set_name(GTK_WIDGET(event), "P1_4");
  IntefazJuegoCajas.playerSignal[0].idSignals[3].Click = g_signal_connect(event, "button-release-event", G_CALLBACK(startGame), NULL);
  IntefazJuegoCajas.playerSignal[0].idSignals[3].Motion = g_signal_connect (event, "enter_notify_event", G_CALLBACK (motionCard), img);
  g_signal_connect (event, "leave_notify_event", G_CALLBACK (motionCard), img);
  gtk_image_set_from_file(GTK_IMAGE(img), jugador[0].baraja[3].srcCompleto);

  event = gtk_builder_get_object(builder, "Ab5");
  img = gtk_builder_get_object(builder, "IAb5");
  gtk_widget_set_name(GTK_WIDGET(event), "P1_5");
  IntefazJuegoCajas.playerSignal[0].idSignals[4].Click = g_signal_connect(event, "button-release-event", G_CALLBACK(startGame), NULL);
  IntefazJuegoCajas.playerSignal[0].idSignals[4].Motion = g_signal_connect (event, "enter_notify_event", G_CALLBACK (motionCard), img);
  g_signal_connect (event, "leave_notify_event", G_CALLBACK (motionCard), img);
  gtk_image_set_from_file(GTK_IMAGE(img), jugador[0].baraja[4].srcCompleto);

  event = gtk_builder_get_object(builder, "Ab6");
  img = gtk_builder_get_object(builder, "IAb6");
  gtk_widget_set_name(GTK_WIDGET(event), "P1_6");
  IntefazJuegoCajas.playerSignal[0].idSignals[5].Click = g_signal_connect(event, "button-release-event", G_CALLBACK(startGame), NULL);
  IntefazJuegoCajas.playerSignal[0].idSignals[5].Motion = g_signal_connect (event, "enter_notify_event", G_CALLBACK (motionCard), img);
  g_signal_connect (event, "leave_notify_event", G_CALLBACK (motionCard), img);
  gtk_image_set_from_file(GTK_IMAGE(img), jugador[0].baraja[5].srcCompleto);

  event = gtk_builder_get_object(builder, "Ab7");
  img = gtk_builder_get_object(builder, "IAb7");
  gtk_widget_set_name(GTK_WIDGET(event), "P1_7");
  IntefazJuegoCajas.playerSignal[0].idSignals[6].Click = g_signal_connect(event, "button-release-event", G_CALLBACK(startGame), NULL);
  IntefazJuegoCajas.playerSignal[0].idSignals[6].Motion = g_signal_connect (event, "enter_notify_event", G_CALLBACK (motionCard), img);
  g_signal_connect (event, "leave_notify_event", G_CALLBACK (motionCard), img);
  gtk_image_set_from_file(GTK_IMAGE(img), jugador[0].baraja[6].srcCompleto);

  boxC = gtk_builder_get_object(builder, "Medio");
  gtk_widget_set_name(GTK_WIDGET(boxC), "CentroBox");

  event =  gtk_builder_get_object(builder, "Juego");
  gtk_widget_set_name(GTK_WIDGET(event), "CentroEventBox");

  img = gtk_builder_get_object(builder, "IJuego");
  gtk_widget_set_name(GTK_WIDGET(img), "CentroImg");
  gtk_image_set_from_file(GTK_IMAGE(img), cardInit.srcCompleto);

  event = gtk_builder_get_object(builder, "Comida");
  // TODO: Falta linkear eventos para los demas jugadores

  gtk_widget_show_all(window);
  return window;
}