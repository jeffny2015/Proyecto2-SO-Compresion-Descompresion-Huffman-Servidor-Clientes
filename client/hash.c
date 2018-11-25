#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hash.h"

int size;

void setSize(int s){
   size = s;
}
int hashCode(int key) {
   int tmp = key % size;
   return tmp;
}

struct DataItem *search(int key) {
   //get the hash 
   int hashIndex = hashCode(key);  
   
   //move in array until an empty 
   while(hashArray[hashIndex] != NULL) {
   
      if(hashArray[hashIndex]->key == key)
         return hashArray[hashIndex]; 
         
      //go to next cell
      ++hashIndex;
      
      //wrap around the table
      hashIndex %= size;
   }        
   
   return NULL;        
}

void insert(int key,char data[]) {
   struct DataItem *item = (struct DataItem*) malloc(sizeof(struct DataItem));
   strcpy(item->data,data);  
   item->key = key;
   //get the hash 
   int hashIndex = hashCode(key);
   //move in array until an empty or deleted cell
   while(hashArray[hashIndex] != NULL && hashArray[hashIndex]->key != -1) {
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
         printf(" (%d,%s)\n",hashArray[i]->key,hashArray[i]->data);
      else
         printf(" ~~ \n");
   }
   
   printf("\n");
}