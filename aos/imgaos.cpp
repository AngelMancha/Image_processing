
#include <iostream>
#include <vector>
#include <filesystem>
#include "Image_aos.h"
using namespace std;

std::vector<std::filesystem::path> getImgPaths(const std::string& path){
    std::vector<std::filesystem::path> archivosBMP;
    for (const std::filesystem::directory_entry & entrada: std::filesystem::directory_iterator(path))
    {
        if(entrada.path().extension()==".bmp")
        {
            archivosBMP.push_back(entrada.path());
        }
    }
    return archivosBMP;
}

int funcion(std::vector<std::filesystem::path> paths, std::filesystem::path outpath, std::string operation ) {
    for (const auto &path: paths)
    {
        if(operation=="copy"){
            Image Copy(0, 0);
            Copy.Copy(path, outpath);
        }

    }
    return 0;

}
int main(int argc, char *argv[]) {
    //if toda las cosas de errores de si histo y pollas
    std::vector<std::filesystem::path> archivosBMP;
    if(argc!=3){
        cout<<"puta"<<endl;
    }
    archivosBMP = getImgPaths(argv[1]);

    //ahora ejecutar funcion, seguramente tenga que comprobar algo de errores
    funcion(archivosBMP, static_cast<std::string>(argv[2]), static_cast<std::string>(argv[3]));

}