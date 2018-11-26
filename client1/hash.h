#ifndef HASH_H
#define HASH_H

struct InfoItem {
   char valor[100];
   int llave;
};
struct InfoItem** tablahash;
struct InfoItem* item;

struct InfoItem *buscar(int llave);
void insertar(int llave,char valor[]);
void setlen(int l);

#endif
