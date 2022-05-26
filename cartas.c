#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//ESTRUCTURA SECUNDARIA
typedef struct
{
    char nom[20];
    char ID[20];
    int player; //jugador

}Perfil;

typedef struct
{
    char color[15];
    int num;
}Cartas;

//ESTRUCTURA PRINCIPAL
typedef struct
{
    Cartas card;
    int tcard; //total de cartas
    Perfil perfil;//usuario
}GameData;

//FUNCION 
void reparte(Perfil,int[7]);


int main()
{
    srand(time(NULL));
    int menu,mode,cards[7];
    Perfil perfil; //jugador
    system("cls");
    printf("\nWELCOME");
    do
    {
        printf("\n");
        printf("\n1.INICIO\n2.Objetivo de juego\n3.COMO JUGAR\n4.SALIR");
        printf("\nOpcion :");
        scanf("%d",&menu);
        system("cls");
        switch (menu)
        {
        case 1:
            printf("\t GAME MODE");
            printf("\t\n Jugadores");
            do
             {
              printf("\nIngrese el numero de jugadores:");
              scanf("%d",&perfil.player);
              if(perfil.player>4)
              {
                  printf("No se admite ese numero de jugadores,ingrese opcion nueva, 'De 2 a 4 JUGADORES' \n");
              }
            }while(perfil.player>4);

              for (int i = 0; i <perfil.player; i++)
                {
                    printf("\n\tPLAYER %d\nUSER NAME:",i+1);
                    fflush(stdin);
                    gets(perfil.nom);
                    /*printf("ID:");
                    fflush(stdin);
                    gets(perfil.ID);*/
                }
                reparte(perfil,cards); 
             //system("cls");
            break;
            system("cls");
         case 2:
             printf("\t\t\t\t\t\t\t Objetivo de juego \n");
             printf("El principal objetivo de este juego llamado UNO es deshacerse de todas las cartas que se reciben inicialmente \nal deshacerse de la mayotia de cartas obtendremos una victoria \n");
             break;
             system("cls");
        case 3:
            printf("\t\t\t\t\t Bienvenido al pequena introduccion del juego: \n");
            printf("1:Cada jugador recibira 7 cartas \n2:Cada carta tiene un valor y un significado diferente, cada significado de dichas cartas se encontrara en la opcion 4 \n");
            printf("3:Se tendran cartas de diferentes colores ya sean:\n\nAZULES\nVERDES\nROJAS \nAMARILLAS y comodines \n\n");
            printf("4:Los comodines son aquellos que se podran usar a favor del jugador ya que existen: \nCartas de acumulacion-estas cartas tienen un +2 en la parte superior derecha, esta carta al tirarla con un color en espesifico le agregara 2 cartas al siguiente jugador dentro de un mazo\nCarta con +4: \nAl tirar esta carta se le agregaran 4 nuevas cartas al siguiente jugador en su mazo,esta carta se podra tirar en cualquier momento ya que tiene todos los colores \n");
            printf("5:Las cartas de cualquier color que tengan un simbolo inverso cambiaran el sentido de juego \nAhora bien, si se tienen cartas con un simbolo de bloqueo le quitara al siguiente jugador su turno y este tendra que esperar hasta su siguiente turno \n");
            break;
        system("cls");
        case 4:
            printf("\nADIOS\n");
            break;
        default:
            printf("\nOPCION NO VALIDA");
            break;
        }
    } while (menu!=4);


    system("pause");
}

void reparte(Perfil perfil,int cards[7])
{
    int i=0;
    while (i<perfil.player)
    {
      for (int i = 0; i <7; i++)
      {
        cards[i]=rand()%15;
        printf(" \t[%d] ",cards[i]);
      }
      i++;     
    }
    //checar que numeros asociarle a los comodines, bloqueo y reversa
    //ver que colores se le asociaran 
}
 
  /*mientras que sea el turno de ese jugador validar
    si su carta es igual en numero o color a la que se encuentra, si es diferente no dejarlo que tire
    validar si hay un +2,+4,comodin,bloqueo o reversa
    validar si alguien no dice "UNO"
    validar que no quieran poner un +2 o +4 para evitar comer 
    validar que siempre haya cartas en el mazo 
    validar que si un jugador ya no tiene cartas el juego se acaba y es el ganador */
