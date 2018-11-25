#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <time.h>
#include <math.h>
#include <pthread.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include "hash.h"
#include "client.h"

char ip_servidor[25];
int *lista_paginas;
ssize_t len;
int cliente_pagina;
pthread_t broadcast_rcv;
int socket_cliente;
int tamanio_archivo;
char buffer[256];
char buffersender[256];
int datos_pendientes = 0;
FILE *archivoAcomprimir;
FILE *binfile;
struct stat file_stat;
struct stat file_stat2;
int contador = 0;
char file_size[256];
int sent_bytes;
long int offset;
int remain_data;
int manejar_archivo;

int bit_actual = 7;
unsigned char bit_buffer;

void escribirBit(int bit){
  //Se recorren los 8 bits comoenzando desde el 
  //que tiene mas valor hasta  el bit 0
  if (bit)
    bit_buffer |= (1<<bit_actual);
  bit_actual--;
  if (bit_actual == -1){
    fwrite (&bit_buffer, 1, 1, binfile);
    bit_actual = 7;
    bit_buffer = 0;
  }
}

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
int iniciarConfig(const char *nombre){
    FILE *archivo;
    archivo = fopen(nombre,"r");

    if (archivo == NULL){
        printf("%s\n","El archivo ingresado no existe");
        return -1;
    }else{
        char line[25];
        fgets(ip_servidor,25, archivo);
    }
    fclose(archivo);
    return 0;
}

