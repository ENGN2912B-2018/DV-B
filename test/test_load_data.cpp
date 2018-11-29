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

#include <map>
using namespace std;

int main(){
    string file_path = "../../dataset/air.vtu";
    data_load input;
    if(!input.load_data(file_path)){
        cout<<"load success"<<endl;
    }
    vtkDataSet *data = input.get_data();
    data->DebugOn();

    vtkLookupTable *lut = vtkLookupTable::New();
    lut->SetNumberOfColors(64);
    lut->SetTableRange(-15,35);
    lut->Build();
    lut->DebugOn();

    vtkDataArray *dis_arr = data->GetFieldData()->GetArray(0);
    cout<<data->GetFieldData()->GetArray(0)->GetName()<<endl;
    double r = 10, g = 10, b = 10; 
    for(int i = 0; i< 16;i ++){
        lut->SetTableValue(i,r,g,b);
        lut->SetTableValue(i+1,r+10,g+10,b+10);
        lut->SetTableValue(i+2,r+20,g+20,b+20);
        lut->SetTableValue(i+3,r+30,g+30,b+30);
        r += 40;
        b += 40;
        g += 40;
    }
    vtkCastToConcrete *cast = vtkCastToConcrete :: New();
    
    //vtkDataSetToDataObjectFilter *ds2do = vtkDataSetToDataObjectFilter::New();
    //cout<< "ds2do" <<endl;
    //ds2do->DebugOn();

    //ds2do->SetInputConnection(data_pipe);
    vtkWarpVector *warp = vtkWarpVector::New();
    cout<< "warp" <<endl;
    //warp->SetInputConnection(ds2do->GetOutputPort());
    warp->SetInputData(data);
    vtkConnectivityFilter *connect = vtkConnectivityFilter :: New();
    connect->DebugOn();
    cout<< "connect" <<endl;
    connect->SetInputConnection(warp->GetOutputPort());
    connect->SetExtractionModeToAllRegions();


    vtkPolyDataMapper *planeMapper = vtkPolyDataMapper::New();
    planeMapper->SetLookupTable(lut);
    planeMapper->DebugOn();

    planeMapper->SetInputConnection(connect->GetOutputPort());
    vtkActor *planeActor = vtkActor :: New();
    planeActor->SetMapper(planeMapper);



    return 0;
}
