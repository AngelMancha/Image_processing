#include <iostream>
#include "../soa/Image_soa.h"
#include "../aos/Image_aos.h"
//comment
using namespace std;


void errores(int numargs, std::string_view indir, std::string_view outdir, std::string_view operation) {

    std::filesystem::path inpath(indir);
    std::filesystem::path outpath(outdir);


    if(numargs != 4) {
        cerr << "Wrong format:\nimage in_path out_path oper\noperation: copy, histo, mono, gauss";
    }

    if((operation != "copy") && (operation != "histo") && (operation != "gauss") && (operation != "mono")) {
        cerr << "Unexpected operation:" << operation << "\nimage in_path out_path oper \noperation: copy, histo, mono, gauss";
    }

    if(!std::filesystem::exists(inpath)){
        cerr << "Input path: " << inpath << "\nOutput path: " << outdir << "\nCannot open directory: " <<"["<<inpath<<"]" << "\nimage in_path out_path oper\noperation: copy, histo, mono, gauss";
    }


    if(!std::filesystem::exists(outpath)){
        cerr << "Input path: " << outpath << "\nOutput path: " << outpath << "\nCannot open directory: " <<"["<<outpath<<"]" << "\nimage in_path out_path oper\noperation: copy, histo, mono, gauss";
    }

}

void tiempo_ejecucion(float tload, float tstore, float top, std::filesystem::path inpath, std::filesystem::path outpath, std::string_view op){

    cout << "Input path: "<<inpath<<"\nOutput path: "<<outpath<<"\nFile: "<<inpath.filename()<<" (time: "<<tload+top+tstore<<")\nLoad time: "<<tload<<endl;
    cout << op<<" time: "<<top<<endl;
    cout<<"Store time: "<<tstore<<"\n";
}

std::vector<std::filesystem::path> getImgPaths(const std::string& path){
    std::vector<std::filesystem::path> archivosBMP;
    for (const std::filesystem::directory_entry & entrada: std::filesystem::directory_iterator(path))
    {
        if(entrada.path().extension()==".bmp")
        {
            archivosBMP.push_back(entrada.path());
        }
    }
    return archivosBMP;
}
