#ifndef HASHMAPLIST_H
#define HASHMAPLIST_H

#include <stdexcept> // Para manejar excepciones
#include "HashEntryProyecto.h" // Entrada clave-valor
#include "ListaProyecto.h"  // Lista simple genérica

// Plantilla de clase HashMapList: mapa hash con manejo de colisiones por listas
template <class K, class T>
class HashMapList {
private:
    Lista<HashEntry<K, T>>** tabla;  // Tabla hash: arreglo de punteros a listas de entradas
    unsigned int (*hashFunc)(K);  // Puntero a función hash
    unsigned int tamanio; // Tamaño del arreglo (cantidad de buckets)
    unsigned int cantidad; // Número total de elementos almacenados

public:
// Constructor: crea la tabla con el tamaño dado y asigna la función hash
    HashMapList(unsigned int tam, unsigned int (*func)(K)) {
        tamanio = tam;
        hashFunc = func;
        tabla = new Lista<HashEntry<K, T>>*[tam]; // arreglo de punteros a listas
        for (unsigned int i = 0; i < tam; i++) {
            tabla[i] = nullptr; // cada posición comienza vacía
        }
        cantidad = 0;
    }

    // Destructor: libera memoria de todas las listas y de la tabla
    ~HashMapList() {
        for (unsigned int i = 0; i < tamanio; i++) {
            if (tabla[i] != nullptr)
                delete tabla[i];
        }
        delete[] tabla;
    }

    // Inserta un par clave-valor o reemplaza si la clave ya existe
    void put(K clave, T valor) {
        unsigned int pos = hashFunc(clave) % tamanio;  // calcula la posición usando hash
        if (tabla[pos] == nullptr) {
            tabla[pos] = new Lista<HashEntry<K, T>>(); // crea la lista si está vacía
        }

        for (int i = 0; i < tabla[pos]->getTamanio(); i++) {
            if (tabla[pos]->getDato(i).getClave() == clave) {
                // Si la clave ya existe, se reemplaza el valor
                tabla[pos]->reemplazar(i, HashEntry<K, T>(clave, valor));
                return;
            }
        }

        // Si no existe, se inserta al final
        tabla[pos]->insertarUltimo(HashEntry<K, T>(clave, valor));
        cantidad++;
    }

     // Obtiene una referencia al valor asociado a la clave
    T& get(K clave) {
        unsigned int pos = hashFunc(clave) % tamanio;
        if (tabla[pos] == nullptr)
            throw std::runtime_error("Clave no encontrada");

        for (int i = 0; i < tabla[pos]->getTamanio(); i++) {
            HashEntry<K, T>& entry = tabla[pos]->getDato(i);
            if (entry.getClave() == clave) {
                return entry.getValor(); // Devuelve referencia válida
            }
        }

        throw std::runtime_error("Clave no encontrada");
    }

     // Elimina el par asociado a la clave
    void remove(K clave) {
        unsigned int pos = hashFunc(clave) % tamanio;
        if (tabla[pos] == nullptr)
            return;

        for (int i = 0; i < tabla[pos]->getTamanio(); i++) {
            if (tabla[pos]->getDato(i).getClave() == clave) {
                tabla[pos]->remover(i); // remueve la entrada
                cantidad--;
                return;
            }
        }
    }

      // Verifica si el mapa está vacío
    bool esVacio() {
        return cantidad == 0;
    }

    // Devuelve una lista con todas las claves almacenadas
    Lista<K> getList() {
        Lista<K> claves;
        for (unsigned int i = 0; i < tamanio; i++) {
            if (tabla[i] != nullptr) {
                for (int j = 0; j < tabla[i]->getTamanio(); j++) {
                    claves.insertarUltimo(tabla[i]->getDato(j).getClave());
                }
            }
        }
        return claves;
    }

     // Verifica si una clave existe en el mapa
    bool contieneClave(K clave) {
        unsigned int pos = hashFunc(clave) % tamanio;
        if (tabla[pos] == nullptr)
            return false;

        for (int i = 0; i < tabla[pos]->getTamanio(); i++) {
            if (tabla[pos]->getDato(i).getClave() == clave) {
                return true;
            }
        }
        return false;
    }

    // Imprime el contenido de la tabla
    void print() {
        for (unsigned int i = 0; i < tamanio; i++) {
            if (tabla[i] != nullptr) {
                std::cout << "Posición " << i << ": ";
                tabla[i]->print(); // usa el método print de la lista
            }
        }
    }

    Lista<K> claves() {
    Lista<K> lista_claves;

    for (unsigned int i = 0; i < tamanio; i++) {
        if (tabla[i] != nullptr) {
            for (int j = 0; j < tabla[i]->getTamanio(); j++) {
                HashEntry<K, T>& entry = tabla[i]->getDato(j);
                lista_claves.insertarUltimo(entry.getClave());
            }
        }
    }

    return lista_claves;
}

};

#endif