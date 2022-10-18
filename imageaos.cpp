#include <iostream>
#include <fstream>
using namespace std;

const int file_header_size = 14; //tamaño cabecera bitmap (bytes 14-17)
const int info_header_size = 40; //desde el byte 14 hasta el 54

struct Image {
    const int width;
    const int height;
    const int colors;
};

struct bitmap_file_header
{
    const char name[2];  //specifies the file type "BM"
    const unsigned int size;  //specifies the size in bytes of the bitmap file
    const int  garbage;  //reserved; must be 0
    const unsigned int image_ffset;  //reserved; must be 0
};

/*
typedef struct Bitmapinfoheader
{
    DWORD biSize;  //specifies the number of bytes required by the struct
    LONG biWidth;  //specifies width in pixels
    LONG biHeight;  //specifies height in pixels
    WORD biPlanes;  //specifies the number of color planes, must be 1
    WORD biBitCount;  //specifies the number of bits per pixel
    DWORD biCompression;  //specifies the type of compression
    DWORD biSizeImage;  //size of image in bytes
    LONG biXPelsPerMeter;  //number of pixels per meter in x axis
    LONG biYPelsPerMeter;  //number of pixels per meter in y axis
    DWORD biClrUsed;  //number of colors used by the bitmap
    DWORD biClrImportant;  //number of colors that are important
}Bitmapinfoheader;
*/

void read_image(const char* path) {
    std::ifstream image;
    // Para poder manipular el archivo primero debemos abrirlo
    image.open(path, ios::in | ios::binary);
    // Comprobamos que el archivo se ha abierto de forma correcta
    if (!image.is_open()) {
        cout << "El fichero no pudo ser abierto" << endl;
        exit(-1);
    }

    //leer desde la estructura file header
    struct bitmap_file_header fileheader;
    fread(fileheader.name, 2, 1, fp);





    char file_header[file_header_size];
    // Leemos el contenido de la imagen y guardamos sus valores en el char file_header
    image.read(file_header, file_header_size);
    // hacemos comprobaciones para ver si tenemos un archivo de tipo bitmap
    if(file_header[0] != 'B' || file_header[1] != 'M'){
        cerr << "El archivo no es de tipo BMP " << endl;
        image.close();
    }
    // Una vez comprobado el archivo de cabecera, guardamos la información en nuestra
    // estructura de bitmap_file_header
    bitmap_file_header pppp;
}
