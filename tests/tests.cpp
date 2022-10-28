#include "gtest/gtest.h"
#include "../aos/Image_aos.h"
#include <filesystem>
using namespace std;

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(copia, amphora) {
    std::string path = std::filesystem::current_path().remove_filename().generic_string();
    const std::filesystem::path origin = path + "input\\amphora.bmp";
    const std::filesystem::path copiada = path + "output_aos\\copia_amphora.bmp";
    ImageAos copia(0, 0);
    copia.Read(copiada);
    ImageAos original(0, 0);
    original.Read(origin);

    EXPECT_EQ(copia.alto_img, original.alto_img);
    EXPECT_EQ(copia.ancho_img, original.ancho_img);

}

/*
TEST(copia, amphora2) {
    std::string path = std::filesystem::current_path().remove_filename().generic_string();
    const std::filesystem::path orignial = path + "input\\amphora.bmp";
    const std::filesystem::path copiada = path + "output_aos\\copia_amphora.bmp";
    ImageAos copia(0,0);
    copia.Read(orignial);
    ImageAos original(0,0);
    original.Read(copiada);
    for (int x = 0; x<copia.alto_img*copia.ancho_img; x++){

        EXPECT_EQ(copia.vector_colores[x].r,original.vector_colores[x].r);
        EXPECT_EQ(copia.vector_colores[x].b,original.vector_colores[x].b);
        EXPECT_EQ(copia.vector_colores[x].g,original.vector_colores[x].g);
        }
}
*/