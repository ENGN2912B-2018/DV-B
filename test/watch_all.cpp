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
#include <vtkLight.h>

using namespace std;

int main(){
    int j = 0;
    vector<string> paths;
    paths.push_back("../../dataset/air.vtu");
    paths.push_back("../../dataset/blade21.vtp");
    paths.push_back("../../dataset/blade22.vtp");
    paths.push_back("../../dataset/blade23.vtp");
    paths.push_back("../../dataset/blade24.vtp");
    paths.push_back("../../dataset/blade25.vtp");
    paths.push_back("../../dataset/blade26.vtp");

    paths.push_back("../../dataset/blade44.vtp");
    paths.push_back("../../dataset/blade46.vtp");
    paths.push_back("../../dataset/blade43.vtp");
    paths.push_back("../../dataset/blade45.vtp");

    vector<vtkDataSet*> datasets(paths.size());
    data_load input;
    while(j < paths.size()){
    string file_path = paths[j];
    if(!input.load_data(file_path)){
        cout<<"load success"<<endl;
    }
    datasets[j] = input.get_data();
    for(int i =0; i < datasets[j]->GetPointData()-> GetNumberOfArrays(); i++){
        cout<<datasets[j]->GetPointData()->GetArray(i)->GetNumberOfValues()<<endl;
    }
    j++;
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
    //vtkScalarBarActor *scalarActor = vtkScalarBarActor :: New();
    //scalarActor->SetLookupTable(lut);


    cout<< "vtkrender"<<endl;
    vtkSmartPointer<vtkRenderer> ren1 = vtkSmartPointer<vtkRenderer> :: New();
    ren1 -> SetBackground(0.1,0.2,0.4);
    j = 1;
    while(j < datasets.size()){
    vtkSmartPointer<vtkDataSet> data = datasets[j];
   // vtkSmartPointer<vtkWarpVector> warp = vtkSmartPointer<vtkWarpVector>::New();
    //warp->SetInputData(data);

    vtkSmartPointer<vtkDataSetMapper> planeMapper = vtkSmartPointer<vtkDataSetMapper>::New();
    planeMapper->SetInputData(data);
    planeMapper -> ScalarVisibilityOff();
    //planeMapper-> SetLookupTable(lut);
    cout<< "actor" <<endl;
    vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor> :: New();
    planeActor ->SetMapper(planeMapper);
    //scalarActor->SetLookupTable(planeMapper->GetLookupTable());
    ren1 -> AddActor(planeActor);
    j++;
    }

    vtkSmartPointer<vtkDataSet> data = datasets[0];
    data->GetPointData()->RemoveArray(1);  // delete the data that we do not want to visualize.
    data->GetPointData()->RemoveArray(1);
    data->GetPointData()->RemoveArray(1);
    data->GetPointData()->RemoveArray(1);
    
    double *bound_copy = data -> GetBounds();
    double cut_pos [3];
    
    double devi[3];
    double norm[3];
    devi[0] = 0;
    devi[1] = 0;
    devi[2] = 0;
    norm[0] = 0;
    norm[1] = 1;
    norm[2] = 0;

    for(int i =0; i<3 ;i++){
        cut_pos[i] = bound_copy[i] + devi[i];
    }

    

    vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    cout<<"set_input"<<endl;
    vertexFilter->SetInputData(data);
    vertexFilter->Update();
    cout<<"input_over"<<endl;
    //vtkSmartPointer<vtkPolyData> ploydata = vtkSmartPointer<vtkPolyData>::New(); //make the data into vertex
    //ploydata->ShallowCopy(vertexFilter->GetOutput());
   // ploydata->GetPointData()->SetScalars(ploydata->GetPointData()->GetArray(0)); // set the scalar value for visualize

    vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
    //double n2[] = {0,1,0};
    plane -> SetOrigin(devi);
    plane -> SetNormal(norm);
    vtkSmartPointer<vtkCutter> planecut = vtkSmartPointer<vtkCutter>::New();

    planecut -> SetInputData(data);
    planecut -> SetCutFunction(plane);
    planecut -> Update();

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
    ren1->AddActor(planeActor);

    ren1->AddActor2D(scalarActor);

    vtkCamera *camera = ren1->GetActiveCamera();
    camera->SetPosition(-50,54,9.5);
    camera->SetFocalPoint(1.65,10,7);
    camera->SetViewUp(0,0,1);
    camera->SetViewAngle(30);
    
    vtkSmartPointer<vtkLight> light = vtkSmartPointer<vtkLight>::New();
    light->SetFocalPoint(1.65,10,7);
    light->SetPosition(50,54,9.5);
    //ren1->AddLight(light);

    ren1 -> SetActiveCamera(camera);
    
    cout<<"vtkrenderWindow"<<endl;
    vtkSmartPointer<vtkRenderWindow> renWin  = vtkSmartPointer<vtkRenderWindow> :: New();
    renWin->AddRenderer(ren1);
    renWin->SetSize(300,150);
    renWin->Render();

    cout<< "window"<<endl;
    vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor> :: New();
    iren -> SetRenderWindow (renWin);
    iren->Initialize();
    iren->Start();

    return 0;
}




