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
#include <vtkPlane.h>
#include <vtkCutter.h>
#include <vtkDataSetMapper.h>
#include <vtkClipPolyData.h>

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
   // data->DebugOn();

    //vtkDataSet *data_q = vtkDataSet :: New();

    vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
    lut->SetNumberOfColors(64);
    lut->SetTableRange(-100,350);
    lut->Build();
    //lut->DebugOn();

    vtkDataArray *dis_arr = data->GetFieldData()->GetArray(0);
    cout<<data->GetFieldData()->GetArray(0)->GetName()<<endl;
    cout<<"number of array"<<data->GetPointData()->GetNumberOfArrays()<<endl;;
    data->GetPointData()->RemoveArray(1);  // delete the data that we do not want to visualize.
    data->GetPointData()->RemoveArray(1);
    data->GetPointData()->RemoveArray(1);
    data->GetPointData()->RemoveArray(1);
    cout<<"number of array"<<data->GetPointData()->GetNumberOfArrays()<<endl;;
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
    vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    cout<<"set_input"<<endl;
    vertexFilter->SetInputData(data);
    vertexFilter->Update();
    cout<<"input_over"<<endl;
    vtkSmartPointer<vtkPolyData> ploydata = vtkSmartPointer<vtkPolyData>::New(); //make the data into vertex
    ploydata->ShallowCopy(vertexFilter->GetOutput());
    ploydata->GetPointData()->SetScalars(ploydata->GetPointData()->GetArray(0)); // set the scalar value for visualize

    vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
    
    double *center = data->GetCenter();
    for(int i =0; i <3 ; i++){
        cout<<center[i]<<" cen"<<endl;
    }
    
    double *bound = data->GetBounds();
    for(int i =0; i <5 ; i++){
        cout<<center[i]<<" boun"<<endl;
    }
    double n1[] = {9,1,1};
    double n2[] = {1,0,0};
    plane -> SetOrigin(n1);
    //plane -> SetNormal(n2);
    //vtkSmartPointer<vtkClipPolyData> planecut = vtkSmartPointer<vtkClipPolyData>::New();
    vtkSmartPointer<vtkCutter> planecut = vtkSmartPointer<vtkCutter>::New();

    //planecut->GetOutput()->GetPointData()->SetScalars(planecut->GetOutput()->GetPointData()->GetArray(0));

    planecut -> SetInputData(data);
    planecut -> SetCutFunction(plane);
    planecut -> Update();

    cout<<planecut->GetOutputPointsPrecision()<<" prec"<<endl;
    cout<<planecut ->GetOutput()-> GetNumberOfCells()<<" number"<<endl;

    //vtkSmartPointer<vtkWarpVector> warp = vtkSmartPointer<vtkWarpVector>::New();
    //warp -> SetInputData(planecut->GetOutput());
    //vtkSmartPointer<vtkDataSet> data_wd = vtkSmartPointer<vtkDataSet> :: NewInstance(warp->GetOutput());
    

    //cout<<"ploy_data"<<ploydata->GetPointData()->GetNumberOfArrays()<<endl;
    vtkSmartPointer<vtkPolyDataMapper> planeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    planeMapper->SetInputData(planecut->GetOutput());
    planeMapper -> ScalarVisibilityOn();
    planeMapper -> SetScalarModeToUsePointData();
    planeMapper -> SetColorModeToMapScalars();

    cout<< "actor" <<endl;
    vtkSmartPointer<vtkScalarBarActor> scalarActor = vtkSmartPointer<vtkScalarBarActor> :: New();
    scalarActor->SetLookupTable(planeMapper->GetLookupTable());

    vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor> :: New();
    planeActor ->SetMapper(planeMapper);
    cout<< "vtkrender"<<endl;
    vtkSmartPointer<vtkRenderer> ren1 = vtkSmartPointer<vtkRenderer> :: New();
    ren1 -> SetBackground(0.1,0.2,0.4);
    ren1 -> AddActor(planeActor);
    ren1->AddActor2D(scalarActor);

    cout<<"vtkrender"<<endl;
    vtkSmartPointer<vtkRenderWindow> renWin  = vtkSmartPointer<vtkRenderWindow> :: New();
    renWin->AddRenderer(ren1);
    renWin->SetSize(900,900);


    cout<< "window"<<endl;
    vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor> :: New();
    iren -> SetRenderWindow (renWin);
   

    iren->Initialize();
    iren->Start();
    return 0;
}













