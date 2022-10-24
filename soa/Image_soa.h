//
// Created by navar on 10/19/2022.
//
#include <vector>
#include <fstream>

#ifndef ARCOS_PROYECTO1_IMAGE_SOA_H
#define ARCOS_PROYECTO1_IMAGE_H

#endif //ARCOS_PROYECTO1_IMAGE_SOA_H



struct Colores{
    std::vector<int> m_r;
    std::vector<int> m_g;
    std::vector<int> m_b;
    Colores();
    Colores(std::vector<int> m_r,std::vector<int> m_g,std::vector<int> m_b);
    ~Colores();
};

class ImageSoa{
public:
    ImageSoa(int width, int height); // constructor de la clase
    ~ImageSoa();// destructor


    bool Copy(const char *SRC, const char* DEST);
    void GrayScale(const char* SRC, const char* DST);
    void Export(const char *path) const;



private:
    int m_width; // atributo para definit la altura de la imagen en px
    int m_height; // atributo para definit la altura de la imagen en px
    Colores colores; //   ESTRUCTURA DE ARRAYS

    static void openFilein(const char *path, std::basic_ifstream<char> &f) ;
    static void openFileout(const char *path, std::basic_ofstream<char> &f) ;
    void Read(const char* path);
    static void checkHeader(std::ifstream &f, const unsigned char *fileheader) ;
    static void checkInformationHeader(std::ifstream &f, const unsigned char *informationheader) ;
    void readColor(std::ifstream &f, int paddingamount);
    [[nodiscard]] Colores GetColor(int x, int y) const;

    void Export2(std::ofstream &j, unsigned char *fileheader, unsigned char *informationheader, int paddingamount,
                 int filesize) const;
};