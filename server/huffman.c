#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"


FILE *f;

struct NodoArbol {
	char caracter;
	unsigned frecuencia;
	struct NodoArbol *izquierda, *derecha;
};

struct Arbol {
	unsigned len;
	unsigned capacidad;
	struct NodoArbol** listaNodos;
};

struct NodoArbol* nuevoNodo(char caracter, unsigned frecuencia){
	//Agregamos un nodo nuevo
	struct NodoArbol* temp = (struct NodoArbol*)malloc(sizeof(struct NodoArbol));
	temp->izquierda = NULL;
	temp->derecha = NULL;
	temp->caracter = caracter;
	temp->frecuencia = frecuencia;
	return temp;
}


struct Arbol* crearArbol(unsigned capacidad){
	//Inicializamos el listaNodos de nodos
	struct Arbol* Arbol = (struct Arbol*)malloc(sizeof(struct Arbol));
	Arbol->len = 0;
	Arbol->capacidad = capacidad;
	Arbol->listaNodos = (struct NodoArbol**)malloc(Arbol->capacidad * sizeof(struct NodoArbol*));
	return Arbol;
}

void swapNodoArbol(struct NodoArbol** a, struct NodoArbol** b){
	//swap entre dos nodos
	struct NodoArbol* t = *a;
	*a = *b;
	*b = t;
}

void ampliarArbol(struct Arbol* Arbol, int i){
	//Buscamos la frecuencia mas pequenia y la juntamos
	int menor = i;
	int izquierda = 2 * i + 1;
	int derecha = 2 * i + 2;

	if (izquierda < Arbol->len && Arbol->listaNodos[izquierda]->frecuencia < Arbol->listaNodos[menor]->frecuencia)
		menor = izquierda;

	if (derecha < Arbol->len && Arbol->listaNodos[derecha]->frecuencia < Arbol->listaNodos[menor]->frecuencia)
		menor = derecha;

	if (menor != i) {
		swapNodoArbol(&Arbol->listaNodos[menor],&Arbol->listaNodos[i]);
		ampliarArbol(Arbol, menor);
	}
}


struct NodoArbol* buscarMinimo(struct Arbol* Arbol){
	//Sacamos minimo
	struct NodoArbol* temp = Arbol->listaNodos[0];
	Arbol->listaNodos[0] = Arbol->listaNodos[Arbol->len - 1];
	Arbol->len--;
	ampliarArbol(Arbol, 0);
	return temp;
}

void Unir(struct Arbol* Arbol, struct NodoArbol* NodoArbol){
	//Unimos el nodo al arbol
	Arbol->len++;
	int i = Arbol->len - 1;
	while (i && NodoArbol->frecuencia < Arbol->listaNodos[(i - 1) / 2]->frecuencia) {
		Arbol->listaNodos[i] = Arbol->listaNodos[(i - 1) / 2];
		i = (i - 1) / 2;
	}
	Arbol->listaNodos[i] = NodoArbol;
}

void inicializarArbol(struct Arbol* Arbol){
	//Inicializamos todo los nodos del arbol y empezamos a unirlos
	int n = Arbol->len - 1;
	int i;
	for (i = (n - 1) / 2; i >= 0; --i)
		ampliarArbol(Arbol, i);
}


void EscribirBinario(int arr[], int n,char c){
	//escribir en el archivo el caracter y el binario
	char bin[n];
	int i;
	for (i = 0; i < n; ++i)
		 bin[i] = arr[i] + '0';
	bin[n] = '\0';
	fprintf(f,"%d %s\n",c,bin);
}

struct Arbol* ingresarCaracList(char caracteres[], int frecuencia[], int len){
	//creamos el arbol con la capacidad igual a numero de caacteres en la lista
	struct Arbol* Arbol = crearArbol(len);
	for (int i = 0; i < len; ++i)
		Arbol->listaNodos[i] = nuevoNodo(caracteres[i], frecuencia[i]);
	Arbol->len = len;
	inicializarArbol(Arbol);
	return Arbol;
}

struct NodoArbol* constrArbolHuffman(char caracteres[], int frecuencia[], int len){
	struct NodoArbol *izquierda, *derecha, *top;
	// Asiganmos el tamanio del arbol
	struct Arbol* Arbol = ingresarCaracList(caracteres, frecuencia, len);
	// recorremos hasta que solo quede un nodo para hacerla raiz
	while (!(Arbol->len == 1)) {
		// Buscamos las 2 frecuencias menores
		izquierda = buscarMinimo(Arbol);
		derecha = buscarMinimo(Arbol);
		/*Se crea un nuevo nodo con el valor de la suma de la frecuencia
		de los 2 nodos anteriores y se le adjuntan como nodo izquierdo y derecho*/
		top = nuevoNodo(' ', izquierda->frecuencia + derecha->frecuencia);
		top->izquierda = izquierda;
		top->derecha = derecha;
		Unir(Arbol, top);
	}
	// el nodo restante se define como la raiz
	return buscarMinimo(Arbol);
}

void recorrerArbol(struct NodoArbol* raiz, int lista[], int i){

	// Asignamos 0 a todos los nodos de la izquierda
	if (raiz->izquierda) {
		lista[i] = 0;
		recorrerArbol(raiz->izquierda, lista, i + 1);
	}
	// Asignamos 1 a todos os nodos de la derecha
	if (raiz->derecha) {
		lista[i] = 1;
		recorrerArbol(raiz->derecha, lista, i + 1);
	}

	// Si es raiz
	if (!(raiz->izquierda) && !(raiz->derecha)) {
		EscribirBinario(lista, i,raiz->caracter);
	}
}




void Huffman(char caracteres[], int frecuencia[], int len, long int total, int clientes){
	char archivo[20] = "valoresHuffman.txt";
	// Se construye el arbolito
	struct NodoArbol* raiz = constrArbolHuffman(caracteres, frecuencia, len);
	int lista[len], i = 0;
	f = fopen(archivo,"w");
	//escribir largo de la lista de frecuencias
   	fprintf(f,"%ld %d %d\n",total, len, clientes);
	
	recorrerArbol(raiz, lista, i);

	fclose(f);
}
