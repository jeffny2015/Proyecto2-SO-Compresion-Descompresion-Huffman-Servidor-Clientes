#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h> // open function
#include <unistd.h> // close function
#include "hash.h"

struct stat file_stat;
char *byte = NULL;  /* declare a pointer, and initialize to NULL */
FILE *f;
int cantidad_caracteres;


/* Print byte as bit sequence with MSB first */
char *print_byte_as_bits(int byte_value)
{
    int i;
    byte = malloc (8 * sizeof *byte);

    if (byte_value >= 0 && byte_value < 256)
    {
        for (i = 0; i < 8; i++)
        {
            //printf("%c ", (byte_value & 0x80) ? '1' : '0');
            byte[i] = (byte_value & 0x80) ? '1' : '0';
            byte_value <<= 1;
        }
    }
    else
        printf("[Not a byte: %X]", byte_value);

    return byte;
}

void ReadBinFile(void)
{

    FILE *ptr;
    unsigned char buffer; // note: 1 byte
    char *nombre = "x00.bin";
    int manejar_archivo;

    manejar_archivo = open(nombre, O_RDONLY);

    if (fstat(manejar_archivo, &file_stat) < 0){
      printf("[-] No se pudo optener la info del archivo: %s\n",nombre);
      exit(1);
    }

    fprintf(stdout, "[-] Largo del archivo: %ld bytes\n", file_stat.st_size);

    int fileSize = file_stat.st_size;
    close(manejar_archivo);

    ptr = fopen(nombre,"rb");
    char *binario;
    int tamanio = 7;
    int indice = 0;
    int num;
    binario = (char *) malloc(1);
    while(cantidad_caracteres > 1){
      for (int i = 0; i < fileSize; i++) {
        fread(&buffer, 1, 1, ptr);
        char *byte = print_byte_as_bits(buffer);
        //printf("%s\n", byte);

        //revisar si pertenece a la tabla hash parte del byte
        //o seguir agarrando bytes y concatenar valores

        //printf("valor del binario %s\n", binario);
        for (int j = 0; j <= 7; j++) {
          //sleep(1);
          //printf("SIZE %d\n", tamanio);
          strncat(binario, &byte[indice], 1 );

          //printf("valor acumulado del binario %s\n", binario );
          //usleep(100000);
          item = search(binario);
          if(item != NULL){
            //escribimos la letra en el archivo
            num = atoi(item->data);
            //printf("SE ENCONTRO UNO %s",item->data);
            printf("%c",(char)num);
            tamanio = 7;
            indice += 1;
            free(binario);
            binario = (char *) malloc(1);
            cantidad_caracteres --;
            if(cantidad_caracteres <= 1){
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

        if(cantidad_caracteres <= 1){
          break;
        }
      }
    }
    fclose (ptr);
}

int main (void){

    dummyItem = (struct DataItem*) malloc(sizeof(struct DataItem));
    strcpy(dummyItem->data,"-1");
    strcpy(dummyItem->key,"-1");
    //dummyItem->key = -1;

    FILE *f;
    f = fopen("hx00","r");
    int i = 0;
    int hash_len;
    char buffer[256];
    char tmp[256];
    char *token;

    fgets(buffer, sizeof(buffer), f);
    token = strtok(buffer," ");
    cantidad_caracteres = atoi(token);

    token = strtok(NULL," ");
    strcpy(tmp,token);
    token = strtok(tmp,"\n");
    hash_len = atoi(token);
    hash_len += 1;
    bzero(tmp,256);
    char key[50];

    hashArray =  malloc(hash_len * sizeof(struct DataItem));
    setSize(hash_len);

    while(fgets(buffer, sizeof(buffer), f) != NULL){

      token = strtok(buffer," ");
      strcpy(key,token);
      //key = atoi(token);
      token = strtok(NULL," ");
      strcpy(tmp,token);
      token = strtok(tmp,"\n");
      insert(token, key);
      bzero(tmp,256);
    }

    //display();
    ReadBinFile();

    return 0;
}
