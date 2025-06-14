#ifndef ORDENADOR_GENERICO_H
#define ORDENADOR_GENERICO_H

#include "ListaProyecto.h" //incluye la definicion de la lista generica

// Funcion auxiliar para intercambiar dos elementos de una lista
template <typename T>
void intercambiar(Lista<T>& lista, int i, int j) {
    T temp = lista.getDato(i);  //guarda temporalmente el elemento de la posicion i
    lista.reemplazar(i, lista.getDato(j)); // coloca en la posición i el valor de la posición j
    lista.reemplazar(j, temp); // coloca en la posición j el valor original de i
}

// Función recursiva de QuickSort generico con comparador externo
// Permite ordenar cualquier tipo de lista siempre que se provea una función de comparacion
template <typename T>
void quickSortGenerico(Lista<T>& lista, int izquierda, int derecha, bool (*comparador)(T, T)) {
    int i = izquierda;
    int j = derecha;
    T pivote = lista.getDato((izquierda + derecha) / 2); // se elige el pivote como el valor central

    // Partición: se mueve i hacia la derecha y j hacia la izquierda, ordenando según el comparador
    while (i <= j) {
        while (comparador(lista.getDato(i), pivote)) i++; // avanza i si cumple con el orden
        while (comparador(pivote, lista.getDato(j))) j--; // retrocede j si cumple con el orden

        if (i <= j) {
            intercambiar(lista, i, j); // intercambia los elementos desordenados
            i++;
            j--;
        }
    }

        // Llamadas recursivas sobre las sublistas izquierda y derecha
    if (izquierda < j)
        quickSortGenerico(lista, izquierda, j, comparador);
    if (i < derecha)
        quickSortGenerico(lista, i, derecha, comparador);
}

// Funcion publica para ordenar usando un comparador externo
template <typename T>
void ordenarListaGenerica(Lista<T>& lista, bool (*comparador)(T, T)) {
     // Solo ordena si la lista no está vacía y tiene más de un elemento
    if (!lista.esVacia() && lista.getTamanio() > 1) {
        quickSortGenerico(lista, 0, lista.getTamanio() - 1, comparador);
    }
}

#endif
