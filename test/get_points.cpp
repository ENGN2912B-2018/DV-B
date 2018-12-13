#include "read_data/data_load.h"
#include <iostream>

#include <vtkSmartPointer.h>
#include <vtkDataSet.h>
#include <vtkUnstructuredGrid.h>
#include <vtkRectilinearGrid.h>
#include <vtkImageData.h>
#include <vtkPolyData.h>
#include <vtkStructuredGrid.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkFieldData.h>
#include <vtkCellTypes.h>
#include <vtksys/SystemTools.hxx>
#include <vtkLookupTable.h>
#include <vtkPolyDataMapper.h>
#include <vtkDataSetToDataObjectFilter.h>
#include <vtkConnectivityFilter.h>
#include <vtkWarpVector.h>
#include <vtkCastToConcrete.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkDataSetMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkScalarBarActor.h>
#include <vtkCamera.h>
//#include "vtkScalarBarActor.h"
#include <map>
using namespace std;

int main(){
    string file_path = "../../dataset/air.vtu";
    data_load input;
    if(!input.load_data(file_path)){
        cout<<"load success"<<endl;
    }
    vtkDataSet *data = input.get_data();
    double i = 0, y_min = INT_MAX;
    vector<double> ids;
    while(i < data->GetNumberOfPoints()){
        double *pos;
        pos = data->GetPoint(i); 
        if(pos[1]==y_min){
            ids.push_back(i);
            cout<<"x: "<<pos[0]<<" y: "<< pos[1]<<" z: "<<pos[2]<<endl;
        }
        else if(pos[1] < y_min){
        y_min = pos[1];
        ids.resize(0);
        ids.push_back(i);
        cout<<"fresh min; clear"<<endl;
        cout<<"x: "<<pos[0]<<" y: "<< pos[1]<<" z: "<<pos[2]<<endl;
        }
        i++;
    }
    return 0;
}
