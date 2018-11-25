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
#include "frecuencia.h"
#include "huffman.h"

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
int contador_archivo_enviado = 0;
int totalcaracteres;
int enviar;
int sent_bytes;
long int offset;
int remain_data;
struct stat file_stat;
ssize_t len;
int tamanio_archivo;
int tamanio_archivo2;
int archivos_por_recibir;

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
    //Socket
    int nuevo_socket = *((int*)param);
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

    int copianumerocliente;
    //Variables 

    int c;
    int esta;
    char buf[1024];
    int cant;
    char letra;


    //Esperamos al server a que quiera enviar
    while(1){
        if (enviar){
            break;
        }
    }

    //Aqui creamos los archivos con las partes separadas del mensaje para cada cliente
    char nombre_archivo[7];
    char num[5];

    if(contador_archivo_enviado < 10){
      strcpy(nombre_archivo, "x0");
    }else{
      strcpy(nombre_archivo, "x");
    }
    copianumerocliente = contador_archivo_enviado;
    sprintf(num, "%d", contador_archivo_enviado);
    strcat(nombre_archivo, num);

    //Abrimos el archivo
    printf("[-] Abriendo: %s\n",nombre_archivo);
    manejar_archivo = open(nombre_archivo, O_RDONLY);

    if (manejar_archivo < 0){
        printf("[-] No se pudo leer el archivo: %s\n",nombre_archivo);
        exit(1);
    }

    if (fstat(manejar_archivo, &file_stat) < 0){
        printf("[-] No se pudo optener la info del archivo: %s\n",nombre_archivo);
        exit(1);
    }


    //Enviamos el archivo


    //Enviamos los datos del archivo
    fprintf(stdout, "[-] Largo del archivo: %ld bytes\n", file_stat.st_size);

    sprintf(file_size, "%ld", file_stat.st_size);

    strcat(file_size, "|");
    strcat(file_size, nombre_archivo);

    len = send(nuevo_socket, file_size, sizeof(file_size), 0);
    if (len <= 0){
        printf("[-] Error enviando info del archivo");
        exit(1);
    }
    len = 0;

    bzero(file_size, 256);


    /*char buftmp[20];
    recv(nuevo_socket,buftmp,strlen(buftmp),0);
    printf("%s\n",buftmp);
    bzero(buftmp,20);*/

    sent_bytes = 0;
    offset = 0;
    remain_data = file_stat.st_size;

    while (remain_data > 0){
        sent_bytes = sendfile(nuevo_socket, manejar_archivo, &offset, remain_data);
        if (sent_bytes <= 0){break;}
        remain_data -= sent_bytes;
    }
    close(manejar_archivo);
    remain_data = 0;
    printf("[-]Paso 1: Enviar archivo al cliente\n");

    //Recibimos las frecuencias

    //Recibimos los datos para leer el archivo con las frecuencaias
    len = recv(nuevo_socket, file_size, sizeof(file_size),0);//MSG_WAITALL);

    token = strtok(file_size,"|");
    tamanio_archivo = atoi(token);
    nomArch = strtok(NULL, "|");
    strcpy(nomArchaux,nomArch);
    //Abrimos para poder escribir
    Arch = fopen(nomArch, "w");
    if (Arch == NULL){
        fprintf(stderr, "Error al abrir el archivo1: %s\n", strerror(errno));
        exit(1);
    }
    remain_data = tamanio_archivo;
    printf("[-] Iniciando Transferencia\n");



    char datos[remain_data];
    aux = remain_data;
    while (remain_data > 0){
        len = recv(nuevo_socket, datos, remain_data, 0);
        if (len <= 0){break;}
        fwrite(datos, sizeof(char), len, Arch);//Size of  ?
        remain_data -= len;
    }

    remain_data = 0;
    len = 0;
    bzero(file_size,256);
    bzero(datos,aux);
    aux = 0;
    fclose(Arch);
    printf("[-]Paso 2: Escribimos el archivo de frecuencias que envio el cliente\n");
    nomArch = nomArchaux;
    Arch =fopen(nomArch,"r");
    while (fgets(buf, sizeof(buf), Arch) != NULL){
      buf[strlen(buf) - 1] = '\0';
      char *caracter = strtok(buf," ");
      letra = atoi(caracter);
      char *cantidad = strtok(NULL, " ");
      cant = atoi(cantidad);
      esta = estaEnLista((char)letra);
      if (esta != -1){
        aumentarAparicion(esta, cant);
      }else{
        agregarElemento((char)letra, cant);
      }
    }
    bzero(buf,1024);
    fclose(Arch);
    printf("[-]Paso 3: Frecuencias listas\n");

    archivos_por_recibir -= 1;
    
    printf("[-]Paso 4: Esperando a los demas clientes para recibir las demas frecuencias\n");
    while(archivos_por_recibir > 0){};

    printf("[-]Paso 5: Sumar frecuencias totales \n");
    if (!copianumerocliente){
        HuffmanCodes(lista_caracter, lista_apariciones, len_lista);
        archivos_por_recibir = -1;
    }
    while(archivos_por_recibir > -1){}


    printf("[-]Paso 6: Eenviarselas al cliente \n");

    manejar_archivo = 0;
    manejar_archivo = open("valoresHuffman", O_RDONLY);

    if (fstat(manejar_archivo, &file_stat) < 0){
        printf("[-] No se pudo optener la info del archivo: valoresHuffman\n");
        exit(1);
    }
    sprintf(file_size, "%ld", file_stat.st_size);
    strcat(file_size, "|");
    len = send(nuevo_socket, file_size, sizeof(file_size), 0);
    if (len <= 0){
        printf("[-] Error enviando info del archivo");
        exit(1);
    }
    bzero(file_size, 256);
    sent_bytes = 0;
    offset = 0;
    len = 0;
    remain_data = file_stat.st_size;

    while (remain_data > 0){
        sent_bytes = sendfile(nuevo_socket, manejar_archivo, &offset, remain_data);
        if (sent_bytes <= 0){break;}
        remain_data -= sent_bytes;
    }
    remain_data = 0;
    close(manejar_archivo);
    printf("[-]Paso 7: Envio Completado \n");
    bzero(nomArchaux,sizeof(nomArchaux));
    printf("[-]Paso 8: Recivimos el archivo comprimido \n");
    //Recibimos los datos para escribir
    len = recv(nuevo_socket, file_size, sizeof(file_size), 0);//MSG_OOB);
    tamanio_archivo = 0;
    token = strtok(file_size,"|");
    tamanio_archivo = atoi(token);
    nomArch = strtok(NULL, "|");
    strcpy(nomArchaux,nomArch);
    totalcaracteres = atoi(strtok(NULL, "|"));

    printf("totalcaracteres %d\n", totalcaracteres);
    printf("[-]Paso 9: Recibimos los datos ahora a escribirlos \n");

    Arch = fopen(nomArch, "w");
    if (Arch == NULL){
        fprintf(stderr, "Error al abrir el archivo1: %s\n", strerror(errno));
        exit(1);
    }
    remain_data = tamanio_archivo;
    char datos1[remain_data];
    aux = 0;
    aux = remain_data;
    while (remain_data > 0){
        len = recv(nuevo_socket, datos1,remain_data, 0);
        if (len <= 0){break;}
        fwrite(datos1, sizeof(char), len, Arch);
        remain_data -= len;
    }
    bzero(file_size,256);
    bzero(datos1,aux);
    len = 0;
    remain_data = 0;
    tamanio_archivo = 0;
    fclose(Arch);
    printf("[-]Paso 10: Escribimos los datos \n");
    close(nuevo_socket);


}



