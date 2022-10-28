//
// Código para el tratamiento de imágenes de una representación AOS (array of structures)
//

#include "Image_aos.h"
#include "../common/common.h"
#include <iostream>
#include <cmath>
#include <iterator>
#include <vector>
#include <filesystem>
#include <chrono>
using namespace std;


/* Definición de las variables globales */
const int fileheadersize = 14; //tamaño cabecera bitmap (bytes 14-17)
const int informationheadersize = 40; //desde el byte 14 hasta el 54

/* Inicialización de la clase imagen y de la estructura Color*/
Color::Color() : r(0), g(0), b(0) {}
Color::Color(float r, float g, float b): r(r), g(g), b(b) {}
Color::~Color()= default;

ImageAos::ImageAos(int width, int height): ancho_img(width), alto_img(height), vector_colores(vector<Color>(width * height)) {}
ImageAos::~ImageAos() = default;

/* Funciones públicas */

// La función copy no realiza ninguna transformación, solamente copia los archivos
// Dentro de esta función llamamos a una función auxiliar Read que se encarga de hacer comprobaciones
// para ver si el tipo de archivo que se va a manipular es el correcto
// Una vez hecho eso se encargará de copìar la imagen de una carpeta origen a una carpeta destino
void ImageAos::Copy(std::filesystem::path SRC, std::filesystem::path DEST) {
    /* Esta función coge*/
    auto start = std::chrono::high_resolution_clock::now();
    ImageAos::Read(SRC);
    std::ifstream src(SRC, std::ios::binary);
    std::ofstream dest(DEST, std::ios::binary);
    std::string new_name="copia_"+(SRC.filename()).string();
    auto target = DEST/new_name;
    auto end = std::chrono::high_resolution_clock::now();
    load = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    start = std::chrono::high_resolution_clock::now();
    try
    {
        std::filesystem::create_directories(DEST);
        std::filesystem::copy_file(SRC, target, std::filesystem::copy_options::overwrite_existing);
    }
    catch (std::exception& e)
    {
        std::cout << e.what();
    }
    end = std::chrono::high_resolution_clock::now();
    operacion = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    store = 0;
}


// Esta función genera un archivo de texto con el histograma de cada canal (RGB)
void ImageAos::Histograma(std::filesystem::path SRC, std::filesystem::path DST){
    auto start = chrono::steady_clock::now();
    std::ifstream f;
    openFilein(SRC, f);
    unsigned char fileheader[fileheadersize];
    f.read(reinterpret_cast<char*>(fileheader), fileheadersize);
    unsigned char informationheader[informationheadersize];
    f.read(reinterpret_cast<char*>(informationheader), informationheadersize);
    ancho_img = informationheader[4] + (informationheader[5] << 8) + (informationheader[6] << 16) + (informationheader[7] << 24);
    alto_img = informationheader[8] + (informationheader[9] << 8) + (informationheader[10] << 16) + (informationheader[11] << 24);
    vector_colores.resize(ancho_img * alto_img);
    auto end = chrono::steady_clock::now();
    load=chrono::duration_cast<chrono::microseconds>(end - start).count();
    start = chrono::steady_clock::now();
    Histo_get_intensities(f);
    //vectores donde guardaremos el numero de ocurrencias
    std::vector<int> r_colors(256);
    std::vector<int> g_colors(256);
    std::vector<int> b_colors(256);

    Histo_count_ocurrencies(r_colors, g_colors, b_colors);
    end = chrono::steady_clock::now();
    operacion=chrono::duration_cast<chrono::microseconds>(end - start).count();
    start = chrono::steady_clock::now();
    Histo_create_output(SRC, DST, r_colors, g_colors, b_colors);
    end = chrono::steady_clock::now();
    store=chrono::duration_cast<chrono::microseconds>(end - start).count();
}

