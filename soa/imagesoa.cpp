#include <iostream>
#include "Image_soa.h"
#include <filesystem>
#include <cmath>
#include <utility>
#include "../common/common.h"
#include <chrono>
using namespace std;
/* Variables globales */
const int fileheadersize = 14; //tamaño cabecera bitmap (bytes 14-17)
const int informationheadersize = 40;

Colores::Colores() : m_r(0), m_g(0), m_b(0) {}
Colores::Colores(std::vector<float> m_r, std::vector<float> m_g, std::vector<float> m_b): m_r(std::move(m_r)),m_g(std::move(m_g)),m_b(std::move(m_b)) {}
Colores::~Colores()= default;

/*Definimos una estructura de parámetros que usaremos de manaera auxiliar*/
parametros::parametros() : r(0), g(0), b(0) {}
parametros::parametros(float r, float g, float b): r(r), g(g), b(b) {}
parametros::~parametros()= default;
//
ImageSoa::ImageSoa(int width, int height): ancho_img(width), alto_img(height) {}
ImageSoa::~ImageSoa() = default;

void ImageSoa::Copy(std::filesystem::path SRC, std::filesystem::path DEST) {
    auto start = std::chrono::high_resolution_clock::now();
    ImageSoa::Read(SRC);
    std::ifstream src(SRC, std::ios::binary);
    std::ofstream dest(DEST, std::ios::binary);
    std::string new_name="copia_"+(SRC.filename()).string();
    auto target = DEST/new_name;
    auto end = std::chrono::high_resolution_clock::now();
    load = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    start = std::chrono::high_resolution_clock::now();
    try // If you want to avoid exception handling, then use the error code overload of the following functions.
    {
        std::filesystem::create_directories(DEST); // Recursively create target directory if not existing.
        std::filesystem::copy_file(SRC, target, std::filesystem::copy_options::overwrite_existing);
    }
    catch (std::exception& e) // Not using fs::filesystem_error since std::bad_alloc can throw too.
    {
        std::cout << e.what();
    }
    end = std::chrono::high_resolution_clock::now();
    operacion = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    store = 0;
}

void ImageSoa::Histograma(std::filesystem::path SRC, std::filesystem::path DST){
    auto start = std::chrono::high_resolution_clock::now();
    std::ifstream f;
    openFilein(SRC, f);
    unsigned char fileheader[fileheadersize];
    f.read(reinterpret_cast<char*>(fileheader), fileheadersize);

    unsigned char informationheader[informationheadersize];
    f.read(reinterpret_cast<char*>(informationheader), informationheadersize);
    ancho_img = informationheader[4] + (informationheader[5] << 8) + (informationheader[6] << 16) + (informationheader[7] << 24);
    alto_img = informationheader[8] + (informationheader[9] << 8) + (informationheader[10] << 16) + (informationheader[11] << 24);
    colores.m_r.resize(ancho_img * alto_img);
    colores.m_g.resize(ancho_img * alto_img);
    colores.m_b.resize(ancho_img * alto_img);
    auto end = std::chrono::high_resolution_clock::now();
    load = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    start = std::chrono::high_resolution_clock::now();
    Histo_get_intensities(f);

    //vectores donde guardaremos el numero de ocurrencias
    std::vector<int> r_colors(256);
    std::vector<int> g_colors(256);
    std::vector<int> b_colors(256);

    Histo_count_ocurrencies(r_colors, g_colors, b_colors);
    end = std::chrono::high_resolution_clock::now();
    operacion = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    start = std::chrono::high_resolution_clock::now();
    Histo_create_output(SRC, DST, r_colors, g_colors, b_colors);
    end = std::chrono::high_resolution_clock::now();
    store = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

}

