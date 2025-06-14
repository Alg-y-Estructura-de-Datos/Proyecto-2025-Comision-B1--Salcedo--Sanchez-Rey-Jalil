#include "Venta.h"

Venta::Venta(){
     IDventa=0;
        fecha= " ";
        pais= " ";
        ciudad= " ";
        cliente= " ";
        producto= " ";
        categoria= " ";
        cantidad=0;
        Medioenvio= " ";
        EstadoEnvio= " ";
        PrecioUnit=0;
        MontoTotal=0;
}
//constructor con parametros
Venta::Venta(int _IDventa, string _fecha, string _pais, string _ciudad, string _cliente, string _producto, string _categoria, int _cantidad, string _Medioenvio, string _EstadoEnvio, double _PrecioUnit, double _MontoTotal){
    IDventa=_IDventa;
    fecha= _fecha;
    pais= _pais;
    ciudad= _ciudad;
    cliente= _cliente;
    producto= _producto;
    categoria= _categoria;
    cantidad=_cantidad;
    Medioenvio= _Medioenvio;
    EstadoEnvio=_EstadoEnvio; 
    PrecioUnit=_PrecioUnit;
    MontoTotal=_MontoTotal;
}

//setters
void Venta::setIDventa(int _IDventa){
    IDventa=_IDventa;
}
void Venta::setfecha(string _fecha){
    fecha=_fecha;
}
void Venta::setpais(string _pais){
    pais=_pais;
}
void Venta::setciudad(string _ciudad){
    ciudad=_ciudad;
}
void Venta::setcliente(string _cliente){
    cliente=_cliente;
}
void Venta::setproducto(string _producto){
    producto=_producto;
}
void Venta::setcategoria(string _categoria){
    categoria=_categoria;
}
void Venta::setcantidad(int _cantidad){
    cantidad=_cantidad;;
}
void Venta::setMedioenvio(string _Medioenvio){
    Medioenvio=_Medioenvio;
}
void Venta::setEstadoEnvio(string _EstadoEnvio){
    EstadoEnvio=_EstadoEnvio;
}
void Venta::setPrecioUnit(double _PrecioUnit){
    PrecioUnit=_PrecioUnit;;
}
void Venta::setMontoTotal(double _MontoTotal){
    MontoTotal=_MontoTotal;
}

//getters
int Venta::getIDventa(){
    return IDventa;
}
string Venta::getfecha(){
    return fecha;
}
string Venta:: getpais(){
    return pais;
}
string Venta:: getciudad(){
    return ciudad;
}
string Venta:: getcliente(){
    return cliente;
}
string Venta:: getproducto(){
    return producto;
}
string Venta:: getcategoria(){
    return categoria;
}
int Venta:: getcantidad(){
    return cantidad;
}
string Venta::getMedioenvio(){
    return Medioenvio;
}
string Venta:: getEstadoEnvio(){
    return EstadoEnvio;
}
double Venta:: getPrecioUnit(){
    return PrecioUnit;
}
double Venta:: getMontoTotal(){
    return MontoTotal;
}

