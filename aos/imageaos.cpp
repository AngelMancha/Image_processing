/*
******PROYECTO 1******
 *
******Autores:******
Ángel José Mancha
Ruth Navarro
Alicia Gutierrez
Alejandro Pardo
 */

#include <iostream>
#include "Image_aos.h"
#include <math.h>
#include <iterator>

using namespace std;


/* Variables globales */
const int fileheadersize = 14; //tamaño cabecera bitmap (bytes 14-17)
const int informationheadersize = 40; //desde el byte 14 hasta el 54

/* Inicialización de la clase imagen y de la estructura Color*/
Color::Color() : r(0), g(0), b(0) {} //initialize the values of the color to 0
Color::Color(float r, float g, float b): r(r), g(g), b(b) {} //initialize to the parameters
Color::~Color()= default;

Image::Image(int width, int height): m_width(width), m_height(height), m_colors(vector<Color>(width * height)) {}
Image::~Image() = default;

/* Funciones públicas */

bool Image::Copy(const char *SRC, const char* DEST) {
    /* Esta función coge*/
    Image::Read(SRC);
    std::ifstream src(SRC, std::ios::binary);
    std::ofstream dest(DEST, std::ios::binary);
    dest << src.rdbuf();
    cout << "El fichero ha sido copiado con exito"<<endl;
    return src && dest;
}

void Image::Histograma(const char *path, const char *end){
    std::ifstream f;
    f.open(path, ios::in | ios::binary);

    unsigned char fileheader[fileheadersize];
    f.read(reinterpret_cast<char*>(fileheader), fileheadersize);

    unsigned char informationheader[informationheadersize];
    f.read(reinterpret_cast<char*>(informationheader), informationheadersize);

    m_width = informationheader[4] + (informationheader[5] << 8) + (informationheader[6] << 16) + (informationheader[7] << 24);
    m_height = informationheader[8] + (informationheader[9] << 8) + (informationheader[10] << 16) + (informationheader[11] << 24);
    m_colors.resize(m_width*m_height);

    const int paddingamount = ((4-(m_width*3)%4)%4);

    vector<float> rojos;
    vector<float> verdes;
    vector<float> azules;

    for (int y = 0; y<m_height; y++){
        for (int x = 0; x < m_width; x++) {
            unsigned char color[3];
            f.read(reinterpret_cast<char *>(color), 3);

            auto rojo = static_cast<float>(color[2]);
            rojos.push_back(rojo);
            auto verde = static_cast<float>(color[1]);
            verdes.push_back(verde);
            auto azul = static_cast<float>(color[0]);
            azules.push_back(azul);

        }
        f.ignore(paddingamount);
    }
    f.close();
    vector<int> histograma_r;
    vector<int> histograma_g;
    vector<int> histograma_b;
    vector<int> histograma;
    for(int i =0; i<256; i++){

        histograma_r.push_back(count(rojos.begin(), rojos.end(), i));
        histograma_g.push_back(count(verdes.begin(), verdes.end(), i));
        histograma_b.push_back(count(azules.begin(), azules.end(), i));
    }
    histograma.reserve( histograma_r.size() + histograma_g.size()  + histograma_b.size()); // preallocate memory
    histograma.insert( histograma.end(), histograma_r.begin(), histograma_r.end());
    histograma.insert( histograma.end(), histograma_g.begin(), histograma_g.end());
    histograma.insert( histograma.end(), histograma_b.begin(), histograma_b.end());
    std::ofstream output_file(end);
    std::ostream_iterator<int> output_iterator(output_file, "\n");
    std::copy(histograma.begin(),histograma.end(),output_iterator);

}
void Image::GrayScale(const char* SRC, const char* DST) {
    /*we open the input and output files*/
    std::ifstream f;
    openFilein(SRC, f);

    std::ofstream j;
    openFileout(DST, j);

    /*Leemos el archivo para así obtener el ancho, alto y el vector de colores*/
    Image::Read(SRC);

    unsigned char fileheader[fileheadersize];
    unsigned char informationheader[informationheadersize];
    const int paddingamount = ((4-(m_width*3)%4)%4);
    const int filesize = fileheadersize + informationheadersize + m_width * m_height * 3 + paddingamount * m_height;


    f.read(reinterpret_cast<char*>(fileheader), fileheadersize);
    f.read(reinterpret_cast<char*>(informationheader), informationheadersize);



    /*Procedemos a realizar los cálculos pertinentes para la conversion a escala de grises*/

    for (int y = 0; y<m_height; y++){
        for (int x = 0; x < m_width; x++) {
            unsigned char color[3];
            f.read(reinterpret_cast<char*>(color),3);
            float nr = static_cast<float>(color[2])/255.0f;
            float ng = static_cast<float>(color[1])/255.0f;
            float nb= static_cast<float>(color[0])/255.0f;

            float cr, cg, cb;
            // 2. Transformación a intensidad lineal
            // Rojo
            if ( nr <= 0.04045)
            {
                cr = nr/12.92;
            }
            if (nr > 0.04045)
            {
                float aux = ((nr+0.055)/1.055);
                cr = pow(aux, 2.4);
            }
            // Green
            if ( ng <= 0.04045)
            {
                cg = nr/12.92;
            }
            if (ng > 0.04045)
            {
                float aux = ((nr+0.055)/1.055);
                cg = pow(aux, 2.4);
            }
            // Blue
            if ( nb <= 0.04045)
            {
                cb = nb/12.92;
            }
            if (nb > 0.04045)
            {
                float aux = ((nb+0.055)/1.055);
                cb = pow(aux, 2.4);
            }
            //3.Transformación lineal
            float cl = 0.2126 * cr + 0.7152 * cg + 0.0722 * cb;

            //4. Correción gamma
            if (cl <= 0.0031308){
                cg = 12.92 * cl;
            }
            if (cl > 0.0031308)
                cg = 1.055 * pow(cl, (1/2.4)) - 0.055;

            // Asignamos a los 3 componentes el mismo valor
            m_colors[y*m_width+x].r = cg;
            m_colors[y*m_width+x].g = cg;
            m_colors[y*m_width+x].b = cg;

        }
        f.ignore(paddingamount);
    }

    f.close();
    /*Exportamos el archivo al fichero de salida*/
    Export2(j, fileheader, informationheader, paddingamount, filesize);


    cout << "El fichero ha convertido a escala de grises" << endl;
}



