#ifndef CLASES_H // Verifica si el archivo ya fue incluido antes.
#define CLASES_H // Si no fue incluido, lo marca como que ya ha sido leido, para no volver a leerlo mas tarde.

#include <iostream>
#include <string>
using namespace std;

class Venta {
//atributos 
    private:
    int IDventa;
    string fecha;
    string pais;
    string ciudad;
    string cliente;
    string producto;
    string categoria;
    int cantidad;
    string Medioenvio;
    string EstadoEnvio;
    double PrecioUnit;
    double MontoTotal;
    public:
//constructor sin parametros
    Venta();
//constructor con parametros
    Venta(int IDventa, string fecha,string pais, string ciudad, string cliente, string producto, string categoria, int cantidad, string Medioenvio, string EstadoEnvio, double PrecioUnit, double MontoTotal);
//setters
    void setIDventa(int _IDventa);
    void setfecha(string _fecha);
    void setpais(string _pais);
    void setciudad(string _ciudad);
    void setcliente(string _cliente);
    void setproducto(string _producto);
    void setcategoria(string _categoria);
    void setcantidad(int _cantidad);
    void setMedioenvio(string _Medioenvio);
    void setEstadoEnvio(string _EstadoEnvio);
    void setPrecioUnit(double _PrecioUnit);
    void setMontoTotal(double _MontoTotal);
//getters
    int getIDventa();
    string getfecha();
    string getpais();
    string getciudad();
    string getcliente();
    string getproducto();
    string getcategoria();
    int getcantidad();
    string getMedioenvio();
    string getEstadoEnvio();
    double getPrecioUnit();
    double getMontoTotal();
};
#endif
