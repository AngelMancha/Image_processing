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
    Image copia(0,0);
    copia.Read(copiada);
    Image original(0,0);
    original.Read(origin);

    EXPECT_EQ(copia.m_height,original.m_height);
    EXPECT_EQ(copia.m_width,original.m_width);

}

/*
TEST(copia, amphora2) {
    std::string path = std::filesystem::current_path().remove_filename().generic_string();
    const std::filesystem::path orignial = path + "input\\amphora.bmp";
    const std::filesystem::path copiada = path + "output_aos\\copia_amphora.bmp";
    Image copia(0,0);
    copia.Read(orignial);
    Image original(0,0);
    original.Read(copiada);
    for (int x = 0; x<copia.m_height*copia.m_width; x++){

        EXPECT_EQ(copia.m_colors[x].r,original.m_colors[x].r);
        EXPECT_EQ(copia.m_colors[x].b,original.m_colors[x].b);
        EXPECT_EQ(copia.m_colors[x].g,original.m_colors[x].g);
        }
}
*/