//
// Created by navar on 10/19/2022.
//
#include <vector>
#include <fstream>

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
    void Export(const char *path) const;



private:
    int m_width; // atributo para definit la altura de la imagen en px
    int m_height; // atributo para definit la altura de la imagen en px
    std::vector<Color> m_colors; // vector donde guardamos la ESTRUCTURA de los colores

    static void openFile(const char *path, std::basic_ifstream<char> &f) ;
    void Read(const char* path);
    static void checkHeader(std::ifstream &f, const unsigned char *fileheader) ;
    static void checkInformationHeader(std::ifstream &f, const unsigned char *informationheader) ;
    void readColor(std::ifstream &f, const int paddingamount);
    Color GetColor(int x, int y) const;

};