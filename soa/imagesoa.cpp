#include <iostream>
#include "Image_soa.h"
#include <filesystem>
#include <cmath>
#include <utility>
using namespace std;
/* Variables globales */
const int fileheadersize = 14; //tamaño cabecera bitmap (bytes 14-17)
const int informationheadersize = 40;

Colores::Colores() : m_r(0), m_g(0), m_b(0) {}
Colores::Colores(std::vector<float> m_r, std::vector<float> m_g, std::vector<float> m_b): m_r(std::move(m_r)),m_g(std::move(m_g)),m_b(std::move(m_b)) {}
Colores::~Colores()= default;

//
ImageSoa::ImageSoa(int width, int height): m_width(width), m_height(height) {}
ImageSoa::~ImageSoa() = default;

void ImageSoa::Copy(std::filesystem::path SRC, std::filesystem::path DEST) {
    ImageSoa::Read(SRC);
    std::ifstream src(SRC, std::ios::binary);
    std::ofstream dest(DEST, std::ios::binary);
    std::string new_name="copia_"+(SRC.filename()).string();
    auto target = DEST/new_name;
    try // If you want to avoid exception handling, then use the error code overload of the following functions.
    {
        std::filesystem::create_directories(DEST); // Recursively create target directory if not existing.
        std::filesystem::copy_file(SRC, target, std::filesystem::copy_options::overwrite_existing);
    }
    catch (std::exception& e) // Not using fs::filesystem_error since std::bad_alloc can throw too.
    {
        std::cout << e.what();
    }
}


void ImageSoa::GrayScale(std::filesystem::path SRC, std::filesystem::path DST) {
    /*we open the input and output files*/
    std::ifstream f;
    std::ofstream j;
    Gray_open_create_files(SRC, DST, f, j);

    /*Leemos el archivo para así obtener el ancho, alto y el vector de colores*/
    ImageSoa::Read(SRC);
    unsigned char fileheader[fileheadersize];
    unsigned char informationheader[informationheadersize];
    const int paddingamount = ((4-(m_width*3)%4)%4);
    const int filesize = fileheadersize + informationheadersize + m_width * m_height * 3 + paddingamount * m_height;
    f.read(reinterpret_cast<char*>(fileheader), fileheadersize);
    f.read(reinterpret_cast<char*>(informationheader), informationheadersize);
    int offset = fileheader[10] + (fileheader[11]<<8) + (fileheader[12]<<16) + (fileheader[13]<<24);
    f.seekg(offset,std::ios_base ::beg);

    /*Procedemos a realizar los cálculos pertinentes para la conversion a escala de grises*/
    Gray_calculations(f, paddingamount);
    f.close();

    /*Exportamos el archivo al fichero de salida*/
    Export2(j, fileheader, informationheader, paddingamount, filesize);
    cout << "El fichero ha convertido a escala de grises" << endl;
}

void ImageSoa::Gray_open_create_files(filesystem::path &SRC, const filesystem::path &DST, ifstream &f, ofstream &j) const {
    openFilein(SRC, f);
    string new_name= "mono_" + (SRC.filename()).string();
    auto target = DST/new_name;
    openFileout(target, j);
}

void ImageSoa::Gray_calculations(ifstream &f, const int paddingamount) {
    for (int y = 0; y < m_height; y++){
        for (int x = 0; x < m_width; x++) {
            unsigned char color[3];
            /*Primero extraemos los colores del vector y los guardamos en variables normalizándolos*/
            f.read(reinterpret_cast<char*>(color),3);
            float nr = static_cast<float>(color[2])/255.0f;
            float ng = static_cast<float>(color[1])/255.0f;
            float nb= static_cast<float>(color[0])/255.0f;

            /*Después procedemos a ejecutar la fórmula para la conversión a escala de grises*/
            float cr=0, cg=0, cb=0;
            cg = Gray_formula(nr, ng, nb, cr, cg, cb);

            /* Y por último asignamos a los 3 componentes del vector de colores el mismo valor*/
            colores.m_r[y * m_width + x] = cg;
            colores.m_g[y * m_width + x] = cg;
            colores.m_b[y * m_width + x] = cg;}

        f.ignore(paddingamount);}}

float ImageSoa::Gray_formula(float nr, float ng, float nb, float cr, float cg, float cb) const {
    // 1. Transformación a intensidad lineal
    Gray_intensidad_lineal(nr, ng, nb, cr, cg, cb);
    //2.Transformación lineal
    float cl = 0.2126 * cr + 0.7152 * cg + 0.0722 * cb;

    //3. Correción gamma
    if (cl <= 0.0031308){
        cg = 12.92 * cl;}
    if (cl > 0.0031308){
        cg = 1.055 * pow(cl, (1/2.4)) - 0.055;}
    return cg;
}

