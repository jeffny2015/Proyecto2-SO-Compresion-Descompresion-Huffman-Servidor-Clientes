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
    //Hilo de cada cliente para que el server se siga comunicando con el cliente
    //printf("[Cliente]Conectado\n");

    int recibir, escribir, nuevo_socket;
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
    printf("contador_archivo_enviado: %d\n", contador_archivo_enviado );
    char nombre_archivo[7];
    char num[5];

    if(contador_archivo_enviado < 10){
      strcpy(nombre_archivo, "x0");
    }else{
      strcpy(nombre_archivo, "x");
    }

    sprintf(num, "%d", contador_archivo_enviado);
    strcat(nombre_archivo, num);


    printf("%s\n", nombre_archivo);
    printf("%d\n", contador_archivo_enviado);


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

    fprintf(stdout, "[-] Largo del archivo: %ld bytes\n", file_stat.st_size);

    sprintf(file_size, "%ld", file_stat.st_size);
    /*
    len = send(nuevo_socket, nombre_archivo, sizeof(nombre_archivo), 0);
    if (len < 0){
        printf("[-] Error enviando el nombre del archivo");
        exit(1);
    }*/
    strcat(file_size, "|");
    strcat(file_size, nombre_archivo);

    len = send(nuevo_socket, file_size, sizeof(file_size), 0);
    if (len < 0){
        printf("[-] Error enviando info del archivo");
        exit(1);
    }

    sent_bytes = 0;
    offset = 0;
    remain_data = file_stat.st_size;

    bzero(file_size, 256);

    recv(nuevo_socket,file_size,256,0);
    printf("Recv file_size: %s\n",file_size);
    bzero(file_size, 256);
    while (((sent_bytes = sendfile(nuevo_socket, manejar_archivo, &offset, sizeof(remain_data))) > 0) && (remain_data > 0)){
        //fprintf(stdout, "[-]Servidor enviando %d bytes del archivo, posicion en el archivo actual: %ld Cantidad de datos restantes = %d\n", sent_bytes, offset, remain_data);
        remain_data -= sent_bytes;
        //printf("Mierda\n");
        //printf("Snet bytes: %d\n",sent_bytes);
        //fprintf(stdout, "[-]Servidor enviando %d bytes del archivo, posicion en el archivo actual: %ld Cantidad de datos restantes = %d\n", sent_bytes, offset, remain_data);
    }
    printf("HOla Hola\n");



    /********/

    bzero(file_size, 256);

    ssize_t len5;
    //bzero(buff, 28);

    printf("[-] Recibiendo datos\n");

    /* Recivimo size del archivo y el nombre*/

    len5 = recv(nuevo_socket, file_size, 256, MSG_WAITALL);

    if(len5>0){
    printf("recibido\n");
    }

    printf("contenido %s\n", file_size );

    //split para sacar el nombre y el size del archivo

    char *token5 = strtok(file_size,"|");
    tamanio_archivo = atoi(token5);

    printf("size %d\n", tamanio_archivo);

    char *nombreArchivo5 = strtok(NULL, "|");
    printf("nombre %s\n", nombreArchivo5);

    FILE *archivoComprimido5;
    archivoComprimido5 = fopen(nombreArchivo5, "w");
    if (archivoComprimido5 == NULL)
    {
    fprintf(stderr, "Error al abrir el archivo1: %s\n", strerror(errno));
    exit(1);
    }
    int datos_pendientes5 = 0;
    datos_pendientes5 = tamanio_archivo;
    printf("[-] Iniciando Transferencia\n");



    char datos5[datos_pendientes5];
    int tmp5 = 0;
    while (((len5 = recv(nuevo_socket, datos5, datos_pendientes5, 0)) > 0) && (datos_pendientes5 > 0)){
        //printf("ENtre Entrew\n");
        fwrite(datos5, sizeof(char), len5, archivoComprimido5);
        datos_pendientes5 -= len5;
        //printf("Hola\n");

      //  fprintf(stdout, "[-] Se recibieron %ld bytes y se esperaban %d bytes\n", len, datos_pendientes);
        /*if (tmp > len)
        {
            break;
        }
        if (tmp < len)
        {
            tmp = len;
        }*/

      //  printf("len %ld",len);
    }

    fclose(archivoComprimido5);
    /******/
    
    printf("BLA\n");

    //recivimos el archivo binario
