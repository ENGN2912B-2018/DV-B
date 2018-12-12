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
#include <vtkStreamer.h>
#include <vtkStreamTracer.h>
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
    
    cout<<"cell contains ";
//    double x[3];
//    vtkCell* cell0;
//    vtkIdType cellId=1;
//    double tol2=0.01;
//    int subId=1;
//    double pcoords[3];
//    double *weights;
//    data->FindCell(x, cell0, cellId, tol2, subId, pcoords, weights );
//    cout<<pcoords;
        //data->GetCellData()->Print(os);
//    vtkDataArray *dis_arr = data->GetFieldData()->GetArray(0);
    cout<<data->GetFieldData()->GetArray(0)->GetName()<<endl;
//    double* u=data->GetPointData()->GetArray(2);
//    double* v=data->GetPointData()->GetArray(3);
//    double* w=data->GetPointData()->GetArray(4);
  
    vtkCell* cell = data->GetCell(0);
    cout<<"cell success"<<endl;
    cout<< cell->GetPoints()->GetData()->GetNumberOfComponents()<<endl;

    double* tup = cell->GetPoints()->GetData()->GetTuple(0);
    cout<<tup[0]<<" "<<tup[1]<<" "<<tup[2]<<endl;
    double* tup1 = cell->GetPoints()->GetData()->GetTuple(1);
    cout<<tup1[0]<<" "<<tup1[1]<<" "<<tup1[2]<<endl;
    double* tup2 = cell->GetPoints()->GetData()->GetTuple(2);
    cout<<tup2[0]<<" "<<tup2[1]<<" "<<tup2[2]<<endl;
    double* tup3 = cell->GetPoints()->GetData()->GetTuple(3);
    cout<<tup3[0]<<" "<<tup3[1]<<" "<<tup3[2]<<endl;
    double* tup4 = cell->GetPoints()->GetData()->GetTuple(4);
    cout<<tup4[0]<<" "<<tup4[1]<<" "<<tup4[2]<<endl;
    double* tup5 = cell->GetPoints()->GetData()->GetTuple(5);
    cout<<tup5[0]<<" "<<tup5[1]<<" "<<tup5[2]<<endl;
    double* tup6 = cell->GetPoints()->GetData()->GetTuple(6);
    cout<<setprecision(10)<<tup6[0]<<" "<<tup6[1]<<" "<<tup6[2]<<endl;
    double* tup7 = cell->GetPoints()->GetData()->GetTuple(7);
    cout<<setprecision(10)<<tup7[0]<<" "<<tup7[1]<<" "<<tup7[2]<<endl;
      
    double* point0 = data->GetPoint(0);
    cout<<"point location "<<point0[0]<<" "<<point0[1]<<" "<<point0[2]<<endl;
    cout<<"number of array"<<data->GetPointData()->GetNumberOfArrays()<<endl;
    
    // vtkSmartPointer<vtkStreamer> streamer = vtkSmartPointer<vtkStreamer>::New();
    
    //vtkSmartPointer<vtkStreamTracer> streamer = vtkSmartPointer<vtkStreamTracer>::New();
   

   // streamer->SetSourceData(data);
   // streamer->Update();
    cout<<"Set success"<<endl;
    double* pos = new double[3];
    cout<<"01"<<endl;
   // pos = streamer->GetStartPosition();
    cout<<"02"<<endl;
   // cout<<pos[0]<<" "<<pos[1]<<" "<<pos[2]<<endl;
    vtkDataArray *u = data->GetPointData()->GetArray("U"); 
    vtkDataArray *v = data->GetPointData()->GetArray("V");
    vtkDataArray *w = data->GetPointData()->GetArray("W");
    double tv[3];
   // int origin = (int) data->GetCell(0)->GetPointId(0);
    
    pos = data->GetPoint(0);
    tv[0] = u->GetTuple(0)[0];
    tv[1] = v->GetTuple(0)[0];
    tv[2] = w->GetTuple(0)[0];
    //cout<<"the initial velocity "<<tv[0]<<" "<<tv[1]<<" "<<tv[2]<<endl;
    //cout<<u->GetMaxId()<<endl; 
    // cout<<"load u success "<<u->GetTuple(1000)[0]<<" "<<u->GetTuple(10000)[0]<<" "<<u->GetTuple(100000)[0]<<" "<<u->GetTuple(1000000)[0]<<endl;    
    cout<<"load vtu success"<<endl;
    
    vtkSmartPointer<vtkPoints> point_array = vtkSmartPointer<vtkPoints>::New();
    point_array->SetNumberOfPoints(500000);
    point_array->InsertNextPoint(pos);    

    double interval = 0.001;
    double *next_p = new double[3];
    next_p[0] = pos[0]+tv[0]*interval;
    next_p[1] = pos[1]+tv[1]*interval;
    next_p[2] = pos[2]+tv[2]*interval;
    cout<<next_p[0]<<" "<<next_p[1]<<" "<<next_p[2]<<" the vague position"<<endl;
    cout<<"get next position success"<<endl;
    double *pcoord = new double[3];
    vtkIdType PointId = data->FindPoint(next_p);
    cout<<"find and get cell success"<<endl;
    pcoord = data->GetPoint(PointId);
    
    point_array->InsertNextPoint(pcoord);
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
            interval = interval*1.5;
            continue;
        }
        if(PointId<0){
            interval = interval*0.1;
            continue;
        }
        pcoord = data->GetPoint(PointId);
        point_array->InsertNextPoint(pcoord);
        count++;
        cout<<count<<endl;

    }

    vtkSmartPointer<vtkLineSource> line = vtkSmartPointer<vtkLineSource>::New();
    line->SetPoints(point_array);
    line->Update();
    line->SetResolution(10);
    line->Update();

    //cout<<NextCell->GetPoints()->GetData()->GetTuple(0)[0]<<endl;
    //cout<<NextCell->GetPoints()->GetData()->GetTuple(1)[0]<<endl;
    //cout<<NextCell->GetPoints()->GetData()->GetTuple(2)[0]<<endl;
    //cout<<NextCell->GetPoints()->GetData()->GetTuple(3)[0]<<endl;
    //cout<<pcoord[0]<<" "<<pcoord[1]<<" "<<pcoord[2]<<endl;
    
    vtkSmartPointer<vtkPolyDataMapper> LineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    LineMapper->SetInputConnection(line->GetOutputPort());
    vtkSmartPointer<vtkActor> LineActor = vtkSmartPointer<vtkActor>::New();
    LineActor->SetMapper(LineMapper);
    LineActor->GetProperty()->SetLineWidth(8);    



 
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
    planeMapper -> ScalarVisibilityOn();
    planeMapper -> SetScalarModeToUsePointData();
    planeMapper -> SetColorModeToMapScalars();
    //planeMapper->SetScalarModeToUseCellData();
    
   // vtkSmartPointer<vtkActor> sActor = vtkSmartPointer<vtkActor>::New();
   // sActor->SetMapper();
   // sActor->Visibilityon();

    cout<< "actor" <<endl;
    vtkScalarBarActor *scalarActor = vtkScalarBarActor :: New();
    scalarActor->SetLookupTable(planeMapper->GetLookupTable());

    vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor> :: New();
    planeActor ->SetMapper(planeMapper);
    cout<< "vtkrender"<<endl;
    vtkSmartPointer<vtkRenderer> ren1 = vtkSmartPointer<vtkRenderer> :: New();
    ren1 -> SetBackground(0.1,0.2,0.4);
    ren1 -> AddActor(planeActor);
    ren1->AddActor2D(scalarActor);

    ren1->AddActor(LineActor);

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
