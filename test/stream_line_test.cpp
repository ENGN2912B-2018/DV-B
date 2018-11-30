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
#include <vtkDataSetMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVertexGlyphFilter.h>


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

    vtkAbstractArray *q_array = data->GetPointData()->GetArray(1);
    

    vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
    lut->SetNumberOfColors(64);
    lut->SetTableRange(-100,350);
    lut->Build();
    //lut->DebugOn();

    vtkDataArray *dis_arr = data->GetFieldData()->GetArray(0);
    cout<<data->GetFieldData()->GetArray(0)->GetName()<<endl;
    cout<<"number of array"<<data->GetPointData()->GetNumberOfArrays()<<endl;;
    data->GetPointData()->RemoveArray(0);
    data->GetPointData()->RemoveArray(1);
    data->GetPointData()->RemoveArray(1);
    data->GetPointData()->RemoveArray(1);
    cout<<"number of array"<<data->GetPointData()->GetNumberOfArrays()<<endl;;
    double r = 0, g = 0, b = 0; 
    for(int i = 0; i< 16; i++){
        lut->SetTableValue(i,r,g,b);
        lut->SetTableValue(i+1,r+0.001,g+0.001,b+0.001);
        lut->SetTableValue(i+2,r+0.002,g+0.002,b+0.002);
        lut->SetTableValue(i+3,r+0.003,g+0.003,b+0.003);
        r += 0.004;
        b += 0.004;
        g += 0.004;
    }
    //vtkCastToConcrete *cast = vtkCastToConcrete :: New();
    
    //vtkDataSetToDataObjectFilter *ds2do = vtkDataSetToDataObjectFilter::New();
    //cout<< "ds2do" <<endl;
    //ds2do->DebugOn();

    //ds2do->SetInputConnection(data_pipe);
    //vtkWarpVector *warp = vtkWarpVector::New();
    //cout<< "warp" <<endl;
    //warp->SetInputConnection(ds2do->GetOutputPort());
    //warp->SetInputData(data);
    //vtkConnectivityFilter *connect = vtkConnectivityFilter :: New();
    //connect->DebugOn();
    //cout<< "connect" <<endl;
    //connect->SetInputConnection(warp->GetOutputPort());
    //connect->SetExtractionModeToAllRegions();

    //vtkPolyData *data_grid = vtkPolyData :: SafeDownCast(data);
    //cout<<data_grid->GetNumberOfPolys()<<endl;

    vtkPointSource seeds;
    seeds->SetRadius();
    seeds->SetCenter(0,0,0);
    seeds->SetNumberOfPoints(data->);
    vtkRungeKutta4 integ;
    vtkStreamTracer streamer;
    streamer->SetInputConnection(data->GetOutputPort()->GetNumberOfPoints());
    streamer->SetSourceConnection(seeds->GetOutputPort());
    streamer->SetMaximumPropagationTime(500);
    streamer->SetMaximumPropagationUnitToTimeUnit();
    streamer->SetInitialIntegrationStep();
    streamer->SetInitialIntegrationStepUnitToCellLengthUnit();
    streamer->SetIntegrationDirectionToBoth();
    streamer->SetIntegrator(integ);

    vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    cout<<"set_input"<<endl;
    vertexFilter->SetInputData(data);
    vertexFilter->Update();
    cout<<"input_over"<<endl;
    
    vtkSmartPointer<vtkPolyData> ploydata = vtkSmartPointer<vtkPolyData>::New();
    ploydata->ShallowCopy(vertexFilter->GetOutput());

    cout<<"ploy_data"<<ploydata->GetPointData()->GetNumberOfArrays()<<endl;
    vtkSmartPointer<vtkDataSetMapper> planeMapper = vtkSmartPointer<vtkDataSetMapper>::New();
    planeMapper->SetLookupTable(lut);
    //planeMapper->DebugOn();
    planeMapper->SetInputData(ploydata);
    //planeMapper->SetScalarModeToUseCellData();
    
    cout<< "actor" <<endl;
    vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor> :: New();
    planeActor->SetMapper(planeMapper);
    
    cout<< "vtkrender"<<endl;
    vtkSmartPointer<vtkRenderer> ren1 = vtkSmartPointer<vtkRenderer> :: New();
    ren1 -> SetBackground(0.1,0.2,0.4);
    ren1->AddActor(planeActor);
    
    cout<<"vtkrender"<<endl;
    vtkSmartPointer<vtkRenderWindow> renWin  = vtkSmartPointer<vtkRenderWindow> :: New();
    renWin->AddRenderer(ren1);
    renWin->SetSize(300,150);

    cout<< "window"<<endl;
    vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor> :: New();
    iren -> SetRenderWindow (renWin);
    //iren->AddObserver(vtkCommand::InteractionEvent,myCallback);
    iren->Initialize();
    iren->Start();
    return 0;
}
