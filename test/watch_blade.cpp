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
using namespace std;
//
int main(int argc, char *argv[]){    
   string file_path = argv[1];
    data_load input;
    if(!input.load_data(file_path)){
        cout<<"load success"<<endl;
    }
    vtkDataSet *data = input.get_data();
    for(int i =0; i < data->GetPointData()-> GetNumberOfArrays(); i++){
        cout<<data->GetPointData()->GetArray(i)->GetNumberOfValues()<<endl; 
    }
    
    cout<<"look up table"<<endl;
    vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
    lut->SetNumberOfColors(64);
    lut->SetTableRange(-100,350);
    lut->Build();
    double r = 0, g = 0, b = 0;
    for(int i = 0; i< 61 ; i+=4){
        lut->SetTableValue(i,r,g,b);
        lut->SetTableValue(i+1,r+0.01,g+0.01,b+0.01);
        lut->SetTableValue(i+2,r+0.02,g+0.02,b+0.02);
        lut->SetTableValue(i+3,r+0.03,g+0.03,b+0.03);
        r += 0.04;
        b += 0.04;
        g += 0.04;
    }

    //data->GetPointData()->SetScalars(data->GetPointData()->GetArray(0)); // set the scalar value for visualize

    vtkSmartPointer<vtkWarpVector> warp = vtkSmartPointer<vtkWarpVector>::New();
    warp->SetInputData(data);
    //vtkSmartPointer<vtkDataSet> data_wd = vtkSmartPointer<vtkDataSet> :: NewInstance(warp->GetOutput());
    //data_wd->ShallowCopy(warp->GetOutput());

    //data_wd->GetPointData()->SetScalars(data_wd->GetPointData()->GetArray(0)); // set the scalar value for visualize
    
    vtkSmartPointer<vtkDataSetMapper> planeMapper = vtkSmartPointer<vtkDataSetMapper>::New();
    planeMapper->SetInputData(data);
    planeMapper -> ScalarVisibilityOn();
    planeMapper-> SetLookupTable(lut);

    cout<< "actor" <<endl;
    vtkScalarBarActor *scalarActor = vtkScalarBarActor :: New();
    scalarActor->SetLookupTable(lut);

    vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor> :: New();
    planeActor ->SetMapper(planeMapper);
    cout<< "vtkrender"<<endl;
    vtkSmartPointer<vtkRenderer> ren1 = vtkSmartPointer<vtkRenderer> :: New();
    ren1 -> SetBackground(0.1,0.2,0.4);
    ren1 -> AddActor(planeActor);
    ren1->AddActor2D(scalarActor);

    cout<<"vtkrenderWindow"<<endl;
    vtkSmartPointer<vtkRenderWindow> renWin  = vtkSmartPointer<vtkRenderWindow> :: New();
    renWin->AddRenderer(ren1);
    renWin->SetSize(300,150);


    cout<< "window"<<endl;
    vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor> :: New();
    iren -> SetRenderWindow (renWin);
    iren->Initialize();
    iren->Start();    
    
    return 0;
}