void ImageAos::Histo_create_output(const filesystem::path &SRC, const filesystem::path &DST, const vector<int> &r_colors,
                                   const vector<int> &g_colors, const vector<int> &b_colors) const {
    string new_name= "histo_" + (SRC.filename().replace_extension(".hst")).string();
    auto target = DST/new_name;
    ofstream output_file(target);
    for(int x=0;x<256;x++){
        output_file<<r_colors[x]<<endl;
    }
    for(int x=0;x<256;x++){
        output_file<<g_colors[x]<<endl;
    }
    for(int x=0;x<256;x++){
        output_file<<b_colors[x]<<endl;
    }
    output_file.close();
}

void ImageAos::Histo_count_ocurrencies(vector<int> &r_colors, vector<int> &g_colors, vector<int> &b_colors) {
    int r,g,b;
    //recorremos nuestra matriz de colores para rellenar nuestras listas de ocurrencias
    for(int i=0; i < ancho_img * alto_img; ++i){
        //aumentamos en uno el valor de la posicion (de la lista) que coincide con la intensidad del pixel rojo
        r= vector_colores[i].r;
        r_colors[r]+=1;
        //aumentamos en uno el valor de la posicion (de la lista) que coincide con la intensidad del pixel verde
        g= vector_colores[i].g;
        g_colors[g]+=1;
        //aumentamos en uno el valor de la posicion (de la lista) que coincide con la intensidad del pixel azul
        b= vector_colores[i].b;
        b_colors[b]+=1;
    }
}

void ImageAos::Histo_get_intensities(ifstream &f) {
    const int paddingamount = ((4 - (ancho_img * 3) % 4) % 4);
    //matriz de colores
    for (int y = 0; y < alto_img; y++){
        for (int x = 0; x < ancho_img; x++) {
            unsigned char color[3];
            f.read(reinterpret_cast<char*>(color),3);

            vector_colores[y * ancho_img + x].r = color[2];
            vector_colores[y * ancho_img + x].g = color[1];
            vector_colores[y * ancho_img + x].b = color[0];

        }
        f.ignore(paddingamount);
    }
    f.close();
}


// Esta función se encarga de convertir una imagen a escala de grises realizando varias transformaciones
// Lo primero que hace esta función es llamar a la función Read2 que se encarga de examinar si
// la imagen que manipulamos tiene el formato correcto.
// Una vez hecho esto, llamará a la función gray_calculations que se encargará de realizar las operaciones
// pertinentes. Cuando tengamos estas operaciones hechas, se encargará de exportar todos los cambios y crear una
// nueva imagen que contenga los cambios con la funcion Export2.
void ImageAos::GrayScale(std::filesystem::path SRC, std::filesystem::path DST) {
    auto start = chrono::steady_clock::now();
    std::ifstream f;
    std::ofstream j;
    Gray_open_create_files(SRC, DST, f, j);
    ImageAos::Read2(SRC);
    auto end = chrono::steady_clock::now();
    load=chrono::duration_cast<chrono::microseconds>(end - start).count();
    start = chrono::steady_clock::now();
    unsigned char fileheader[fileheadersize];
    unsigned char informationheader[informationheadersize];
    const int paddingamount = ((4- (ancho_img * 3) % 4) % 4);
    const int filesize = fileheadersize + informationheadersize + ancho_img * alto_img * 3 + paddingamount * alto_img;
    f.read(reinterpret_cast<char*>(fileheader), fileheadersize);
    f.read(reinterpret_cast<char*>(informationheader), informationheadersize);
    int offset = fileheader[10] + (fileheader[11]<<8) + (fileheader[12]<<16) + (fileheader[13]<<24);
    f.seekg(offset,std::ios_base ::beg);
    Gray_calculations(f, paddingamount);
    f.close();
    end = chrono::steady_clock::now();
    operacion=chrono::duration_cast<chrono::microseconds>(end - start).count();
    start = chrono::steady_clock::now();
    Export2(j, SRC, paddingamount, filesize);
    end = chrono::steady_clock::now();
    store=chrono::duration_cast<chrono::microseconds>(end - start).count();
}


