#include "Image.h"

int main(){

    //image.Export("original.bmp");
    Image copia(0,0);
    copia.Read("balloon.bmp");
    Image::Copy("balloon.bmp","copia.bmp");


}