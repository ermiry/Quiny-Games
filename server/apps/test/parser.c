#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct queryString{
    char key[20];
    char value[20];
}typedef queryString;

#define maxURL 50
#define maxLista 5

int main()
{
    char URL[maxURL];
    queryString lista[maxLista];
    bool keyW=false, valueW=false, QueryN=false;
    int i=0,j=0, posicion;

    strcpy( lista[0].key, "");
    strcpy( lista[0].value, "");

    printf("Ingresa el URL\n");
    scanf("%s",URL);
    printf("URL %s",URL);

    do{

        if ( ( URL[i] == '/') || ( URL[i] == '&') ){
            keyW = true;
            valueW = false;
            if (QueryN == true) {
                j++;
                QueryN = false;
                strcpy( lista[j].key, "");
                strcpy( lista[j].value, "");
            }
            i++;
        }

        if ( URL[i] == '?') {
            strcpy( lista[j].key, "");
            i++;
        }

        if ( URL[i] == '=') {
            keyW = false;
            valueW = true;
            QueryN = true;
            i++;
        }

        if ( URL[i] != '\0') {

            if ( keyW == true){
                //strcat( lista[j].key, URL[i] );
                posicion = strlen(lista[j].key);
                lista[j].key[posicion] = URL[i]; //caracter a agregar
                lista[j].key[posicion + 1 ] = '\0';
            }
            if ( valueW == true){
                //strcat( lista[j].value, URL[i] );
                posicion = strlen(lista[j].value);
                lista[j].value[posicion] = URL[i]; //caracter a agregar
                lista[j].value[posicion + 1 ] = '\0';
            }

            printf("\nj:%d\nURL[%d]: %c\nkeyW: %d  valueW: %d\nkey: %s\nvalue: %s\n", j, i, URL[i], keyW, valueW, lista[j].key, lista[j].value);

            i++;
        }

    }while ( URL[i] != '\0');

    printf("\nListas:\n");
    int jp = 0;
    while( jp < j){
        printf("\nlista: %d\nkey: %s\nvalue: %s\n", jp, lista[jp].key, lista[jp].value);
        jp++;
    }
//system("pause");

    return 0;
}


/*   Vestigios del Caos

    while(URL[i]!=NULL){
    printf("\nkey: %s\nvalue: %s", lista[0].key, lista[0].value);
*/