// Función auxiliar de GrayScale que sirve para crear y nombrar las nuevas imágenes
void ImageAos::Gray_open_create_files(filesystem::path &SRC, const filesystem::path &DST, ifstream &f, ofstream &j) const {
    openFilein(SRC, f);
    string new_name= "mono_" + (SRC.filename()).string();
    auto target = DST/new_name;
    openFileout(target, j);
}


//Función auxiliar de GrayScale que se encarga de realizar las operaciones para poder convertir a escala de grises.
// Primeeo extrae los colores del vector y los guarda en variables normalizándolos.
// Una vez hecho esto se ejecuta la fórmula para la conversión a escala de grises usando la función gray_formula
void ImageAos::Gray_calculations(ifstream &f, const int paddingamount) {
    for (int y = 0; y < alto_img; y++){
        for (int x = 0; x < ancho_img; x++) {
            unsigned char color[3];

            f.read(reinterpret_cast<char*>(color),3);
            float nr = static_cast<float>(color[2])/255.0f;
            float ng = static_cast<float>(color[1])/255.0f;
            float nb= static_cast<float>(color[0])/255.0f;



            Color c;
            c.r=0;
            c.g=0;
            c.b=0;
            c = Gray_operations(nr, ng, nb, c);


            vector_colores[y * ancho_img + x].r = c.g;
            vector_colores[y * ancho_img + x].g = c.g;
            vector_colores[y * ancho_img + x].b = c.g;}
        f.ignore(paddingamount);
    }
}

Color &ImageAos::Gray_operations(float nr, float ng, float nb, Color &c) const {// 1. Transformación a intensidad lineal
/*Aplicamos la transformación lineal a cada uno de los colores*/Gray_intensidad(nr, ng, nb, c);

    //2.Transformación lineal
    float cl = 0.2126 * c.r + 0.7152 * c.g + 0.0722 * c.b;
    //3. Correción gamma
    if (cl <= 0.0031308){
        c.g = 12.92 * cl;}
    if (cl > 0.0031308){
        c.g = 1.055 * pow(cl, (1/2.4)) - 0.055;}
    return c;
}

void ImageAos::Gray_intensidad(float nr, float ng, float nb, Color &c) const {// Rojo
    if ( nr <= 0.04045){
        c.r = nr/12.92;}
    if (nr > 0.04045){
        float aux = ((nr+0.055)/1.055);
        c.r = pow(aux, 2.4);}
    // Green
    if ( ng <= 0.04045){
        c.g = nr/12.92;}
    if (ng > 0.04045){
        float aux = ((nr+0.055)/1.055);
        c.g = pow(aux, 2.4);}
    // Blue
    if ( nb <= 0.04045){
        c.b = nb/12.92;}
    if (nb > 0.04045){
        float aux = ((nb+0.055)/1.055);
        c.b = pow(aux, 2.4);}
}


// Disminuye la nitidez de una imagen.
// Toma como parámetros los directorios de entrada y de salida
// Primero llama a la funcion read y despues llama a la funcion gauss_calculations que se encarga de hacer
// las operaciones necesarias para difuminar la imagen. Despues llama a la función export2
// que se encarga de realizar las operaciones para crear la imagen.
void ImageAos::GaussianBlur(std::filesystem::path SRC, std::filesystem::path DST) {
    auto start = chrono::steady_clock::now();
    std::ifstream f;
    std::ofstream j;
    Gauss_open_create_files(SRC, DST, f, j);
    ImageAos::Read(SRC);
    unsigned char fileheader[fileheadersize];
    unsigned char informationheader[informationheadersize];
    const int paddingamount = ((4 - (ancho_img * 3) % 4) % 4);
    const int filesize = fileheadersize + informationheadersize + ancho_img * alto_img * 3 + paddingamount * alto_img;
    f.read(reinterpret_cast<char *>(fileheader), fileheadersize);
    f.read(reinterpret_cast<char *>(informationheader), informationheadersize);
    auto end = chrono::steady_clock::now();
    load=chrono::duration_cast<chrono::microseconds>(end - start).count();
    start = chrono::steady_clock::now();
    vector<Color> color_aux = get_Color_vector();
    Gauss_calculations(f, paddingamount, color_aux);
    f.close();
    end = chrono::steady_clock::now();
    operacion=chrono::duration_cast<chrono::microseconds>(end - start).count();
    start = chrono::steady_clock::now();
    Export2(j, SRC, paddingamount, filesize);
    end = chrono::steady_clock::now();
    store=chrono::duration_cast<chrono::microseconds>(end - start).count();
}


