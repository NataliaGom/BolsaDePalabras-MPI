#include "../include/comunicacion.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>

using namespace std;

// Convertir los resultados locales en un string
string serializar(const vector<ResultadoLibro> &resultados)
{
    ostringstream datos;

    // Cantidad de libros procesados
    datos << resultados.size() << '\n';

    for (const auto &libro : resultados)
    {
        // Identificador y titulo
        datos << libro.id << '\n';
        datos << quoted(libro.titulo) << '\n';

        // Cantidad de palabras diferentes
        datos << libro.frecuencias.size() << '\n';

        // Palabras y sus frecuencias
        for (const auto &elemento : libro.frecuencias)
        {
            datos << quoted(elemento.first) << " "
                  << elemento.second << '\n';
        }
    }

    return datos.str();
}

// Reconstruir los resultados a partir del string
vector<ResultadoLibro> deserializar(const string &datos)
{
    istringstream entrada(datos);

    vector<ResultadoLibro> resultados;

    int cantidadLibros;

    if (!(entrada >> cantidadLibros) || cantidadLibros < 0)
    {
        throw runtime_error("Datos de libros invalidos");
    }

    for (int i = 0; i < cantidadLibros; i++)
    {
        ResultadoLibro libro;
        int cantidadPalabras;

        if (!(entrada >> libro.id >> quoted(libro.titulo) >> cantidadPalabras) || cantidadPalabras < 0)
        {
            throw runtime_error("Datos de libro invalidos");
        }

        for (int j = 0; j < cantidadPalabras; j++)
        {
            string palabra;
            int frecuencia;

            if (!(entrada >> quoted(palabra) >> frecuencia))
            {
                throw runtime_error("Frecuencia invalida");
            }

            libro.frecuencias[palabra] = frecuencia;
        }

        resultados.push_back(std::move(libro));
    }

    return resultados;
}