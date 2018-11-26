#ifndef HASH_H
#define HASH_H

struct InfoItem {
   char valor[30];
   char llave[30];
};
struct InfoItem** tablahash;
struct InfoItem* item;

struct InfoItem *buscar(char llave[]);
void insertar(char llave[],char valor[]);
void imprimirHash();
void setlen(int l);

#endif
