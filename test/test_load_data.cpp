#include "read_data/data_load.h"
#include <iostream>

using namespace std;

int main(){
    string file_path = "../../dataset/air.vtu";
    data_load input;
    if(!input.load_data(file_path)){
        cout<<"load success"<<endl;
    }

    return 0;
}
