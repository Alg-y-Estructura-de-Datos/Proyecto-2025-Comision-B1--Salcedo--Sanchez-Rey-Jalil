#ifndef LISTA_H
#define LISTA_H

#include <iostream>
using namespace std;

template <class T>
class Nodo {
private:
    T dato; // Dato que almacena el nodo
    Nodo<T>* siguiente; // Puntero al siguiente nodo
public:
    Nodo(T d) : dato(d), siguiente(nullptr) {} // Constructor: inicializa el dato y el puntero a null
    T& getDato() { return dato; }  // Devuelve una referencia al dato almacenado
    void setDato(T d) { dato = d; } // Asigna un nuevo dato al nodo
    Nodo<T>* getSiguiente() { return siguiente; }  // Devuelve el puntero al siguiente nodo
    void setSiguiente(Nodo<T>* s) { siguiente = s; }  // Asigna el puntero al siguiente nodo
};

//Clase de lista generica simple
template <class T>
class Lista {
private:
    Nodo<T>* inicio; // Puntero al primer nodo de la lista
    int tamanio; // Cantidad de elementos en la lista
public:
    Lista() : inicio(nullptr), tamanio(0) {}  // Constructor: lista vacía

    // Verifica si la lista esta vacia
    bool esVacia() {
        return inicio == nullptr;
    }

    // Devuelve la cantidad de elementos de la lista
    int getTamanio() {
        return tamanio;
    }

    // Inserta un dato en una posición específica de la lista
    void insertar(T dato, int pos) {
    if (pos < 0 || pos > tamanio) throw out_of_range("Índice fuera de rango");
    
    Nodo<T>* nuevo = new Nodo<T>(dato);
    
    if (pos == 0) {
        nuevo->setSiguiente(inicio);
        inicio = nuevo;
    } else {
        Nodo<T>* aux = inicio;
        for (int i = 0; i < pos - 1; i++) {
            aux = aux->getSiguiente();
        }
        nuevo->setSiguiente(aux->getSiguiente());
        aux->setSiguiente(nuevo);
    }

    tamanio++;
}

  // Inserta al final si no se especifica posición
void insertar(T dato) {
    insertar(dato, tamanio);
}

// Inserta al principio de la lista
void insertarPrimero(T dato) {
    Nodo<T>* nuevo = new Nodo<T>(dato);
    nuevo->setSiguiente(inicio);
    inicio = nuevo;
    tamanio++;
}

    // Inserta al final de la lista
    void insertarUltimo(T dato) {
        Nodo<T>* nuevo = new Nodo<T>(dato);
        if (esVacia()) {
            inicio = nuevo;
        } else {
            Nodo<T>* aux = inicio;
            while (aux->getSiguiente() != nullptr) {
                aux = aux->getSiguiente();
            }
            aux->setSiguiente(nuevo);
        }
        tamanio++;
    }

     // Elimina el nodo en la posición dada
    void remover(int pos) {
        if (pos < 0 || pos >= tamanio) throw out_of_range("Índice fuera de rango");

        Nodo<T>* actual = inicio;
        if (pos == 0) {
            inicio = inicio->getSiguiente();
        } else {
            Nodo<T>* anterior = nullptr;
            for (int i = 0; i < pos; i++) {
                anterior = actual;
                actual = actual->getSiguiente();
            }
            anterior->setSiguiente(actual->getSiguiente());
        }
        delete actual; // libera la memoria del nodo eliminado
        tamanio--;
    }

    // Devuelve una referencia al dato en la posición dada
    T& getDato(int pos) {
        if (pos < 0 || pos >= tamanio) throw out_of_range("Índice fuera de rango");
        Nodo<T>* aux = inicio;
        for (int i = 0; i < pos; i++) {
            aux = aux->getSiguiente();
        }
        return aux->getDato();
    }

     // Reemplaza el dato en una posición específica
    void reemplazar(int pos, T nuevoDato) {
        if (pos < 0 || pos >= tamanio) throw out_of_range("Índice fuera de rango");
        Nodo<T>* aux = inicio;
        for (int i = 0; i < pos; i++) {
            aux = aux->getSiguiente();
        }
        aux->setDato(nuevoDato);
    }

     // Elimina todos los nodos de la lista
    void vaciar() {
        while (!esVacia()) {
            remover(0); //remueve desde el inicio
        }
    }

    // Imprime los elementos de la lista
    void print() {
        Nodo<T>* aux = inicio;
        while (aux != nullptr) {
            cout << aux->getDato() << " -> ";
            aux = aux->getSiguiente();
        }
        cout << "NULL" << endl;
    }
};

#endif
