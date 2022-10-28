//main
#include <iostream>
#include <vector>
#include <filesystem>
#include "../common/common.h"
#include "Image_aos.h"
using namespace std;

int main(int argc, char *argv[]) {
    std::vector<std::filesystem::path> archivosBMP;
    errores(argc, argv[1],argv[2],argv[3]);
    archivosBMP = getImgPaths(argv[1]);
    Image::funcion(archivosBMP, static_cast<std::string>(argv[2]), static_cast<std::string>(argv[3]));

}