#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hash.h"

int size;

void setSize(int s){
   size = s;
}
int hashCode(char key[]) {
   int tmp = atoi(key) % size;
   return tmp;
}

struct DataItem *search(char key[]) {
   //get the hash
   int hashIndex = hashCode(key);

   //move in array until an empty
   while(hashArray[hashIndex] != NULL) {
     //printf("eqwewqe %d\n", (size-1) );
      if(strcmp(hashArray[hashIndex]->key ,key) == 0){
         return hashArray[hashIndex];
      }

      //go to next cell
      ++hashIndex;

      //wrap around the table
      hashIndex %= size;
      //printf("hash i %d\n", hashIndex );

      if(hashIndex == (size)){
        break;
      }
      //printf("hola\n" );
   }
   //printf("DSADSADSA\n");
   return NULL;
}

void insert(char key[],char data[]) {
   struct DataItem *item = (struct DataItem*) malloc(sizeof(struct DataItem));
   strcpy(item->data,data);
   //item->key = key;
   strcpy(item->key,key);
   //get the hash
   int hashIndex = hashCode(key);
   //move in array until an empty or deleted cell
   while(hashArray[hashIndex] != NULL && atoi(hashArray[hashIndex]->key) != -1) {
      //go to next cell
      ++hashIndex;
      //wrap around the table
      hashIndex %= size;

   }
   hashArray[hashIndex] = item;
}

void display() {
   int i = 0;

   for(i = 0; i<size; i++) {

      if(hashArray[i] != NULL)
         printf(" (%s,%s)\n",hashArray[i]->key,hashArray[i]->data);
      else
        break;
   }

   printf("\n");
}
