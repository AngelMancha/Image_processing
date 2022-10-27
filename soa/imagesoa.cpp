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

//
ImageSoa::ImageSoa(int width, int height): m_width(width), m_height(height) {}
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
    m_width = informationheader[4] + (informationheader[5] << 8) + (informationheader[6] << 16) + (informationheader[7] << 24);
    m_height = informationheader[8] + (informationheader[9] << 8) + (informationheader[10] << 16) + (informationheader[11] << 24);
    colores.m_r.resize(m_width*m_height);
    colores.m_g.resize(m_width*m_height);
    colores.m_b.resize(m_width*m_height);
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
    for(int i=0; i < m_width * m_height; ++i){
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
    const int paddingamount = ((4 - (m_width * 3) % 4) % 4);
    //matriz de colores
    for (int y = 0; y < m_height; y++){
        for (int x = 0; x < m_width; x++) {
            unsigned char color[3];
            f.read(reinterpret_cast<char*>(color),3);

            colores.m_r[y * m_width + x] = color[2];
            colores.m_g[y * m_width + x] = color[1];
            colores.m_b[y * m_width + x] = color[0];

        }
        f.ignore(paddingamount);
    }
    f.close();
}

void ImageSoa::GrayScale(std::filesystem::path SRC, std::filesystem::path DST) {
    /*we open the input and output files*/
    auto start = std::chrono::high_resolution_clock::now();
    std::ifstream f;
    std::ofstream j;
    Gray_open_create_files(SRC, DST, f, j);

    /*Leemos el archivo para así obtener el ancho, alto y el vector de colores*/
    ImageSoa::Read2(SRC);
    unsigned char fileheader[fileheadersize];
    unsigned char informationheader[informationheadersize];
    const int paddingamount = ((4-(m_width*3)%4)%4);
    const int filesize = fileheadersize + informationheadersize + m_width * m_height * 3 + paddingamount * m_height;
    f.read(reinterpret_cast<char*>(fileheader), fileheadersize);
    f.read(reinterpret_cast<char*>(informationheader), informationheadersize);
    int offset = fileheader[10] + (fileheader[11]<<8) + (fileheader[12]<<16) + (fileheader[13]<<24);
    f.seekg(offset,std::ios_base ::beg);
    auto end = std::chrono::high_resolution_clock::now();
    load = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    /*Procedemos a realizar los cálculos pertinentes para la conversion a escala de grises*/
    start = std::chrono::high_resolution_clock::now();
    Gray_calculations(f, paddingamount);
    f.close();
    end = std::chrono::high_resolution_clock::now();
    operacion = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    /*Exportamos el archivo al fichero de salida*/
    start = std::chrono::high_resolution_clock::now();
    Export2(j, SRC, paddingamount, filesize);
    end = std::chrono::high_resolution_clock::now();
    store = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

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

/*
Colores ImageSoa::get_Color_vector() {
    Colores color_aux;

    memcpy(&color_aux, &colores);
    return color_aux;
}
 */


/////////////////////////////////////////////////////
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
}

