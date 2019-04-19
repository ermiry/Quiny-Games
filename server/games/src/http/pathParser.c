#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "http/pathParser.h"

#include "collections/dllist.h"

static KeyValuePair *key_value_pair_new (void) {

    KeyValuePair *kvp = (KeyValuePair *) malloc (sizeof (KeyValuePair));
    if (kvp) kvp->key = kvp->value = NULL;
    return kvp;

}

static void key_value_pair_delete (void *ptr) {

    if (ptr) {
        KeyValuePair *kvp = (KeyValuePair *) ptr;
        if (kvp->key) free (kvp->key);
        if (kvp->value) free (kvp->value);

        free (kvp);
    }

}

// compares the key of the key value pair
// compare must return -1 if one < two, must return 0 if they are equal, and must return 1 if one > two
static int key_value_pair_compare (void *one, void *two) {



}

DoubleList *parse_path_into_pairs (char *path) {

    DoubleList *pairs = NULL;

    if (path) {
        pairs = dlist_init (key_value_pair_delete, key_value_pair_compare);

    }

    return pairs;

}

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
    scanf("%s",&URL);
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