//
// Created by navar on 10/19/2022.
//
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
    int m_width; // atributo para definit la altura de la imagen en px
    int m_height; // atributo para definit la altura de la imagen en px
    Colores colores; //   ESTRUCTURA DE ARRAYS
    int load;
    int store;
    int operacion;

    static void openFilein(std::filesystem::path path, std::basic_ifstream<char> &f);
    static void openFileout(std::filesystem::path path, std::basic_ofstream<char> &f) ;
    void Read(std::filesystem::path path);
    void Read2(std::filesystem::path path);
    static void checkHeader(std::ifstream &f, const unsigned char *fileheader) ;
    static void checkInformationHeader(std::ifstream &f, const unsigned char *informationheader) ;
    void readColor(std::ifstream &f, int paddingamount);
    //[[nodiscard]] Colores GetColor(int x, int y) const;
    /*GRAY*/
    void Gray_calculations(std::ifstream &f, const int paddingamount);
    void Gray_open_create_files(std::filesystem::path &SRC, const std::filesystem::path &DST, std::ifstream &f, std::ofstream &j) const;
    float Gray_formula(float nr, float ng, float nb, float cr, float cg, float cb) const;
    void Gray_intensidad_lineal(float nr, float ng, float nb, float &cr, float &cg, float &cb) const;

    /*GAUSS*/


    float GetColorRed(int x, int y) const;
    float GetColorGreen(int x, int y) const;
    float GetColorBlue(int x, int y) const;

    void Gauss_open_create_files(std::filesystem::path &SRC, const std::filesystem::path &DST, std::ifstream &f,
                                 std::ofstream &j) const;

    void gauss_aux_vector(std::vector<float> &color_aux_red, std::vector<float> &color_aux_green,
                          std::vector<float> &color_aux_blue);

    void gauss_calculations(std::ifstream &f, const int paddingamount, const std::vector<float> &color_aux_red,
                            const std::vector<float> &color_aux_green, const std::vector<float> &color_aux_blue);

    void gauss_pyxeles_alrededor(const std::vector<float> &color_aux_red, const std::vector<float> &color_aux_green,
                                 const std::vector<float> &color_aux_blue, int y, int pyxel, float &final_cr,
                                 float &final_cg, float &final_cb) const;

    void gauss_formula(const std::vector<float> &color_aux_red, const std::vector<float> &color_aux_green,
                       const std::vector<float> &color_aux_blue, int y, int pyxel, int sumatorio_s, int sumatorio_t,
                       float &final_cr, float &final_cg, float &final_cb) const;

    //HISTOGRAMA
    void Histo_count_ocurrencies(std::vector<int> &r_colors, std::vector<int> &g_colors, std::vector<int> &b_colors);

    void Histo_create_output(const std::filesystem::path &SRC, const std::filesystem::path &DST,
                             const std::vector<int> &r_colors, const std::vector<int> &g_colors,
                             const std::vector<int> &b_colors) const;

    void Histo_get_intensities(std::ifstream &f);


};