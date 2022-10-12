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
    [[nodiscard]] Color GetColor(int x, int y) const;

    void SetColor(const Color& color, int x, int y);
    void Read(const char* path);

    void Export(const char* path) const;
private:
    int m_width;
    int m_height;
    std::vector<Color> m_colors;
};