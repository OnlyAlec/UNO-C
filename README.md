# Proyecto Final
## Juego Desarrollado : "UNO"

A medida de que se avance en el proyecto, iremos documentando a este mismo.
Por ejemplo, podemos documentar que hace una funcion, los archivos en el directorio, librerias que ocupamos, etc.

---
# Graficos
Por el momento estamos ocupando **"The GTK Project"**,
El Fichero `HelloWorld.c` es un ejemplo simple para ver la compilacion de esta libreria.

# Valores Standard
## Medidas de la ventana:
    - 720p (1280x720)

## Paletas de colores (HEX):
El orden es el mismo de la lista siguiente.
    - Rojo    : #ec6464
    - Verde   : #64bb8e
    - Azul    : #2dbbde
    - Amarillo: #


# Flujo de Ventanas
![Flujo de Ventanas](etc/README/FlujoVentanas.png "Flujo de Ventanas")

# Acciones
## Menus de Prejuego
1. Al presionar el boton "Jugar", cerrar la ventana principal.
2. Abrir "SelectMode", al presionar "Modo Local", cerrar ventana "SelectMode"
3. Abrir "SelectPlayer", guardar que cuantos jugadores y definir el estilo de juego, cerrar ventana "SelectPlayer"
4. Abrir "ProfileBuild", guardar la imagen y nombres de los jugadores en la struct, cerrar ventana y repetir las veces de los # de jugadores
5.  Abrir "GAMESTART"
