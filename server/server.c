#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <sys/sendfile.h>

pthread_t interrupt,detener_servidor;
socklen_t len_dir;
int detener;
int nuevo_socket_broadcast;
int socket_server;
char *ipHilo;
int puertoHilo;
int len_tabla_clientes = 0;
char *archivo;
int manejar_archivo;

int enviar;
int sent_bytes;
long int offset;
int remain_data;
struct stat file_stat;
ssize_t len;

struct InfoAdd{
    char *ip;
    int puerto;
    int socket;
} *TablaClientes;

void iniciarTablaClientes(){
    TablaClientes = (struct InfoAdd *) malloc(sizeof(struct InfoAdd));
    len_tabla_clientes = 0;
}
void agregarCliente(char *ip,int puerto, int socket){
    len_tabla_clientes++;
    TablaClientes = (struct InfoAdd *)realloc(TablaClientes,len_tabla_clientes*sizeof(struct InfoAdd));
    TablaClientes[len_tabla_clientes-1].ip = ip;
    TablaClientes[len_tabla_clientes-1].puerto = puerto;
    TablaClientes[len_tabla_clientes-1].socket = socket;
}

void removerCliente(int puerto){
    int indice = -1;
    for (int i = 0; i < len_tabla_clientes; i++) {
        if(TablaClientes[i].puerto == puerto){
          len_tabla_clientes --;
          indice = i;
          printf("%d\n",len_tabla_clientes );
          break;
        }
    }
    //si entro al if significa que se borro un cliente que no esta al final de la lista
    if((len_tabla_clientes > 1 || indice == 0) && (indice != len_tabla_clientes)){
        for (int j = indice; j < len_tabla_clientes; j++) {
            TablaClientes[j].ip = TablaClientes[j+1].ip;
            TablaClientes[j].puerto = TablaClientes[j+1].puerto;
            TablaClientes[j].socket = TablaClientes[j+1].socket;
        }
    }
    //reducimos en 1 el tamanio de la lista
    TablaClientes = (struct InfoAdd *) realloc(TablaClientes,len_tabla_clientes*sizeof(struct InfoAdd));
}

void imprimirClientes(){
    printf("Pagina\t\tVersion\n");
    for (size_t i = 0; i  < len_tabla_clientes; i++) {

    }

}
void *escuchandoServidor(){

    char enter[10];
    while(1){
        scanf("%s",enter);

        detener = 1;
        close(socket_server);
        exit(1);
    }



}

void *conexionClientes(void *param){
    //Hilo de cada cliente para que el server se siga comunicando con el cliente
    //printf("[Cliente]Conectado\n");
    int recibir, escribir,nuevo_socket;
    nuevo_socket = *((int*)param);
    char* ip = ipHilo;
    int puerto = puertoHilo;
    char file_size[256];
    //while(1){
    while(1){
        if (enviar){
            break;
        }
    }

    sprintf(file_size, "%ld", file_stat.st_size);

    len = send(nuevo_socket, file_size, sizeof(file_size), 0);
    if (len < 0){
        printf("[-] Error enviando info del archivo");
        exit(1);
    }



    sent_bytes = 0;
    offset = 0;
    remain_data = file_stat.st_size;
    while (((sent_bytes = sendfile(nuevo_socket, manejar_archivo, &offset, BUFSIZ)) > 0) && (remain_data > 0)){
        //fprintf(stdout, "[-]Servidor enviando %d bytes del archivo, posicion en el archivo actual: %ld Cantidad de datos restantes = %d\n", sent_bytes, offset, remain_data);
        remain_data -= sent_bytes;
        //fprintf(stdout, "[-]Servidor enviando %d bytes del archivo, posicion en el archivo actual: %ld Cantidad de datos restantes = %d\n", sent_bytes, offset, remain_data);
    }

        /*
       if(recv(nuevo_socket, buffer, 256, 0) < 0){
            printf("Error al recivir paquetes.\n");
       }else{

            char *token = strtok(buffer,"|");
            l_e = strcmp(token, "L");
            // L => para leer  En => para escribir
            if (!l_e){

                token =  strtok(NULL,"|");
                indice_pagina_pedido = index_paginas[atoi(token)];
                bzero(buffer, 256);
                sprintf(buffer, "%d", indice_pagina_pedido);
                send(nuevo_socket, buffer, strlen(buffer)+1,0);
            }else{

                if (!(strcmp(token, "E1"))){
                    //Quiere escribir, es dueño de la página y tiene la página
                    //al ser dueño sabe quién tiene copia de las páginas, pide que las borren y la actualiza
                    //iPagina|UpdatePagina
                    token =  strtok(NULL,"|");
                    ipagina = atoi(token);
                    token =  strtok(NULL,"|");
                    updatePagina = atoi(token);
                    index_paginas[ipagina] = updatePagina;
                    broadcastUpdate(ipagina,updatePagina,ip,puerto);
            }
       }*/

    bzero(file_size, 256);
       //imprimirClientes();
    //}
    close(nuevo_socket);
}

