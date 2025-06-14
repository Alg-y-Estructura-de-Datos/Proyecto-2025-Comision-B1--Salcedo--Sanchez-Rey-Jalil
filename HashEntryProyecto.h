#ifndef HASHENTRY_H
#define HASHENTRY_H

template <class K, class T>
class HashEntry {
private:
    K clave;
    T valor;
public:
    HashEntry() = delete;
    HashEntry(K c, T v) : clave(c), valor(v) {}
    K getClave() const { return clave; }
    T& getValor() { return valor; }
    void setValor(T nuevoValor) { valor = nuevoValor; }
};

#endif

