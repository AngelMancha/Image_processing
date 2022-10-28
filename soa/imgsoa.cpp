
#include <iostream>
#include <vector>
#include <filesystem>
#include "Image_soa.h"
#include "../common/common.h"
using namespace std;


int main(int argc, char *argv[]) {
    std::vector<std::filesystem::path> archivosBMP;
    errores(argc, argv[1],argv[2],argv[3]);
    archivosBMP = getImgPaths(argv[1]);
    ImageSoa::funcion(archivosBMP, static_cast<std::string>(argv[2]), static_cast<std::string>(argv[3]));

}