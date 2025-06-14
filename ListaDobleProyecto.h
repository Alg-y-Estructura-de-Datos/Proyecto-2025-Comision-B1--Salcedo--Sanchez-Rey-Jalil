#ifndef LISTADOBLE_H
#define LISTADOBLE_H

#include <iostream>
#include <stdexcept>
using namespace std;

template <typename T>
class NodoDoble {
public:
    T dato;
    NodoDoble* siguiente;
    NodoDoble* anterior;

    NodoDoble(T dato) : dato(dato), siguiente(nullptr), anterior(nullptr) {}
};

template <typename T>
class ListaDoble {
private:
    NodoDoble<T>* cabeza;
    NodoDoble<T>* cola;
    int tamanio;

public:
    ListaDoble() : cabeza(nullptr), cola(nullptr), tamanio(0) {}

    bool esVacia() const {
        return tamanio == 0;
    }

    int getTamanio() const {
        return tamanio;
    }

    void insertarPrimero(T dato) {
        NodoDoble<T>* nuevo = new NodoDoble<T>(dato);
        if (esVacia()) {
            cabeza = cola = nuevo;
        } else {
            nuevo->siguiente = cabeza;
            cabeza->anterior = nuevo;
            cabeza = nuevo;
        }
        tamanio++;
    }

    void insertarUltimo(T dato) {
        NodoDoble<T>* nuevo = new NodoDoble<T>(dato);
        if (esVacia()) {
            cabeza = cola = nuevo;
        } else {
            cola->siguiente = nuevo;
            nuevo->anterior = cola;
            cola = nuevo;
        }
        tamanio++;
    }

    T getDato(int pos) const {
        if (pos < 0 || pos >= tamanio) {
            throw out_of_range("Posición inválida");
        }

        NodoDoble<T>* aux = cabeza;
        for (int i = 0; i < pos; i++) {
            aux = aux->siguiente;
        }

        return aux->dato;
    }

    void reemplazar(int pos, T dato) {
        if (pos < 0 || pos >= tamanio) {
            throw out_of_range("Posición inválida");
        }

        NodoDoble<T>* aux = cabeza;
        for (int i = 0; i < pos; i++) {
            aux = aux->siguiente;
        }

        aux->dato = dato;
    }

    void remover(int pos) {
        if (pos < 0 || pos >= tamanio) {
            throw out_of_range("Posición inválida");
        }

        NodoDoble<T>* borrar;
        if (pos == 0) {
            borrar = cabeza;
            cabeza = cabeza->siguiente;
            if (cabeza) cabeza->anterior = nullptr;
            else cola = nullptr;
        } else if (pos == tamanio - 1) {
            borrar = cola;
            cola = cola->anterior;
            cola->siguiente = nullptr;
        } else {
            borrar = cabeza;
            for (int i = 0; i < pos; i++) {
                borrar = borrar->siguiente;
            }
            borrar->anterior->siguiente = borrar->siguiente;
            borrar->siguiente->anterior = borrar->anterior;
        }

        delete borrar;
        tamanio--;
    }

    void vaciar() {
        while (!esVacia()) {
            remover(0);
        }
    }

    void print() const {
        NodoDoble<T>* aux = cabeza;
        while (aux != nullptr) {
            cout << aux->dato << " ";
            aux = aux->siguiente;
        }
        cout << endl;
    }

    ~ListaDoble() {
        vaciar();
    }
};

#endif