void iniciarSocketTCP(char *ip,int puerto){

    //Buffers
    char file_size[256];
    //Para separar los mensajes serializados
    char *token;
    //para recibir el nomebre de  los arhivos
    char *nomArch;
    char nomArchaux[15];
    //para manejar archivos
    FILE *Arch;
    //temporal
    int aux;
    //tmp del nombre del archivo
    char copiaNombre[15];
    //Otras variables
    int c;
    int esta;
    char frecuencia_f[20];

    struct sockaddr_in direccion_servidor;
    struct hostent* servidor;
    totalCaracteres = 0;
    bzero(buffer, 256);
    socket_cliente = socket(AF_INET, SOCK_STREAM, 0);
    printf("[-] Iniciando Socket\n");
    if(socket_cliente < 0){
        printf("[-] Error al iniciar Socket\n");
    }
    printf("[-] Iniciando configuracion\n");
    memset(&direccion_servidor, '\0', sizeof(direccion_servidor));
    direccion_servidor.sin_family = AF_INET;
    direccion_servidor.sin_addr.s_addr = inet_addr(ip);
    direccion_servidor.sin_port = htons(puerto);

    printf("[-] Conectando\n");
    if(connect(socket_cliente, (struct sockaddr*)&direccion_servidor, sizeof(direccion_servidor)) < 0){
        printf("[-] Error de conexion\n");
        exit(1);
    }
    printf("[-] Paso 1: Recibimos\n");
    recv(socket_cliente, buffer, sizeof(buffer), 0);

    token = strtok(buffer,"|");
    tamanio_archivo = atoi(token);
    nomArch = strtok(NULL, "|");
    strcpy(copiaNombre, nomArch);
    strcpy(NombreArchivo,nomArch);

    char msg[10];
    strcpy(msg,"ok");
    send(socket_cliente,msg,sizeof(msg),0);
    bzero(msg,10);


    Arch = fopen(copiaNombre, "w");
    if (Arch == NULL){
        fprintf(stderr, "Error al abrir el archivo: %s\n", strerror(errno));
        exit(1);
    }
    len = 0;
    remain_data = 0;
    remain_data = tamanio_archivo;
    bzero(buffer, 256);
    tamanio_archivo = 0;
    char datos[BUFSIZ];
    aux = remain_data;
    while (remain_data > 0){
        len = recv(socket_cliente, datos, BUFSIZ,0);
        if (len <= 0){break;}
        fwrite(datos, sizeof(char), len, Arch);
        remain_data -= len;
    }
    remain_data = 0;
    len = 0;
    bzero(datos,BUFSIZ);
    aux = 0;
    fclose(Arch);

    printf("[-] Paso 2: Recibimos\n");
    printf("[-] Paso 3: leemos Frecuencias\n");
    len_lista = inicializarLista();
    Arch =fopen(copiaNombre,"r");

    while(1) {
      c = fgetc(Arch);
      totalCaracteres++;
      if(feof(Arch)) {break;}
      esta = estaEnLista((char)c);
      if (esta != -1){aumentarAparicion(esta);
      }else{agregarElemento((char)c);}
    }
    fclose(Arch);
    printf("[-] Paso 4: terminamos  frecuencias\n");
    printf("[-] Paso 5: Las escribimos en  archivo\n");
    strcpy(frecuencia_f,"f");
    strcat(frecuencia_f,copiaNombre);
    Arch = fopen(frecuencia_f,"w");
    for (int j = 0; j < len_lista; j++){
        fprintf(Arch,"%d %d\n",lista_caracter[j],lista_apariciones[j]);
    }
    fclose(Arch);
    printf("[-] Paso 6: Preparamos frecuencias para enviar\n");
    manejar_archivo = 0;
    manejar_archivo = open(frecuencia_f, O_RDONLY);
    if (fstat(manejar_archivo, &file_stat) < 0){
        printf("[-] No se pudo optener la info del archivo: %s\n",copiaNombre);
        exit(1);
    }
    bzero(file_size,256);
    sprintf(file_size, "%ld", file_stat.st_size);
    strcat(file_size, "|");
    strcat(file_size, frecuencia_f);
    printf("[-] Paso 7: Enviamos datos\n");
    len = 0;
    len = send(socket_cliente, file_size, sizeof(file_size), 0);




    sent_bytes = 0;
    offset = 0;
    remain_data = file_stat.st_size;
    printf("[-] Paso 8: Enviamos arhivo con frecuencias\n");
    bzero(file_size, 256);

    recv(socket_cliente,msg,sizeof(msg),0);
    bzero(msg,10);

    while (remain_data > 0){
        sent_bytes = sendfile(socket_cliente, manejar_archivo, &offset, BUFSIZ);
        if (sent_bytes <= 0){break;}
        remain_data -= sent_bytes;
    }
    sent_bytes = 0;
    len = 0;
    remain_data = 0;
    close(manejar_archivo);

    printf("[-] Paso 9: Recibimos datos con los valores totales\n");

    bzero(buffer,256);
    recv(socket_cliente, buffer, sizeof(buffer), 0);
    token = strtok(buffer,"|");
    tamanio_archivo = 0;
    tamanio_archivo = atoi(token);

    printf("[-] Paso 10: Recibimos archivo y lo escribimos\n");
    Arch = fopen("valoresHuffman.txt", "w");
    if (Arch == NULL){
      fprintf(stderr, "Error al abrir el archivo1: %s\n", strerror(errno));
      exit(1);
    }

    remain_data = tamanio_archivo;
    char datos1[BUFSIZ];
    aux = remain_data;



    strcpy(msg,"ok");
    send(socket_cliente,msg,sizeof(msg),0);
    bzero(msg,10);

    while (remain_data > 0){
        len = recv(socket_cliente, datos1,BUFSIZ,0);
        if (len <= 0){break;}
        fwrite(datos1, sizeof(char), len, Arch);
        remain_data -= len;
    }
    bzero(datos1,BUFSIZ);
    aux = 0;
    len = 0;
    remain_data = 0;
    fclose(Arch);

    printf("[-] Paso 11: Preparamos para comprimir\n");
    dummyItem = (struct DataItem*) malloc(sizeof(struct DataItem));
    strcpy(dummyItem->data,"-1");
    dummyItem->key = -1;
    Arch = fopen("valoresHuffman.txt","r");

    int i = 0;
    int hash_len;
    char tmp6[256];
    int cantidad_caracteres;

    fgets(buffer, 256,Arch);
    token = strtok(buffer," ");
    cantidad_caracteres = atoi(token);
    token = strtok(NULL," ");
    strcpy(tmp6,token);
    token = strtok(tmp6,"\n");
    hash_len = atoi(token);
    hash_len += 1;
    bzero(tmp6,256);
    bzero(buffer,256);
    int key;
    hashArray =  malloc(hash_len * sizeof(struct DataItem));
    setSize(hash_len);

    while(fgets(buffer, 256, Arch) != NULL){
      token = strtok(buffer," ");
      key = atoi(token);
      token = strtok(NULL," ");
      strcpy(tmp6,token);
      token = strtok(tmp6,"\n");
      insert(key, token);
      bzero(tmp6,256);
    }

    bzero(buffer,256);
    bzero(tmp6,256);
    fclose(Arch);
    printf("[-] Paso 12: Vamos comprimiendo\n");
    Arch = fopen(copiaNombre,"r");
    char cpNombre[15];
    char nombreHuffman[20];
    strcpy(nombreHuffman, "h");
    strcat(nombreHuffman, copiaNombre);
    strcpy(cpNombre,copiaNombre);
    strcat(cpNombre,".txt");
    binfile = fopen(cpNombre, "w");
    while(1) {
      c = fgetc(Arch);

      if(feof(Arch) ) {break;}

      item = search(c);
      if(item != NULL) {
        for (int i = 0; i < strlen(item->data); i++) {
          if (item->data[i] == '1'){escribirBit(1);
          }else{escribirBit(0);}
          contador++;
          if (contador == 8){contador = 0;}
        }
      }
    }

    int restantes = 8 - contador;
    if (restantes > 0){
        for (int i = 0; i < restantes; i++)
        {
            escribirBit(0);
        }
    }
    fclose(Arch);
    fclose(binfile);
    printf("[-] Paso 13: Preparamos para enviar\n");
    manejar_archivo = open(cpNombre, O_RDONLY);

    if (manejar_archivo <= 0){
        printf("[-] No se pudo leer el archivo: %s\n",cpNombre);
        exit(1);
    }

    if (fstat(manejar_archivo, &file_stat) < 0){
        printf("[-] No se pudo optener la info del archivo: %s\n",cpNombre);
        exit(1);
    }
    bzero(file_size,256);
    sprintf(file_size, "%ld", file_stat.st_size);
    strcat(file_size, "|");
    strcat(file_size, cpNombre);
    strcat(file_size, "|");
    char cant[21];
    sprintf(cant,"%d",totalCaracteres);
    strcat(file_size,cant);

    len = send(socket_cliente, file_size, sizeof(file_size), 0);
    if (len <= 0){
      printf("[-] Error enviando info del archivo");
      exit(1);
    }

    sent_bytes = 0;
    offset = 0;
    remain_data = file_stat.st_size;

    bzero(file_size, 256);

    recv(socket_cliente,msg,sizeof(msg),0);
    bzero(msg,10);

    while (remain_data > 0){
        sent_bytes = sendfile(socket_cliente, manejar_archivo, &offset, BUFSIZ);
        if (sent_bytes <= 0){break;}
        remain_data -= sent_bytes;
    }
}

int main(int argc, char const *argv[]){

    const char *nombre = argv[1];

    //netstat -tupln
    //sudo kill -9 PID
    char *ip;
    int puerto = 4444;
    system("rm -f valoresHuffman");

    printf("[-] Abriendo: %s\n",nombre);
    iniciarConfig(nombre);

    ip = ip_servidor;
    iniciarSocketTCP(ip,puerto);
    return 0;
}
