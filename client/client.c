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
#include "huffman.h"
#include "binario.h"
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
struct stat file_stat;
int contador = 0;
char file_size[256];
int sent_bytes;
long int offset;
int remain_data;


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
    int escribir;
    struct sockaddr_in direccion_servidor;
    struct hostent* servidor;
    totalCaracteres = 0;

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

    //while(1){
    bzero(buffer, sizeof(buffer));
    //bzero(buff, 28);

    printf("[-] Recibiendo datos\n");

    /* Recivimo size del archivo y el nombre*/
    recv(socket_cliente, buffer, sizeof(buffer), 0);
    printf("contenido %s\n", buffer );

    //split para sacar el nombre y el size del archivo

    char *token = strtok(buffer,"|");
    tamanio_archivo = atoi(token);

    printf("size %d\n", tamanio_archivo);

    char *nombreArchivo = strtok(NULL, "|");
    char copiaNombre[15];
    strcpy(copiaNombre, nombreArchivo);
    strcpy(NombreArchivo,nombreArchivo);
    printf("nombre %s\n", nombreArchivo);

    printf("[-] Cargando archivo\n");
    archivoAcomprimir = fopen(nombreArchivo, "w");
    if (archivoAcomprimir == NULL)
    {
            fprintf(stderr, "Error al abrir el archivo: %s\n", strerror(errno));
            exit(1);
    }

    datos_pendientes = tamanio_archivo;
    printf("[-] Iniciando Transferencia\n");
    bzero(buffer, sizeof(buffer));

    strcpy(buffersender,"recibiendo recibiendo");

    send(socket_cliente,buffersender,sizeof(buffersender),0);
    bzero(buffersender, sizeof(buffersender));
    bzero(buffer, sizeof(buffer));    
    printf("QUe pasa pasa\n");
    char datos[datos_pendientes];
    int tmp = 0;
    while (((len = recv(socket_cliente, datos, sizeof(datos_pendientes), 0)) > 0) && (datos_pendientes > 0)){
        printf("ENtre Entrew\n");
        fwrite(datos, sizeof(char), len, archivoAcomprimir);
        datos_pendientes -= len;
        
        fprintf(stdout, "[-] Se recibieron %ld bytes y se esperaban %d bytes\n", len, datos_pendientes);
        if (tmp > len)
        {
            break;
        }
        if (tmp < len)
        {
            tmp = len;
        }
        
        printf("len %ld",len);
    }
    fclose(archivoAcomprimir);
    

    bzero(buffer,sizeof(buffer));
    len_lista = inicializarLista();
    //int f;
    FILE *f;
    f =fopen(copiaNombre,"r");
    //f = open(, O_RDONLY);
    int c;
    int esta;
    while(1) {
      c = fgetc(f);
      totalCaracteres++;
      if( feof(f) ) {
         break;
      }
      esta = estaEnLista((char)c);
      if (esta != -1){
        aumentarAparicion(esta);
      }else{
        agregarElemento((char)c);
      }
      //printf("%c", c);

    }
    fclose(f);

    imprimirLista();
    HuffmanCodes(lista_caracter, lista_apariciones, len_lista);

    f = fopen(copiaNombre,"r");
    char cpNombre[15];
    strcpy(cpNombre,copiaNombre);
    strcat(cpNombre,".bin");
    binfile = fopen(cpNombre, "w");
    while(1) {
      c = fgetc(f);
      
      if(feof(f) ) { 
         break;
      }
      item = search(c);
      if(item != NULL) {
        for (int i = 0; i < strlen(item->data); i++) {
            if (item->data[i] == '1')
            {
                WriteBit(1);
            }else{
                WriteBit(0);
            }
            contador++;

            if (contador == 8){
                contador = 0;
            }
        }
      }
    }

    int restantes = 8 - contador;
    if (restantes > 0){
        for (int i = 0; i < restantes; i++)
        {
            WriteBit(0);
        }
    }
    fclose(f);
    fclose(binfile);



    bzero(buffersender,sizeof(buffersender));
    strcpy(buffersender,"Johan Johan");
    printf("Enviando\n");
    send(socket_cliente,buffersender,256,0);
    printf("Se envio\n");




    bzero(datos, sizeof(datos));

    
    int manejar_archivo;
    manejar_archivo = open(cpNombre, O_RDONLY);

    if (manejar_archivo < 0){
    printf("[-] No se pudo leer el archivo: %s\n",cpNombre);
    exit(1);
    }

    if (fstat(manejar_archivo, &file_stat) < 0){
    printf("[-] No se pudo optener la info del archivo: %s\n",cpNombre);
    exit(1);
    }

    fprintf(stdout, "[-] Largo del archivo: %ld bytes\n", file_stat.st_size);

    sprintf(file_size, "%ld", file_stat.st_size);

    strcat(file_size, "|");
    strcat(file_size, cpNombre);
    printf("info del archivo %s\n", file_size);

    len = send(socket_cliente, file_size, sizeof(file_size), 0);
    if (len < 0){
    printf("[-] Error enviando info del archivo");
    exit(1);
    }


    printf("primer enviando\n" );

    sent_bytes = 0;
    offset = 0;
    remain_data = file_stat.st_size;
    //CODIGO AGREGADO


    bzero(file_size, 256);
    while (((sent_bytes = sendfile(socket_cliente, manejar_archivo, &offset, sizeof(remain_data))) > 0) && (remain_data > 0)){
        //fprintf(stdout, "[-]Servidor enviando %d bytes del archivo, posicion en el archivo actual: %ld Cantidad de datos restantes = %d\n", sent_bytes, offset, remain_data);
        remain_data -= sent_bytes;
        printf("Mierda\n");
        printf("Snet bytes: %d\n",sent_bytes);
        //fprintf(stdout, "[-]Servidor enviando %d bytes del archivo, posicion en el archivo actual: %ld Cantidad de datos restantes = %d\n", sent_bytes, offset, remain_data);
    }
}

int main(int argc, char const *argv[]){

    const char *nombre = argv[1];

    //netstat -tupln
    //sudo kill -9 PID
    char *ip;
    int puerto = 4444;

    printf("[-] Abriendo: %s\n",nombre);
    iniciarConfig(nombre);

    ip = ip_servidor;
    iniciarSocketTCP(ip,puerto);
    return 0;
}


/*
int main(){

    len_lista = inicializarLista();
    //int f;
    FILE *f =fopen("archivo.txt","r");
    //f = open(, O_RDONLY);
    int c;
    int esta;
    while(1) {
      c = fgetc(f);
      if( feof(f) ) { 
         break;
      }
      esta = estaEnLista((char)c);
      if (esta != -1){
        aumentarAparicion(esta);
      }else{
        agregarElemento((char)c);
      }
      printf("%c", c);
    }
    fclose(f);  
        
    imprimirLista();

    HuffmanCodes(lista_caracter, lista_apariciones, len_lista);


    f = fopen("archivo.txt","r");
    binfile = fopen("prueba.bin", "w");
    while(1) {
      c = fgetc(f);
      if(feof(f) ) { 
         break;
      }
      item = search(c);
      if(item != NULL) {
        for (int i = 0; i < strlen(item->data); i++) {
            if (item->data[i] == '1')
            {
                WriteBit(1);
            }else{
                WriteBit(0);
            }
            contador++;

            if (contador == 8){
                contador = 0;
            }
        }
      } else {
       printf("Element not found\n");
      }
    }

    int restantes = 8 - contador;
    if (restantes > 0){
        for (int i = 0; i < restantes; i++)
        {
            WriteBit(0);
        }
    }
    fclose(f);
    fclose(binfile);
    return 0;
}*/