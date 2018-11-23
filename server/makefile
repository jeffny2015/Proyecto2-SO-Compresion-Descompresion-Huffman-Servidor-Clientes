#Bloque de variables

BANDERAS = -g -lm --std=gnu99
SALIDA = ejecutable

#SALIDA es el nombre final del producto compilado

#regla que se ejecuta por defecto

all:
	gcc $(BANDERAS) -pthread server.c frecuencia.c -o $(SALIDA)

#regla especial para limpiar

.PHONY clean:
	rm *.o $(SALIDA)
