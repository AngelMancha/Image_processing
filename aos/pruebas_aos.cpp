//
// Created by navar on 10/19/2022.
//
#include "Image_aos.h"
#include <iostream>
#include <filesystem>
#include <iostream>
#include <vector>
#include <filesystem>
using namespace std;
int main(){
    std::vector<std::filesystem::path> getImgPaths(const std::string& path){
        std::vector<std::filesystem::path> archivosBMP;
        for(const std::filesystem::directory_entry & entrada: std::filesystem::directory_iterator(path))
        {
            if(entrada.path().extension()==".bmp")
            {
                archivosBMP.push_back(entrada.path());
            }
        }
        return (archivosBMP);
    }


    Image copy(0,0);
    copy.GaussianBlur("gr.bmp","prueba.bmp");

}


//