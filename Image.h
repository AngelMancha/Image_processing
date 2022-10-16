#include <vector>
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

private:
    int m_width;
    int m_height;
    std::vector<Color> m_colors;

};