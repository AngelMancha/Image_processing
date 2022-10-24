//
// Created by navar
//
#include <vector>
#include <fstream>
#include <filesystem>

#ifndef ARCOS_PROYECTO1_IMAGE_AOS_H
#define ARCOS_PROYECTO1_IMAGE_H

#endif //ARCOS_PROYECTO1_IMAGE_AOS_H

struct Color{
    float r,g,b;
    Color();
    Color(float r, float g, float b);
    ~Color();
};

class Image{
public:
    Image(int width, int height); // constructor de la clase
    ~Image();// destructor


    void Copy(std::filesystem::path SRC, std::filesystem::path DEST);
    void GrayScale(std::filesystem::path SRC, std::filesystem::path DST);
    void GaussianBlur(std::filesystem::path SRC, std::filesystem::path DST);
    void Export2(std::ofstream &j, unsigned char *fileheader, unsigned char *informationheader, int paddingamount, int filesize) const;
    void Histograma(std::filesystem::path SRC,std::filesystem::path DST);

private:
    int m_width; // atributo para definit la altura de la imagen en px
    int m_height; // atributo para definit la altura de la imagen en px
    std::vector<Color> m_colors; // vector donde guardamos la ESTRUCTURA de los colores

    static void openFilein(std::filesystem::path path, std::basic_ifstream<char> &f) ;
    static void openFileout(std::filesystem::path path, std::basic_ofstream<char> &f) ;
    void Read(std::filesystem::path path);
    static void checkHeader(std::ifstream &f, const unsigned char *fileheader) ;
    static void checkInformationHeader(std::ifstream &f, const unsigned char *informationheader) ;
    void readColor(std::ifstream &f, int paddingamount);[[nodiscard]] Color GetColor(int x, int y) const;
    void Grey_calculations(std::ifstream &f, const int paddingamount);

    void Gauss_calculations(std::ifstream &f, const int paddingamount, const std::vector<Color> &color_aux);

    std::vector<Color> get_Color_vector();

    void
    open_create_gauss_files(std::filesystem::path &SRC, const std::filesystem::path &DST, std::ifstream &f,
                            std::ofstream &j) const;

    void pixeles_alrededor(const std::vector<Color> &color_aux, int y, int pyxel, float &final_cr, float &final_cg,
                           float &final_cb) const;

    void
    gauss_formula(const std::vector<Color> &color_aux, int y, int pyxel, int sumatorio_s, int sumatorio_t,
                  float &final_cr,
                  float &final_cg, float &final_cb) const;
};