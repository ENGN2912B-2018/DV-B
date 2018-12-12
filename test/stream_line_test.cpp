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
#include <vtkLineSource.h>
#include <vtkProperty.h>

//#include "vtkScalarBarActor.h"
#include <map>
VTK_MODULE_INIT(vtkRenderingFreeType)
using namespace std;

int main(){
    string file_path = "../../dataset/air.vtu";
    data_load input;
    if(!input.load_data(file_path)){
        cout<<"load success"<<endl;
    }
    vtkDataSet *data = input.get_data();
//Get the start point
    double* pos = new double[3];
    pos = data->GetPoint(0);

    vtkDataArray *u = data->GetPointData()->GetArray("U");
    vtkDataArray *v = data->GetPointData()->GetArray("V");
    vtkDataArray *w = data->GetPointData()->GetArray("W");
    
    double tv[3];
    tv[0] = u->GetTuple(0)[0];
    tv[1] = v->GetTuple(0)[0];
    tv[2] = w->GetTuple(0)[0];
    
    vtkSmartPointer<vtkPoints> point_array = vtkSmartPointer<vtkPoints> point_array;
    point_array->SetNumberOfPoints(500000);
    vtkId PointId = point_array->SetNextPoint(pos);

    double interval = 0.01;
    double next_p* = new double[3];
    next_p[0] = pos[0]+tv[0]*interval;
    next_p[1] = pos[1]+tv[1]*interval;
    next_p[2] = pos[2]+tv[2]*interval;

    vtkIdType PointId = data->FindPoint(next_p);

    double *pcoords = new double[3];
    pcoord = data->GetPoint(PointId);

    point_array->SetNextPoint(pcoord);
    vtkIdType count = 2;
    while(count<=100){

        pos = pcoord;
        int index = (int)PointId;
        tv[0] = u->GetTuple(index)[0];
        tv[1] = v->GetTuple(index)[0];
        tv[2] = w->GetTuple(index)[0];
    
        next_p[0] = pos[0]+tv[0]*interval;
        next_p[1] = pos[1]+tv[1]*interval;
        next_p[2] = pos[2]+tv[2]*interval;
    
        PointId = data->FindPoint(next_p);
        if(index==(int)PointId){
            interval = interval*2;
            continue;
        }
        if(pointId<0){
            interval = interval*0.1;
            continue;
        }
        pcoord = data->GetPoint(PointId);
        point_array->SetNextPoint(pcoord);
        count++;

    }

    vtkSmartPointer<vtkLineSource> line = vtkSmartPointer<vtkLineSource>::New();
    line->SetPoints(point_array);
    line->Update();
    line->SetResolution(10);
    line->Update();

    vtkSmartPointer<vtkPolyDataMapper> LineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    LineMapper->SetInputConnection(line->GetOutputPort());
    vtkSmartPointer<vtkActor> LineActor = vtkSmartPointer<vtkActor>::New();
    LineActor->SetMapper(LineMapper);
    LineActor->GetProperty()->SetLineWidth(8);



    cout<< "vtkrender"<<endl;
    vtkSmartPointer<vtkRenderer> ren1 = vtkSmartPointer<vtkRenderer> :: New();
    ren1 -> SetBackground(0.1,0.2,0.4);
    ren1->AddActor(LinActor);

    cout<<"vtkrender"<<endl;
    vtkSmartPointer<vtkRenderWindow> renWin  = vtkSmartPointer<vtkRenderWindow> :: New();
    renWin->AddRenderer(ren1);
    renWin->SetSize(300,150);

    //vtkCamera *camera = ren1->GetActiveCamera();   //set the camera
    //camera->Azimuth(0);
    //camera->Elevation(0);
    //camera->SetDistance(200);
    //renWin -> Render();
    cout<< "window"<<endl;
    vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor> :: New();
    iren -> SetRenderWindow (renWin);
    //iren->AddObserver(vtkCommand::InteractionEvent,myCallback);
    iren->Initialize();
    iren->Start();
    return 0;
}

