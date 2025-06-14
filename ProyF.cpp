// ==========================================
// PROYECTO FINAL – SISTEMA DE VENTAS
// Alumnas: JALIL, Carola (2403755)
//          REY, Valentina (2401874)
//          SANCHEZ DEMARIA, Guillermina (2426384)
// Materia: Algoritmos y estructuras de datos – 2025
// ------------------------------------------
// Leer el MANUAL DE USUARIO antes de ejecutar
// para conocer funciones, validaciones y menú
// ==========================================

#include <iostream> //entrada y salida estandar
#include "Venta.h" //definicion de la clase venta
#include "Ordenador_Generico.h" //para ordenar listas genericas
#include <string> 
#include <sstream>  //procesar cada linea del archivo csv, dividiendola en campos separados por coma
#include <fstream>  //abrir y leer el archivo csv
#include <chrono> //para medir el tiempo de ejecucion
#include "ListaProyecto.h" //lista simple personalizada
#include "ListaDobleProyecto.h" //lista doble personalizada
#include "HashMapListProyecto.h" //hashmaplist personalizada
#include "ArbolBinarioProyecto.h"
#include <stack> // Para usar pila
#include <filesystem> //para verificar si el archivo existe
#define NOMBRE_ARCHIVO "C:/Users/guill/OneDrive/Documentos/PROYECTO/ventas_sudamerica.csv" //reemplaza NOMBRE_ARCHIVO automaticamente por "ventas_sudamericanas.csv"
using namespace std;
using namespace std::chrono;

//funcion hash para string (Mmultiplica por 31 c/caracter)
unsigned int funcionHashString(string clave) {
    unsigned int hash = 0;
    for (char c : clave) {
        hash = hash * 31 + c;
    }
    return hash;
}

// Normaliza texto: convierte a minúsculas y reemplaza tildes y eñes
void normalizar(string& texto) {
   for (size_t i = 0; i < texto.length(); i++) {
        switch (static_cast<unsigned char>(texto[i])) {
            case 0xE1: texto[i] = 'a'; break; // á
            case 0xE9: texto[i] = 'e'; break; // é
            case 0xED: texto[i] = 'i'; break; // í
            case 0xF3: texto[i] = 'o'; break; // ó
            case 0xFA: texto[i] = 'u'; break; // ú
            case 0xF1: texto[i] = 'n'; break; // ñ
            case 0xC1: texto[i] = 'a'; break; // Á
            case 0xC9: texto[i] = 'e'; break; // É
            case 0xCD: texto[i] = 'i'; break; // Í
            case 0xD3: texto[i] = 'o'; break; // Ó
            case 0xDA: texto[i] = 'u'; break; // Ú
            case 0xD1: texto[i] = 'n'; break; // Ñ
            default:
                texto[i] = tolower(static_cast<unsigned char>(texto[i]));
                break;
        }
    }
}

// Valida que la fecha tenga formato "DD/MM/AAAA"
bool validarFormatoFecha(const string& fecha) {
    if (fecha.size() != 10) return false;
    if (fecha[2] != '/' || fecha[5] != '/') return false;
    for (size_t i = 0; i < fecha.size(); i++) {
        if (i == 2 || i == 5) continue;
        if (!isdigit(fecha[i])) return false;
    }
    return true;
}


