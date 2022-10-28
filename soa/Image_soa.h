#include <vector>
#include <fstream>
#include <filesystem>
#ifndef ARCOS_PROYECTO1_IMAGE_SOA_H
#define ARCOS_PROYECTO1_IMAGE_H

#endif //ARCOS_PROYECTO1_IMAGE_SOA_H



struct Colores{
    std::vector<float> m_r;
    std::vector<float> m_g;
    std::vector<float> m_b;
    Colores();
    Colores(std::vector<float> m_r,std::vector<float> m_g,std::vector<float> m_b);
    ~Colores();
};

struct parametros{
    float r,g,b;
    parametros();
    parametros(float r, float g, float b);
    ~parametros();
};


class ImageSoa{
public:
    ImageSoa(int width, int height); // constructor de la clase
    ~ImageSoa();// destructor

    void GaussianBlur(std::filesystem::path SRC, std::filesystem::path DST);
    void Copy(std::filesystem::path SRC, std::filesystem::path DEST);
    void GrayScale(std::filesystem::path SRC, std::filesystem::path DST);
    void Export2(std::ofstream &j,std::filesystem::path SRC, int paddingamount, int filesize) ;
    void Histograma(std::filesystem::path SRC, std::filesystem::path DST);
    //Elegir funcion
    static int funcion(std::vector<std::filesystem::path> paths, std::filesystem::path outpath, std::string operation);

private:
    int ancho_img; // atributo para definit la altura de la imagen en px
    int alto_img; // atributo para definit la altura de la imagen en px
    Colores colores; //   ESTRUCTURA DE ARRAYS
    int load;
    int store;
    int operacion;

    static void openFilein(std::filesystem::path path, std::basic_ifstream<char> &f);
    static void openFileout(std::filesystem::path path, std::basic_ofstream<char> &f) ;
    void Read(std::filesystem::path path);
    void Read2(std::filesystem::path path);
    static void checkHeader(std::filesystem::path path) ;

    void readColor(std::ifstream &f, int paddingamount);
    //[[nodiscard]] Colores GetColor(int x, int y) const;
    float GetColorRed(int x, int y) const;
    float GetColorGreen(int x, int y) const;
    float GetColorBlue(int x, int y) const;


    //HISTOGRAMA
    void Histo_count_ocurrencies(std::vector<int> &r_colors, std::vector<int> &g_colors, std::vector<int> &b_colors);
    void Histo_create_output(const std::filesystem::path &SRC, const std::filesystem::path &DST,
                             const std::vector<int> &r_colors, const std::vector<int> &g_colors,
                             const std::vector<int> &b_colors) const;
    void Histo_get_intensities(std::ifstream &f);
    parametros Gray_operations(float nr, float ng, float nb) const;
    parametros &Gray_intensidad(float nr, float ng, float nb, parametros &c) const;

    /*GRAY*/
    void Gray_calculations(std::ifstream &f, const int paddingamount);
    void Gray_open_create_files(std::filesystem::path &SRC, const std::filesystem::path &DST, std::ifstream &f, std::ofstream &j) const;


    /*GAUSS*/
    void Gauss_open_create_files(std::filesystem::path &SRC, const std::filesystem::path &DST, std::ifstream &f,std::ofstream &j) const;
    void Gauss_Calculations(std::ifstream &f, const int paddingamount, const Colores &color_aux);
    parametros &Gauss_operations(const Colores &color_aux, int y, int pyxel, parametros &final) const;
    Colores Gauss_auxiliarvector();




};