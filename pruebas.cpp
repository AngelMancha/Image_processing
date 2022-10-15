#include "Image.h"

int main(){

    Image copy(0,0);
    copy.Read("balloon.bmp");
    copy.Export("copy.bmp");


}