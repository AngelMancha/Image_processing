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
    openFile(path, f); // function to open the image and see if there is an error
    // Variable definition
    const int fileheadersize = 14;
    const int informationheadersize = 40;
    unsigned char fileheader[fileheadersize];
    unsigned char informationheader[informationheadersize];
    f.read(reinterpret_cast<char*>(fileheader), fileheadersize);

    if(fileheader[0] != 'B' || fileheader[1] != 'M'){
        cerr << "El archivo no es de tipo BMP " << endl;
        f.close();
        return;
    }


    f.read(reinterpret_cast<char*>(informationheader), informationheadersize);

    if(informationheader[14]!=24 || informationheader[12]!=1 || informationheader[16]!=0 || informationheader[17]!=0 || informationheader[18]!=0 || informationheader[19]!=0){
        cerr << "El archivo no es de tipo BMP " << endl;
        f.close();
        return;
    }

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

void Image::openFile(const char *path, ifstream &f) const {
    f.open(path, ios::in | ios::binary);
    if(!f.is_open()){
        cout << "El fichero no pudo ser abierto" << endl;
        exit(-1);
    }
}

bool Image::Copy(const char *SRC, const char* DEST) {
    Image::Read(SRC);
    std::ifstream src(SRC, std::ios::binary);
    std::ofstream dest(DEST, std::ios::binary);
    dest << src.rdbuf();
    cout << "El fichero ha sido copiado con exito"<<endl;
    return src && dest;
}
