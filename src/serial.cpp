#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <curl/curl.h>

#include "../include/descarga.h"
#include "../include/procesamiento.h"
#include "../include/matriz.h"

using namespace std;

int main(int argc, char *argv[])
{
    // Verificar argumentos de entrada
    if (argc != 2)
    {
        cerr << "Uso: serial.exe archivo_urls.txt" << endl;
        return 1;
    }

    string archivoURLs = argv[1];

    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK)
    {
        cerr << "Error inicializando libcurl" << endl;
        return 1;
    }

    try
    {
        // Inicio de la medicion
        auto start = chrono::steady_clock::now();

        // Leer URLs
        vector<string> urls = leerURLs(archivoURLs);

        int k = urls.size();

        if (k == 0)
        {
            throw runtime_error("No hay URLs para procesar");
        }

        vector<unordered_map<string, int>> conteos(k);
        vector<string> titulos(k);

        // Descargar y procesar los libros
        for (int i = 0; i < k; i++)
        {
            string texto = descargarLibro(urls[i]);

            titulos[i] = obtenerTitulo(texto);

            conteos[i] = contarPalabras(texto);
        }

        // Construir vocabulario global
        vector<string> vocabulario =
            construirVocabulario(conteos);

        // Construir matriz
        vector<vector<int>> matriz =
            construirMatriz(conteos, vocabulario);

        // Exportar CSV
        exportarCSV(
            "output/bolsa_serial.csv",
            vocabulario,
            matriz,
            titulos);

        // Fin de la medicion
        auto end = chrono::steady_clock::now();

        double tiempo_serial =
            chrono::duration<double>(end - start).count();

        // Imprimir resultados
        cout << "\n===== VERSION SERIAL =====" << endl;
        cout << "Numero de procesos: 1" << endl;
        cout << "Numero de libros: " << k << endl;
        cout << "Vocabulario global: "
             << vocabulario.size() << endl;

        cout << "Dimensiones: "
             << k << " x " << vocabulario.size() << endl;

        cout << fixed << setprecision(6);
        cout << "Tiempo de ejecucion: "
             << tiempo_serial << " segundos" << endl;

        // Guardar tiempo serial
        ofstream archivoTiempo("output/tiempo_serial.txt");

        if (!archivoTiempo.is_open())
        {
            throw runtime_error("No se pudo guardar el tiempo");
        }

        archivoTiempo << setprecision(17)
                      << tiempo_serial << endl;

        archivoTiempo.close();

        cout << "CSV generado correctamente" << endl;
        cout << "Tiempo serial guardado correctamente" << endl;
    }
    catch (const exception &e)
    {
        cerr << e.what() << endl;
        curl_global_cleanup();
        return 1;
    }

    curl_global_cleanup();

    return 0;
}