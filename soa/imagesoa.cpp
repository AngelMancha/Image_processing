#include <iostream>
#include "Image_soa.h"

#include <cmath>
#include <utility>
using namespace std;
/* Variables globales */
const int fileheadersize = 14; //tamaño cabecera bitmap (bytes 14-17)
const int informationheadersize = 40;

Colores::Colores() : m_r(0), m_g(0), m_b(0) {}
Colores::Colores(std::vector<int> m_r, std::vector<int> m_g, std::vector<int> m_b): m_r(std::move(m_r)),m_g(std::move(m_g)),m_b(std::move(m_b)) {}
Colores::~Colores()= default;


Image::Image(int width, int height): m_width(width), m_height(height) {}
Image::~Image() = default;

void Image::Read(const char *path) {
    /* Esta función lee una imagen y comprueba que todos los campos de la cabecera sean correctos y guarda en la clase
     * Image los valores para m_width, m_height y m_colors */

    std::ifstream f;
    openFilein(path, f);

    // Definimos 2 arrays que contienen la cabecera y la información de cabecera y hacemos comprobaciones
    unsigned char fileheader[fileheadersize]; //desde el byte 0 hasta el 14 --> Contiene el byte hasta el tamaño de cabecera de BMP
    unsigned char informationheader[informationheadersize];
    f.read(reinterpret_cast<char*>(fileheader), fileheadersize);
    checkHeader(f, fileheader); // Comprobamos que la cabecera sea correcta llamando a la funcion checkHeader
    f.read(reinterpret_cast<char*>(informationheader), informationheadersize);
    checkInformationHeader(f, informationheader);  // Restricción para que el fichero pueda ser válido

    //Anchura en px de la imagen (Comprende desde el byte 18-21)
    m_width = informationheader[4] + (informationheader[5] << 8) + (informationheader[6] << 16) + (informationheader[7] << 24);
    //Altura en px de la imagen (Comprende desde el byte 22-25)
    m_height = informationheader[8] + (informationheader[9] << 8) + (informationheader[10] << 16) + (informationheader[11] << 24);
    colores.m_r.resize(m_width*m_height);
    colores.m_b.resize(m_width*m_height);
    colores.m_g.resize(m_width*m_height);
    const int paddingamount = ((4-(m_width*3)%4)%4);
    readColor(f, paddingamount);
    f.close();
    cout << "El fichero ha sido leido con éxito" << endl;
}


void Image::readColor(ifstream &f, const int paddingamount) {
    /* Esta función lee el color de cada píxel y lo guarda dentro del array de estructuras definido para los colores
     * (m_colors)*/
    for (int y = 0; y < m_height; y++){
        for (int x = 0; x < m_width; x++) {
            unsigned char color[3];
            f.read(reinterpret_cast<char*>(color),3);
            //m_colors[y * m_width + x].r = static_cast<float>(color[2]) / 255.0f;
            //m_colors[y * m_width + x].g = static_cast<float>(color[1]) / 255.0f;
            //m_colors[y * m_width + x].b = static_cast<float>(color[0]) / 255.0f;
        }
        f.ignore(paddingamount);
    }
}


void Image::checkInformationHeader(ifstream &f, const unsigned char *informationheader) {
    /* Función que comprueba si el archivo es de tipo BMP considerando en número de planos, el tamaño de cada punto
     * y el valor de compresión */

    if(informationheader[14] != 24 || // tamaño de cada punto == 24 bits
       informationheader[12] != 1 || // # planos == 1
       informationheader[16] != 0 || // valor compresión == 0
       informationheader[17] != 0 || // valor compresión == 0
       informationheader[18] != 0 || // valor compresión == 0
       informationheader[19] != 0){
        cerr << "El formato BMP no es válido " << endl;
        f.close();
    }
}


void Image::checkHeader(ifstream &f, const unsigned char *fileheader) {
    /* Esta función comprueba si el archivo es un BMP, en el caso que no lo sea escribe una salida de error indicando
     * que el archivo adjuntado no es un BMP */
    if(fileheader[0] != 'B' || fileheader[1] != 'M'){
        cerr << "El archivo no es de tipo BMP " << endl;
        f.close();
    }
}


void Image::openFilein(const char *path, ifstream &f) {
    /* function to open the image and see if there is an error */
    f.open(path, ios::in | ios::binary);
    if(!f.is_open()){
        cout << "El fichero no pudo ser abierto" << endl;
        exit(-1);
    }
}

void Image::openFileout(const char *path, ofstream &f) {
    /* function to open the image and see if there is an error */
    f.open(path, ios::out | ios::binary);
    if(!f.is_open()){
        cout << "El fichero no pudo ser abierto" << endl;
        exit(-1);
    }
}