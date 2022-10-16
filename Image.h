#include <vector>
#include <fstream>

#ifndef ARCOS_PROYECTO1_IMAGE_H
#define ARCOS_PROYECTO1_IMAGE_H

#endif //ARCOS_PROYECTO1_IMAGE_H

struct Color{
    float r,g,b;
    Color();
    Color(float r, float g, float b);
    ~Color();
};

class Image{
public:
    Image(int width, int height);
    ~Image();

    void Read(const char* path);
    bool Copy(const char *SRC, const char* DEST);
    bool GrayScale(const char* path);

private:
    int m_width;
    int m_height;
    std::vector<Color> m_colors;

    void openFile(const char *path, std::basic_ifstream<char> &f) const;

    void checkHeader(std::ifstream &f, const int fileheadersize, const int informationheadersize,
                     const unsigned char *fileheader, const unsigned char *informationheader) const;

    void checkHeader(std::ifstream &f, const unsigned char *fileheader) const;

    void checkInformationHeader(std::ifstream &f, const unsigned char *informationheader) const;

    void readColor(std::ifstream &f, const int paddingamount);

    void getWidthHeight(const unsigned char *informationheader);
};