void ImageSoa::Histo_create_output(const filesystem::path &SRC, const filesystem::path &DST, const vector<int> &r_colors,
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

void ImageSoa::Histo_count_ocurrencies(vector<int> &r_colors, vector<int> &g_colors, vector<int> &b_colors) {
    int r,g,b;
    //recorremos nuestra matriz de colores para rellenar nuestras listas de ocurrencias
    for(int i=0; i < ancho_img * alto_img; ++i){
        //aumentamos en uno el valor de la posicion (de la lista) que coincide con la intensidad del pixel rojo
        r= colores.m_r[i];
        r_colors[r]+=1;
        //aumentamos en uno el valor de la posicion (de la lista) que coincide con la intensidad del pixel verde
        g= colores.m_g[i];
        g_colors[g]+=1;
        //aumentamos en uno el valor de la posicion (de la lista) que coincide con la intensidad del pixel azul
        b= colores.m_b[i];
        b_colors[b]+=1;
    }
}

void ImageSoa::Histo_get_intensities(ifstream &f) {
    const int paddingamount = ((4 - (ancho_img * 3) % 4) % 4);
    //matriz de colores
    for (int y = 0; y < alto_img; y++){
        for (int x = 0; x < ancho_img; x++) {
            unsigned char color[3];
            f.read(reinterpret_cast<char*>(color),3);

            colores.m_r[y * ancho_img + x] = color[2];
            colores.m_g[y * ancho_img + x] = color[1];
            colores.m_b[y * ancho_img + x] = color[0];

        }
        f.ignore(paddingamount);
    }
    f.close();
}

/*La función GrayScale se encarga de convertir a ecala de grises la imagaen original:
 * Primero lee el archivo original y crea el archivo destino.
 * Después lee la imagen para así poder rellenar los parámetros de ancho, alto y la
 * estructura de colores.
 * Posteriormente se llama a la función que se encarga de modificar los colores de cada pyxel
 * Y por último se escriben los nuevos colores en la imagen de destinol*/
void ImageSoa::GrayScale(std::filesystem::path SRC, std::filesystem::path DST) {
    auto start = std::chrono::high_resolution_clock::now();
    std::ifstream f;
    std::ofstream j;
    Gray_open_create_files(SRC, DST, f, j);
    ImageSoa::Read2(SRC);
    unsigned char fileheader[fileheadersize];
    unsigned char informationheader[informationheadersize];
    const int paddingamount = ((4- (ancho_img * 3) % 4) % 4);
    const int filesize = fileheadersize + informationheadersize + ancho_img * alto_img * 3 + paddingamount * alto_img;
    f.read(reinterpret_cast<char*>(fileheader), fileheadersize);
    f.read(reinterpret_cast<char*>(informationheader), informationheadersize);
    int offset = fileheader[10] + (fileheader[11]<<8) + (fileheader[12]<<16) + (fileheader[13]<<24);
    f.seekg(offset,std::ios_base ::beg);
    auto end = std::chrono::high_resolution_clock::now();
    load = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    start = std::chrono::high_resolution_clock::now();

    Gray_calculations(f, paddingamount);

    f.close();
    end = std::chrono::high_resolution_clock::now();
    operacion = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    start = std::chrono::high_resolution_clock::now();
    Export2(j, SRC, paddingamount, filesize);
    end = std::chrono::high_resolution_clock::now();
    store = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}

/*Esta función se encarga de crear los arhcivos destino en el directorio destino*/
void ImageSoa::Gray_open_create_files(filesystem::path &SRC, const filesystem::path &DST, ifstream &f, ofstream &j) const {
    openFilein(SRC, f);
    string new_name= "mono_" + (SRC.filename()).string();
    auto target = DST/new_name;
    openFileout(target, j);
}

/*En esta función se llevan a cabo los pasos para la conversión a escala de grises.*/
void ImageSoa::Gray_calculations(ifstream &f, const int paddingamount) {
    for (int y = 0; y < alto_img; y++){
        for (int x = 0; x < ancho_img; x++) {
            unsigned char color[3];
            /*Primero extraemos los colores del vector y los guardamos en variables normalizándolos*/
            f.read(reinterpret_cast<char*>(color),3);
            float nr = static_cast<float>(color[2])/255.0f;
            float ng = static_cast<float>(color[1])/255.0f;
            float nb= static_cast<float>(color[0])/255.0f;
            /*Después procedemos a ejecutar la fórmula para la conversión a escala de grises*/
            parametros c = Gray_operations(nr, ng, nb);


            /* Y por último asignamos a los 3 componentes del vector de colores el mismo valor*/
            colores.m_r[y * ancho_img + x] = c.g;

            colores.m_g[y * ancho_img + x] = c.g;
            colores.m_b[y * ancho_img + x] = c.g;}

        f.ignore(paddingamount);}}


/*Esta función contiene la fórmula para la conversión a la escala de grises:
* Primero: se procede con la transformación a intensidad lineal
* Segundo: se procede con la transformación lineal
* Tercero: se procede con la corrección gamma*/

parametros ImageSoa::Gray_operations(float nr, float ng, float nb) const {
    parametros c;
    c.r=0;
    c.g=0;
    c.b=0;
    // 1. Transformación a intensidad lineal
    c = Gray_intensidad(nr, ng, nb, c);
    //2.Transformación lineal
    float cl = 0.2126 * c.r + 0.7152 * c.g + 0.0722 * c.b;

    //3. Correción gamma
    if (cl <= 0.0031308){
        c.g = 12.92 * cl;}
    if (cl > 0.0031308){
        c.g = 1.055 * pow(cl, (1/2.4)) - 0.055;}
    return c;
}


/*Esta función implementa únicamente el paso 1 de la anterior función para así poder
 * hacer la transformación lineal a cada uno de los colores*/
parametros &ImageSoa::Gray_intensidad(float nr, float ng, float nb, parametros &c) const {
    if (nr <= 0.04045){
        c.r = nr/12.92;}
    if (nr > 0.04045){
        float aux = ((nr+0.055)/1.055);
        c.r = pow(aux, 2.4);}
    if ( ng <= 0.04045){
        c.g = nr/12.92;}
    if (ng > 0.04045){
        float aux = ((nr+0.055)/1.055);
        c.g = pow(aux, 2.4);}
    if ( nb <= 0.04045){
        c.b = nb/12.92;}
    if (nb > 0.04045){
        float aux = ((nb+0.055)/1.055);
        c.b = pow(aux, 2.4);
    }
    return c;
}



/*La función GaussianBlur se encarga de difuminar una imagen
 * Primero lee el archivo de origen y posteriormente crea el archivo destino para
 * Después crea tres vectores auxiliares en los que va a guardar los 3 colores.
 * De esta forma se consigue que los valores para los píxeles no se sobreescriban
 * Posteriormente llamamos a la función que se encarga de realizar las modificaciones gausianas
 * Y finalmente escribimos los valores nuevos con la función export.
 * */

void ImageSoa::GaussianBlur(std::filesystem::path SRC, std::filesystem::path DST) {
    auto start = chrono::steady_clock::now();
    std::ifstream f;
    std::ofstream j;
    Gauss_open_create_files(SRC, DST, f, j);
    ImageSoa::Read(SRC);
    unsigned char fileheader[fileheadersize];
    unsigned char informationheader[informationheadersize];
    const int paddingamount = ((4 - (ancho_img * 3) % 4) % 4);
    const int filesize = fileheadersize + informationheadersize + ancho_img * alto_img * 3 + paddingamount * alto_img;
    f.read(reinterpret_cast<char *>(fileheader), fileheadersize);
    f.read(reinterpret_cast<char *>(informationheader), informationheadersize);
    auto end = chrono::steady_clock::now();
    load = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    start = chrono::steady_clock::now();

    Colores color_aux = Gauss_auxiliarvector();

    Gauss_Calculations(f, paddingamount, color_aux);

    f.close();
    end = chrono::steady_clock::now();
    operacion = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    start = chrono::steady_clock::now();
    Export2(j, SRC, paddingamount, filesize);
    end = chrono::steady_clock::now();
    store = chrono::duration_cast<chrono::milliseconds>(end - start).count();
}

/*Esta función crea una estructura auxiliar en la que guardar los colores*/
Colores ImageSoa::Gauss_auxiliarvector() {
    Colores color_aux;

    for (unsigned long long i=0; i < colores.m_r.size(); i++) {
        color_aux.m_r.push_back(colores.m_r[i]);
        color_aux.m_g.push_back(colores.m_g[i]);
        color_aux.m_b.push_back(colores.m_b[i]);
    }
    return color_aux;
}

/*Esta función es la encargada de obtener pixel por pixel el valor deseado para su difuminación llamando
 * a la función gauss_operatons y después guarda los valores deseados en la estructura de colores
 * */
void ImageSoa::Gauss_Calculations(ifstream &f, const int paddingamount, const Colores &color_aux) {
    for (int y =0; y < alto_img; y++) {
        for (int pyxel = 0; pyxel < ancho_img; pyxel++) {

            parametros final;
            final.r=0;
            final.g=0;
            final.b=0;

            final = Gauss_operations(color_aux, y, pyxel, final);

            colores.m_r[y * ancho_img + pyxel] = final.r / 273;
            colores.m_g[y * ancho_img + pyxel] = final.g / 273;
            colores.m_b[y * ancho_img + pyxel] = final.b / 273;
        }
        f.ignore(paddingamount);
    }
}

/*Esta función implementa la fórmula final para aplicarla en cada uno de los 25 pixeles de alrededor
 * al pixel central*/
parametros &ImageSoa::Gauss_operations(const Colores &color_aux, int y, int pyxel, parametros &final) const {
    for (int sumatorio_s = -2; sumatorio_s < 3; sumatorio_s++) {
        for (int sumatorio_t=-2; sumatorio_t < 3; sumatorio_t++) {
            /*En el caso de que uno de los 25 pixeles que rodean al pixel central esté fuera de los límites, se le asigna un valor de 0 */
            if ((pyxel + sumatorio_s > ancho_img) or (pyxel + sumatorio_s < 0) or (y + sumatorio_t > alto_img) or (y + sumatorio_t < 0)) {
                final.r = final.r + 0;
                final.g = final.g + 0;
                final.b = final.b + 0;}
            /*de lo contrario se multiplica el valor correspondiente por la máscara y se suma el resultado a la correspondiente variable final que
             * recopila la suma de los 25 pixeles*/
            else {
                int mascara[5][5] = {{1, 4,  7,  4,  1},{4, 16, 26, 16, 4},{7, 26, 41, 26, 7},{4, 16, 26, 16, 4},{1, 4,  7,  4,  1}};
                float nr = (color_aux.m_r[((y + sumatorio_t) * ancho_img) + pyxel + sumatorio_s]);
                float ng = (color_aux.m_g[((y + sumatorio_t) * ancho_img) + pyxel + sumatorio_s]);
                float nb = (color_aux.m_b[((y + sumatorio_t) * ancho_img) + pyxel + sumatorio_s]);

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

/*Esta función genera el archivo destino*/
void ImageSoa::Gauss_open_create_files(filesystem::path &SRC, const filesystem::path &DST, ifstream &f, ofstream &j) const {
    openFilein(SRC, f);/*Escribimos el nombre con el que queremos que se guarde el fichero de salida en el directorio destino*/
    string new_name="gauss_"+(SRC.filename()).string();
    auto target = DST/new_name;
    openFileout(target, j);
}

/* Esta función lee una imagen y comprueba que todos los campos de la cabecera sean correctos y guarda en la clase
 * ImageSoa los valores para ancho_img, alto_img y vector_colores */
void ImageSoa::Read(std::filesystem::path path) {
    std::ifstream f;
    openFilein(path, f);
    unsigned char fileheader[fileheadersize];
    unsigned char informationheader[informationheadersize];
    f.read(reinterpret_cast<char*>(fileheader), fileheadersize);
    checkHeader(path); // Comprobamos que la cabecera sea correcta llamando a la funcion checkHeader
    f.read(reinterpret_cast<char*>(informationheader), informationheadersize);
    if(informationheader[14] != 24 || informationheader[12] != 1 ||
       informationheader[16] != 0 || informationheader[17] != 0 ||
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
    colores.m_r.resize(ancho_img * alto_img);
    colores.m_b.resize(ancho_img * alto_img);
    colores.m_g.resize(ancho_img * alto_img);
    const int paddingamount = ((4- (ancho_img * 3) % 4) % 4);
    readColor(f, paddingamount);
    f.close();
}

/* Esta función lee una imagen y comprueba que todos los campos de la cabecera sean correctos y guarda en la clase
 * ImageSoa los valores para ancho_img, alto_img y vector_colores */
void ImageSoa::Read2(std::filesystem::path path) {
    std::ifstream f;
    openFilein(path, f);
    unsigned char fileheader[fileheadersize];
    unsigned char informationheader[informationheadersize];
    f.read(reinterpret_cast<char*>(fileheader), fileheadersize);
    checkHeader(path);
    f.read(reinterpret_cast<char*>(informationheader), informationheadersize);
    if(informationheader[14] != 24 || informationheader[12] != 1 ||
       informationheader[16] != 0 || informationheader[17] != 0 ||
       informationheader[18] != 0 || informationheader[19] != 0){
        cerr << "El formato BMP no es válido " << endl;
        f.close();
    }
    int offset = fileheader[10] + (fileheader[11]<<8) + (fileheader[12]<<16) + (fileheader[13]<<24);
    f.seekg(offset,std::ios_base ::beg);
    ancho_img = informationheader[4] + (informationheader[5] << 8) + (informationheader[6] << 16) + (informationheader[7] << 24);
    alto_img = informationheader[8] + (informationheader[9] << 8) + (informationheader[10] << 16) + (informationheader[11] << 24);
    colores.m_r.resize(ancho_img * alto_img);
    colores.m_b.resize(ancho_img * alto_img);
    colores.m_g.resize(ancho_img * alto_img);
    f.close();
}


void ImageSoa::readColor(ifstream &f, const int paddingamount) {
    /* Esta función lee el color de cada píxel y lo guarda dentro del array de estructuras definido para los colores
     * (m_colors)*/
    for (int y = 0; y < alto_img; y++){
        for (int x = 0; x < ancho_img; x++) {
            unsigned char color[3];
            f.read(reinterpret_cast<char*>(color),3);
            colores.m_r[y * ancho_img + x] = static_cast<float>(color[2]) / 255.0f;
            colores.m_g[y * ancho_img + x] = static_cast<float>(color[1]) / 255.0f;
            colores.m_b[y * ancho_img + x] = static_cast<float>(color[0]) / 255.0f;
        }
        f.ignore(paddingamount);
    }
}



void ImageSoa::checkHeader(std::filesystem::path SRC) {
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


void ImageSoa::Export2(ofstream &j, std::filesystem::path SRC, const int paddingamount, const int filesize) {

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

void ImageSoa::openFilein(std::filesystem::path path, ifstream &f) {
    /* función para abrir el archivo origen y ver si hay algún error*/
    f.open(path, ios::in | ios::binary);
    if(!f.is_open()){
        cout << "El fichero no pudo ser abierto" << endl;
        exit(-1);
    }
}

void ImageSoa::openFileout(std::filesystem::path path, ofstream &f) {
    /* función para abrir el archivo destino y ver si hay algún error */
    f.open(path.generic_string(), ios::out | ios::binary);
    if(!f.is_open()){
        cout << "El fichero no pudo ser abierto" << endl;
        exit(-1);
    }
}
float ImageSoa::GetColorRed(int x, int y) const {
    return colores.m_r[y * ancho_img + x];
}

float ImageSoa::GetColorGreen(int x, int y) const {
    return colores.m_g[y * ancho_img + x];
}

float ImageSoa::GetColorBlue(int x, int y) const {
    return colores.m_b[y * ancho_img + x];
}





/*Esta función es llamada por el main del ejecutable de Soa cuando recibe como argumento en nombre de
 * la función que hay que ejecutar (copy, histo, mono, o gauss*/
int ImageSoa::funcion(std::vector<std::filesystem::path> paths, std::filesystem::path outpath, std::string op) {
    for (const auto &path: paths)
    {
        if(op=="copy"){
            ImageSoa copia(0, 0);
            copia.Copy(path, outpath);
            tiempo_ejecucion(copia.load,copia.store,copia.operacion,path,outpath,op);

        }

        if(op=="mono"){
            ImageSoa mono(0, 0);
            mono.GrayScale(path, outpath);
            tiempo_ejecucion(mono.load,mono.store,mono.operacion,path,outpath,op);

        }
        if(op=="histo") {
            ImageSoa histo(0, 0);
            histo.Histograma(path, outpath);
            tiempo_ejecucion(histo.load,histo.store,histo.operacion,path,outpath,op);
        }
        if(op=="gauss"){
            ImageSoa gauss(0, 0);
            gauss.GaussianBlur(path, outpath);
            tiempo_ejecucion(gauss.load,gauss.store,gauss.operacion,path,outpath,op);

        }

    }
    return 0;

}

