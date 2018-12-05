#include "read_data/data_load.h"
#include <iostream>
#include <thread>
#include <vtkDataSet.h>

using namespace std;

void read_data(vector<string> paths, vector<vtkDataSet*> &data){
    for (int i = 0; i<paths.size();i++){
        
        data_load input;
        if(!input.load_data(paths[i])){
            cout<<"load data at: "<<paths[i]<<" successfully"<<endl;
        }
        data.push_back(input.get_data());

    }
}
int main(){ 
    vector<string> paths_blade;
    paths_blade.push_back("../../dataset/blade21.vtp");
    paths_blade.push_back("../../dataset/blade22.vtp");
    paths_blade.push_back("../../dataset/blade23.vtp");
    paths_blade.push_back("../../dataset/blade24.vtp");
    paths_blade.push_back("../../dataset/blade25.vtp");
    paths_blade.push_back("../../dataset/blade26.vtp");

    paths_blade.push_back("../../dataset/blade44.vtp");
    paths_blade.push_back("../../dataset/blade46.vtp");
    paths_blade.push_back("../../dataset/blade43.vtp");
    paths_blade.push_back("../../dataset/blade45.vtp");



    string path_air = "../../dataset/air.vtu";
    vector<string> paths_air;
    paths_air.push_back(path_air);
    
    vector<vtkDataSet*> data_blade;
    thread t_air(read_data,paths_blade, ref(data_blade));

    vector<vtkDataSet*> data_air;
    read_data(paths_air,data_air);

    t_air.join();

    for( int i =0; i< data_air.size(); i++){
        cout<<"air_file: "<<i<<endl;
        cout<< data_air[i]->GetPointData()->GetNumberOfArrays()<<endl;
    }
    for( int i =0; i< data_blade.size(); i++){
        cout<<"object_file: "<<i<<endl;
        cout<<data_blade[i]->GetPointData()->GetNumberOfArrays()<<endl;
    }

    return 0;
}
