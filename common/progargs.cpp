#include <iostream>
#include <cstring>
#include <dirent.h>
#include "../soa/Image_soa.h"

using namespace std;

void errores(int argc, char *argv[]) {
    if(argc != 3) {
        cerr << "Wrong format:\nimage in_path out_path oper\noperation: copy, histo, mono, gauss";
    }

    if(strcmp(argv[2], "copy") != 0  || strcmp(argv[2], "histo") != 0 || strcmp(argv[2], "mono") != 0 || strcmp(argv[2], "gauss") !=0){
        cerr << "Unexpected operation:"<<argv[2]<<"\nimage in_path out_path oper\noperation: copy, histo, mono, gauss";
    }

    DIR *dirin = opendir(argv[0]);
    if(dirin == nullptr){
        cerr << "Input path: "<<argv[0]<<"\nOutput path: "<<argv[1]<<"\nCannot open directory: "<<argv[0]<<"\nimage in_path out_path oper\noperation: copy, histo, mono, gauss";
    }

    DIR *dirout = opendir(argv[1]);
    if(dirout == nullptr){
        cerr << "Input path: "<<argv[0]<<"\nOutput path: "<<argv[1]<<"\nOutput directory"<<argv[1]<<"does not exist\nimage in_path out_path oper\noperation: copy, histo, mono, gauss";
    }

}

void tiempo_ejecucion(float tload, float tstore, float top, DIR *dirin, DIR *dirout,char archivo, char op){
    cout << "Input path: "<<dirin<<"\nOutput path: "<<dirout<<"File: "<<archivo<<" (time: "<<tload+top+tstore<<")\nLoad time: "<<tload;
    cout << op<<" time: "<<top<<"\nStore time: "<<tstore<<"\n";
}

int main(){

};