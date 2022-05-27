#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*typedef struct
{
  char color[15];
  int numero; // 0-9 : Numeors, 10-14 : Acciones
  int src;

}cartas;*/

int main()
{
    int menu;
    char color1[15],color2[15];
    do
    {
        int menu,num1,num2;
        printf("\n");
        printf("1.Comparacion de numeros en cartas \n");
        printf("2.Comparacion de colores en cartas \n");
        printf("3.SALIR \n");
        printf("OPCION: \n");
        scanf("%d",&menu);
        switch(menu)
        {
        case 1:
            printf("se recibiran dos numeros, ingreselos para probar: \n");
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

            /*printf("\nCOLORES: ");
            if(x.color==COLORES)
            {
              printf("\nSiguiente jugador");
            }
            else
            {
                printf("\nTOMA UNA CARTA DEL MAZO"); //solo es posible agarrar una carta del mazo
            }*/
            break;
        case 2:
            printf("\t \tCOLORES DE CARTAS VERDE,AZUL,AMARILLO Y ROJO \n");
            printf("CODIGO DE COLOR: ROJO: #ec6464,Verde: #64bb8e,Azul: #2dbbde,Amarillo: #f7e359 \n");
            printf("ingrese codigo de color: \n");
            fflush(stdin);
            gets(color1);
            printf("Ingrese nuevamente el codigo: \n");
            fflush(stdin);
            gets(color2);
            if(strcmp(color2,color1)==0)
            {

                printf("Los colores son iguales, puede seguir \n");
            }
            else
            {
                printf("Los colores no son iguales por lo que debe intentar con otro \n");

            }

            break;
        case 3:
            printf("SALIR \n");
            break;
            default: printf("\nNO VALIDO");
            break;
        }
    }while(menu!=3);
    return 0;
}