/*
    recv(nuevo_socket,file_size,sizeof(file_size),0);
    printf("Recv file_size: %s\n",file_size);
    */

    bzero(file_size, 256);

    ssize_t len2;
    //bzero(buff, 28);

    printf("[-] Recibiendo datos\n");

    /* Recivimo size del archivo y el nombre*/

    len2 = recv(nuevo_socket, file_size, sizeof(file_size), MSG_WAITALL);

    if(len2>0){
    printf("recibido\n");
    }

    printf("contenido %s\n", file_size );

    //split para sacar el nombre y el size del archivo

    char *token = strtok(file_size,"|");
    tamanio_archivo = atoi(token);

    printf("size %d\n", tamanio_archivo);

    char *nombreArchivo = strtok(NULL, "|");
    printf("nombre %s\n", nombreArchivo);

    char *token2 = strtok(NULL,"|");
    tamanio_archivo2 = atoi(token2);

    printf("size %d\n", tamanio_archivo2);

    char *nombreArchivo2 = strtok(NULL, "|");
    printf("nombre %s\n", nombreArchivo2);

    char *token3 = strtok(NULL, "|");
    totalcaracteres = atoi(token3);
    printf("Caracteres %d\n", totalcaracteres);

    printf("[-] Cargando archivo\n");
    FILE *archivoComprimido;
    archivoComprimido = fopen(nombreArchivo, "w");
    if (archivoComprimido == NULL)
    {
    fprintf(stderr, "Error al abrir el archivo1: %s\n", strerror(errno));
    exit(1);
    }
    int datos_pendientes = 0;
    datos_pendientes = tamanio_archivo;
    printf("[-] Iniciando Transferencia\n");



    char datos[datos_pendientes];
    int tmp = 0;
    while (((len = recv(nuevo_socket, datos, datos_pendientes, 0)) > 0) && (datos_pendientes > 0)){
        //printf("ENtre Entrew\n");
        fwrite(datos, sizeof(char), len, archivoComprimido);
        datos_pendientes -= len;
        //printf("Hola\n");

      //  fprintf(stdout, "[-] Se recibieron %ld bytes y se esperaban %d bytes\n", len, datos_pendientes);
        /*if (tmp > len)
        {
            break;
        }
        if (tmp < len)
        {
            tmp = len;
        }*/

      //  printf("len %ld",len);
    }

    fclose(archivoComprimido);
    printf("dasnudnsai %s\n", nombreArchivo2 );
    //bzero(file_size, 256);
    ssize_t len3;
    //bzero(buff, 28);

    printf("[-] Recibiendo datos\n");

    /* Recivimo size del archivo y el nombre*/

    /*
    recv(nuevo_socket,file_size,sizeof(file_size), MSG_WAITALL);
    len3 = recv(nuevo_socket, file_size, sizeof(file_size), 0);

    if(len3>0){
      printf("recibido\n");
    }
    */

    //split para sacar el nombre y el size del archivo
    //sleep(1);
    archivos_por_recibir -= 1;
    printf("espero algo \n");
    while(archivos_por_recibir > 0){
      //printf("espero ? %d\n", archivos_por_recibir);
    };
    printf("todos los archivos de los clientes han sido recividos\n");

    printf("[-] Cargando archivo\n");
    printf("nombre del archivo que quiero abrir %s\n", nombreArchivo2);
    archivoComprimido = fopen(nombreArchivo2, "w");
    if (archivoComprimido == NULL)
    {
    fprintf(stderr, "Error al abrir el archivo2: %s\n", strerror(errno));
    exit(1);
    }
    datos_pendientes = 0;
    datos_pendientes = tamanio_archivo2;
    printf("[-] Iniciando Transferencia\n");



    char datos2[datos_pendientes];
    tmp = 0;
    while (((len = recv(nuevo_socket, datos2, datos_pendientes, 0)) > 0) && (datos_pendientes > 0)){
        //fprintf(stdout, "[-] Se recibieron %ld bytes y se esperaban %d bytes\n", len, datos_pendientes);
        fwrite(datos2, sizeof(char), len, archivoComprimido);
        datos_pendientes -= len;

        //fprintf(stdout, "[-] Se recibieron %ld bytes y se esperaban %d bytes\n", len, datos_pendientes);
        /*if (tmp > len)
        {
            break;
        }
        if (tmp < len)
        {
            tmp = len;
        }*/

      //  printf("len %ld",len);
    }
    bzero(file_size, 256);
    printf("termino\n");
    fclose(archivoComprimido);
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

    iniciarTablaClientes();
    iniciarSocketTCP(ip,puerto,disponibilidad);

    return 0;
}