void ImageSoa::Read2(std::filesystem::path path) {
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
    f.close();
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
float ImageSoa::GetColorRed(int x, int y) const {
    return colores.m_r[y*m_width+x];
}

float ImageSoa::GetColorGreen(int x, int y) const {
    return colores.m_g[y*m_width+x];
}

float ImageSoa::GetColorBlue(int x, int y) const {
    return colores.m_b[y*m_width+x];
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
    f.open(path.generic_string(), ios::out | ios::binary);
    //f.open(path, ios::out | ios::binary);
    if(!f.is_open()){
        cout << "El fichero no pudo ser abierto" << endl;
        exit(-1);
    }
}

void ImageSoa::Gauss_open_create_files(filesystem::path &SRC, const filesystem::path &DST, ifstream &f, ofstream &j) const {
    openFilein(SRC, f);/*Escribimos el nombre con el que queremos que se guarde el fichero de salida en el directorio destino*/
    string new_name="gauss_"+(SRC.filename()).string();
    auto target = DST/new_name;
    openFileout(target, j);
}


void ImageSoa::GaussianBlur(std::filesystem::path SRC, std::filesystem::path DST) {
    auto start = chrono::steady_clock::now();
    std::ifstream f;
    std::ofstream j;
    Gauss_open_create_files(SRC, DST, f, j);
    /*Leemos el archivo para así obtener el ancho, alto y el vector de colores*/
    ImageSoa::Read(SRC);

    unsigned char fileheader[fileheadersize];
    unsigned char informationheader[informationheadersize];
    const int paddingamount = ((4 - (m_width * 3) % 4) % 4);
    const int filesize = fileheadersize + informationheadersize + m_width * m_height * 3 + paddingamount * m_height;
    f.read(reinterpret_cast<char *>(fileheader), fileheadersize);
    f.read(reinterpret_cast<char *>(informationheader), informationheadersize);
    auto end = chrono::steady_clock::now();
    load = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    start = chrono::steady_clock::now();
    /*Inicializamos 3 vectores auxiliares para guardar los colores*/
    vector<float> color_aux_red;
    vector<float> color_aux_green;
    vector<float> color_aux_blue;
    gauss_aux_vector(color_aux_red, color_aux_green, color_aux_blue);
    gauss_calculations(f, paddingamount, color_aux_red, color_aux_green, color_aux_blue);
    f.close();
    end = chrono::steady_clock::now();
    operacion = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    /*Exportamos el archivo al fichero de salida*/
    start = chrono::steady_clock::now();
    Export2(j, SRC, paddingamount, filesize);
    end = chrono::steady_clock::now();
    store = chrono::duration_cast<chrono::milliseconds>(end - start).count();
}

void ImageSoa::gauss_calculations(ifstream &f, const int paddingamount, const vector<float> &color_aux_red,
                                  const vector<float> &color_aux_green, const vector<float> &color_aux_blue) {



    for (int y =0; y < m_height; y++) {
        for (int pyxel = 0; pyxel < m_width; pyxel++) {
            float final_cr = 0;
            float final_cg = 0;
            float final_cb = 0;

            gauss_pyxeles_alrededor(color_aux_red, color_aux_green, color_aux_blue, y, pyxel, final_cr, final_cg,
                                    final_cb);/*sumatorio_s*/
            /*metemos los colores finales en el vector m_colors*/
            //cout << "The color is blabalabkansaonsiabd" <<final_cr << endl;
            colores.m_r[y * m_width + pyxel] = final_cr / 273;
            colores.m_g[y * m_width + pyxel] = final_cg / 273;
            colores.m_b[y * m_width + pyxel] = final_cb / 273;
        }
        f.ignore(paddingamount);
    }
}

void ImageSoa::gauss_pyxeles_alrededor(const vector<float> &color_aux_red, const vector<float> &color_aux_green,
                                       const vector<float> &color_aux_blue, int y, int pyxel, float &final_cr,
                                       float &final_cg, float &final_cb) const {
    for (int sumatorio_s = -2; sumatorio_s < 3; sumatorio_s++) {
        for (int sumatorio_t=-2; sumatorio_t < 3; sumatorio_t++) {

            /*Controlamos que el pyxel no esté fuera de los límites de la imagen.
             * De ser así, asignamos 0 a las variables de los colores*/
            if ((pyxel + sumatorio_s > m_width) or (pyxel + sumatorio_s < 0) or (y + sumatorio_t > m_height) or (y + sumatorio_t < 0)) {

                final_cr = final_cr + 0;
                final_cg = final_cg + 0;
                final_cb = final_cb + 0;

            }
                /*cogemos el color del pyxel que está en la posición x = pyxel + sumatorio_s y la posición y = y + sumatorio_t)*/
            else {
                gauss_formula(color_aux_red, color_aux_green, color_aux_blue, y, pyxel, sumatorio_s,
                              sumatorio_t, final_cr, final_cg, final_cb);


            }

        } /*loop sumatorio_t*/
    }
}

void ImageSoa::gauss_formula(const vector<float> &color_aux_red, const vector<float> &color_aux_green,
                             const vector<float> &color_aux_blue, int y, int pyxel, int sumatorio_s, int sumatorio_t,
                             float &final_cr, float &final_cg, float &final_cb) const {
    int mascara[5][5] = {{1, 4,  7,  4,  1},
             {4, 16, 26, 16, 4},
             {7, 26, 41, 26, 7},
             {4, 16, 26, 16, 4},
             {1, 4,  7,  4,  1}};
    float nr = (color_aux_red[((y + sumatorio_t) * m_width) + pyxel + sumatorio_s]);

    float ng = (color_aux_green[((y + sumatorio_t) * m_width) + pyxel + sumatorio_s]);
    float nb = (color_aux_blue[((y + sumatorio_t) * m_width) + pyxel + sumatorio_s]);


    /*Calculamos el color para uno de los 25 pixeles que está alrededor del pyxel (x,y) */
    //cout << "valor de la mascara es" << mascara[sumatorio_s + 2][sumatorio_t + 2] << endl;
    float cr = (mascara[sumatorio_s + 2][sumatorio_t + 2]) *  nr;
    float cg = (mascara[sumatorio_s + 2][sumatorio_t + 2]) *  ng;
    float cb = (mascara[sumatorio_s + 2][sumatorio_t + 2]) *  nb;

    /*le sumamos a la variable que va a recopilar la suma de todos los colores de los 25 pyxeles*/
    final_cr = final_cr + cr;
    final_cg = final_cg + cg;
    final_cb = final_cb + cb;
}

void
ImageSoa::gauss_aux_vector(vector<float> &color_aux_red, vector<float> &color_aux_green,
                           vector<float> &color_aux_blue) {
    for (unsigned long long i=0; i < colores.m_r.size(); i++) {
        color_aux_red.push_back(colores.m_r[i]);
    }
    for (unsigned long long i=0; i < colores.m_g.size(); i++) {
        color_aux_green.push_back(colores.m_g[i]);
    }
    for (unsigned long long i=0; i < colores.m_b.size(); i++) {
        color_aux_blue.push_back(colores.m_b[i]);
    }
}

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

