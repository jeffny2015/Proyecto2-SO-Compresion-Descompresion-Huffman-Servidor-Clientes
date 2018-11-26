#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hash.h"

int len;

void setlen(int l){
   len = l;
}
int indice(char llave[]) {
   int tmp = atoi(llave) % len;
   return tmp;
}

struct InfoItem *buscar(char llave[]) {
   // Buscamos el valor en latablahash
   int i = indice(llave);
   while(tablahash[i] != NULL) {
      //si las llaves son iguales
      if(strcmp(tablahash[i]->llave ,llave) == 0){
         return tablahash[i];
      }
      ++i;
      i %= len;
      if(i == (len)){
        break;
      }
   }
   return NULL;
}

void insertar(char llave[],char valor[]) {
   //inicializamos y agregamos la llave y el valor
   struct InfoItem *item = (struct InfoItem*) malloc(sizeof(struct InfoItem));
   strcpy(item->valor,valor);
   strcpy(item->llave,llave);
   //le asignamos una pos en el hash
   int i = indice(llave);
   while(tablahash[i] != NULL && atoi(tablahash[i]->llave) != -1) {
      ++i;
      i %= len;
   }
   tablahash[i] = item;
}

void imprimirHash() {
   int i = 0;

   for(i = 0; i<len; i++) {
      if(tablahash[i] != NULL)
         printf(" (%s,%s)\n",tablahash[i]->llave,tablahash[i]->valor);
      else
        break;
   }
   printf("\n");
}
