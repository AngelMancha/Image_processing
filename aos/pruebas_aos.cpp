//
// Created by navar on 10/19/2022.
//
#include "Image_aos.h"
#include <iostream>
using namespace std;
int main(){
    Image copy(0,0);
    copy.Histograma("bmp_24.bmp","prueba.txt");
    Image copy2(0,0);
    copy.Histograma("elephant.bmp","prueba2.txt");
    Image copy3(0,0);
    copy.Histograma("gr.bmp","prueba3.txt");
    Image copy4(0,0);
    copy.Histograma("landscape.bmp","prueba4.txt");

}
//