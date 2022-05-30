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

/*   Globales   */
GObject *windowMain;
static GstElement *playbin, *play, *sink;
static int registrados = 0, *apRegistrados = &registrados, goGame = 0;
Menu DB_Menu;

// Global para que no se pierda el valor del builder. Cada vez que se desocupe le hacemos un "unref"
GtkBuilder *builder;
// Situacion similar con el builder
GList *events = {0};

/*   Globales para el juego   */
int cardSelected=0;
DatosJugador jugador[4] = {0};
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
void menuPerfiles(GObject *);
void menuLocalOnline(GObject *, gpointer);
void menuComoJugar(GObject *, gpointer);
void menuInstrucciones(GObject*);
void menuObjetivo(GObject*);
void menuSeleccionJugadores(GObject *);
//PreJuego
void numJugadores(GObject *);
void modoJuego(GObject *);
void restartMenuPerfiles (GObject *);
void guardarApodo(GObject *);
void guardarPFP(GObject *);
// Juego Principal
void startGame(GtkWidget *, gpointer *);
void repartirCartasInicio();
int comparacion_num_color(GtkWidget*, gpointer);
void ocultarValidar(GObject *);
void colocarCarta(); //TODO:
void desabilitarCarta(); //TODO:
int totalCartasUsuario(int);
void finjuego();
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
    g_print("Se cierra: %s\n", gtk_window_get_title (GTK_WINDOW (windowClose)));
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
    g_print("Se cierra: %s\n", gtk_window_get_title (GTK_WINDOW (windowClose)));
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
void menuPerfiles(GObject *buttonInit){
    /*   Cierre de ventana   */
    GtkWindow *windowClose={0};
    windowClose = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(buttonInit)));
    g_print("\t=> %s", gtk_widget_get_name(GTK_WIDGET(windowClose)));
    if(GTK_IS_WINDOW(windowClose)){
      g_print("\nSe cierra: %s\n", gtk_window_get_title (GTK_WINDOW (windowClose)));
      gtk_widget_destroy(GTK_WIDGET(windowClose));
    }

    g_print("\nMenu Perfiles\n");
    GObject *window;
    window = gtk_builder_get_object (builder, "ProfileBuild");
    // g_signal_connect (window, "destroy", G_CALLBACK (destroyWindow), NULL);

    /*   Obtencion de Objetos   */
    GObject *text=NULL, *event=NULL, *button=NULL;
    // text = gtk_builder_get_object (builder, "NamePlayer");
    // g_signal_connect (text, "focus_out_event", G_CALLBACK(guardarApodo), NULL);

    // event = gtk_builder_get_object (builder, "AvatarM1");
    // gtk_widget_set_name(GTK_WIDGET(event), "avatarW1");
    // g_signal_connect (event, "button-release-event", G_CALLBACK(guardarPFP), NULL);

    // event = gtk_builder_get_object (builder, "AvatarM2");
    // gtk_widget_set_name(GTK_WIDGET(event), "avatarW2");
    // g_signal_connect (event, "button-release-event", G_CALLBACK(guardarPFP), NULL);

    // event = gtk_builder_get_object (builder, "AvatarM3");
    // gtk_widget_set_name(GTK_WIDGET(event), "avatarW3");
    // g_signal_connect (event, "button-release-event", G_CALLBACK(guardarPFP), NULL);

    // event = gtk_builder_get_object (builder, "AvatarM4");
    // gtk_widget_set_name(GTK_WIDGET(event), "avatarW4");
    // g_signal_connect (event, "button-release-event", G_CALLBACK(guardarPFP), NULL);

    // event = gtk_builder_get_object (builder, "AvatarM5");
    // gtk_widget_set_name(GTK_WIDGET(event), "avatarW5");
    // g_signal_connect (event, "button-release-event", G_CALLBACK(guardarPFP), NULL);

    // event = gtk_builder_get_object (builder, "AvatarH1");
    // gtk_widget_set_name(GTK_WIDGET(event), "avatarM1");
    // g_signal_connect (event, "button-release-event", G_CALLBACK(guardarPFP), NULL);

    // event = gtk_builder_get_object (builder, "AvatarH2");
    // gtk_widget_set_name(GTK_WIDGET(event), "avatarM2");
    // g_signal_connect (event, "button-release-event", G_CALLBACK(guardarPFP), NULL);

    // event = gtk_builder_get_object (builder, "AvatarH3");
    // gtk_widget_set_name(GTK_WIDGET(event), "avatarM3");
    // g_signal_connect (event, "button-release-event", G_CALLBACK(guardarPFP), NULL);

    // event = gtk_builder_get_object (builder, "AvatarH4");
    // gtk_widget_set_name(GTK_WIDGET(event), "avatarM4");
    // g_signal_connect (event, "button-release-event", G_CALLBACK(guardarPFP), NULL);

    // event = gtk_builder_get_object (builder, "AvatarH5");
    // gtk_widget_set_name(GTK_WIDGET(event), "avatarM5");
    // g_signal_connect (event, "button-release-event", G_CALLBACK(guardarPFP), NULL);

    // button = gtk_builder_get_object (builder, "Next");
    // g_signal_connect (button, "clicked", G_CALLBACK(restartMenuPerfiles), NULL);

    gtk_widget_show_all(GTK_WIDGET(window)) ;
}
void menuSeleccionJugadores(GObject *buttonInit){
  g_print("Menu Cuantos Jugadores\n");
    /*   Cierre de ventana   */
  GtkWindow *windowClose;
  windowClose = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(buttonInit)));
  if(GTK_IS_WINDOW(windowClose)){
    g_print("Se cierra: %s\n", gtk_window_get_title (GTK_WINDOW (windowClose)));
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
void numJugadores(GObject *buttonInit){
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
void modoJuego(GObject *vsImg){
  const gchar* data = gtk_widget_get_name(GTK_WIDGET(vsImg));
  g_print("VS = %s!\n", data);
  if (strcmp(data, "Bot") == 0)
    strcpy(DB_Menu.modJuego, "Bot");
  else if (strcmp(data, "Human") == 0){
    strcpy(DB_Menu.modJuego, "Human");
  }
  g_print("\n\t =>%s!\n",gtk_widget_get_name(GTK_WIDGET(vsImg)));
  menuPerfiles(vsImg);
  *apRegistrados+=1;
  g_print("\n\t =>%s!\n",gtk_widget_get_name(GTK_WIDGET(vsImg)));
  // FIXME: La segunda llamada al parecer tiene una modificacion en el XML o la estructura del UI. Debido a que del EventBox no encuentra el padre de este.
  menuPerfiles(vsImg);
  *apRegistrados+=1;
}
void restartMenuPerfiles (GObject *object){
  // Si o si debe ser un boton, porque viene del MENU_PERFILES
  if (GTK_IS_BUTTON(object)) {
      if (*apRegistrados < DB_Menu.numJugadores) {
        menuPerfiles(object);
        *apRegistrados+=1;
      } else
        goGame = 1;
  }
  if (goGame == 1) {
    // Limpia el constructor de las ventanas anteriores
    g_object_unref(builder);
    startGame(GTK_WIDGET(object), GINT_TO_POINTER(1));
  }
}
void guardarApodo(GObject *entryText){
  const gchar *apodo =  gtk_entry_get_text  (GTK_ENTRY(entryText));
  g_print("Apodo Jugador %d : %s\n", *apRegistrados, apodo);
  strcpy(DB_Menu.jugadores[*apRegistrados-1].apodo, apodo);
}
void guardarPFP(GObject *playerImg){
  g_print("Player %d\n", *apRegistrados);
  const gchar* nameImg = gtk_widget_get_name(GTK_WIDGET(playerImg));
  char path[MAX_PATH] = "..\\assets\\profiles\\";
  strcat(path, nameImg);
  strcat(path, ".png");
  strcpy(DB_Menu.jugadores[*apRegistrados-1].src, path);

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
    g_print("Se cierra: %s\n", gtk_window_get_title (GTK_WINDOW (windowClose)));
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
}
void menuObjetivo(GObject *buttonInit) {
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
}

/*   Funciones para el JUEGO   */
// UNICAMENTE LOGICO
void startGame(GtkWidget *widget, gpointer *aPstatus){
  int status = GPOINTER_TO_INT(aPstatus);
  int turno = 0; // Debe de ser de 0 a al numero de Jugadores - 1.

  GtkWidget *windowGame;
  gpointer aPturno = GINT_TO_POINTER(turno);
  /*
    status 0 - Inicia el Juego
    status 1 - Comprobacion y puesta en mesa
    status 2 - Fin del juego
  */
    // FIXME: CRASH!
    // Esperar la accion del GTK, ya estara redireccionada a la funcion correspondiente.
    // No podemos seguir linealmente porque el boton de la interfaz ya redirecciona a la funcion de validacion.
    // Aparte de que es necesario hacer que la funcion sea un CALLBACK, ya que si no nunca regresara a esta funcion.

    // Podemos solucionarlo con un control de variable
    // TODO: Por medio de un switch podemos hacer un handle (control de variable)

  switch (status) {
  case 1:
    repartirCartasInicio();
    windowGame = interfazJuego(widget);
    break;

  case 2:
    jugador[turno].numCartasTotales = totalCartasUsuario(turno);
    if(jugador[turno].numCartasTotales > 0){
      g_print("Aun existen cartas!\n");

      if (comparacion_num_color(widget, aPturno) == 1)
        colocarCarta();
      else
        // Nunca deberia de llegar aqui!
        desabilitarCarta();
    } else
      startGame(NULL, GINT_TO_POINTER(3));

    if(turno < (DB_Menu.numJugadores-1))
      turno++;
    else
      turno = 0;
    break;

  case 3:
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
      end=0, lengSRC=0; j=0;
      memset(srcSimple,'\0', sizeof(srcSimple));

      strcpy(jugador[jugadoresRepartidos].baraja[i].srcCompleto, image_names[randNum]);
      strcpy(tempString, image_names[randNum]);
      srcSimple = strtok(tempString, "\\");
      srcSimple = strtok(NULL, "\\");
      srcSimple = strtok(NULL, "\\");
      srcSimple = strtok(NULL, "\\");

      strcpy(jugador[jugadoresRepartidos].baraja[i].srcSimple,srcSimple);
      while(end != 1){
        switch(jugador[jugadoresRepartidos].baraja[i].srcSimple[j]) {
          case 'r':
           strcpy(jugador[jugadoresRepartidos].baraja[i].color, "Rojo");
              end=1;
            break;
          case 'b':
            strcpy(jugador[jugadoresRepartidos].baraja[i].color, "Azul");
            end=1;
            break;
          case 'y':
            strcpy(jugador[jugadoresRepartidos].baraja[i].color, "Amarillo");
            end=1;
            break;
          case 'g':
            strcpy(jugador[jugadoresRepartidos].baraja[i].color, "Verde");
            end=1;
            break;
          case 'f':
            strcpy(jugador[jugadoresRepartidos].baraja[i].color, "ComodinMas4");
            end=1;
            break;
          case 'c':
            strcpy(jugador[jugadoresRepartidos].baraja[i].color, "Comodin");
            end=1;
          break;
          case 'p':
            strcpy(jugador[jugadoresRepartidos].baraja[i].color, "ComodinMas2");
            end=1;
            break;
          default:
            if(jugador[jugadoresRepartidos].baraja[i].srcSimple[i] != '\0')
              j++;
            else
              end=1;
            break;
        }
        /* Switch que valida el numero de las cartas y si es un +2, reversa, negar turno etc... */
        lengSRC=strlen(jugador[jugadoresRepartidos].baraja[i].srcSimple);//Obtiene la ultima letra de la cadena, sabiendo asi su numero o poder de la carta
        switch (jugador[jugadoresRepartidos].baraja[i].srcSimple[lengSRC-5]){//Se resta menos cinco ya que se toma en cuenta el .png
           case '0':
             jugador[jugadoresRepartidos].baraja[i].numero=0;
             break;
           case '1':
             jugador[jugadoresRepartidos].baraja[i].numero=1;
             break;
           case '2':
             jugador[jugadoresRepartidos].baraja[i].numero=2;
             break;
           case '3':
             jugador[jugadoresRepartidos].baraja[i].numero=3;
             break;
           case '4':
             jugador[jugadoresRepartidos].baraja[i].numero=4;
             break;
           case '5':
             jugador[jugadoresRepartidos].baraja[i].numero=5;
             break;
           case '6':
             jugador[jugadoresRepartidos].baraja[i].numero=6;
             break;
           case '7':
             jugador[jugadoresRepartidos].baraja[i].numero=7;
             break;
           case '8':
             jugador[jugadoresRepartidos].baraja[i].numero=8;
             break;
           case '9':
             jugador[jugadoresRepartidos].baraja[i].numero=9;
             break;
           case 'r':/*Carta + 2*/
             jugador[jugadoresRepartidos].baraja[i].numero=10;
             break;
           case 'w':/*Carta de +4*/
             jugador[jugadoresRepartidos].baraja[i].numero=11;
             break;
           case 'e':/*Carta de reversa*/
             jugador[jugadoresRepartidos].baraja[i].numero=12;
             break;
           case 'p':/*Carta para negar turno*/
             jugador[jugadoresRepartidos].baraja[i].numero=13;
             break;
           }
      }
      if(jugador[jugadoresRepartidos].baraja[i].color == NULL)
        g_print("Error en el color\n");
      else
        g_print("Jugador %d - Carta %d - %s - %d\n", jugadoresRepartidos, i, jugador[jugadoresRepartidos].baraja[i].color, jugador[jugadoresRepartidos].baraja[i].numero);
    }
    jugadoresRepartidos++;
  }

// CARTA INICIAL ------------------------------------------------------------------------------------------
    randNum = rand() % 54;
    end=0, lengSRC=0; j=0;
    memset(srcSimple,'\0', sizeof(srcSimple));

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
          case 'f':/*Carta de +4*/
            cardInit.numero=11;
            break;
          case 'e':/*Carta de reversa*/
            cardInit.numero=12;
            break;
          case 'p':/*Carta para negar turno*/
            cardInit.numero=13;
            break;
          }
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

  const char *IDChar = gtk_widget_get_name(GTK_WIDGET(card));
  char* tempChar;
  strcpy(tempChar, IDChar);
  int ID = atoi(tempChar);
  int indexCartaComida;

  strtok(tempChar, "_");
  strtok(NULL, "_");

  if(strcmp(jugador[turno].baraja[ID].color, cardInit.color)==0 || jugador[turno].baraja[ID].numero==cardInit.numero) {
     printf("\nTiro de carta");
     return 1;
  } else { // Si no es del NUMERO O No es del COLOR
    do {
      printf("\nCOME DEL MAZO");
      int random = rand() % 54;
      char *CartaComida = image_names[random];

      //TODO: Guardar la informacion de la carta a la estructura del JUGADOR
      //TODO: Asiganar el indice de la ultima carta comida
      //      Se obtiene el indice con alguna funcion que funcione como un STRLEN del GBOX

      indexCartaComida = 0;

      // FUNCION: Obtener la informacion de la carta random que comio
      // DATO [CartaComida]: La carta random que comio
    } while (jugador[turno].baraja[indexCartaComida].numero==cardInit.numero || strcmp(jugador[turno].baraja[indexCartaComida].color,cardInit.color)==0);
    return 1;
  }
  return 0;
}

