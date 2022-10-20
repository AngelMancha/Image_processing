//
// Created by navar on 10/19/2022.
//
#include "Image_aos.h"
#include <iostream>
using namespace std;
int main(){
    cout << "hola" << endl;
    Image copia(0,0);
    copia.GrayScale("elephant.bmp", "prueba3.bmp");
    cout << "hola" << endl;
    return 0;
}
//