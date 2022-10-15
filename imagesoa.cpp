#include <fstream>
#include <iostream>
#include "Image.h"
using namespace std;


Color::Color() : r(0), g(0), b(0) {}
Color::Color(float r, float g, float b): r(r), g(g), b(b) {}
Color::~Color()= default;

Image::Image(int width, int height): m_width(width), m_height(height), m_colors(vector<Color>(width*height)) {}
Image::~Image() = default;

Color Image::GetColor(int x, int y) const {
    return m_colors[y*m_width+x];
}

void Image::SetColor(const Color &color, int x, int y) {
    m_colors[y*m_width+x].r = color.r;
    m_colors[y*m_width+x].g = color.r;
    m_colors[y*m_width+x].b = color.r;
}


void Image::Read(const char *path) {
    std::ifstream f;
    f.open(path, std::ios::in | std::ios::binary);
    if(!f.is_open()){
        cout << "El fichero no pudo ser abierto" << endl;
        return;
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

    int filesize = fileheader[2] + (fileheader[3]<<8) + (fileheader[4] << 16) + (fileheader[5] << 24);
    m_width = informationheader[4] + (informationheader[5] << 8) + (informationheader[6] << 16) + (informationheader[7] << 24);
    m_height = informationheader[8] + (informationheader[9] << 8) + (informationheader[10] << 16) + (informationheader[11] << 24);
    m_colors.resize(m_width*m_width);
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

void Image::Export(const char* path) const {
    std::ofstream f;
    f.open(path, std::ios::out | std::ios::binary);
    if(!f.is_open()){
        cerr << "Fichero no pudo ser abierto" << endl;
        return;
    }

    unsigned char bmpPad[3]={0,0,0};
    const int paddinamount = ((4-(m_width*3)%4)%4);

    const int fileheadersize = 14;
    const int informationheadersize = 40;
    const int filesize = fileheadersize+informationheadersize;

    unsigned char fileheader[fileheadersize];
    fileheader[0] = 'B';
    fileheader[1]='M';

    fileheader[2]=filesize;
    fileheader[3]=filesize>>8;
    fileheader[4]=filesize>>16;
    fileheader[5]=filesize>>24;

    fileheader[6]=0;
    fileheader[7]=0;
    fileheader[8]=0;
    fileheader[9]=0;

    fileheader[10]=fileheadersize+informationheadersize;
    fileheader[11]=0;
    fileheader[12]=0;
    fileheader[13]=0;

    unsigned char informationheader[informationheadersize];

    informationheader[0]= informationheadersize;
    informationheader[1]=0;
    informationheader[2]=0;
    informationheader[3]=0;

    informationheader[4]=m_width;
    informationheader[5]=m_width>>8;
    informationheader[6]=m_width>>16;
    informationheader[7]=m_width>>24;

    informationheader[8]=m_height;
    informationheader[9]=m_height >> 8;
    informationheader[10]=m_height >> 16;
    informationheader[11]=m_height >> 24;

    informationheader[12]=1;
    informationheader[13]=0;

    informationheader[14]=24;
    informationheader[15]=0;

    informationheader[16]=0;informationheader[17]=0;informationheader[18]=0;informationheader[19]=0;
    informationheader[20]=0;informationheader[21]=0;informationheader[22]=0;informationheader[23]=0;
    informationheader[24]=0;informationheader[25]=0;informationheader[26]=0;informationheader[27]=0;
    informationheader[28]=0;informationheader[29]=0;informationheader[30]=0;informationheader[31]=0;
    informationheader[32]=0;informationheader[33]=0;informationheader[34]=0;informationheader[35]=0;
    informationheader[36]=0;informationheader[37]=0;informationheader[38]=0;informationheader[39]=0;

    f.write(reinterpret_cast<char*>(fileheader),fileheadersize);
    f.write(reinterpret_cast<char*>(informationheader),informationheadersize);

    for (int y = 0; y<m_height; y++){
        for (int x = 0; x < m_width; x++) {
            unsigned char r = static_cast<unsigned char>(GetColor(x,y).r*225.0f);
            unsigned char g = static_cast<unsigned char>(GetColor(x,y).g*225.0f);
            unsigned char b = static_cast<unsigned char>(GetColor(x,y).b*225.0f);

            unsigned char color[]= {b, g, r};
            f.write(reinterpret_cast<char*>(color), 3);
        }

        f.write(reinterpret_cast<char*>(bmpPad), paddinamount);
    }

    f.close();
    cout << "Archivo copiado\n";
}