// Función auxiliar de GaussianBlur que sirve para crear y nombrar las nuevas imágenes
void ImageAos::Gauss_open_create_files(filesystem::path &SRC, const filesystem::path &DST, ifstream &f, ofstream &j) const {
    openFilein(SRC, f);/*Escribimos el nombre con el que queremos que se guarde el fichero de salida en el directorio destino*/
    string new_name="gauss_"+(SRC.filename()).string();
    auto target = DST/new_name;
    openFileout(target, j);
}


// Se encarga de hacer las operaciones necesarias para difuminar la imagen.
void ImageAos::Gauss_calculations(ifstream &f, const int paddingamount, const vector<Color> &color_aux) {
    for (int y =0; y < alto_img; y++) {
        for (int pyxel = 0; pyxel < ancho_img; pyxel++) {

            Color final = Gauss_operations(color_aux, y, pyxel);
            vector_colores[y * ancho_img + pyxel].r = final.r / 273;
            vector_colores[y * ancho_img + pyxel].g = final.g / 273;
            vector_colores[y * ancho_img + pyxel].b = final.b / 273;
        }
        f.ignore(paddingamount);
    }
}

Color ImageAos::Gauss_operations(const vector<Color> &color_aux, int y, int pyxel) const {
    Color final;
    final.r=0;
    final.g=0;
    final.b=0;
    for (int sumatorio_s = -2; sumatorio_s < 3; sumatorio_s++) {
        for (int sumatorio_t=-2; sumatorio_t < 3; sumatorio_t++) {
            if ((pyxel + sumatorio_s > ancho_img) or (pyxel + sumatorio_s < 0) or (y + sumatorio_t > alto_img) or (y + sumatorio_t < 0)) {
                final.r = final.r + 0;
                final.g = final.g + 0;
                final.b = final.b + 0;
            }
            else {
                int mascara[5][5] = {{1, 4,  7,  4,  1},{4, 16, 26, 16, 4},{7, 26, 41, 26, 7},{4, 16, 26, 16, 4},{1, 4,  7,  4,  1}};
                float nr = (color_aux[((y + sumatorio_t) * ancho_img) + pyxel + sumatorio_s].r);
                float ng = (color_aux[((y + sumatorio_t) * ancho_img) + pyxel + sumatorio_s].g);
                float nb = (color_aux[((y + sumatorio_t) * ancho_img) + pyxel + sumatorio_s].b);

                float cr = (mascara[sumatorio_s + 2][sumatorio_t + 2]) *  nr;
                float cg = (mascara[sumatorio_s + 2][sumatorio_t + 2]) *  ng;
                float cb = (mascara[sumatorio_s + 2][sumatorio_t + 2]) *  nb;

                final.r = final.r + cr;
                final.g = final.g + cg;
                final.b = final.b + cb;
            }
        }
    }
    return final;
}

/* Funciones privadas*/

