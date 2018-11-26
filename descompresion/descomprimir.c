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

/* Print byte as bit sequence with MSB first */
char *print_byte_as_bits(int byte_value){
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
        printf("[Not a byte: %X]", byte_value);

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

    dummyItem = (struct DataItem*) malloc(sizeof(struct DataItem));
    strcpy(dummyItem->data,"-1");
    strcpy(dummyItem->key,"-1");
    //dummyItem->key = -1;

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

    fgets(bufferLinea, sizeof(bufferLinea), f);
    token = strtok(bufferLinea," ");
    cantidad_caracteres = atoi(token);

    token = strtok(NULL," ");
    strcpy(tmp,token);
    hash_len = atoi(token);
    hash_len += 1;
    bzero(tmp,256);

    token = strtok(NULL," ");
    strcpy(tmp,token);
    token = strtok(tmp,"\n");
    cantidad_clientes = atoi(token);
    bzero(tmp,256);

    char key[50];

    hashArray =  malloc(hash_len * sizeof(struct DataItem));
    setSize(hash_len);

    int indice = 0;

    while(indice < (hash_len-1)){
      fgets(bufferLinea, sizeof(bufferLinea), f);
      token = strtok(bufferLinea," ");
      strcpy(key,token);
      //key = atoi(token);
      token = strtok(NULL," ");
      strcpy(tmp,token);
      token = strtok(tmp,"\n");
      insert(token, key);
      bzero(tmp,256);
      indice++;
    }

    //display();

    unsigned char buffer;
    int manejar_archivo;
    int bytesCliente;
    int charsPorCliente;
    char casoEspecial = 0;

    FILE *fp;
    fp = fopen(nombreDescomprimido, "wb");


    int copiaCantidad = cantidad_clientes;
    if(cantidad_clientes > 1){
      bytesCliente = cantidad_caracteres / cantidad_clientes;
      printf("bytes %d\n", bytesCliente );
      //distribuimos correctamente los archivos
      //si la cantidad de clientes es impar, la cantidad de bytes tambien y lo mismo si es par, para que no quede 1 byte suelto
      if(bytesCliente % 2 == 1 && cantidad_clientes % 2 == 0){
        bytesCliente += 1;
        casoEspecial = 1;
        printf("caso especial 1\n");
      }else if(bytesCliente % 2 == 0 && cantidad_clientes % 2 == 1){
        bytesCliente += 1;
        casoEspecial = 1;
        printf("caso especial 2\n");
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

    //cantidad_caracteres = charsPorCliente;
    char x[cantidad_caracteres];

    binario = (char *) malloc(1);
    while(cantidad_caracteres > 0){
      printf("\n\n\n\n ME MUEVO AL SIGUIENTE CLIENTE\n\n\n\n");
      if(cantidad_clientes == 0){
        break;
      }
      /*indice = 0;
      char *binario;
      binario = (char *) malloc(1);
      tamanio = 7;*/
      while (charsPorCliente > indiceChar) {
        fread(&buffer, 1, 1, f);
        char *byte = print_byte_as_bits(buffer);
        //printf("BYTE %s\n", byte);

        //revisar si pertenece a la tabla hash parte del byte
        //o seguir agarrando bytes y concatenar valores

        printf("valor del binario %s\n", binario);
        for (int j = 0; j <= 7; j++) {
          strncat(binario, &byte[indice], 1 );

          printf("valor acumulado del binario %s\n", binario );
          //usleep(10000);
          item = search(binario);
          if(item != NULL){
            //escribimos la letra en el archivo
            num = atoi(item->data);
            printf("Se encontro uno %s  %c \n",item->data, (char)num);
            if(casoEspecial && cantidad_clientes == 1 && charsPorCliente == indiceChar){
              printf("entro aqui?\n" );
              break;
              //x[indiceChar] = num;
            }
            x[indiceChar] = num;
            indiceChar++;
            tamanio = 7;
            indice += 1;
            free(binario);
            binario = (char *) malloc(1);
            cantidad_caracteres --;
            if(indiceChar == charsPorCliente){
              //free(binario);
              break;
            }
          }else{
            indice += 1;
            tamanio += 1;
            binario= (char *) realloc(binario, tamanio);
          }

        }
        tamanio +=1;
        indice = 0;
        if(!casoEspecial && cantidad_clientes != copiaCantidad && (indiceChar+1) > charsPorCliente){
          //printf("%s\n", x);
          printf("entro en esta condicion\n" );
          indiceChar += 1;
          x[indiceChar] = ' ';
          break;
        }
        if(indiceChar == charsPorCliente){
          //free(binario);
          break;
        }
      }
      cantidad_clientes --;
      charsPorCliente += charsPorCliente;
    }

    fwrite(x, sizeof(x[0]), sizeof(x)/sizeof(x[0]), fp);

    fclose(fp);
    fclose (f);
    printf("Descompresion finalizada\n");
    return 0;
}
