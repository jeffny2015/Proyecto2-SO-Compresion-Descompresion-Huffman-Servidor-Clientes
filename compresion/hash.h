#ifndef HASH_H
#define HASH_H

struct DataItem {
   char data[30];
   char key[30];
};
struct DataItem** hashArray;
struct DataItem* dummyItem;
struct DataItem* item;

struct DataItem *search(char key[]);
void insert(char key[],char data[]);
void display();
void setSize(int s);

#endif
