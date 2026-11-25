#include "../include/procesamiento.h"
#include <cctype>

using namespace std;

string obtenerTitulo(const string &texto)
{
    size_t inicio = texto.find("Title:");

    if (inicio == string::npos)
    {
        return "Titulo desconocido";
    }

    inicio += 6;

    size_t fin = texto.find('\n', inicio);

    string titulo = texto.substr(inicio, fin - inicio);

    // Eliminar espacios y caracteres sobrantes
    size_t primero = titulo.find_first_not_of(" \t\r");

    if (primero == string::npos)
    {
        return "Titulo desconocido";
    }

    size_t ultimo = titulo.find_last_not_of(" \t\r");

    return titulo.substr(primero, ultimo - primero + 1);
}

unordered_map<string, int> contarPalabras(const string &texto)
{
    unordered_map<string, int> frecuencias;
    string palabra;

    for (unsigned char caracter : texto)
    {
        if (isalpha(caracter))
        {
            palabra += static_cast<char>(tolower(caracter));
        }
        else
        {
            if (palabra.size() >= 2)
            {
                frecuencias[palabra]++;
            }

            palabra.clear();
        }
    }

    // Contar la última palabra del texto
    if (palabra.size() >= 2)
    {
        frecuencias[palabra]++;
    }

    return frecuencias;
}