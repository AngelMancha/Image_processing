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

class ImageAos{
public:
    ImageAos(int width, int height); // constructor de la clase
    ~ImageAos();// destructor


    void Copy(std::filesystem::path SRC, std::filesystem::path DEST);
    void GrayScale(std::filesystem::path SRC, std::filesystem::path DST);
    void GaussianBlur(std::filesystem::path SRC, std::filesystem::path DST);
    void Histograma(std::filesystem::path SRC,std::filesystem::path DST);
    static int funcion(std::vector<std::filesystem::path> paths, std::filesystem::path outpath, std::string op);



    int ancho_img;
    int alto_img;
    std::vector<Color> vector_colores;
    /*COMUNES A TODAS LAS FUNCIONES*/
    void Read(std::filesystem::path path);

private:
    // atributo para definit la altura de la imagen en px
    // atributo para definit la altura de la imagen en px
    // vector donde guardamos la ESTRUCTURA de los colores
    int load;
    int store;
    int operacion;

    static void openFilein(std::filesystem::path path, std::basic_ifstream<char> &f) ;
    static void openFileout(std::filesystem::path path, std::basic_ofstream<char> &f) ;
    static void checkHeader(std::filesystem::path path) ;
    void readColor(std::ifstream &f, int paddingamount);
    [[nodiscard]] Color GetColor(int x, int y) const;
    void Export2(std::ofstream &j,std::filesystem::path SRC, int paddingamount, int filesize);

    /*GRAY*/
    void Gray_calculations(std::ifstream &f, const int paddingamount);
    void Gray_open_create_files(std::filesystem::path &SRC, const std::filesystem::path &DST, std::ifstream &f, std::ofstream &j) const;
    Color &Gray_operations(float nr, float ng, float nb, Color &c) const;
    void Gray_intensidad(float nr, float ng, float nb, Color &c) const;
    void Read2(std::filesystem::path path);


    /*GAUSS*/
    void Gauss_calculations(std::ifstream &f, const int paddingamount, const std::vector<Color> &color_aux); std::vector<Color> get_Color_vector();
    void Gauss_open_create_files(std::filesystem::path &SRC, const std::filesystem::path &DST, std::ifstream &f, std::ofstream &j) const;
    Color Gauss_operations(const std::vector<Color> &color_aux, int y, int pyxel) const;

    //HISTOGRAMA
    void Histo_count_ocurrencies(std::vector<int> &r_colors, std::vector<int> &g_colors, std::vector<int> &b_colors);
    void Histo_create_output(const std::filesystem::path &SRC, const std::filesystem::path &DST,
                             const std::vector<int> &r_colors, const std::vector<int> &g_colors,
                             const std::vector<int> &b_colors) const;
    void Histo_get_intensities(std::ifstream &f);


};