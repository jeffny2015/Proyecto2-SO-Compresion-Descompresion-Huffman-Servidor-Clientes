#ifndef FRECUENCIA_H
#define FRECUENCIA_H

char *lista_caracter;
int *lista_apariciones;
int len_lista;

int inicializarLista();
int agregarElemento(char caracter, int cantidad);
int estaEnLista(char caracter);
int aumentarAparicion(int i, int cantidad);
int imprimirLista();

#endif