/* Funciones privadas*/

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
    m_colors.resize(m_width*m_height);
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
            m_colors[y * m_width + x].r = static_cast<float>(color[2]) / 255.0f;
            m_colors[y * m_width + x].g = static_cast<float>(color[1]) / 255.0f;
            m_colors[y * m_width + x].b = static_cast<float>(color[0]) / 255.0f;
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


Color Image::GetColor(int x, int y) const {
    return m_colors[y*m_width+x];
}


void Image::Export(const char* path) const {
    std::ofstream f;
    f.open(path, std::ios::out | std::ios::binary);
    if (!f.is_open()) {
        cerr << "Fichero no pudo ser abierto" << endl;
        return;
    }

    unsigned char bmpPad[3] = {0, 0, 0};
    const int paddingamount = ((4 - (m_width * 3) % 4) % 4);

    //fileheadersize+informationheadersize;
    const int filesize = fileheadersize + informationheadersize + m_width * m_height * 3 + paddingamount * m_height;

    unsigned char fileheader[fileheadersize];
    fileheader[0] = 'B';
    fileheader[1] = 'M';

    fileheader[2] = filesize;
    fileheader[3] = filesize >> 8;
    fileheader[4] = filesize >> 16;
    fileheader[5] = filesize >> 24;

    fileheader[6] = 0;
    fileheader[7] = 0;
    fileheader[8] = 0;
    fileheader[9] = 0;

    fileheader[10] = fileheadersize + informationheadersize;
    fileheader[11] = 0;
    fileheader[12] = 0;
    fileheader[13] = 0;

    unsigned char informationheader[informationheadersize];

    informationheader[0] = informationheadersize;
    informationheader[1] = 0;
    informationheader[2] = 0;
    informationheader[3] = 0;

    informationheader[4] = m_width;
    informationheader[5] = m_width >> 8;
    informationheader[6] = m_width >> 16;
    informationheader[7] = m_width >> 24;

    informationheader[8] = m_height;
    informationheader[9] = m_height >> 8;
    informationheader[10] = m_height >> 16;
    informationheader[11] = m_height >> 24;

    informationheader[12] = 1;
    informationheader[13] = 0;

    informationheader[14] = 24;
    informationheader[15] = 0;

    informationheader[16] = 0;
    informationheader[17] = 0;
    informationheader[18] = 0;
    informationheader[19] = 0;
    informationheader[20] = 0;
    informationheader[21] = 0;
    informationheader[22] = 0;
    informationheader[23] = 0;
    informationheader[24] = 0;
    informationheader[25] = 0;
    informationheader[26] = 0;
    informationheader[27] = 0;
    informationheader[28] = 0;
    informationheader[29] = 0;
    informationheader[30] = 0;
    informationheader[31] = 0;
    informationheader[32] = 0;
    informationheader[33] = 0;
    informationheader[34] = 0;
    informationheader[35] = 0;
    informationheader[36] = 0;
    informationheader[37] = 0;
    informationheader[38] = 0;
    informationheader[39] = 0;

    f.write(reinterpret_cast<char *>(fileheader), fileheadersize);
    f.write(reinterpret_cast<char *>(informationheader), informationheadersize);

    for (int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; x++) {
            unsigned char r = static_cast<unsigned char>(GetColor(x, y).r * 255.0f);
            unsigned char g = static_cast<unsigned char>(GetColor(x, y).g * 255.0f);
            unsigned char b = static_cast<unsigned char>(GetColor(x, y).b * 255.0f);

            unsigned char color[] = {b, g, r};
            f.write(reinterpret_cast<char *>(color), 3);
        }
        f.write(reinterpret_cast<char *>(bmpPad), paddingamount);
    }

    f.close();
    cout << "Archivo copiado\n";
}

void Image::Export2(ofstream &j, unsigned char *fileheader, unsigned char *informationheader, const int paddingamount,
                    const int filesize) const {

    unsigned char bmpPad[3] = {0, 0, 0};
    fileheader[2] = filesize;
    fileheader[10] = fileheadersize + informationheadersize;
    j.write(reinterpret_cast<char *>(fileheader), fileheadersize);

    informationheader[0] = informationheadersize;
    j.write(reinterpret_cast<char *>(informationheader), informationheadersize);

    for (int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; x++) {
            unsigned char r = static_cast<unsigned char>(GetColor(x, y).r * 255.0f);
            unsigned char g = static_cast<unsigned char>(GetColor(x, y).g * 255.0f);
            unsigned char b = static_cast<unsigned char>(GetColor(x, y).b * 255.0f);

            unsigned char color[] = {b, g, r};
            j.write(reinterpret_cast<char *>(color), 3);
        }
        j.write(reinterpret_cast<char *>(bmpPad), paddingamount);
    }

    j.close();
}

