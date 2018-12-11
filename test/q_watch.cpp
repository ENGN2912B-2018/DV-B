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
   // data->DebugOn();

    //vtkDataSet *data_q = vtkDataSet :: New();

    vtkAbstractArray *q_array = data->GetPointData()->GetArray(1);
    


    //lut->DebugOn();

    vtkDataArray *dis_arr = data->GetFieldData()->GetArray(0);
    cout<<data->GetFieldData()->GetArray(0)->GetName()<<endl;
    cout<<"number of array"<<data->GetPointData()->GetNumberOfArrays()<<endl;;
    data->GetPointData()->RemoveArray(0);  // delete the data that we do not want to visualize.
    data->GetPointData()->RemoveArray(1);
    data->GetPointData()->RemoveArray(1);
    data->GetPointData()->RemoveArray(1);
    cout<<"number of array"<<data->GetPointData()->GetNumberOfArrays()<<endl;;


    vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    cout<<"set_input"<<endl;
    vertexFilter->SetInputData(data);
    vertexFilter->Update();
    cout<<"input_over"<<endl;
    
    vtkSmartPointer<vtkPolyData> ploydata = vtkSmartPointer<vtkPolyData>::New(); //make the data into vertex
    ploydata->ShallowCopy(vertexFilter->GetOutput());   

    

    ploydata->GetPointData()->SetScalars(ploydata->GetPointData()->GetArray(0)); // set the scalar value for visualize

    cout<<"ploy_data"<<ploydata->GetPointData()->GetNumberOfArrays()<<endl;
    vtkSmartPointer<vtkPolyDataMapper> planeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    //planeMapper->SetLookupTable(lut);
    //planeMapper->DebugOn();
    planeMapper->SetInputData(ploydata);
    //planeMapper -> ScalarVisibilityOn();
    //planeMapper -> SetScalarModeToUsePointData();
    //planeMapper -> SetColorModeToMapScalars();
    //planeMapper->SetScalarModeToUseCellData();





    double* arr_range = data->GetPointData()->GetArray(0)->GetRange();
    cout<<arr_range[0]<<" arr_range0"<<endl;
    cout<<arr_range[1]<<" arr_range1"<<endl;
    

    //planeMapper->SetScalarRange(2,2);

    vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
    //lut->SetNumberOfColors(planeMapper->GetLookupTable()->GetNumberOfAvailableColors());
    //lut->IndexedLookupOff();
    //lut->SetHueRange(0.6,0.6);
    //lut->SetSaturationRange(0,0);
    //lut->SetTableRange(0,arr_range[1]);
    //lut->SetValueRange(0,1);
    //lut->SetNanColor(0,0.2,0.3,0);
    //lut->SetSaturationRange(1,0);
    //lut->SetBelowRangeColor(0,0.2,0.4,1);
    //lut->SetAboveRangeColor(0,3,0.2,1);
    lut->SetNumberOfColors(256);
    lut->SetTableRange(0,0.5);
    lut->SetRange(0,4);
    //lut->SetRange()
    lut->Build();
    double interval = 1;
    double r = 256, g = 256, b = 256;
    for(int i = 0; i< 253 ; i+=4){
        lut->SetTableValue(i,r,g,b);
        lut->SetTableValue(i+1,r-interval,g-interval,b-interval);
        lut->SetTableValue(i+2,r-interval*2,g-interval*2,b-interval*2);
        lut->SetTableValue(i+3,r-interval*3,g-interval*3,b-interval*3);
        r -= interval * 4;
        b -= interval * 4;
        g -= interval * 4;
    }
    lut->SetTableValue(0,0,0,0,0);
    //lut->SetBelowRangeColor(122,133,5,0);
    

    double* range = lut->GetRange();
    cout<<range[0]<<" range0"<<endl;
    cout<<range[1]<<" range1"<<endl;
    




     cout<<lut->GetNumberOfTableValues()<<" table Value"<<endl;

    //planeMapper->GetLookupTable()->UsingLogScale();
    //planeMapper->GetLookupTable()->SetVectorMode(0);
    //planeMapper->GetLookupTable()->SetRange(0,0);
    //planeMapper->GetLookupTable()->Build();
    
    cout<< "actor" <<endl;
    vtkScalarBarActor *scalarActor = vtkScalarBarActor :: New();
    scalarActor->SetLookupTable(lut);
    scalarActor->SetLabelFormat("%6.4g");
    scalarActor->SetNumberOfLabels(3);

    //double* range = planeMapper->GetLookupTable()->GetRange();
    //cout<<range[0]<<" range0"<<endl;
    //cout<<range[1]<<" range1"<<endl;
    planeMapper->SetLookupTable(lut);
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
    renWin->SetSize(300,150);

    cout<< "window"<<endl;
    vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor> :: New();
    iren -> SetRenderWindow (renWin);
    
    //iren->AddObserver(vtkCommand::InteractionEvent,myCallback);
    iren->Initialize();
    iren->Start();
    return 0;
}
