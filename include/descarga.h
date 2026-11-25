#ifndef DESCARGA_H
#define DESCARGA_H

#include <string>
#include <vector>

std::vector<std::string> leerURLs(const std::string &nombreArchivo);

std::string descargarLibro(const std::string &url);

#endif