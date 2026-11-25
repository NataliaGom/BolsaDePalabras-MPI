#ifndef PROCESAMIENTO_H
#define PROCESAMIENTO_H

#include <string>
#include <unordered_map>

std::unordered_map<std::string, int> contarPalabras(
    const std::string &texto);

std::string obtenerTitulo(const std::string &texto);

#endif