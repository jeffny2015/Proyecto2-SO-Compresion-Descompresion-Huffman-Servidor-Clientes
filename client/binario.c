#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "binario.h"

int current_bit = 7;
unsigned char bit_buffer;

void WriteBit (int bit){
  if (bit)
    bit_buffer |= (1<<current_bit);
  current_bit--;
  if (current_bit == -1){
    printf("%u\n",bit_buffer);
    fwrite (&bit_buffer, 1, 1, binfile);
    current_bit = 7;
    bit_buffer = 0;
  }
}