// Valida que la fecha sea una fecha posible real
bool validarFechaValida(const string& fecha) {
    int dia = stoi(fecha.substr(0, 2));
    int mes = stoi(fecha.substr(3, 2));
    int anio = stoi(fecha.substr(6, 4));
    if (mes < 1 || mes > 12 || dia < 1) return false;
    int diasPorMes[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    bool bisiesto = (anio % 4 == 0 && (anio % 100 != 0 || anio % 400 == 0));
    if (bisiesto && mes == 2) {
        return dia <= 29;
    } else {
        return dia <= diasPorMes[mes - 1];
    }
}

// Convierte "DD/MM/AAAA" en un entero YYYYMMDD para poder comparar fechas
int fechaComoEntero(const string& f) {
    if (!validarFormatoFecha(f)) {
        throw invalid_argument("Fecha con formato incorrecto: " + f);
    }
    
    int dia = stoi(f.substr(0, 2));
    int mes = stoi(f.substr(3, 2));
    int anio = stoi(f.substr(6, 4));
    return anio * 10000 + mes * 100 + dia; // Ej: 20250410
}

bool fechaEnRango(const string& fecha, const string& inicio, const string& fin) {
    if (!validarFormatoFecha(fecha) || !validarFormatoFecha(inicio) || !validarFormatoFecha(fin)) {
        throw runtime_error("Alguna fecha tiene formato inválido.");
    }
    
    int f = fechaComoEntero(fecha);
    int i = fechaComoEntero(inicio);
    int t = fechaComoEntero(fin);
    return f >= i && f <= t;
}

//Procesamiento de datos durante la lectura: (1) Top 3 de ciudades con mayor monto de ventas por pais.
//----------------------------------------------------------------------------------------------------
////estructura que representa una ciudad con su monto total de ventas acumulado.
struct ciudad_monto {
    string ciudad;
    float monto_total;
};

//comparar para ordenar por monto descendente
bool compararMontoDesc(ciudad_monto a, ciudad_monto b) {
    return a.monto_total > b.monto_total;
}

//funcion recursiva que muestra el top 3 de ciudades
void mostrar_top3_recursivo(Lista<ciudad_monto>& ciudades, int pos) {
    if (pos >= 3 || pos >= ciudades.getTamanio()) return; //condicion de corte

    ciudad_monto cm = ciudades.getDato(pos);
    cout << "  " << pos + 1 << ". " << cm.ciudad << " - $" << cm.monto_total << endl;

    mostrar_top3_recursivo(ciudades, pos + 1); //llamada recursiva
}

//procesa y muestra el top 3 de ciudades con mayor monto de ventas por pais.
void top3_ciudades_hash(HashMapList<string, HashMapList<string, float>*>& mapa , Lista<string>& paises, HashMapList<string, Lista<string>>& ciudadesPorPais) {
    auto inicio = high_resolution_clock::now(); //medir el tiempo
    int contador_if = 0;

    cout << "Entrando a top3_ciudades_hash. Paises detectados: " << paises.getTamanio() << endl;

    for (int i = 0; i < paises.getTamanio(); i++) {
        string pais = paises.getDato(i);
        cout << "Top 3 ciudades de " << pais << ":" << endl;

        try {
            cout << "Recuperando ciudades del pais: " << pais << endl;
            //recuperar las ventas por ciudad para este pais
            HashMapList<string, float> *ciudades= mapa.get(pais);
            //recuperar nombres de las ciudades
            Lista<string> nombres = ciudadesPorPais.get(pais);
            //crear lista aux para ordenar
            Lista<ciudad_monto> lista_ciudades;

            for (int j = 0; j < nombres.getTamanio(); j++) {
                string ciudad = nombres.getDato(j);
                float monto = ciudades->get(ciudad);
                lista_ciudades.insertarUltimo({ciudad, monto});
            }

            //ordenar la lista por monto descendente
            ordenarListaGenerica(lista_ciudades, compararMontoDesc);
            cout << "Total de ciudades cargadas: " << nombres.getTamanio() << endl;
            mostrar_top3_recursivo(lista_ciudades, 0); //mostrar top 3

        } catch (exception &e) {
            cout << " Error al procesar el pais: " << pais << endl;
            cout << " Detalle del error: " << e.what() << endl;
        }
    }

    auto fin = high_resolution_clock::now(); //fin de la medicion 
    auto duracion = duration_cast<milliseconds>(fin - inicio); //calcular duracion

    //resumen
    cout << "Proceso: Top 3 ciudades por pais" << endl;
    cout << "Tiempo de ejecucion: " << duracion.count() << " ms" << endl;
    cout << " Condicionales 'if' utilizados: " << contador_if << endl;
    std::cout << " Estructura utilizada: HashMapList + Lista + Quicksort generico" << endl;
}

//Procesamiento de datos durante la lectura: (2) Monto total vendido por producto de c/pais.
//----------------------------------------------------------------------------------------------------
struct producto_monto {
    string producto;
    float monto_total;
};

bool compararMontoDesc(producto_monto a, producto_monto b) {
    return a.monto_total > b.monto_total;
}

void mostrar_monto_por_pais_producto(HashMapList<string, HashMapList<string, float>*>& mapa_pais_producto, Lista<string>& paises, HashMapList<string, Lista<string>>& productos_por_pais) {
    auto inicio = high_resolution_clock::now();
    int contador_if = 0;

    for (int i = 0; i < paises.getTamanio(); i++) {
        string pais = paises.getDato(i);
        cout << "Pais: " << pais << endl;

        try {
            HashMapList<string, float>* productos = mapa_pais_producto.get(pais);
            Lista<string> listaProductos = productos_por_pais.get(pais);
            Lista<producto_monto> lista_montos;

            for (int j = 0; j < listaProductos.getTamanio(); j++) {
                string prod = listaProductos.getDato(j);
                float monto = productos->get(prod);
                lista_montos.insertarUltimo({prod, monto});
            }

            ordenarListaGenerica(lista_montos, compararMontoDesc);

            for (int j = 0; j < lista_montos.getTamanio(); j++) {
                producto_monto pm = lista_montos.getDato(j);
                cout << "\n-------------------------------------\n";
                cout << " Producto: " << pm.producto << " - Monto total: $" << pm.monto_total << endl;
            }

        } catch (...) {
            cout << "Error. No hay productos registrados" << endl;
        }
        cout << endl;
    }

    auto fin = high_resolution_clock::now();
    auto duracion = duration_cast<milliseconds>(fin - inicio);

    cout << "\n Proceso: Monto total por producto y pais" << endl;
    cout << " Tiempo de ejecucion: " << duracion.count() << " ms" << endl;
    cout << " Condicionales 'if' utilizados: " << contador_if << endl;
    cout << "Estructura utilizada: HashMapList + Lista + Quicksort" << endl;
}

//Procesamiento de datos durante la lectura: (3) Promedio de ventas p/ categoria de cada pais.
//----------------------------------------------------------------------------------------------------
struct categoria_monto {
    string categoria;
    float monto_total;
    int cantidad_ventas;
};

struct pais_categorias {
    string pais;
    Lista<categoria_monto> categorias;
};

void insertarOrdenadoPaisCategoria(Lista<pais_categorias>& lista, pais_categorias nuevo) {
    int i = 0;
    while (i < lista.getTamanio() && lista.getDato(i).pais < nuevo.pais) i++;
    lista.insertar(nuevo, i);
}

void insertarOrdenadoCategoria(Lista<categoria_monto>& lista, categoria_monto nuevo) {
    int i = 0;
    while (i < lista.getTamanio() && lista.getDato(i).categoria < nuevo.categoria) i++;
    lista.insertar(nuevo, i);
}

int buscarPaisBinarioCategoria(Lista<pais_categorias>& lista, string pais) {
    int izq = 0, der = lista.getTamanio() - 1;
    while (izq <= der) {
        int medio = (izq + der) / 2;
        string actual = lista.getDato(medio).pais;
        if (actual == pais) return medio;
        else if (actual < pais) izq = medio + 1;
        else der = medio - 1;
    }
    return -1;
}

int buscarCategoriaBinario(Lista<categoria_monto>& lista, string categoria) {
    int izq = 0, der = lista.getTamanio() - 1;
    while (izq <= der) {
        int medio = (izq + der) / 2;
        string actual = lista.getDato(medio).categoria;
        if (actual == categoria) return medio;
        else if (actual < categoria) izq = medio + 1;
        else der = medio - 1;
    }
    return -1;
}

void agrupar_ventas_por_pais_categoria(ListaDoble<Venta>& ventas, Lista<pais_categorias>& paises) {
    for (int i = 0; i < ventas.getTamanio(); i++) {
        Venta v = ventas.getDato(i);
        string pais = v.getpais();
        string categoria = v.getcategoria();
        float monto = v.getMontoTotal();

        int posPais = buscarPaisBinarioCategoria(paises, pais);
        if (posPais != -1) {
            pais_categorias pc = paises.getDato(posPais);
            int posCat = buscarCategoriaBinario(pc.categorias, categoria);
            if (posCat != -1) {
                categoria_monto cm = pc.categorias.getDato(posCat);
                cm.monto_total += monto;
                cm.cantidad_ventas++;
                pc.categorias.reemplazar(posCat, cm);
            } else {
                categoria_monto nuevo_cm{categoria, monto, 1};
                insertarOrdenadoCategoria(pc.categorias, nuevo_cm);
            }
            paises.reemplazar(posPais, pc);
        } else {
            pais_categorias nuevo_pc;
            nuevo_pc.pais = pais;
            categoria_monto nuevo_cm{categoria, monto, 1};
            insertarOrdenadoCategoria(nuevo_pc.categorias, nuevo_cm);
            insertarOrdenadoPaisCategoria(paises, nuevo_pc);
        }
    }
}

void mostrar_promedio_por_categoria(Lista<pais_categorias>& paises) {
    auto inicio = high_resolution_clock::now();
    int contador_if = 0;

    for (int i = 0; i < paises.getTamanio(); i++) {
        pais_categorias  pc = paises.getDato(i);
        cout << "\n-------------------------------------\n";
        cout << "Pais: " << pc.pais << endl;

        for (int j = 0; j < pc.categorias.getTamanio(); j++) {
            const categoria_monto& cm = pc.categorias.getDato(j);
            float promedio = cm.monto_total / cm.cantidad_ventas;
            cout << "  Categoría: " << cm.categoria << " - Promedio: $" << promedio << endl;
        }
    }

    auto fin = high_resolution_clock::now();
    auto duracion = duration_cast<milliseconds>(fin - inicio);

    cout << "\n Proceso: Promedio por categoría y pais" << endl;
    cout << " Tiempo de ejecucion: " << duracion.count() << " ms" << endl;
    cout << " Condicionales 'if' utilizados: " << contador_if << endl;
    cout << " Estructura utilizada: Lista ordenada + búsqueda binaria" << endl;
}

//Procesamiento de datos durante la lectura: (4) Medio de envio mas utilizado p/ pais
//----------------------------------------------------------------------------------------------------
// Estructura para representar un medio de envío
struct envio_dato {
    string medio;
    int cantidad;

    bool operator<(const envio_dato &otro) const {
        return medio < otro.medio;
    }
    bool operator>(const envio_dato &otro) const {
        return medio > otro.medio;
    }
    bool operator==(const envio_dato &otro) const {
        return medio == otro.medio;
    }
};

// Estructura para representar un país con su árbol de medios de envío
struct pais_envios {
    string pais;
    ArbolBinario<envio_dato*>* envios=new ArbolBinario<envio_dato*>();
    envio_dato medio_mas_usado = {"", 0};

    bool operator<(const pais_envios &otro) const {
        return pais < otro.pais;
    }
    bool operator>(const pais_envios &otro) const {
        return pais > otro.pais;
    }
    bool operator==(const pais_envios &otro) const {
        return pais == otro.pais;
    }
};

ostream& operator<<(ostream& os, const pais_envios* pe) {
    os << "Pais: " << pe->pais
       << " - Medio mas usado: " << pe->medio_mas_usado.medio
       << " (" << pe->medio_mas_usado.cantidad << " veces)";
    return os;
}


// Agrupar los medios de envío por país
void acumular_envios_por_pais(ArbolBinario<pais_envios*>& arbol, Venta& v) {
    string pais = v.getpais();
    string medio = v.getMedioenvio();

    pais_envios* peTemp = new pais_envios;
    peTemp->pais = pais;

    try {
        pais_envios* pe = arbol.search(peTemp);
        envio_dato* envioTemp = new envio_dato{medio, 0};
        try {
            envio_dato* encontrado = pe->envios->search(envioTemp);
            pe->envios->remove(encontrado);
            encontrado->cantidad++;
            pe->envios->put(encontrado);
            if (encontrado->cantidad > pe->medio_mas_usado.cantidad) {
                pe->medio_mas_usado = *encontrado;
            }
        } catch (...) {
            pe->envios->put(new envio_dato{medio, 1});
            if (pe->medio_mas_usado.cantidad == 0) {
                pe->medio_mas_usado = {medio, 1};
            }
        }
        arbol.remove(pe);
        arbol.put(pe);
    } catch (...) {
        pais_envios* nuevoPais = new pais_envios;
        nuevoPais->pais = pais;
        nuevoPais->envios->put(new envio_dato{medio, 1});
        nuevoPais->medio_mas_usado = {medio, 1};
        arbol.put(nuevoPais);
    }

    delete peTemp;
}


// Mostrar el medio más usado por país
void mostrar_medio_envio_mas_usado(ArbolBinario<pais_envios*> &arbol) {
    auto inicio = high_resolution_clock::now();
    int contador_if = 0;

    cout << "\n-------------------------------------\n";
    cout << "\nMedio de envio mas usado por pais:\n";
    arbol.inorder();

    auto fin = high_resolution_clock::now();
    auto duracion = duration_cast<milliseconds>(fin - inicio);

    cout << "\n Proceso: Medio de envío mas usado por pais" << endl;
    cout << "Tiempo de ejecucion: " << duracion.count() << " ms" << endl;
    cout << " Condicionales 'if' utilizados: " << contador_if << endl;
    cout << " Estructura utilizada: Arbol binario" << endl;
}

//Procesamiento de datos durante la lectura: (5) Medio de envio mas utilizado p/ categoria
//----------------------------------------------------------------------------------------------------
struct envio_categoria {
    string categoria;
    HashMapList<string, int>* medios;

    envio_categoria() : categoria(""), medios(new HashMapList<string, int>(100, funcionHashString)) {}

    envio_categoria(string cat) : categoria(cat), medios(new HashMapList<string, int>(100, funcionHashString)) {}

    bool operator<(const envio_categoria& other) const {
        return categoria < other.categoria;
    }
    bool operator>(const envio_categoria& other) const {
        return categoria > other.categoria;
    }
    bool operator==(const envio_categoria& other) const {
        return categoria == other.categoria;
    }
};

ostream& operator<<(ostream& os, const envio_categoria* ec) {
    os << "Categoria: " << ec->categoria << " - ";

    string medio_mas_usado = "";
    int max_cantidad = 0;

    Lista<string> claves = ec->medios->claves();
    for (int i = 0; i < claves.getTamanio(); i++) {
        string medio = claves.getDato(i);
        int cant = ec->medios->get(medio);
        if (cant > max_cantidad) {
            max_cantidad = cant;
            medio_mas_usado = medio;
        }
    }

    os << "Medio mas usado: " << medio_mas_usado << " (" << max_cantidad << " veces)";
    return os;
}

void agregarMedioEnvioPorCategoria(ArbolBinario<envio_categoria*>& arbolCategorias, Venta& venta) {
    string categoria = venta.getcategoria(); normalizar(categoria);
    string medio = venta.getMedioenvio(); normalizar(medio);

    envio_categoria* temp = new envio_categoria(categoria);

    try {
        envio_categoria* existente = arbolCategorias.search(temp);
        int cantidad = 0;

        try {
            cantidad = existente->medios->get(medio);
        } catch (...) {
            cantidad = 0;
        }

        existente->medios->put(medio, cantidad + 1);
        arbolCategorias.remove(existente);
        arbolCategorias.put(existente);
    } catch (...) {
        envio_categoria* nuevo = new envio_categoria(categoria);
        nuevo->medios->put(medio, 1);
        arbolCategorias.put(nuevo);
    }

    delete temp; // liberar el temporal si ya no se usa
}

void mostrarMedioMasUsadoPorCategoria(ArbolBinario<envio_categoria*>& arbolCategorias) {
    cout << "\n-------------------------------------\n";
    cout << "Medio de envio mas utilizado por categoria:\n";
    arbolCategorias.inorder(); 
}

//Procesamiento de datos durante la lectura: (6) Dia con mayor cantidad de ventas (p/ monto)
//----------------------------------------------------------------------------------------------------
// Acumula los montos por fecha
void acumularMontosPorFecha(ListaDoble<Venta>& ventas, HashMapList<string, float>* ventasPorDia, Lista<string>* fechasUnicas) {
    for (int i = 0; i < ventas.getTamanio(); i++) {
        Venta v = ventas.getDato(i);
        string fecha = v.getfecha();
        float monto = v.getMontoTotal();

        float monto_actual = 0;
        try {
            monto_actual = ventasPorDia->get(fecha);
        } catch (...) {
            fechasUnicas->insertarUltimo(fecha); // Solo si es una nueva fecha
        }

        ventasPorDia->put(fecha, monto_actual + monto);
    }
}

void mostrarDiaConMasVentas(HashMapList<string, float>* ventasPorDia, Lista<string>* fechasUnicas) {
    auto inicio = high_resolution_clock::now();
    int contador_if = 0;

    string fechaMax = "";
    float montoMax = -1;

    for (int i = 0; i < fechasUnicas->getTamanio(); i++) {
        string fecha = fechasUnicas->getDato(i);
        float monto = ventasPorDia->get(fecha);

        if (monto > montoMax) {
            montoMax = monto;
            fechaMax = fecha;
        }
    }

    cout << "\n-------------------------------------\n";
    cout << "Dia con mayor monto total de ventas: " << fechaMax << " ($" << montoMax << ")" << endl;

    auto fin = high_resolution_clock::now();
    auto duracion = duration_cast<milliseconds>(fin - inicio);

    cout << "\n Proceso: Dia con mas ventas" << endl;
    cout << "Tiempo de ejecucion: " << duracion.count() << " ms" << endl;
    cout << " Condicionales 'if' utilizados: " << contador_if << endl;
    cout << " Estructura utilizada: HashMapList + comparación secuencial" << endl;
}

//Procesamiento de datos durante la lectura: (7) Estado de envio mas frecuente p/ pais.
//----------------------------------------------------------------------------------------------------
void acumularEstadosPorPais(
    Venta& v,
    HashMapList<string, HashMapList<string, int>*>& estadosPorPais,
    HashMapList<string, Lista<string>*>& estadosPorPais_claves,
    Lista<string>*& paises)
{
    string pais = v.getpais(); normalizar(pais);
    string estado = v.getEstadoEnvio(); normalizar(estado);

    HashMapList<string, int>* estados = nullptr;
    Lista<string>* listaEstados = nullptr;

    try {
        estados = estadosPorPais.get(pais);
    } catch (...) {
        estados = new HashMapList<string, int>(50, funcionHashString);
        estadosPorPais.put(pais, estados);
        paises->insertarUltimo(pais);
    }

    try {
        listaEstados = estadosPorPais_claves.get(pais);
    } catch (...) {
        listaEstados = new Lista<string>();
        estadosPorPais_claves.put(pais, listaEstados);
    }

    int cantidad = 0;
    try {
        cantidad = estados->get(estado);
    } catch (...) {
        listaEstados->insertarUltimo(estado); // Solo si es nuevo
    }

    estados->put(estado, cantidad + 1);
}
void mostrarEstadoMasFrecuentePorPais(
    HashMapList<string, HashMapList<string, int>*>& estadosPorPais,
    HashMapList<string, Lista<string>*>& estadosPorPais_claves,
    Lista<string>*& paises)
{
    auto inicio = high_resolution_clock::now();
    int contador_if = 0;

    for (int i = 0; i < paises->getTamanio(); i++) {
        string pais = paises->getDato(i);
        cout << "\n-------------------------------------\n";
        cout << "Pais: " << pais << endl;

        try {
            HashMapList<string, int>* estados = estadosPorPais.get(pais);
            Lista<string>* claves = estadosPorPais_claves.get(pais);

            int max = -1;
            string estado_mas_frecuente;

            for (int j = 0; j < claves->getTamanio(); j++) {
                string estado = claves->getDato(j);
                int cantidad = estados->get(estado);
                if (cantidad > max) {
                    max = cantidad;
                    estado_mas_frecuente = estado;
                }
            }

            cout << "  Estado mas frecuente: " << estado_mas_frecuente << " (" << max << " veces)" << endl;

        } catch (...) {
            cout << "  No hay datos para este pais" << endl;
        }

        cout << endl;
    }

    auto fin = high_resolution_clock::now();
    auto duracion = duration_cast<milliseconds>(fin - inicio);

    cout << "\nProceso: Estado de envio mas frecuente por pais" << endl;
    cout << "Tiempo de ejecucion: " << duracion.count() << " ms" << endl;
    cout << "Condicionales 'if' utilizados: " << contador_if << endl;
    cout << "Estructura utilizada: HashMapList anidado + Lista" << endl;
}

////Procesamiento de datos durante la lectura: (8) Producto más y menos vendido por cantidad total .
//----------------------------------------------------------------------------------------------------
// Estructura auxiliar para el ranking
struct producto_cantidad {
    string producto;
    int cantidad;
};

// Criterio de ordenamiento descendente
bool criterioDesc(producto_cantidad a, producto_cantidad b) {
    return a.cantidad > b.cantidad;
}

// Acumula cantidades por producto (para usar en cargar_ventas)
void acumularCantidadPorProducto(
    Venta& v,
    HashMapList<string, int>* cantidadPorProducto,
    Lista<string>* productos)
{
    string producto = v.getproducto();
    normalizar(producto);
    int actual = 0;

    try {
        actual = cantidadPorProducto->get(producto);
    } catch (...) {
        productos->insertarUltimo(producto); // Producto nuevo
    }

    cantidadPorProducto->put(producto, actual + 1);
}

// Mostrar el producto más y menos vendido
void mostrarProductoMasYMenosVendido(
    HashMapList<string, int>* cantidadPorProducto,
    Lista<string>* productos)
{
    auto inicio = high_resolution_clock::now();
    int contador_if = 0;

    if (productos->getTamanio() == 0) {
        cout << "No hay productos cargados." << endl;
        return;
    }

    string masVendido = productos->getDato(0);
    string menosVendido = productos->getDato(0);
    int maxCantidad = cantidadPorProducto->get(masVendido);
    int minCantidad = maxCantidad;

    for (int i = 1; i < productos->getTamanio(); i++) {
        string nombre = productos->getDato(i);
        int cant = cantidadPorProducto->get(nombre);

        if (cant > maxCantidad) {
            maxCantidad = cant;
            masVendido = nombre;
        }
        if (cant < minCantidad) {
            minCantidad = cant;
            menosVendido = nombre;
        }
    }

    cout << "\n-------------------------------------\n";
    cout << "\nProducto mas vendido: " << masVendido << " (" << maxCantidad << " unidades)" << endl;
    cout << "Producto menos vendido: " << menosVendido << " (" << minCantidad << " unidades)" << endl;

    auto fin = high_resolution_clock::now();
    auto duracion = duration_cast<milliseconds>(fin - inicio);

    cout << "\nProceso: Producto mas y menos vendido" << endl;
    cout << "Tiempo de ejecucion: " << duracion.count() << " ms" << endl;
    cout << "Condicionales 'if' utilizados: " << contador_if << endl;
    cout << "Estructura utilizada: HashMapList + Lista" << endl;
}

// Mostrar ranking de productos más vendidos
void mostrarRankingProductos(
    HashMapList<string, int>* cantidadPorProducto,
    Lista<string>* productos)
{
    Lista<producto_cantidad> listaRanking;

    for (int i = 0; i < productos->getTamanio(); i++) {
        string nombre = productos->getDato(i);
        int cant = cantidadPorProducto->get(nombre);
        listaRanking.insertarUltimo({nombre, cant});
    }

    ordenarListaGenerica(listaRanking, criterioDesc);

    cout << "\n-------------------------------------\n";
    cout << "\nRanking de productos mas vendidos:\n";
    for (int i = 0; i < listaRanking.getTamanio(); i++) {
        producto_cantidad pc = listaRanking.getDato(i);
        cout << " " << i + 1 << ". " << pc.producto << " - " << pc.cantidad << " unidades" << endl;
    }
}

//---------------------------------------------------------------------------------------------------------------
//FUNCION PARA PROCESAR LAS 8 FUNCIONES ANTERIORES
void procesarTodo(ListaDoble<Venta>& ventas) {
    // Estructuras auxiliares para cada procesamiento

     // (1) Top 3 de ciudades con mayor monto de ventas por pais.
    HashMapList<string, HashMapList<string, float>*> ventas_ciudad_pais(50, funcionHashString);
    HashMapList<string, Lista<string>> ciudadesPorPais(50, funcionHashString);
    Lista<string> paises;

    // (2) Monto total vendido por producto de c/pais.
    HashMapList<string, HashMapList<string, float>*> ventas_producto_pais(50, funcionHashString);
    HashMapList<string, Lista<string>> productosPorPais(50, funcionHashString);

    //(3) promedio por categoria
    Lista<pais_categorias> lista_pais_categorias;

    //(4) medio de envio mas usado por pais
    ArbolBinario<pais_envios*> arbol_envios;

    //(5) medio de envio mas usado por categoria
    ArbolBinario<envio_categoria*> arbol_categorias_envio;

    //(6) dia con mas ventas
    HashMapList<string, float>* ventasPorDia = new HashMapList<string, float>(100, funcionHashString);
    Lista<string>* fechasUnicas = new Lista<string>();

    //(7) Estado de envio mas frecuente p/ pais.
    HashMapList<string, HashMapList<string, int>*>* estadosPorPais = new HashMapList<string, HashMapList<string, int>*>(50, funcionHashString);
    HashMapList<string, Lista<string>*>* estadosPorPais_claves = new HashMapList<string, Lista<string>*>(50, funcionHashString);
    Lista<string>* paises_estado = new Lista<string>();

    //(8) Producto más y menos vendido por cantidad total.
    HashMapList<string, int>* cantidadPorProducto = new HashMapList<string, int>(50, funcionHashString);
    Lista<string>* productos_2 = new Lista<string>();

    // Recorrer las ventas y procesar
    for (int i = 0; i < ventas.getTamanio(); i++) {
        Venta v = ventas.getDato(i);

        string pais = v.getpais(); normalizar(pais); //(1) y (2)
        string ciudad = v.getciudad(); normalizar(ciudad);
        string producto = v.getproducto(); normalizar(producto); //(2)
        float monto_total = v.getMontoTotal(); //(1) y (2)
        string categoria = v.getcategoria();  normalizar(categoria); //(3) y (4)
        string medio = v.getMedioenvio(); normalizar(medio); //(4)
        medio = v.getMedioenvio(); //(5)
        string fecha = v.getfecha(); //(6)

        // (1) Top 3 ciudades con mayor monto por país
        if (!ventas_ciudad_pais.contieneClave(pais)) {
            ventas_ciudad_pais.put(pais, new HashMapList<string, float>(50, funcionHashString));
            ciudadesPorPais.put(pais, Lista<string>());
            paises.insertarUltimo(pais);
        }
        HashMapList<string, float>* mapa_ciudades = ventas_ciudad_pais.get(pais);
        Lista<string>& lista_ciudades = ciudadesPorPais.get(pais);
        if (mapa_ciudades->contieneClave(ciudad)) {
            float actual = mapa_ciudades->get(ciudad);
            mapa_ciudades->put(ciudad, actual + monto_total);
        } else {
            mapa_ciudades->put(ciudad, monto_total);
            lista_ciudades.insertarUltimo(ciudad);
        }

        // (2) Monto por producto y país
        if (!ventas_producto_pais.contieneClave(pais)) {
            ventas_producto_pais.put(pais, new HashMapList<string, float>(50, funcionHashString));
            productosPorPais.put(pais, Lista<string>());
        }
        HashMapList<string, float>* mapa_productos = ventas_producto_pais.get(pais);
        Lista<string>& lista_productos = productosPorPais.get(pais);
        if (mapa_productos->contieneClave(producto)) {
            float actual = mapa_productos->get(producto);
            mapa_productos->put(producto, actual + monto_total);
        } else {
            mapa_productos->put(producto, monto_total);
            lista_productos.insertarUltimo(producto);
        }

        // (3) Promedio por categoría
        int posPais = buscarPaisBinarioCategoria(lista_pais_categorias, pais);
        if (posPais != -1) {
            pais_categorias pc = lista_pais_categorias.getDato(posPais);
            int posCat = buscarCategoriaBinario(pc.categorias, categoria);
            if (posCat != -1) {
                categoria_monto cm = pc.categorias.getDato(posCat);
                cm.monto_total += monto_total;
                cm.cantidad_ventas++;
                pc.categorias.reemplazar(posCat, cm);
            } else {
                categoria_monto nuevo_cm{categoria, monto_total, 1};
                insertarOrdenadoCategoria(pc.categorias, nuevo_cm);
            }
            lista_pais_categorias.reemplazar(posPais, pc);
        } else {
            pais_categorias nuevo_pc;
            nuevo_pc.pais = pais;
            categoria_monto nuevo_cm{categoria, monto_total, 1};
            insertarOrdenadoCategoria(nuevo_pc.categorias, nuevo_cm);
            insertarOrdenadoPaisCategoria(lista_pais_categorias, nuevo_pc);
        }

        // (4) Medio de envío más usado por país
        acumular_envios_por_pais(arbol_envios, v);

        // (5) Medio de envío más usado por categoría
        agregarMedioEnvioPorCategoria(arbol_categorias_envio, v);

        // (6) Día con más ventas
        float monto_actual = 0;
        try {
            monto_actual = ventasPorDia->get(fecha);
        } catch (...) {
            fechasUnicas->insertarUltimo(fecha);
        }
        ventasPorDia->put(fecha, monto_actual + monto_total);

        // (7) Estado de envío más frecuente por país
        acumularEstadosPorPais(v, *estadosPorPais, *estadosPorPais_claves, paises_estado);

        // (8) Producto más y menos vendido por cantidad
        acumularCantidadPorProducto(v, cantidadPorProducto, productos_2);
    }

    // Mostrar los resultados
    top3_ciudades_hash(ventas_ciudad_pais, paises, ciudadesPorPais);
    mostrar_monto_por_pais_producto(ventas_producto_pais, paises, productosPorPais);
    mostrar_promedio_por_categoria(lista_pais_categorias);
    mostrar_medio_envio_mas_usado(arbol_envios);
    mostrarMedioMasUsadoPorCategoria(arbol_categorias_envio);
    mostrarDiaConMasVentas(ventasPorDia, fechasUnicas);
    mostrarEstadoMasFrecuentePorPais(*estadosPorPais, *estadosPorPais_claves, paises_estado);
    mostrarProductoMasYMenosVendido(cantidadPorProducto, productos_2);
    mostrarRankingProductos(cantidadPorProducto, productos_2);

    // Liberar memoria dinámica
    delete ventasPorDia;
    delete fechasUnicas;
    delete estadosPorPais;
    delete estadosPorPais_claves;
    delete paises_estado;
    delete cantidadPorProducto;
    delete productos_2;

    cout << "\nFin del reprocesamiento.\n";
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//FUNCION PARA CARGAR VENTAS DESDE EL CSV
void cargar_ventas(ListaDoble<Venta> &ventas, const string &nombre_archivo = NOMBRE_ARCHIVO){ //funcion que recibe como parametro la lista que guardara las ventas y el archivo de donde se van a leer los datos
    cout<<"Ruta del archivo: "<<nombre_archivo<<endl;

    //verifica si el archivo existe fisicamente
    if (!std::filesystem::exists(nombre_archivo)) {
        cout << "El archivo NO existe fisicamente en esa ruta." << endl;
    }else {
        cout << "El archivo SI existe en esa ruta." << endl;
    }

    ifstream archivo_ventas(nombre_archivo); //crea el objeto archivo_ventas que abre el archivo
    if(!archivo_ventas.is_open()){ //verifica si se abrio
        cout<<"Error al abrir el archivo."<<endl;
        return;
    }

    string linea; //almacena c/linea del archivo
    char delimitador= ','; //separa los campos en el archivo
    getline(archivo_ventas >> ws, linea); //lee los encabezados del archivo

     while (getline(archivo_ventas >> ws, linea)) { //mientras haya lineas por leer
        try{
            stringstream stream(linea); //trata la linea como un flujo
            string id_venta, fecha, pais, ciudad, cliente, producto, categoria, cantidad_str, precio_unitario_str, monto_total_str, medio_envio, estado_envio;

            //extrae los campos separados por coma
            getline(stream, id_venta, delimitador);
            getline(stream, fecha, delimitador);
            getline(stream, pais, delimitador);
            getline(stream, ciudad, delimitador);
            getline(stream, cliente, delimitador);
            getline(stream, producto, delimitador);
            getline(stream, categoria, delimitador);
            getline(stream, cantidad_str, delimitador);
            getline(stream, precio_unitario_str, delimitador);
            getline(stream, monto_total_str, delimitador);
            getline(stream, medio_envio, delimitador);
            getline(stream, estado_envio, delimitador); //se extraen los campos de la linea y los separa por el delimitador, los almacena en las variables que corresponde.

            //normalizar
            normalizar(pais);
            normalizar(ciudad);
            normalizar(cliente);
            normalizar(producto);
            normalizar(categoria);
            normalizar(medio_envio);
            normalizar(estado_envio);

            if (medio_envio.empty() || estado_envio.empty()) {
                throw runtime_error("Campos vacios detectados");
            }

            // Convertir strings numéricos
            int id= stoi(id_venta);
            int cantidad=stoi(cantidad_str); //convierte a int
            double precio_unitario=stof(precio_unitario_str); //convierte a float
            double monto_total=stof(monto_total_str);

            if (!validarFormatoFecha(fecha)) {
                throw runtime_error("Formato de fecha invalido en archivo CSV: " + fecha);
            }

            // Crear objeto Venta y agregar a la lista
            Venta v(id, fecha,pais, ciudad, cliente, producto, categoria, cantidad, medio_envio, estado_envio, precio_unitario, monto_total);
            ventas.insertarUltimo(v);

            }catch (exception &e) {
            cout << "Linea con error: " << linea << endl;
            cout<< "Error: "<<e.what()<<endl;
        }
    }
    archivo_ventas.close();

    cout << "Total de ventas cargadas: " << ventas.getTamanio() << endl;

    // Reprocesar todo una vez que se cargaron las ventas
    procesarTodo(ventas);           
}

void guardarVentasEnCSV(ListaDoble<Venta>& ventas, const string& nombre_archivo = NOMBRE_ARCHIVO) {
    ofstream archivo(nombre_archivo);
    if (!archivo.is_open()) {
        cout << "Error al abrir el archivo para guardar ventas.\n";
        return;
    }

    // Escribir encabezados
    archivo << "ID,Fecha,Pais,Ciudad,Cliente,Producto,Categoria,Cantidad,Precio_Unitario,Monto_Total,Medio_Envio,Estado_Envio\n";

    // Escribir cada venta
    for (int i = 0; i < ventas.getTamanio(); i++) {
        Venta v = ventas.getDato(i);
        archivo << v.getIDventa() << ","
                << v.getfecha() << ","
                << v.getpais() << ","
                << v.getciudad() << ","
                << v.getcliente() << ","
                << v.getproducto() << ","
                << v.getcategoria() << ","
                << v.getcantidad() << ","
                << v.getPrecioUnit() << ","
                << v.getMontoTotal() << ","
                << v.getMedioenvio() << ","
                << v.getEstadoEnvio() << "\n";
    }

    archivo.close();
    cout << "Archivo CSV actualizado correctamente.\n";
}

//------------------------------------------------------------------------------------------
//AGREGAR VENTA
void agregarVenta(ListaDoble<Venta>& ventas) {
    try {
        int id, cantidad;
        float precio_unitario;
        string fecha, pais, ciudad, cliente, producto, categoria, medio_envio, estado_envio;

        cout << "\n--- Agregar nueva venta ---\n";

        cout << "ID: ";
        cin >> id;
        cin.ignore();

        cout << "Fecha (dd/mm/aaaa): ";
        getline(cin, fecha);
         if (fecha.length() != 10 || fecha[2] != '/' || fecha[5] != '/') {
            throw runtime_error("Formato de fecha invalido. Debe ser dd/mm/aaaa.");
        }
        int primer_valor = stoi(fecha.substr(0, 2));
        if (primer_valor > 1900) {
            throw runtime_error("Fecha ingresada en orden incorrecto (parece aaaa/mm/dd). Debe ser dd/mm/aaaa.");
        }

        cout << "Pais: ";
        getline(cin, pais);

        cout << "Ciudad: ";
        getline(cin, ciudad);

        cout << "Cliente: ";
        getline(cin, cliente);
        
        cout << "Producto: ";
        getline(cin, producto);

        cout << "Categoria: ";
        getline(cin, categoria);

        cout << "Cantidad: ";
        cin >> cantidad;

        cout << "Precio unitario: ";
        cin >> precio_unitario;
        cin.ignore();

        cout << "Medio de envio: ";
        getline(cin, medio_envio);

        cout << "Estado de envio: ";
        getline(cin, estado_envio);

         // Normalización
        normalizar(pais);
        normalizar(ciudad);
        normalizar(cliente);
        normalizar(producto);
        normalizar(categoria);
        normalizar(medio_envio);
        normalizar(estado_envio);

        if (medio_envio.empty() || estado_envio.empty()) {
            throw runtime_error("Error: Medio o estado de envio no puede estar vacio.");
        }

        float monto_total = cantidad * precio_unitario;

        Venta nueva(id, fecha, pais, ciudad, cliente, producto, categoria,
                    cantidad, medio_envio, estado_envio, precio_unitario, monto_total);

        ventas.insertarUltimo(nueva);

        cout << "\nVenta agregada con exito.\n";
        guardarVentasEnCSV(ventas);


        procesarTodo(ventas); // Recalcula todas las estadísticas
    }
    catch (exception& e) {
        cout << "Error al agregar la venta: " << e.what() << endl;
    }
}

//--------------------------------------------------------------------------------
//ELIMINAR VENTA
void eliminarVenta(ListaDoble<Venta>& ventas, stack<Venta>& pilaDeshacer) {
    try{
    string filtro;
    cout << "\n--- Eliminar venta ---\n";
    cout << "Ingrese pais o ciudad para filtrar ventas a eliminar: ";
    getline(cin, filtro);
    normalizar(filtro);

    Lista<int> posiciones; // Guarda las posiciones reales en la lista de ventas

    // Mostrar coincidencias
    for (int i = 0; i < ventas.getTamanio(); i++) {
        Venta v = ventas.getDato(i);
        if (v.getpais() == filtro || v.getciudad() == filtro) {
            cout << "[" << posiciones.getTamanio() << "] "
                 << "ID: " << v.getIDventa()
                 << " | " << v.getproducto()
                 << " | $" << v.getMontoTotal()
                 << " | " << v.getfecha() << endl;
            posiciones.insertarUltimo(i);
        }
    }

    if (posiciones.esVacia()) {
         throw runtime_error("No hay ventas para ese filtro.");
    }

    int seleccion;
    cout << "Ingrese el numero de la venta a eliminar (0 a " << posiciones.getTamanio() - 1 << "): ";
    cin >> seleccion;
     if (cin.fail()) throw invalid_argument("Entrada no numerica.");
    cin.ignore();

    if (seleccion >= 0 && seleccion < posiciones.getTamanio()) {
        int posReal = posiciones.getDato(seleccion);
        Venta eliminada=ventas.getDato(posReal);
        ventas.remover(posReal);
        pilaDeshacer.push(eliminada); // Guardar en pila para deshacer
        cout << "Venta eliminada con exito.\n";
        guardarVentasEnCSV(ventas);
        procesarTodo(ventas); // Reprocesar luego de eliminar
    } else {
         throw out_of_range("Seleccion fuera de rango.");
    }

    } catch (const exception& e) {
        cout << "Error al eliminar venta: " << e.what() << endl;
    }
}

void deshacerEliminacion(ListaDoble<Venta>& ventas, stack<Venta>& pilaDeshacer) {
    if (pilaDeshacer.empty()) {
        cout << "No hay eliminaciones para deshacer.\n";
        return;
    }

    Venta recuperada = pilaDeshacer.top();
    pilaDeshacer.pop();
    ventas.insertarUltimo(recuperada);
    cout << "Venta con ID " << recuperada.getIDventa() << " restaurada exitosamente.\n";
}

//------------------------------------------------------------------------
//MODIFICAR VENTA
void modificarVenta(ListaDoble<Venta>& ventas) {
    try {
        int id;
        cout << "\n--- Modificar venta ---\n";
        cout << "Ingrese ID de venta a modificar: ";
        cin >> id;
         if (cin.fail()) throw runtime_error("ID invalido.");
        cin.ignore();

        for (int i = 0; i < ventas.getTamanio(); i++) {
            Venta v = ventas.getDato(i);
            if (v.getIDventa() == id) {
                string campo;
                cout << "Campo a modificar (pais, ciudad, producto, categoria, cantidad, precio_unitario): ";
                getline(cin, campo);

                if (campo == "pais") {
                    string nuevo;
                    cout << "Nuevo pais: ";
                    getline(cin, nuevo);
                    if (nuevo.empty()) throw runtime_error("El valor no puede estar vacio.");
                    normalizar(nuevo);
                    v.setpais(nuevo);

                } else if (campo == "ciudad") {
                    string nuevo;
                    cout << "Nueva ciudad: ";
                    getline(cin, nuevo);
                    if (nuevo.empty()) throw runtime_error("El valor no puede estar vacio.");
                    normalizar(nuevo);
                    v.setciudad(nuevo);

                } else if (campo == "producto") {
                    string nuevo;
                    cout << "Nuevo producto: ";
                    getline(cin, nuevo);
                    if (nuevo.empty()) throw runtime_error("El valor no puede estar vacio.");
                    normalizar(nuevo);
                    v.setproducto(nuevo);

                } else if (campo == "categoria") {
                    string nuevo;
                    cout << "Nueva categoria: ";
                    getline(cin, nuevo);
                    if (nuevo.empty()) throw runtime_error("El valor no puede estar vacio.");
                    normalizar(nuevo);
                    v.setcategoria(nuevo);

                } else if (campo == "cantidad") {
                    int nuevo;
                    cout << "Nueva cantidad: ";
                    cin >> nuevo;
                     if (cin.fail() || nuevo < 0) throw runtime_error("Cantidad invalida.");
                    cin.ignore();
                    v.setcantidad(nuevo);
                    v.setMontoTotal(nuevo * v.getPrecioUnit());

                } else if (campo == "precio_unitario") {
                    float nuevo;
                    cout << "Nuevo precio unitario: ";
                    cin >> nuevo;
                      if (cin.fail() || nuevo <= 0) throw runtime_error("Precio unitario invalido.");
                    cin.ignore();
                    v.setPrecioUnit(nuevo);
                    v.setMontoTotal(v.getcantidad() * nuevo);

                } else {
                     throw runtime_error("Campo no valido.");
                }

                ventas.reemplazar(i, v);
                cout << "Venta modificada correctamente.\n";
                guardarVentasEnCSV(ventas);
                procesarTodo(ventas);
                return;
            }
        }

        cout << "No se encontro una venta con ese ID.\n";
    }
    catch (exception& e) {
        cout << "Error al modificar la venta: " << e.what() << endl;
    }
}

// Consultas dinámicas solicitadas por el usuario. (1) Listado de ventas realizadas en una ciudad especifica.
void mostrarVentasPorCiudad(const ListaDoble<Venta>& ventas) {
    try{
    auto inicio = high_resolution_clock::now();
    int contador_if = 0;

    string ciudad;
    cout << "\n--- Consultar ventas por ciudad ---\n";
    cout << "Ingrese la ciudad: ";
    getline(cin, ciudad);
    normalizar(ciudad);

     // Validar que la ciudad no esté vacía
        if (ciudad.empty()) {
            throw invalid_argument("La ciudad no puede estar vacía.");
        }
        if (ventas.getTamanio() == 0) {
            cout << "No hay ventas registradas.\n";
            return;
        }

    cout << "\nVentas realizadas en " << ciudad << ":\n";
    bool hayResultados = false;

    for (int i = 0; i < ventas.getTamanio(); i++) {
        Venta v = ventas.getDato(i);
        contador_if++;

        string ciudadVenta = v.getciudad();
        normalizar(ciudadVenta);
        if (ciudadVenta == ciudad) {
            cout << "ID: " << v.getIDventa()
                 << " | Fecha: " << v.getfecha()
                 << " | Producto: " << v.getproducto()
                 << " | Cliente: " << v.getcliente()
                 << " | Monto: $" << v.getMontoTotal() << endl;
            hayResultados = true;
        }
    }

    if (!hayResultados) {
        cout << " No se encontraron ventas en esa ciudad.\n";
    }

    auto fin = high_resolution_clock::now();
    auto duracion = duration_cast<milliseconds>(fin - inicio);

    cout << "\nProceso: Ventas por ciudad especifica\n";
    cout << "Tiempo de ejecucion: " << duracion.count() << " ms\n";
    cout << "Condicionales 'if' utilizados: " << contador_if << endl;
    cout << "Estructura utilizada: Lista doblemente enlazada\n";

} catch (const invalid_argument& e) {
        cout << "Error de entrada: " << e.what() << endl;
    } catch (const exception& e) {
        cout << "Se produjo un error inesperado: " << e.what() << endl;
    } catch (...) {
        cout << "Error desconocido ocurrido.\n";
    }
}

// Consultas dinámicas solicitadas por el usuario. (2) Listado de ventas realizadas en un rango de fechas por pais.

// Función principal con manejo de excepciones
void mostrarVentasPorPaisYRangoFechas(const ListaDoble<Venta>& ventas) {
    try{
    auto inicioTiempo = high_resolution_clock::now();
    int contador_if = 0;

    string pais, fecha_inicio, fecha_fin;
    cout << "\n--- Consultar ventas por país y rango de fechas ---\n";
    cout << "Ingrese el pais: ";
    getline(cin, pais);
    if (pais.empty()) throw invalid_argument("El pais no puede estar vacío.");
    normalizar(pais);

    cout << "Ingrese la fecha de inicio (formato DD/MM/AAAA): ";
    getline(cin, fecha_inicio);
     if (!validarFormatoFecha(fecha_inicio) || !validarFechaValida(fecha_inicio)){
            throw invalid_argument("Fecha de inicio inválida.");}

    cout << "Ingrese la fecha de fin (formato DD/MM/AAAA): ";
    getline(cin, fecha_fin);
      if (!validarFormatoFecha(fecha_fin) || !validarFechaValida(fecha_fin)){
            throw invalid_argument("Fecha de fin inválida.");}
        if (ventas.getTamanio() == 0) {
            cout << "No hay ventas registradas.\n";
            return;
        }

    cout << "\nVentas en " << pais << " entre " << fecha_inicio << " y " << fecha_fin << ":\n";
    bool hayResultados = false;

    for (int i = 0; i < ventas.getTamanio(); i++) {
        Venta v = ventas.getDato(i);
        contador_if++;

        string paisVenta = v.getpais();
        normalizar(paisVenta);
        if (paisVenta == pais && fechaEnRango(v.getfecha(), fecha_inicio, fecha_fin)) {
            hayResultados = true;
            cout << "\n--------------------------------------\n";
            cout << "ID: " << v.getIDventa()
                 << " | Fecha: " << v.getfecha()
                 << " | Ciudad: " << v.getciudad()
                 << " | Producto: " << v.getproducto()
                 << " | Cliente: " << v.getcliente()
                 << " | Categoria: " << v.getcategoria()
                 << "\nMonto total: $" << v.getMontoTotal() << endl;
        }
    }

    if (!hayResultados) {
        contador_if++;
        cout << "\nNo se encontraron ventas en ese pais y rango de fechas.\n";
    }

    auto finTiempo = high_resolution_clock::now();
    auto duracion = duration_cast<milliseconds>(finTiempo - inicioTiempo);

    cout << "\nProceso: Ventas por pais y rango de fechas\n";
    cout << "Tiempo de ejecucion: " << duracion.count() << " ms\n";
    cout << "Condicionales 'if' utilizados: " << contador_if << endl;
    cout << "Estructura utilizada: Lista doblemente enlazada\n";

}catch (const invalid_argument& e) {
        cout << "Error de entrada: " << e.what() << endl;
    } catch (const exception& e) {
        cout << "Se produjo un error inesperado: " << e.what() << endl;
    } catch (...) {
        cout << "Error desconocido ocurrido.\n";
    }
}

// Consultas dinámicas solicitadas por el usuario. (3) Comparacion entre 2 paises:
// (a) Monto total de ventas
void compararMontosEntrePaises(const ListaDoble<Venta>& ventas, const string& pais1, const string& pais2) {
    float total_pais1 = 0, total_pais2 = 0;

    for (int i = 0; i < ventas.getTamanio(); i++) {
        try{
        Venta v = ventas.getDato(i);
        string paisVenta = v.getpais();
        normalizar(paisVenta);
        if (paisVenta== pais1) total_pais1 += v.getMontoTotal();
        else if (paisVenta== pais2) total_pais2 += v.getMontoTotal();
        }catch (const std::exception& e) {
            cerr << "Error accediendo a venta: " << e.what() << endl;
        }
    }

    cout<<"---------------------------------------"<<endl;
    cout << "\nCOMPARACION DE MONTO TOTAL VENDIDO\n";
    cout << pais1 << ": $" << total_pais1 << endl;
    cout << pais2 << ": $" << total_pais2 << endl;
}

// (b) Producto más vendido por cantidad
void compararProductosMasVendidosEntrePaises(const ListaDoble<Venta>& ventas, const string& pais1, const string& pais2) {
    HashMapList<string, int> productos1(50, funcionHashString), productos2(50, funcionHashString);
    Lista<string> lista_prod1, lista_prod2;

    for (int i = 0; i < ventas.getTamanio(); i++) {
        try{
        Venta v = ventas.getDato(i);
        string pais = v.getpais();
        string producto = v.getproducto();
        normalizar(pais);
        normalizar(producto);

        if (pais == pais1) {
             if (productos1.contieneClave(producto)) {
                int cant = productos1.get(producto);
                productos1.put(producto, cant + v.getcantidad());
            } else {
                productos1.put(producto, v.getcantidad());
                lista_prod1.insertarUltimo(producto);
            }
        }

        if (pais == pais2) {
            if (productos2.contieneClave(producto)){
                int cant = productos2.get(producto);
                productos2.put(producto, cant + v.getcantidad());
            } else{
                productos2.put(producto, v.getcantidad());
                lista_prod2.insertarUltimo(producto);
            }
        }
    }catch (const std::exception& e) {
            cerr << "Error procesando producto: " << e.what() << endl;
        }
    }

    string topProd1 = "", topProd2 = "";
    int max1 = -1, max2 = -1;

    for (int i = 0; i < lista_prod1.getTamanio(); i++) {
        string p = lista_prod1.getDato(i);
        int c = productos1.get(p);
        if (c > max1) { max1 = c; topProd1 = p; }
    }

    for (int i = 0; i < lista_prod2.getTamanio(); i++) {
        string p = lista_prod2.getDato(i);
        int c = productos2.get(p);
        if (c > max2) { max2 = c; topProd2 = p; }
    }

    cout<<"---------------------------------------"<<endl;
    cout << "\nPRODUCTO MAS VENDIDO (POR CANTIDAD)\n";
    cout << pais1 << ": " << (topProd1.empty() ? "No se encontraron productos." : topProd1 + " (" + to_string(max1) + " unidades)") << endl;
    cout << pais2 << ": " << (topProd2.empty() ? "No se encontraron productos." : topProd2 + " (" + to_string(max2) + " unidades)") << endl;
}

// (c) Medio de envío más usado
void compararMediosMasUsadosEntrePaises(const ListaDoble<Venta>& ventas, const string& pais1, const string& pais2) {
    HashMapList<string, int> medios1(50, funcionHashString), medios2(50, funcionHashString);
    Lista<string> lista_medios1, lista_medios2;

    for (int i = 0; i < ventas.getTamanio(); i++) {
        try{
        Venta v = ventas.getDato(i);
        string pais = v.getpais();
        string medio = v.getMedioenvio();
        normalizar(pais);
        normalizar(medio);

        if (pais == pais1) {
            if (medios1.contieneClave(medio)){
                int c = medios1.get(medio);
                medios1.put(medio, c + 1);
            } else {
                medios1.put(medio, 1);
                lista_medios1.insertarUltimo(medio);
            }
        }

        if (pais == pais2) {
            if (medios2.contieneClave(medio)){
                int c = medios2.get(medio);
                medios2.put(medio, c + 1);
            } else {
                medios2.put(medio, 1);
                lista_medios2.insertarUltimo(medio);
            }
        }
    } catch (const std::exception& e) {
            cerr << "Error procesando medio de envio: " << e.what() << endl;
        }
    }

    string medio1 = "", medio2 = "";
    int max1 = -1, max2 = -1;

    for (int i = 0; i < lista_medios1.getTamanio(); i++) {
        string m = lista_medios1.getDato(i);
        int c = medios1.get(m);
        if (c > max1) { max1 = c; medio1 = m; }
    }

    for (int i = 0; i < lista_medios2.getTamanio(); i++) {
        string m = lista_medios2.getDato(i);
        int c = medios2.get(m);
        if (c > max2) { max2 = c; medio2 = m; }
    }

    cout<<"---------------------------------------"<<endl;
    cout << "\nMEDIO DE ENVIO MAS USADO\n";
     cout << pais1 << ": " << (medio1.empty() ? "No se encontraron envios." : medio1 + " (" + to_string(max1) + " veces)") << endl;
    cout << pais2 << ": " << (medio2.empty() ? "No se encontraron envios." : medio2 + " (" + to_string(max2) + " veces)") << endl;
}

// Función principal de comparacion entre paises
void compararDosPaises(const ListaDoble<Venta>& ventas) {
    auto inicio = high_resolution_clock::now();

      if (ventas.esVacia()) {
        cout << "No hay ventas registradas para comparar.\n";
        return;
    }

    string pais1, pais2;

    cout<<"---------------------------------------"<<endl;
    cout << "\n--- Comparacion entre dos países ---\n";
    cout << "Ingrese el primer pais: ";
    getline(cin, pais1);
    normalizar(pais1);
    cout << "Ingrese el segundo pais: ";
    getline(cin, pais2);
    normalizar(pais2);

     if (pais1.empty() || pais2.empty()) {
        cout << "Error: Debe ingresar ambos nombres de paises.\n";
        return;
    }

    cout << "\nComparando: " << pais1 << " vs. " << pais2 << "\n";

    compararMontosEntrePaises(ventas, pais1, pais2);
    compararProductosMasVendidosEntrePaises(ventas, pais1, pais2);
    compararMediosMasUsadosEntrePaises(ventas, pais1, pais2);

    auto fin = high_resolution_clock::now();
    auto duracion = duration_cast<milliseconds>(fin - inicio);

    cout << "\nProceso: Comparacion entre dos paises\n";
    cout << "Tiempo de ejecucion: " << duracion.count() << " ms\n";
    cout << "Estructura utilizada: HashMapList + Lista\n";
}

// Consultas dinámicas solicitadas por el usuario. (4) Comparacion entre 2 productos p/ pais:
// (a) cantidad total vendida y (b) monto total
void compararDosProductos(const ListaDoble<Venta>& ventas) {
    auto inicio = high_resolution_clock::now();
    int contador_if = 0;

    if (ventas.esVacia()) {
        cout << "No hay ventas registradas para comparar.\n";
        return;
    }

    string prod1, prod2;

    cout<<"---------------------------------------"<<endl;
    cout << "\n--- Comparacion entre dos productos ---\n";
    cout << "Ingrese el primer producto: ";
    getline(cin, prod1);
    normalizar(prod1);
    cout << "Ingrese el segundo producto: ";
    getline(cin, prod2);
    normalizar(prod2);

      if (prod1.empty() || prod2.empty()) {
        cout << "Error: Debe ingresar ambos nombres de productos.\n";
        return;
    }

    int cant1 = 0, cant2 = 0;
    float monto1 = 0, monto2 = 0;

    for (int i = 0; i < ventas.getTamanio(); i++) {
        try{
        Venta v = ventas.getDato(i);
        string producto = v.getproducto();
        normalizar(producto);

        if (producto == prod1) {
            contador_if++;
            cant1 += v.getcantidad();
            monto1 += v.getMontoTotal();
        } else if (producto == prod2) {
            contador_if++;
            cant2 += v.getcantidad();
            monto2 += v.getMontoTotal();
        }
    } catch (const std::exception& e) {
            cerr << "Error accediendo a una venta: " << e.what() << endl;
        }
    }

     if (cant1 == 0 && cant2 == 0) {
        cout << "\nNo se encontraron ventas de los productos ingresados.\n";
        return;
    }

    cout << "\nCOMPARACION DE PRODUCTOS\n";
    cout << prod1 << " → " << cant1 << " unidades, $" << monto1 << endl;
    cout << prod2 << " → " << cant2 << " unidades, $" << monto2 << endl;

    auto fin = high_resolution_clock::now();
    auto duracion = duration_cast<milliseconds>(fin - inicio);

    cout << "\n Proceso: Comparacion entre dos productos\n";
    cout << "Tiempo de ejecucion: " << duracion.count() << " ms\n";
    cout << "Condicionales 'if' utilizados: " << contador_if << "\n";
    cout << "Estructura utilizada: Lista doblemente enlazada\n";
}

//(5) Productos con promedio menor a un umbral por país
struct producto_acumulado {
    float monto_total = 0;
    int cantidad_total = 0;

    producto_acumulado() : monto_total(0), cantidad_total(0) {}
};

void productosPromedioMenorPorPais(const ListaDoble<Venta>& ventas) {
    auto inicio = high_resolution_clock::now();
    int contador_if = 0;

    if (ventas.esVacia()) {
        cout << "No hay ventas registradas para procesar.\n";
        return;
    }

    string pais;
    float umbral;
    
    cout<<"---------------------------------------"<<endl;
    cout << "\n--- Productos con promedio por unidad MENOR a un umbral ---\n";
    cout << "Ingrese el pais: ";
    getline(cin, pais);
    normalizar(pais);
    cout << "Ingrese el umbral de promedio por unidad (ej: 250): ";
    cin >> umbral;
    cin.ignore();

    if (pais.empty()) {
        cout << "Error: Debe ingresar un nombre de pais.\n";
        return;
    }
    if (umbral < 0) {
        cout << "Error: El umbral debe ser un valor positivo.\n";
        return;
    }

    HashMapList<string, producto_acumulado> productos(50, funcionHashString);
    Lista<string> nombres;
    int ventasEnPais = 0;

    for (int i = 0; i < ventas.getTamanio(); i++) {
        Venta v = ventas.getDato(i);
        string paisVenta = v.getpais();
        normalizar(paisVenta);

        if (paisVenta == pais) {
            ventasEnPais++;
            contador_if++;
            string nombre = v.getproducto();
            producto_acumulado acum;

            try {
                acum = productos.get(nombre);
            } catch (...) {
                nombres.insertarUltimo(nombre);
            }

            acum.cantidad_total += v.getcantidad();
            acum.monto_total += v.getMontoTotal();
            productos.put(nombre, acum);
        }
    }

      if (ventasEnPais == 0) {
        cout << "No hay ventas registradas para ese pais.\n";
        return;
    }

    cout << "\nProductos con promedio < $" << umbral << " en " << pais << ":\n";
    bool hayResultados = false;

    for (int i = 0; i < nombres.getTamanio(); i++) {
        string nombre = nombres.getDato(i);
        producto_acumulado acum = productos.get(nombre);

        if (acum.cantidad_total > 0) {
            contador_if++;
            float promedio = acum.monto_total / acum.cantidad_total;
            if (promedio < umbral) {
                contador_if++;
                cout << "- " << nombre << " ($" << promedio << " por unidad)\n";
                hayResultados = true;
            }
        }
    }

    if (!hayResultados) {
        contador_if++;
        cout << "No se encontraron productos bajo ese umbral.\n";
    }

    auto fin = high_resolution_clock::now();
    auto duracion = duration_cast<milliseconds>(fin - inicio);

    cout << "\nProceso: Productos con promedio menor por pais\n";
    cout << "Tiempo de ejecucion: " << duracion.count() << " ms\n";
    cout << "Condicionales 'if' utilizados: " << contador_if << "\n";
    cout << "Estructura utilizada: HashMapList + Lista\n";
}

//(6) Productos con promedio mayor a un umbral por país
void productosPromedioMayorPorPais(const ListaDoble<Venta>& ventas) {
    auto inicio = high_resolution_clock::now();
    int contador_if = 0;

     if (ventas.esVacia()) {
        cout << "No hay ventas registradas para procesar.\n";
        return;
    }

    string pais;
    float umbral;

    cout<<"---------------------------------------"<<endl;
    cout << "\n--- Productos con promedio por unidad MAYOR a un umbral ---\n";
    cout << "Ingrese el pais: ";
    getline(cin, pais);
    normalizar(pais);
    cout << "Ingrese el umbral de promedio por unidad (ej: 500): ";
    cin >> umbral;
    cin.ignore();

      if (pais.empty()) {
        cout << "Error: Debe ingresar un nombre de país.\n";
        return;
    }

      if (umbral < 0) {
        cout << "Error: El umbral debe ser un valor positivo.\n";
        return;
    }


    HashMapList<string, producto_acumulado> productos(50, funcionHashString);
    Lista<string> nombres;
    int ventasEnPais = 0;


    for (int i = 0; i < ventas.getTamanio(); i++) {
        Venta v = ventas.getDato(i);
        string paisVenta = v.getpais();
        normalizar(paisVenta);

        if (paisVenta == pais) {
            ventasEnPais++;
            contador_if++;
            string nombre = v.getproducto();
            producto_acumulado acum;

            try {
                acum = productos.get(nombre);
            } catch (...) {
                nombres.insertarUltimo(nombre);
            }

            acum.cantidad_total += v.getcantidad();
            acum.monto_total += v.getMontoTotal();
            productos.put(nombre, acum);
        }
    }

     if (ventasEnPais == 0) {
        cout << "No hay ventas registradas para ese pais.\n";
        return;
    }


    cout << "\nProductos con promedio > $" << umbral << " en " << pais << ":\n";
    bool hayResultados = false;

    for (int i = 0; i < nombres.getTamanio(); i++) {
        string nombre = nombres.getDato(i);
        producto_acumulado acum = productos.get(nombre);

        if (acum.cantidad_total > 0) {
            contador_if++;
            float promedio = acum.monto_total / acum.cantidad_total;
            if (promedio > umbral) {
                contador_if++;
                cout << "- " << nombre << " ($" << promedio << " por unidad)\n";
                hayResultados = true;
            }
        }
    }

    if (!hayResultados) {
        contador_if++;
        cout << "No se encontraron productos sobre ese umbral.\n";
    }

    auto fin = high_resolution_clock::now();
    auto duracion = duration_cast<milliseconds>(fin - inicio);

    cout << "\nProceso: Productos con promedio mayor por pais\n";
    cout << "Tiempo de ejecucion: " << duracion.count() << " ms\n";
    cout << "Condicionales 'if' utilizados: " << contador_if << "\n";
    cout << "Estructura utilizada: HashMapList + Lista\n";
}

void start(ListaDoble<Venta>& ventas) {
    cout << "==============================================\n";
    cout << "  SISTEMA DE ANALISIS DE VENTAS SUDAMERICANO\n";
    cout << "==============================================\n";
    cout << "  Bienvenido/a!\n";
    cout << "  Por favor, consulte el MANUAL DE USUARIO\n";
    cout << "  para conocer el funcionamiento de cada opcion.\n";
    cout << "==============================================\n";

    int opcion;
    stack<Venta> pilaDeshacer; // Pila para deshacer eliminaciones

    do {
        cout << "\n===== MENU DE OPCIONES =====" << endl;
        cout << "1. Cargar archivo de ventas" << endl;
        cout << "2. Agregar venta" << endl;
        cout << "3. Eliminar venta por ciudad o pais" << endl;
        cout << "4. Modificar venta por ID" << endl;
        cout << "5. Mostrar ventas en una ciudad especifica" << endl;
        cout << "6. Mostrar ventas en un rango de fechas por pais" << endl;
        cout << "7. Comparar dos paises" << endl;
        cout << "8. Comparar dos productos (cantidad y monto total)" << endl;
        cout << "9. Buscar productos con promedio MENOR a umbral en un pais" << endl;
        cout << "10. Buscar productos con promedio MAYOR a umbral en un pais" << endl;
        cout << "11. Deshacer ultima eliminacion\n";
        cout << "0. Salir" << endl;
        cout << "Seleccione una opcion: ";
        cin >> opcion;
        cin.ignore(); // Para limpiar el salto de línea

        switch (opcion) {
            case 1:
                cargar_ventas(ventas, NOMBRE_ARCHIVO);
                cout << "Se cargaron " << ventas.getTamanio() << " ventas del archivo.\n";
                break;
            case 2:
                agregarVenta(ventas);
                break;
            case 3:
                if (ventas.getTamanio() == 0) {
                    cout << "Debe cargar ventas o agregar al menos una primero.\n";
                    break;
                }
                eliminarVenta(ventas, pilaDeshacer);
                break;
            case 4:
                if (ventas.getTamanio() == 0) {
                    cout << "Debe cargar ventas o agregar al menos una primero.\n";
                    break;
                }
                modificarVenta(ventas);
                break;
            case 5:
                if (ventas.getTamanio() == 0) {
                    cout << "Debe cargar ventas o agregar al menos una primero.\n";
                    break;
                }
                mostrarVentasPorCiudad(ventas);
                break;
            case 6:
                if (ventas.getTamanio() == 0) {
                    cout << "Debe cargar ventas o agregar al menos una primero.\n";
                    break;
                }
                mostrarVentasPorPaisYRangoFechas(ventas);
                break;
            case 7:
                if (ventas.getTamanio() == 0) {
                    cout << "Debe cargar ventas o agregar al menos una primero.\n";
                    break;
                }
                compararDosPaises(ventas);
                break;
            case 8:
                if (ventas.getTamanio() == 0) {
                    cout << "Debe cargar ventas o agregar al menos una primero.\n";
                    break;
                }
                compararDosProductos(ventas);
                break;
            case 9:
                if (ventas.getTamanio() == 0) {
                    cout << "Debe cargar ventas o agregar al menos una primero.\n";
                    break;
                }
                productosPromedioMenorPorPais(ventas);
                break;
            case 10:
                if (ventas.getTamanio() == 0) {
                    cout << "Debe cargar ventas o agregar al menos una primero.\n";
                    break;
                }
                productosPromedioMayorPorPais(ventas);
                break;
            case 11:
            deshacerEliminacion(ventas, pilaDeshacer);
            break;
            case 0:
                cout << "Saliendo del programa. ¡Hasta luego!" << endl;
                break;
            default:
                cout << "Opcion invalida. Intente nuevamente." << endl;
        }
    } while (opcion != 0);
}


int main(){
    ListaDoble<Venta> ventas; //lista para almacenar las ventas
    start(ventas);
    return 0;
}