void iniciarSocketTCP(char *ip,int puerto,int disponibilidad){
    //Configuracionde scoket he inicio para escuchar por los clientes
    int nuevo_socket;
    int hilo;
    int one = 1;
    char quiere[10];
    enviar = 0;
    struct sockaddr_in direccion_servidor, direccion_cliente, direccion_cliente_broadcast;

    printf("[-] Iniciando Socket\n");

    socket_server = socket(AF_INET, SOCK_STREAM, 0);

    if(socket_server < 0){
        printf("[-] Error en el socket\n");
        exit(1);
    }

    printf("[-] Iniciando configuracion\n");
    memset(&direccion_servidor, '\0', sizeof(direccion_servidor));

    direccion_servidor.sin_family = AF_INET;
    direccion_servidor.sin_addr.s_addr = inet_addr(ip);
    direccion_servidor.sin_port = htons(puerto);

    setsockopt(socket_server, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    if(bind(socket_server, (struct sockaddr*) &direccion_servidor, sizeof(direccion_servidor)) < 0){
        printf("[-] Error en el enlance\n");
        exit(1);
    }
    if(listen(socket_server, disponibilidad) < 0){
        printf("[-] Error en el listen\n");
        exit(1);
    }

    printf("[-] Abriendo: %s\n",archivo);
    manejar_archivo = open(archivo,O_RDONLY);

    if (manejar_archivo < 0){
        printf("[-] No se pudo leer el archivo: %s\n",archivo);
        exit(1);
    }
    /* Get file stats */
    if (fstat(manejar_archivo, &file_stat) < 0){
        printf("[-] No se pudo optener la info del archivo: %s\n",archivo);
        exit(1);
    }

    fprintf(stdout, "[-] Largo del archivo: %ld bytes\n", file_stat.st_size);


    //detener = 0;
    //pthread_create(&detener_servidor,NULL,escuchandoServidor,NULL);

    printf("[-] Escuchando ...\n");
    while(1){

        /*if (detener){
            printf("[-] Adios\n");
            break;
        }*/
        nuevo_socket = accept(socket_server, (struct sockaddr*)&direccion_cliente, &len_dir);
        printf("[-] Conexion aceptada de la direccion %s:%d\n", inet_ntoa(direccion_cliente.sin_addr), ntohs(direccion_cliente.sin_port));
        ipHilo = inet_ntoa(direccion_cliente.sin_addr);
        puertoHilo = ntohs(direccion_cliente.sin_port);
        if(nuevo_socket < 0){
            printf("[-] Error en el socket con el cliente\n");
            exit(1);
        }
        agregarCliente(inet_ntoa(direccion_cliente.sin_addr), ntohs(direccion_cliente.sin_port), nuevo_socket);
        printf("[-] Cantidad de clientes: %d\n",len_tabla_clientes);
        printf("[-] Quiere comprimir el archivo? y/n: ");
        scanf("%s",quiere);
        if (strcmp(quiere,"y") == 0){
            enviar = 1;
            for (int i = 0; i < len_tabla_clientes; i++) {
                printf("[-] Conectando ..\n");
                hilo = pthread_create(&interrupt, NULL, conexionClientes, (void*)&(TablaClientes[i].socket));
                if(hilo){
                    printf("[-] Error con el Server al crear el hilo%d\n", hilo);
                }
            }
        }
        bzero(quiere, 10);

    }
    close(socket_server);
}

int main(int argc, char *argv[]){


    //COnfiguracion  para el socket
    char *ip = argv[1];
    int puerto = 4444;
    int disponibilidad = 20;


    archivo = argv[2];

    iniciarTablaClientes();
    iniciarSocketTCP(ip,puerto,disponibilidad);

    return 0;
}
