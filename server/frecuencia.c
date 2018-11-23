#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "frecuencia.h"

int inicializarLista(){
    lista_caracter = (char *) malloc(sizeof(char));
    lista_apariciones = (int *) malloc(sizeof(int));
    return 0;
}

int agregarElemento(char caracter){
    lista_caracter = realloc(lista_caracter,(len_lista+1)*sizeof(char));
    lista_apariciones = realloc(lista_apariciones,(len_lista+1)*sizeof(int));
    lista_caracter[len_lista] = caracter;
    lista_apariciones[len_lista] = 1;
    len_lista++;
    return 0;
}

int estaEnLista(char caracter){
    for (int i = 0; i < len_lista; i++){
        if (caracter == lista_caracter[i]){
            return i;
        }
    }
    return -1;
}
int aumentarAparicion(int i){
    lista_apariciones[i]++;
    return 0;
}
int imprimirLista(){
    for (int i = 0; i < len_lista; i++){
        printf("Caracter: %c  => Cantidad: %d\n",lista_caracter[i],lista_apariciones[i]);
    }
    return 0;
}