void ImageAos::Read(std::filesystem::path path) {
    /* Esta función lee una imagen y comprueba que todos los campos de la cabecera sean correctos y guarda en la clase
     * ImageSoa los valores para ancho_img, alto_img y vector_colores */
    std::ifstream f;
    openFilein(path, f);
    // Definimos 2 arrays que contienen la cabecera y la información de cabecera y hacemos comprobaciones
    unsigned char fileheader[fileheadersize]; //desde el byte 0 hasta el 14 --> Contiene el byte hasta el tamaño de cabecera de BMP
    unsigned char informationheader[informationheadersize];
    f.read(reinterpret_cast<char*>(fileheader), fileheadersize);
    checkHeader(path); // Comprobamos que la cabecera sea correcta llamando a la funcion checkHeader
    f.read(reinterpret_cast<char*>(informationheader), informationheadersize);
    if(informationheader[14] != 24 || informationheader[12] != 1 || // # planos == 1
       informationheader[16] != 0 || informationheader[17] != 0 || // valor compresión == 0
       informationheader[18] != 0 || informationheader[19] != 0){
        cerr << "El formato BMP no es válido " << endl;
        f.close();
    }
    int offset = fileheader[10] + (fileheader[11]<<8) + (fileheader[12]<<16) + (fileheader[13]<<24);
    f.seekg(offset,std::ios_base ::beg);
    //Anchura en px de la imagen (Comprende desde el byte 18-21)
    ancho_img = informationheader[4] + (informationheader[5] << 8) + (informationheader[6] << 16) + (informationheader[7] << 24);
    //Altura en px de la imagen (Comprende desde el byte 22-25)
    alto_img = informationheader[8] + (informationheader[9] << 8) + (informationheader[10] << 16) + (informationheader[11] << 24);
    vector_colores.resize(ancho_img * alto_img);
    const int paddingamount = ((4- (ancho_img * 3) % 4) % 4);
    readColor(f, paddingamount);
    f.close();
}

void ImageAos::Read2(std::filesystem::path path) {
    /* Esta función lee una imagen y comprueba que todos los campos de la cabecera sean correctos y guarda en la clase
     * ImageSoa los valores para ancho_img, alto_img y vector_colores */
    std::ifstream f;
    openFilein(path, f);
    // Definimos 2 arrays que contienen la cabecera y la información de cabecera y hacemos comprobaciones
    unsigned char fileheader[fileheadersize]; //desde el byte 0 hasta el 14 --> Contiene el byte hasta el tamaño de cabecera de BMP
    unsigned char informationheader[informationheadersize];
    f.read(reinterpret_cast<char*>(fileheader), fileheadersize);
    checkHeader(path); // Comprobamos que la cabecera sea correcta llamando a la funcion checkHeader
    f.read(reinterpret_cast<char*>(informationheader), informationheadersize);
    if(informationheader[14] != 24 || informationheader[12] != 1 || // # planos == 1
       informationheader[16] != 0 || informationheader[17] != 0 || // valor compresión == 0
       informationheader[18] != 0 || informationheader[19] != 0){
        cerr << "El formato BMP no es válido " << endl;
        f.close();
    }
    int offset = fileheader[10] + (fileheader[11]<<8) + (fileheader[12]<<16) + (fileheader[13]<<24);
    f.seekg(offset,std::ios_base ::beg);
    //Anchura en px de la imagen (Comprende desde el byte 18-21)
    ancho_img = informationheader[4] + (informationheader[5] << 8) + (informationheader[6] << 16) + (informationheader[7] << 24);
    //Altura en px de la imagen (Comprende desde el byte 22-25)
    alto_img = informationheader[8] + (informationheader[9] << 8) + (informationheader[10] << 16) + (informationheader[11] << 24);
    vector_colores.resize(ancho_img * alto_img);
    const int paddingamount = ((4- (ancho_img * 3) % 4) % 4);
    readColor(f, paddingamount);
    f.close();
}

vector<Color> ImageAos::get_Color_vector() {
    vector<Color>color_aux;
    for (unsigned long long i=0; i < vector_colores.size(); i++) {
        color_aux.push_back(vector_colores[i]);
    }
    return color_aux;
}

