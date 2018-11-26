#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "hash.h"

struct stat file_stat;
char *byte = NULL;
FILE *f;
int cantidad_caracteres;
int cantidad_clientes;

//extraemos los bit que conforman los bytes del archivo
char *extraerBits(int byte_value){
    int i;
    byte = malloc (8 * sizeof *byte);

    if (byte_value >= 0 && byte_value < 256){
        for (i = 0; i < 8; i++){
            //printf("%c ", (byte_value & 0x80) ? '1' : '0');
            byte[i] = (byte_value & 0x80) ? '1' : '0';
            byte_value <<= 1;
        }
    }
    else
          printf("Esto no es un byte: %X", byte_value);

    return byte;
}

int main (int argc, char **argv){

    char *nombreComprimido = argv[1];
    char *nombreDescomprimido = argv[2];
    printf("%s %s\n", nombreComprimido, nombreDescomprimido );
    if(argc < 1){
      printf("ingrese como parametro el nombre del archivo a comprimir y el de descomprimido\n");
      exit(1);
    }

    FILE *f;
    f = fopen(nombreComprimido,"r");
    if(f == NULL){
      printf("No existe el archivo\n");
      exit(1);
    }

    int i = 0;
    int hash_len;
    char bufferLinea[256];
    char tmp[256];
    char *token;

    //Se lee la primera linea del archivo comprimido
    //para extraer la cantidad de caracteres a descomprimir,
    //la cantidad de caracteres que se encontraron en huffman
    //y la cantidad de clientes que lo comprimieron
    fgets(bufferLinea, sizeof(bufferLinea), f);
    //caracteres a descomprimir
    token = strtok(bufferLinea," ");
    cantidad_caracteres = atoi(token);
    //cantidad de caracteres en huffman
    token = strtok(NULL," ");
    strcpy(tmp,token);
    hash_len = atoi(token);
    hash_len += 1;
    bzero(tmp,256);
    //cantidad de clientes que comprimieron el archivo
    token = strtok(NULL," ");
    strcpy(tmp,token);
    token = strtok(tmp,"\n");
    cantidad_clientes = atoi(token);
    bzero(tmp,256);

    char key[50];
    //se guardan todos los valores de huffman en un hash
    tablahash =  malloc(hash_len * sizeof(struct InfoItem));
    setlen(hash_len);
    int indice = 0;
    while(indice < (hash_len-1)){
      fgets(bufferLinea, sizeof(bufferLinea), f);
      token = strtok(bufferLinea," ");
      strcpy(key,token);
      token = strtok(NULL," ");
      strcpy(tmp,token);
      token = strtok(tmp,"\n");
      insertar(token, key);
      bzero(tmp,256);
      indice++;
    }

    //imprimirHash();

    unsigned char buffer;
    int manejar_archivo;
    int bytesCliente;
    int charsPorCliente;
    char casoEspecial = 0;

    FILE *fp;
    fp = fopen(nombreDescomprimido, "wb");

    //Se realiza la descompresion
    int copiaCantidad = cantidad_clientes;
    if(cantidad_clientes > 1){
      bytesCliente = cantidad_caracteres / cantidad_clientes;
      //distribuimos correctamente los archivos
      //si la cantidad de clientes es impar, la cantidad de bytes tambien y lo mismo si es par, para que no quede 1 byte suelto
      if(bytesCliente % 2 == 1 && cantidad_clientes % 2 == 0){
        bytesCliente += 1;
        casoEspecial = 1;
      }else if(bytesCliente % 2 == 0 && cantidad_clientes % 2 == 1){
        bytesCliente += 1;
        casoEspecial = 1;
      }
      charsPorCliente = bytesCliente;
    }else{
      charsPorCliente = cantidad_caracteres;
    }

    printf("chars por cada cliente %d\n", charsPorCliente );

    char *binario;
    int tamanio = 7;
    indice = 0;
    int indiceChar = 0;
    int num;

    char x[cantidad_caracteres];
    int zz = cantidad_caracteres;

    binario = (char *) malloc(1);

    while(cantidad_caracteres--){

      if(cantidad_clientes == 0){
        break;
      }

      while (charsPorCliente > indiceChar) {
        //leemos 1 byte
        fread(&buffer, 1, 1, f);
        //extraemos los bits que conforman el archivo
        char *byte = extraerBits(buffer);
        //printf("BYTE %s\n", byte);

        //revisar si pertenece a la tabla hash parte del byte
        //o seguir agarrando bytes y concatenar valores
        indice = 0;
        //printf("valor del binario %s\n", binario);
        for (int j = 0; j <= 7; j++) {
          //printf("Indice %d\n",indice );
          strncat(binario, &byte[indice], 1 );

          //printf("valor acumulado del binario %s\n", binario );
          item = buscar(binario);
          //en caso de que se encuentra algun valor correspondiente a la
          //combinacion de unos y ceros
          if(item != NULL){
            //escribimos la letra en el archivo
            num = atoi(item->valor);
            //printf("Se encontro uno %s  %c \n",item->valor, (char)num);
            //almacenamos el valor encontrado para posteriormente escribirlo
            x[indiceChar] = num;
            indiceChar++;
            tamanio = 7;
            indice += 1;
            //reiniciamos estos valores para ingresa nuevamente unos y ceros
            free(binario);
            binario = (char *) malloc(1);
            //nos detenemos si ya se recorrieron los bytes de un cliente
            if(indiceChar == charsPorCliente){
              break;
            }
          }else{
            //en caso de que no se encuentre un valor con los valores binarios
            //se aumenta el tamanio para agregarla otro valor y seguir probando
            indice += 1;
            tamanio += 1;
            binario= (char *) realloc(binario, tamanio);
          }
        }
        tamanio +=1;
        indice = 0;
        //en caso de que sobre un byte al final del utimo archivo
        if(!casoEspecial && cantidad_clientes != copiaCantidad && (indiceChar+1) > charsPorCliente){
          indiceChar += 1;
          x[indiceChar] = ' ';
          break;
        }
        //nos detenemos si ya se recorrieron los bytes de un cliente
        if(indiceChar == charsPorCliente){
          break;
        }
      }
      cantidad_clientes --;
      charsPorCliente += charsPorCliente;
    }

    //realizamos la escritura de los datos decomprimidos
    //fwrite(x, sizeof(x[0]), sizeof(x)/sizeof(x[0]), fp);
    for (i = 0; i < zz-1; i++){
      putc(x[i],fp);

    }

    fclose(fp);
    fclose (f);
    printf("Descompresion finalizada\n");
    return 0;
}
