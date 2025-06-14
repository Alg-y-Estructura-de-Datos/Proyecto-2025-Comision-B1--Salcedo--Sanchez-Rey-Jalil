#ifndef ARBOLBINARIO_H
#define ARBOLBINARIO_H

#include <iostream>
#include <stdexcept>
using namespace std;

template <class T>
class NodoArbol {
public:
    T dato;
    NodoArbol* izq;
    NodoArbol* der;

    NodoArbol(T d) : dato(d), izq(nullptr), der(nullptr) {}
};

template <class T>
class ArbolBinario {
private:
    NodoArbol<T>* raiz;

    void put(NodoArbol<T>*& nodo, T dato) {
        if (nodo == nullptr) {
            nodo = new NodoArbol<T>(dato);
        } else if (*dato < *(nodo->dato)) {
            put(nodo->izq, dato);
        } else if (*dato > *(nodo->dato)) {
            put(nodo->der, dato);
        } else {
            throw runtime_error("Elemento duplicado");
        }
    }

    T search(NodoArbol<T>* nodo, T dato) {
        if (nodo == nullptr) {
            throw runtime_error("Elemento no encontrado");
        } else if (*dato == *(nodo->dato)) {
            return nodo->dato;
        } else if (*dato < *(nodo->dato)) {
            return search(nodo->izq, dato);
        } else {
            return search(nodo->der, dato);
        }
    }

    NodoArbol<T>* remove(NodoArbol<T>* nodo, T dato) {
        if (nodo == nullptr) return nullptr;

        if (*dato < *(nodo->dato)) {
            nodo->izq = remove(nodo->izq, dato);
        } else if (*dato > *(nodo->dato)) {
            nodo->der = remove(nodo->der, dato);
        } else {
            if (nodo->izq == nullptr && nodo->der == nullptr) {
                delete nodo;
                return nullptr;
            } else if (nodo->izq == nullptr) {
                NodoArbol<T>* temp = nodo->der;
                delete nodo;
                return temp;
            } else if (nodo->der == nullptr) {
                NodoArbol<T>* temp = nodo->izq;
                delete nodo;
                return temp;
            } else {
                NodoArbol<T>* temp = findMin(nodo->der);
                nodo->dato = temp->dato;
                nodo->der = remove(nodo->der, temp->dato);
            }
        }
        return nodo;
    }

    NodoArbol<T>* findMin(NodoArbol<T>* nodo) {
        while (nodo->izq != nullptr) nodo = nodo->izq;
        return nodo;
    }

    void inorder(NodoArbol<T>* nodo) {
        if (nodo != nullptr) {
            inorder(nodo->izq);
            cout << nodo->dato << endl;
            inorder(nodo->der);
        }
    }

    void destruir(NodoArbol<T>* nodo) {
        if (nodo != nullptr) {
            destruir(nodo->izq);
            destruir(nodo->der);
            delete nodo->dato;
            delete nodo;
        }
    }

public:
    ArbolBinario() : raiz(nullptr) {}

    ~ArbolBinario() {
        destruir(raiz);
    }

    void put(T dato) {
        put(raiz, dato);
    }

    T search(T dato) {
        return search(raiz, dato);
    }

    void remove(T dato) {
        raiz = remove(raiz, dato);
    }

    void inorder() {
        inorder(raiz);
    }
};

#endif