#ifndef HASH_H 
#define HASH_H

struct DataItem {
   char data[100];   
   int key;
};
struct DataItem* hashArray[5000]; 
struct DataItem* dummyItem;
struct DataItem* item;

struct DataItem *search(int key);
void insert(int key,char data[]);
void display();
void setSize(int s);

#endif