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
#include <vtkStreamTracer.h>
#include <vtkAttributeDataToFieldDataFilter.h>
#include <map>
VTK_MODULE_INIT(vtkRenderingFreeType)
using namespace std;

int main(){
    string file_path = "../../dataset/1.vtu";
    data_load input;
    if(!input.load_data(file_path)){
        cout<<"load success"<<endl;
    }
    vtkDataSet *data = input.get_data();


    vtkAbstractArray *q_array = data->GetPointData()->GetArray(1);
   
    vtkDataArray *nut = data->GetPointData()->GetArray(1);
    vtkDataArray *k = data->GetPointData()->GetArray(2);
    vtkDataArray *epsilon = data->GetPointData()->GetArray(3);
    vtkDataArray *U = data->GetPointData()->GetArray(4);
    cout<<"field "<<U->GetNumberOfComponents()<<endl;

    cout<<"U value "<<U->GetTuple(12120)[0]<<" "<<U->GetTuple(12120)[1]<<" "<<U->GetTuple(12120)[2]<<" "<<U->GetTuple(1102)[0]<<endl;
    
    vtkSmartPointer<vtkStreamTracer> tracer = vtkSmartPointer<vtkStreamTracer>::New();
    tracer->SetInputData(data);
    tracer->SetSourceData(data);
    tracer->Update();
    double *tp = tracer->GetStartPosition();    
    cout<<"intial position "<<tp[0]<<" "<<tp[1]<<" "<<tp[2]<<endl;
     

    vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
    lut->SetNumberOfColors(64);
    lut->SetTableRange(-100,350);
    lut->Build();


    cout<<"number of array"<<data->GetPointData()->GetNumberOfArrays()<<endl;;
    data->GetPointData()->RemoveArray(0);  
    data->GetPointData()->RemoveArray(0);
    data->GetPointData()->RemoveArray(0);
    data->GetPointData()->RemoveArray(0);
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
    vtkSmartPointer<vtkStreamTracer> stream = vtkSmartPointer<vtkStreamTracer>::New();
    stream->SetInputData(data);
    stream->SetStartPosition(data->GetPoint(5000));
    cout<<"initial position "<<data->GetPoint(5000)[0]<<" "<<data->GetPoint(5000)[1]<<" "<<data->GetPoint(5000)[2]<<endl;
    stream->SetIntegratorTypeToRungeKutta4();
    stream->SetMaximumPropagation(200);

    stream->SetInitialIntegrationStep(0.1);

    stream->Update();
    
    vtkSmartPointer<vtkPolyDataMapper> streamMapper = vtkSmartPointer<vtkPolyDataMapper>::New();

    streamMapper->SetInputData(stream->GetOutput());
    streamMapper->Update();
    vtkSmartPointer<vtkActor> streamActor = vtkSmartPointer<vtkActor>::New();
    streamActor->SetMapper(streamMapper);   
    streamActor->VisibilityOn();

    cout<<"set_input"<<endl;
    cout<<"actor "<<endl;
    vtkSmartPointer<vtkRenderer> ren1 = vtkSmartPointer<vtkRenderer> :: New();
    ren1 -> SetBackground(0.1,0.2,0.4);
    ren1 -> AddActor(streamActor);
    cout<<"vtkrender"<<endl;
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

