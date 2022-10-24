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


    bool Copy(const char *SRC, const char* DEST);
    void GrayScale(const char* SRC, const char* DST);
    void GaussianBlur(const char* SRC, const char* DST);
    void Export(const char *path) const;
    void Export2(std::ofstream &j, unsigned char *fileheader, unsigned char *informationheader, int paddingamount, int filesize) const;
    void Histograma(const char* path,const char *end);

private:
    int m_width; // atributo para definit la altura de la imagen en px
    int m_height; // atributo para definit la altura de la imagen en px
    std::vector<Color> m_colors; // vector donde guardamos la ESTRUCTURA de los colores

    static void openFilein(const char *path, std::basic_ifstream<char> &f) ;
    static void openFileout(const char *path, std::basic_ofstream<char> &f) ;
    void Read(const char* path);
    static void checkHeader(std::ifstream &f, const unsigned char *fileheader) ;
    static void checkInformationHeader(std::ifstream &f, const unsigned char *informationheader) ;
    void readColor(std::ifstream &f, int paddingamount);[[nodiscard]] Color GetColor(int x, int y) const;
    void Grey_calculations(std::ifstream &f, const int paddingamount);
};