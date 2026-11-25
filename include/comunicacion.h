#ifndef COMUNICACION_H
#define COMUNICACION_H

#include <string>
#include <vector>
#include <unordered_map>

struct ResultadoLibro
{
    int id;
    std::string titulo;
    std::unordered_map<std::string, int> frecuencias;
};

std::string serializar(
    const std::vector<ResultadoLibro> &resultados);

std::vector<ResultadoLibro> deserializar(
    const std::string &datos);

#endif