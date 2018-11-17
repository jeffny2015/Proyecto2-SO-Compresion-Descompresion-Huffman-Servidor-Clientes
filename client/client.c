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

char ip_servidor[25];
int *lista_paginas;
ssize_t len;
int cliente_pagina;
pthread_t broadcast_rcv;
int socket_cliente;
int tamanio_archivo;
char buffer[BUFSIZ];
int datos_pendientes = 0;
FILE *archivoAcomprimir;

char *lista_caracter;
int *lista_apariciones;
int len_lista;
//char buff[28];


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
    bzero(buffer, BUFSIZ);
    //bzero(buff, 28);

    printf("[-] Recibiendo datos\n");

    /* Recivimo size del archivo y el nombre*/
    recv(socket_cliente, buffer, BUFSIZ, 0);
    printf("contenido %s\n", buffer );

    //split para sacar el nombre y el size del archivo

    char *token = strtok(buffer,"|");
    tamanio_archivo = atoi(token);

    printf("size %d\n", tamanio_archivo);

    char *nombreArchivo = strtok(NULL, "|");
    char copiaNombre[15];
    strcpy(copiaNombre, nombreArchivo);

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
    while (((len = recv(socket_cliente, buffer, BUFSIZ, 0)) > 0) && (datos_pendientes > 0)){
        fwrite(buffer, sizeof(char), len, archivoAcomprimir);
        datos_pendientes -= len;
        fprintf(stdout, "[-] Se recibieron %ld bytes y se esperaban %d bytes\n", len, datos_pendientes);
    }
    fclose(archivoAcomprimir);

    len_lista = inicializarLista();
    //int f;
    FILE *f;
    f =fopen(copiaNombre,"r");
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
      //printf("%c", c);

    }
    fclose(f);

    imprimirLista();
        //strcpy(buffer, "hola mundo");
        //No es dueño de la página y no tiene la página
        //escribir = send(socket_cliente, buffer, strlen(buffer)+1,0);

        //bzero(buffer, 256);
        //bzero(buff, 28);

        //strcpy (buffer,"L|");
        //sprintf(buff, "%d", cliente_pagina);
        //strcat(buffer,buff);

    //}
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
