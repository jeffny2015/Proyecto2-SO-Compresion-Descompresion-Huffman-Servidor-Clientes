#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hash.h"

int len;

void setlen(int l){
   len = l;
}
int indice(int llave) {
   int tmp = llave % len;
   return tmp;
}
struct InfoItem *buscar(int llave) {
   // Buscamos el valor en latablahash
   int i = indice(llave);   
   while(tablahash[i] != NULL) {
      //si las llaves son iguales 
      if(tablahash[i]->llave == llave){
         return tablahash[i]; 
      }
      i++;
      i %= len;
   }        
   return NULL;        
}

void insertar(int llave,char valor[]) {
   //inicializamos y agregamos la llave y el valor
   struct InfoItem *item = (struct InfoItem*) malloc(sizeof(struct InfoItem));
   item->llave = llave;
   strcpy(item->valor,valor);  
   //le asignamos una pos en el hash
   int i = indice(llave);
   while(tablahash[i] != NULL && tablahash[i]->llave != -1) {
      i++;
      i %= len;
   }
   tablahash[i] = item;
}

void imprimirHash() {
   int i = 0;
   for(i = 0; i < len; i++) {
      if(tablahash[i] != NULL)
         printf("(%d,%s)\n",tablahash[i]->llave,tablahash[i]->valor);
   }
   printf("\n");
}