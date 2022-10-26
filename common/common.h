//
// Created by xboxa on 26/10/2022.
//

#ifndef ARCOS_PROYECTO1_COMMON_H
#define ARCOS_PROYECTO1_COMMON_H
#include <iostream>
#include <filesystem>
std::vector<std::filesystem::path> getImgPaths(const std::string& path);
void errores(int numargs, std::string_view indir, std::string_view outdir, std::string_view operation);
void tiempo_ejecucion(float tload, float tstore, float top, std::filesystem::path indir, std::filesystem::path outdir, std::string_view op);
#endif //ARCOS_PROYECTO1_COMMON_H