void ImageAos::readColor(ifstream &f, const int paddingamount) {
    /* Esta función lee el color de cada píxel y lo guarda dentro del array de estructuras definido para los colores
     * (vector_colores)*/
    for (int y = 0; y < alto_img; y++){
        for (int x = 0; x < ancho_img; x++) {
            unsigned char color[3];
            f.read(reinterpret_cast<char*>(color),3);
            vector_colores[y * ancho_img + x].r = static_cast<float>(color[2]) / 255.0f;
            vector_colores[y * ancho_img + x].g = static_cast<float>(color[1]) / 255.0f;
            vector_colores[y * ancho_img + x].b = static_cast<float>(color[0]) / 255.0f;
        }
        f.ignore(paddingamount);
    }
}




void ImageAos::checkHeader(std::filesystem::path SRC) {
    /* Esta función comprueba si el archivo es un BMP, en el caso que no lo sea escribe una salida de error indicando
     * que el archivo adjuntado no es un BMP */
    unsigned char fileheader[fileheadersize];
    std::ifstream f;
    f.open(SRC, ios::in | ios::binary);
    f.read(reinterpret_cast<char*>(fileheader), fileheadersize);

    if(fileheader[0] != 'B' || fileheader[1] != 'M'){
        cerr << "El archivo no es de tipo BMP " << endl;
        f.close();
    }
}

void ImageAos::openFilein(std::filesystem::path path, ifstream &f) {
    /* function to open the image and see if there is an error */
    f.open(path.generic_string(), ios::in | ios::binary);
    if(!f.is_open()){
        cout << "El fichero no pudo ser abierto" << endl;
        exit(-1);
    }
}

void ImageAos::openFileout(std::filesystem::path path, ofstream &f) {
    /* function to open the image and see if there is an error */
    f.open(path.generic_string(), ios::out | ios::binary);
    if(!f.is_open()){
        cout << "El fichero no pudo ser abierto" << endl;
        exit(-1);
    }
}


Color ImageAos::GetColor(int x, int y) const {
    return vector_colores[y * ancho_img + x];
}


void ImageAos::Export2(ofstream &j, std::filesystem::path SRC, const int paddingamount, const int filesize)  {


    unsigned char fileheader[fileheadersize];
    unsigned char informationheader[informationheadersize];

    std::ifstream f;
    f.open(SRC, ios::in | ios::binary);
    f.read(reinterpret_cast<char*>(fileheader), fileheadersize);
    f.read(reinterpret_cast<char*>(informationheader), informationheadersize);

    unsigned char bmpPad[3] = {0, 0, 0};
    fileheader[2] = filesize;
    fileheader[10] = fileheadersize + informationheadersize;
    j.write(reinterpret_cast<char *>(fileheader), fileheadersize);

    informationheader[0] = informationheadersize;
    j.write(reinterpret_cast<char *>(informationheader), informationheadersize);

    for (int y = 0; y < alto_img; y++) {
        for (int x = 0; x < ancho_img; x++) {
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

/*Esta función es llamada por el main del ejecutable de Aos cuando recibe como argumento en nombre de
 * la función que hay que ejecutar (copy, histo, mono, o gauss*/
int ImageAos::funcion(std::vector<std::filesystem::path> paths, std::filesystem::path outpath, std::string op) {
    for (const auto &path: paths)
    {
        if(op=="copy"){
            ImageAos copia(0, 0);
            copia.Copy(path, outpath);
            tiempo_ejecucion(copia.load,copia.store,copia.operacion,path,outpath,op);

        }

        if(op=="mono"){
            ImageAos mono(0, 0);
            mono.GrayScale(path, outpath);
            tiempo_ejecucion(mono.load,mono.store,mono.operacion,path,outpath,op);

        }
        if(op=="histo") {
            ImageAos histo(0, 0);
            histo.Histograma(path, outpath);
            tiempo_ejecucion(histo.load,histo.store,histo.operacion,path,outpath,op);
        }
        if(op=="gauss"){
            ImageAos gauss(0, 0);
            gauss.GaussianBlur(path, outpath);
            tiempo_ejecucion(gauss.load,gauss.store,gauss.operacion,path,outpath,op);

        }


    }
    return 0;

}