void iniciarSocketTCP(char *ip,int puerto,int disponibilidad){
    //Configuracionde scoket he inicio para escuchar por los clientes
    int nuevo_socket;
    int hilo;
    int one = 1;
    char quiere[10];
    long int bytesCliente;
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
    manejar_archivo = open(archivo, O_RDONLY);

    if (manejar_archivo < 0){
        printf("[-] No se pudo leer el archivo: %s\n",archivo);
        exit(1);
    }
    // Obtenemos la informacion del archivo
    if (fstat(manejar_archivo, &file_stat) < 0){
        printf("[-] No se pudo obtener la info del archivo: %s\n",archivo);
        exit(1);
    }

    fprintf(stdout, "[-] Largo del archivo: %ld bytes\n", file_stat.st_size);

    len_lista = inicializarLista();
    //detener = 0;
    //pthread_create(&detener_servidor,NULL,escuchandoServidor,NULL);

    printf("[-] Escuchando ...\n");
    while(1){

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

            printf("[-] Tamanio del archivo %ld dividido entre %d clientes\n", file_stat.st_size, len_tabla_clientes);

            archivos_por_recibir = len_tabla_clientes;
            bytesCliente = file_stat.st_size / len_tabla_clientes;

            //distribuimos correctamente los archivos
            //si la cantidad de clientes es impar, la cantidad de bytes tambien y lo mismo si es par, para que no quede 1 byte suelto
            if(bytesCliente % 2 == 1 && len_tabla_clientes % 2 == 0){
              bytesCliente += 1;
            }else if(bytesCliente % 2 == 0 && len_tabla_clientes % 2 == 1){
              bytesCliente += 1;
            }

            printf("[-] Cada cliente debera comprimir %ld bytes\n", bytesCliente);

            //separamos el archivo en n partes

            char comandoSystem[100];
            char num[5];

            strcpy(comandoSystem, "split -b ");

            sprintf(num, "%ld", bytesCliente);

            strcat(comandoSystem, num);

            strcat(comandoSystem, " -d ");
            strcat(comandoSystem, archivo);


            printf("%s\n", comandoSystem);

            // ejemplo de un posible comandoSystem split -b 500 -d archivo.txt, genera x00, x01, x02, x0n archivos

            system(comandoSystem);


            enviar = 1;
            for (int i = 0; i < len_tabla_clientes; i++) {
                printf("[-] Conectando ..\n");
                hilo = pthread_create(&interrupt, NULL, conexionClientes, (void*)&(TablaClientes[i].socket));
                printf("dsono%d\n", contador_archivo_enviado );
                if(hilo){
                    printf("[-] Error con el Server al crear el hilo%d\n", hilo);
                }
                usleep(100000);
                contador_archivo_enviado += 1;
            }
            contador_archivo_enviado = 0;
        }
        bzero(quiere, 10);
    }
    close(socket_server);
}

int main(int argc, char *argv[]){

    //Configuracion  para el socket
    char *ip = argv[1];
    int puerto = 4444;
    int disponibilidad = 20;

    archivo = argv[2];
    system("rm -f valoresHuffman");
    iniciarTablaClientes();
    iniciarSocketTCP(ip,puerto,disponibilidad);

    return 0;
}
