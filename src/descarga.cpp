
#include "../include/descarga.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <curl/curl.h>

using namespace std;

// Leer las URLs desde un archivo TXT
vector<string> leerURLs(const string &nombreArchivo)
{
    vector<string> urls;
    ifstream archivo(nombreArchivo);
    string linea;

    if (!archivo.is_open())
    {
        throw runtime_error("No se pudo abrir: " + nombreArchivo);
    }

    while (getline(archivo, linea))
    {
        if (!linea.empty() && linea.back() == '\r')
        {
            linea.pop_back();
        }

        if (!linea.empty() && linea[0] != '#')
        {
            urls.push_back(linea);
        }
    }

    archivo.close();

    return urls;
}

// Recibir los bloques de datos de libcurl
size_t escribirDatos(char *datos, size_t size,
                     size_t nmemb, void *destino)
{
    size_t bytes = size * nmemb;

    string *texto = static_cast<string *>(destino);
    texto->append(datos, bytes);

    return bytes;
}

// Descargar el contenido de un libro
string descargarLibro(const string &url)
{
    string texto;

    CURL *curl = curl_easy_init();

    if (!curl)
    {
        throw runtime_error("No se pudo inicializar curl");
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, escribirDatos);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &texto);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);

    CURLcode resultado = curl_easy_perform(curl);

    curl_easy_cleanup(curl);

    if (resultado != CURLE_OK)
    {
        throw runtime_error(
            "Error descargando " + url + ": " +
            curl_easy_strerror(resultado));
    }

    return texto;
}