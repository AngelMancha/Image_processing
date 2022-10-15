#include "Image.h"

int main(){

    Image copia(0,0);
    copia.Read("elephant.bmp");
    copia.Export("copia.bmp");


}