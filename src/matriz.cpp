#include "../include/matriz.h"
#include <set>
#include <fstream>
#include <stdexcept>

using namespace std;

// Construir el vocabulario global
vector<string> construirVocabulario(
    const vector<unordered_map<string, int>> &conteos)
{
    set<string> palabras;

    for (const auto &libro : conteos)
    {
        for (const auto &elemento : libro)
        {
            palabras.insert(elemento.first);
        }
    }

    return vector<string>(palabras.begin(), palabras.end());
}

// Construir la matriz de Bolsa de Palabras
vector<vector<int>> construirMatriz(
    const vector<unordered_map<string, int>> &conteos,
    const vector<string> &vocabulario)
{
    int k = conteos.size();
    int n = vocabulario.size();

    vector<vector<int>> matriz(k, vector<int>(n, 0));

    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < n; j++)
        {
            auto elemento = conteos[i].find(vocabulario[j]);

            if (elemento != conteos[i].end())
            {
                matriz[i][j] = elemento->second;
            }
        }
    }

    return matriz;
}

// Exportar la matriz a un archivo CSV
void exportarCSV(
    const string &nombreArchivo,
    const vector<string> &vocabulario,
    const vector<vector<int>> &matriz,
    const vector<string> &titulos)
{
    ofstream archivo(nombreArchivo);

    if (!archivo.is_open())
    {
        throw runtime_error(
            "No se pudo crear: " + nombreArchivo);
    }

    // Escribir encabezados
    archivo << "Libro";

    for (const auto &palabra : vocabulario)
    {
        archivo << "," << palabra;
    }

    archivo << "\n";

    // Escribir las frecuencias
    for (int i = 0; i < matriz.size(); i++)
    {
        archivo << '"';

        for (char c : titulos[i])
        {
            if (c == '"')
                archivo << '"';

            archivo << c;
        }

        archivo << '"';

        for (int frecuencia : matriz[i])
        {
            archivo << "," << frecuencia;
        }

        archivo << "\n";
    }

    archivo.close();
}