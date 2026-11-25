#ifndef MATRIZ_H
#define MATRIZ_H

#include <string>
#include <vector>
#include <unordered_map>

std::vector<std::string> construirVocabulario(
    const std::vector<std::unordered_map<std::string, int>> &conteos);

std::vector<std::vector<int>> construirMatriz(
    const std::vector<std::unordered_map<std::string, int>> &conteos,
    const std::vector<std::string> &vocabulario);

void exportarCSV(
    const std::string &nombreArchivo,
    const std::vector<std::string> &vocabulario,
    const std::vector<std::vector<int>> &matriz,
    const std::vector<std::string> &titulos);

#endif