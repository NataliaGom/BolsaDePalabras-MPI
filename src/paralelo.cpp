#include <mpi.h>
#include <curl/curl.h>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <limits>
#include <utility>

#include "../include/descarga.h"
#include "../include/procesamiento.h"
#include "../include/matriz.h"
#include "../include/comunicacion.h"

using namespace std;

int main(int argc, char *argv[])
{
    const int MASTER = 0;
    int num_processes = 0;
    int process_id = 0;

    double start;
    double end;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);

    // Verificar argumentos
    if (argc != 2)
    {
        if (process_id == MASTER)
        {
            cerr << "Uso: paralelo.exe archivo_urls.txt"
                 << endl;
        }

        MPI_Finalize();
        return 1;
    }

    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK)
    {
        cerr << "Error inicializando libcurl" << endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 1;
    }

    try
    {
        vector<string> urls;
        int k = 0;

        // Sincronizar antes de medir
        MPI_Barrier(MPI_COMM_WORLD);

        if (process_id == MASTER)
        {
            start = MPI_Wtime();

            urls = leerURLs(argv[1]);
            k = urls.size();

            if (k == 0)
            {
                throw runtime_error("No hay URLs para procesar");
            }
        }

        // PASO 1: Comunicar numero de libros
        MPI_Bcast(&k, 1, MPI_INT,
                  MASTER, MPI_COMM_WORLD);

        if (process_id != MASTER)
        {
            urls.resize(k);
        }

        // PASO 2: Comunicar las URLs
        for (int i = 0; i < k; i++)
        {
            int longitud = 0;

            if (process_id == MASTER)
            {
                if (urls[i].size() >
                    numeric_limits<int>::max())
                {
                    throw runtime_error("URL demasiado larga");
                }

                longitud = urls[i].size();
            }

            // Comunicar longitud de la URL
            MPI_Bcast(&longitud, 1, MPI_INT,
                      MASTER, MPI_COMM_WORLD);

            if (process_id != MASTER)
            {
                urls[i].resize(longitud);
            }

            // Comunicar contenido de la URL
            MPI_Bcast(urls[i].data(), longitud, MPI_CHAR,
                      MASTER, MPI_COMM_WORLD);
        }

        // PASO 3: Procesamiento paralelo
        vector<ResultadoLibro> resultadosLocales;

        for (int i = process_id; i < k; i += num_processes)
        {
            string texto = descargarLibro(urls[i]);

            ResultadoLibro libro;

            libro.id = i;
            libro.titulo = obtenerTitulo(texto);
            libro.frecuencias = contarPalabras(texto);

            resultadosLocales.push_back(std::move(libro));
        }

        // PASO 4: Reunir resultados en el MASTER
        if (process_id == MASTER)
        {
            vector<unordered_map<string, int>> conteos(k);
            vector<string> titulos(k);

            // Integrar resultados del propio MASTER
            for (auto &libro : resultadosLocales)
            {
                titulos[libro.id] = libro.titulo;
                conteos[libro.id] =
                    std::move(libro.frecuencias);
            }

            // Recibir resultados de los demas procesos
            for (int p = 1; p < num_processes; p++)
            {
                int longitud;

                // Recibir tamano del mensaje
                MPI_Recv(&longitud, 1, MPI_INT,
                         p, 0, MPI_COMM_WORLD,
                         MPI_STATUS_IGNORE);

                string datos(longitud, '\0');

                // Recibir mensaje
                MPI_Recv(datos.data(), longitud, MPI_CHAR,
                         p, 1, MPI_COMM_WORLD,
                         MPI_STATUS_IGNORE);

                // Reconstruir los resultados
                vector<ResultadoLibro> recibidos =
                    deserializar(datos);

                // Integrar en las posiciones originales
                for (auto &libro : recibidos)
                {
                    titulos[libro.id] = libro.titulo;

                    conteos[libro.id] =
                        std::move(libro.frecuencias);
                }
            }

            // PASO 5: Construir vocabulario global
            vector<string> vocabulario =
                construirVocabulario(conteos);

            // PASO 6: Construir matriz global
            vector<vector<int>> matriz =
                construirMatriz(conteos, vocabulario);

            // PASO 7: Generar CSV
            exportarCSV(
                "output/bolsa_paralela.csv",
                vocabulario,
                matriz,
                titulos);

            // Fin de la medicion
            end = MPI_Wtime();

            double tiempo_paralelo = end - start;

            // Imprimir resultados
            cout << "\n===== VERSION PARALELA =====" << endl;

            cout << "Numero de procesos: "
                 << num_processes << endl;

            cout << "Numero de libros: "
                 << k << endl;

            cout << "Vocabulario global: "
                 << vocabulario.size() << endl;

            cout << "Dimensiones: "
                 << k << " x "
                 << vocabulario.size() << endl;

            cout << fixed << setprecision(6);

            cout << "Tiempo paralelo: "
                 << tiempo_paralelo
                 << " segundos" << endl;

            // Guardar tiempo paralelo
            ofstream archivoTiempo(
                "output/tiempo_paralelo.txt");

            if (!archivoTiempo.is_open())
            {
                throw runtime_error(
                    "No se pudo guardar el tiempo paralelo");
            }

            archivoTiempo << setprecision(17)
                          << tiempo_paralelo << endl;

            archivoTiempo.close();

            // Leer tiempo serial y calcular speed-up
            ifstream archivoSerial(
                "output/tiempo_serial.txt");

            double tiempo_serial;

            if ((archivoSerial >> tiempo_serial) && tiempo_serial > 0 && tiempo_paralelo > 0)
            {
                double speedup =
                    tiempo_serial / tiempo_paralelo;

                cout << "Tiempo serial: "
                     << tiempo_serial
                     << " segundos" << endl;

                cout << "Speed-up: "
                     << speedup << "x" << endl;

                if (speedup > 1.2)
                {
                    cout << "Objetivo alcanzado" << endl;
                }
                else
                {
                    cout << "Objetivo no alcanzado" << endl;
                }
            }
            else
            {
                cout << "No hay un tiempo serial valido"
                     << endl;
            }

            cout << "CSV generado correctamente" << endl;
        }
        else
        {
            // Serializar resultados locales
            string datos = serializar(resultadosLocales);

            if (datos.size() > numeric_limits<int>::max())
            {
                throw runtime_error("Mensaje demasiado grande");
            }

            int longitud = datos.size();

            // Enviar tamano del mensaje
            MPI_Send(&longitud, 1, MPI_INT,
                     MASTER, 0, MPI_COMM_WORLD);

            // Enviar resultados
            MPI_Send(datos.data(), longitud, MPI_CHAR,
                     MASTER, 1, MPI_COMM_WORLD);
        }
    }
    catch (const exception &e)
    {
        cerr << "Error en proceso "
             << process_id << ": "
             << e.what() << endl;

        curl_global_cleanup();

        MPI_Abort(MPI_COMM_WORLD, 1);
        return 1;
    }

    curl_global_cleanup();

    MPI_Finalize();

    return 0;
}