void colocarCarta() {
  g_print("Se coloca la carta");
}

void desabilitarCarta() {
  g_print("Se desabilita la carta");
}

int totalCartasUsuario(int turno){
   int i=0;
    while (jugador[turno].baraja[i].color[0]!='\0') {
          i++;
    }
    return i;
}

void finjuego(){
  g_print("Pasar a Menu de Victoria/Derrota/Puntuaciones\n");
}

// UNICAMENTE INTERFAZ
GtkWidget* interfazJuego(GtkWidget *widget){

  GtkWidget *windowClose = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(widget)));
  if(GTK_IS_WINDOW(windowClose)){
    g_print("Se cierra: %s\n", gtk_window_get_title (GTK_WINDOW (windowClose)));
    gtk_widget_destroy(GTK_WIDGET(windowClose));
  }

  g_print("\t> Inicio del juego - INTERFAZ\n");
  GObject *boxA, *boxAB, *event, *img;
  GtkWidget *window;
  GError *error = NULL;

  builder = gtk_builder_new ();
  if (gtk_builder_add_from_file (builder, "XML/menuplay2jugadores.glade", &error) == 0) {
    g_printerr ("Error loading file: %s\n", error->message);
    g_clear_error (&error);
  }

  window = GTK_WIDGET(gtk_builder_get_object (builder, "menuplay2jugadores"));

  boxA = gtk_builder_get_object(builder, "Arriba");
  boxAB = gtk_builder_get_object(builder, "Abajo");

  /* Lo que debe de enviar de la click de la carta:
   * - El objeto (EventBox)
   * - Que estatus mandar, en todo caso es el 2
  */

  event = gtk_builder_get_object(builder, "Ab1");
  img = gtk_builder_get_object(builder, "IAb1");
  g_signal_connect(event, "button-release-event", G_CALLBACK(startGame), GINT_TO_POINTER(2));
  g_signal_connect (event, "enter_notify_event", G_CALLBACK (motionCard), img);
  g_signal_connect (event, "leave_notify_event", G_CALLBACK (motionCard), img);
  gtk_image_set_from_file(GTK_IMAGE(img), jugador[0].baraja[0].srcCompleto);

  event = gtk_builder_get_object(builder, "Ab2");
  img = gtk_builder_get_object(builder, "IAb2");
  g_signal_connect(event, "button-release-event", G_CALLBACK(startGame), GINT_TO_POINTER(2));
  g_signal_connect (event, "enter_notify_event", G_CALLBACK (motionCard), img);
  g_signal_connect (event, "leave_notify_event", G_CALLBACK (motionCard), img);
  gtk_image_set_from_file(GTK_IMAGE(img), jugador[0].baraja[1].srcCompleto);


  event = gtk_builder_get_object(builder, "Ab3");
  img = gtk_builder_get_object(builder, "IAb3");
  g_signal_connect(event, "button-release-event", G_CALLBACK(startGame), GINT_TO_POINTER(2));
  g_signal_connect (event, "enter_notify_event", G_CALLBACK (motionCard), img);
  g_signal_connect (event, "leave_notify_event", G_CALLBACK (motionCard), img);
  gtk_image_set_from_file(GTK_IMAGE(img), jugador[0].baraja[2].srcCompleto);


  event = gtk_builder_get_object(builder, "Ab4");
  img = gtk_builder_get_object(builder, "IAb4");
  g_signal_connect(event, "button-release-event", G_CALLBACK(startGame), GINT_TO_POINTER(2));
  g_signal_connect (event, "enter_notify_event", G_CALLBACK (motionCard), img);
  g_signal_connect (event, "leave_notify_event", G_CALLBACK (motionCard), img);
  gtk_image_set_from_file(GTK_IMAGE(img), jugador[0].baraja[3].srcCompleto);

  event = gtk_builder_get_object(builder, "Ab5");
  img = gtk_builder_get_object(builder, "IAb5");
  g_signal_connect(event, "button-release-event", G_CALLBACK(startGame), GINT_TO_POINTER(2));
  g_signal_connect (event, "enter_notify_event", G_CALLBACK (motionCard), img);
  g_signal_connect (event, "leave_notify_event", G_CALLBACK (motionCard), img);
  gtk_image_set_from_file(GTK_IMAGE(img), jugador[0].baraja[4].srcCompleto);

  event = gtk_builder_get_object(builder, "Ab6");
  img = gtk_builder_get_object(builder, "IAb6");
  g_signal_connect(event, "button-release-event", G_CALLBACK(startGame), GINT_TO_POINTER(2));
  g_signal_connect (event, "enter_notify_event", G_CALLBACK (motionCard), img);
  g_signal_connect (event, "leave_notify_event", G_CALLBACK (motionCard), img);
  gtk_image_set_from_file(GTK_IMAGE(img), jugador[0].baraja[5].srcCompleto);

  event = gtk_builder_get_object(builder, "Ab7");
  img = gtk_builder_get_object(builder, "IAb7");
  g_signal_connect(event, "button-release-event", G_CALLBACK(startGame), GINT_TO_POINTER(2));
  g_signal_connect (event, "enter_notify_event", G_CALLBACK (motionCard), img);
  g_signal_connect (event, "leave_notify_event", G_CALLBACK (motionCard), img);
  gtk_image_set_from_file(GTK_IMAGE(img), jugador[0].baraja[6].srcCompleto);

  img = gtk_builder_get_object(builder, "IJuego");
  gtk_image_set_from_file(GTK_IMAGE(img), cardInit.srcCompleto);

  event = gtk_builder_get_object(builder, "Comida");
  // TODO: Falta linkear eventos

  gtk_widget_show_all(window);
  return window;
}

/*   Una vez el usuario clickee la carta debe de oculatarse o eliminrse del GBOX   */
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
