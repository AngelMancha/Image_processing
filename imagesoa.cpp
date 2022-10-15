#include <fstream>
#include <iostream>
#include "Image.h"
#include <cmath>
using namespace std;


Color::Color() : r(0), g(0), b(0) {}
Color::Color(float r, float g, float b): r(r), g(g), b(b) {}
Color::~Color()= default;

Image::Image(int width, int height): m_width(width), m_height(height), m_colors(vector<Color>(width*height)) {}
Image::~Image() = default;

void Image::Read(const char *path) {
    std::ifstream f;
    f.open(path, ios::in | ios::binary);
    if(!f.is_open()){
        cout << "El fichero no pudo ser abierto" << endl;
        exit(-1);
    }

    const int fileheadersize = 14;
    const int informationheadersize = 40;

    unsigned char fileheader[fileheadersize];
    f.read(reinterpret_cast<char*>(fileheader), fileheadersize);

    if(fileheader[0] != 'B' || fileheader[1] != 'M'){
        cerr << "El archivo no es de tipo BMP " << endl;
        f.close();
        return;
    }

    unsigned char informationheader[informationheadersize];
    f.read(reinterpret_cast<char*>(informationheader), informationheadersize);

    m_width = informationheader[4] + (informationheader[5] << 8) + (informationheader[6] << 16) + (informationheader[7] << 24);
    m_height = informationheader[8] + (informationheader[9] << 8) + (informationheader[10] << 16) + (informationheader[11] << 24);
    m_colors.resize(m_width*m_height);
    const int paddingamount = ((4-(m_width*3)%4)%4);
    for (int y = 0; y<m_height; y++){
        for (int x = 0; x < m_width; x++) {
            unsigned char color[3];
            f.read(reinterpret_cast<char*>(color),3);
            m_colors[y*m_width+x].r = static_cast<float>(color[2])/255.0f;
            m_colors[y*m_width+x].g = static_cast<float>(color[1])/255.0f;
            m_colors[y*m_width+x].b = static_cast<float>(color[0])/255.0f;
        }
        f.ignore(paddingamount);
    }
    f.close();
    cout << "El fichero ha sido leido" << endl;
}

bool Image::Copy(const char *SRC, const char* DEST) {
    std::ifstream src(SRC, std::ios::binary);
    std::ofstream dest(DEST, std::ios::binary);
    dest << src.rdbuf();
    return src && dest;
}

int Image::convert(long long n){
    int dec = 0, i = 0, rem;

    while (n!=0) {
        rem = n % 10;
        n /= 10;
        dec += rem * pow(2, i);
        ++i;
    }
    return dec;
}