void ImageSoa::Gray_intensidad_lineal(float nr, float ng, float nb, float &cr, float &cg, float &cb) const {
    /*Aplicamos la transformación lineal a cada uno de los colores*/
    // Rojo
    if ( nr <= 0.04045){
        cr = nr/12.92;}
    if (nr > 0.04045){
        float aux = ((nr+0.055)/1.055);
        cr = pow(aux, 2.4);}
    // Green
    if ( ng <= 0.04045){
        cg = nr/12.92;}
    if (ng > 0.04045){
        float aux = ((nr+0.055)/1.055);
        cg = pow(aux, 2.4);}
    // Blue
    if ( nb <= 0.04045){
        cb = nb/12.92;}
    if (nb > 0.04045){
        float aux = ((nb+0.055)/1.055);
        cb = pow(aux, 2.4);}

}

/////////////////////////////////////////////////////Guarrona
void ImageSoa::Read(std::filesystem::path path) {
    /* Esta función lee una imagen y comprueba que todos los campos de la cabecera sean correctos y guarda en la clase
     * ImageSoa los valores para m_width, m_height y m_colors */
    std::ifstream f;
    openFilein(path, f);
    // Definimos 2 arrays que contienen la cabecera y la información de cabecera y hacemos comprobaciones
    unsigned char fileheader[fileheadersize]; //desde el byte 0 hasta el 14 --> Contiene el byte hasta el tamaño de cabecera de BMP
    unsigned char informationheader[informationheadersize];
    f.read(reinterpret_cast<char*>(fileheader), fileheadersize);
    checkHeader(f, fileheader); // Comprobamos que la cabecera sea correcta llamando a la funcion checkHeader
    f.read(reinterpret_cast<char*>(informationheader), informationheadersize);
    checkInformationHeader(f, informationheader);  // Restricción para que el fichero pueda ser válido
    int offset = fileheader[10] + (fileheader[11]<<8) + (fileheader[12]<<16) + (fileheader[13]<<24);
    f.seekg(offset,std::ios_base ::beg);
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


void ImageSoa::readColor(ifstream &f, const int paddingamount) {
    /* Esta función lee el color de cada píxel y lo guarda dentro del array de estructuras definido para los colores
     * (m_colors)*/
    for (int y = 0; y < m_height; y++){
        for (int x = 0; x < m_width; x++) {
            unsigned char color[3];
            f.read(reinterpret_cast<char*>(color),3);
            colores.m_r[y * m_width + x] = static_cast<float>(color[2]) / 255.0f;
            colores.m_g[y * m_width + x] = static_cast<float>(color[1]) / 255.0f;
            colores.m_b[y * m_width + x] = static_cast<float>(color[0]) / 255.0f;
        }
        f.ignore(paddingamount);
    }
}


void ImageSoa::checkInformationHeader(ifstream &f, const unsigned char *informationheader) {
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


void ImageSoa::checkHeader(ifstream &f, const unsigned char *fileheader) {
    /* Esta función comprueba si el archivo es un BMP, en el caso que no lo sea escribe una salida de error indicando
     * que el archivo adjuntado no es un BMP */
    if(fileheader[0] != 'B' || fileheader[1] != 'M'){
        cerr << "El archivo no es de tipo BMP " << endl;
        f.close();
    }
}


void ImageSoa::openFilein(std::filesystem::path path, ifstream &f) {
    /* function to open the image and see if there is an error */
    f.open(path, ios::in | ios::binary);
    if(!f.is_open()){
        cout << "El fichero no pudo ser abierto" << endl;
        exit(-1);
    }
}
int ImageSoa::GetColorRed(int x, int y) const {
    return colores.m_r[y*m_width+x];
}

int ImageSoa::GetColorGreen(int x, int y) const {
    return colores.m_g[y*m_width+x];
}

int ImageSoa::GetColorBlue(int x, int y) const {
    return colores.m_b[y*m_width+x];
}

void ImageSoa::Export2(ofstream &j, unsigned char *fileheader, unsigned char *informationheader, const int paddingamount,
                    const int filesize) const {

    unsigned char bmpPad[3] = {0, 0, 0};
    fileheader[2] = filesize;
    fileheader[10] = fileheadersize + informationheadersize;
    j.write(reinterpret_cast<char *>(fileheader), fileheadersize);

    informationheader[0] = informationheadersize;
    j.write(reinterpret_cast<char *>(informationheader), informationheadersize);

    for (int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; x++) {
            unsigned char r = static_cast<unsigned char>(GetColorRed(x,y) * 255.0f);
            unsigned char g = static_cast<unsigned char>(GetColorGreen(x,y) * 255.0f);
            unsigned char b = static_cast<unsigned char>(GetColorBlue(x,y) * 255.0f);

            unsigned char color[] = {b, g, r};
            j.write(reinterpret_cast<char *>(color), 3);
        }
        j.write(reinterpret_cast<char *>(bmpPad), paddingamount);
    }

    j.close();
}

void ImageSoa::openFileout(std::filesystem::path path, ofstream &f) {

    f.open(path, ios::out | ios::binary);
    if(!f.is_open()){
        cout << "El fichero no pudo ser abierto" << endl;
        exit(-1);
    }
}