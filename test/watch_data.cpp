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
#include <math.h>
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
//    cout<< cell->GetPoints()->GetData()->GetNumberOfComponents()<<endl;
//
//    double* tup = cell->GetPoints()->GetData()->GetTuple(0);
//    cout<<tup[0]<<" "<<tup[1]<<" "<<tup[2]<<endl;
//    double* tup1 = cell->GetPoints()->GetData()->GetTuple(1);
//    cout<<tup1[0]<<" "<<tup1[1]<<" "<<tup1[2]<<endl;
//    double* tup2 = cell->GetPoints()->GetData()->GetTuple(2);
//    cout<<tup2[0]<<" "<<tup2[1]<<" "<<tup2[2]<<endl;
//    double* tup3 = cell->GetPoints()->GetData()->GetTuple(3);
//    cout<<tup3[0]<<" "<<tup3[1]<<" "<<tup3[2]<<endl;
//    double* tup4 = cell->GetPoints()->GetData()->GetTuple(4);
//   cout<<tup4[0]<<" "<<tup4[1]<<" "<<tup4[2]<<endl;
//    double* tup5 = cell->GetPoints()->GetData()->GetTuple(5);
//    cout<<tup5[0]<<" "<<tup5[1]<<" "<<tup5[2]<<endl;
//    double* tup6 = cell->GetPoints()->GetData()->GetTuple(6);
//    cout<<setprecision(10)<<tup6[0]<<" "<<tup6[1]<<" "<<tup6[2]<<endl;
//    double* tup7 = cell->GetPoints()->GetData()->GetTuple(7);
//    cout<<setprecision(10)<<tup7[0]<<" "<<tup7[1]<<" "<<tup7[2]<<endl;
      
//    double* point0 = data->GetPoint(0);
//    cout<<"point location "<<point0[0]<<" "<<point0[1]<<" "<<point0[2]<<endl;
//    cout<<"number of array"<<data->GetPointData()->GetNumberOfArrays()<<endl;
    
    // vtkSmartPointer<vtkStreamer> streamer = vtkSmartPointer<vtkStreamer>::New();
    
    //vtkSmartPointer<vtkStreamTracer> streamer = vtkSmartPointer<vtkStreamTracer>::New();
   

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
    int origin = (int) data->GetCell(220212)->GetPointId(0);
    cout<<origin<<" the initial location"<<endl;
    pos = data->GetPoint(origin);
    tv[0] = u->GetTuple(origin)[0];
    tv[1] = v->GetTuple(origin)[0];
    tv[2] = w->GetTuple(origin)[0];
    //cout<<"the initial velocity "<<tv[0]<<" "<<tv[1]<<" "<<tv[2]<<endl;
    //cout<<u->GetMaxId()<<endl; 
    cout<<"load vtu success"<<endl;
    
    vtkSmartPointer<vtkPoints> point_array = vtkSmartPointer<vtkPoints>::New();
    point_array->SetNumberOfPoints(52000);
    point_array->InsertNextPoint(pos);    
    
    double interval = 0.001;
    double *next_p = new double[3];
    next_p[0] = pos[0]+tv[0]*interval;
    next_p[1] = pos[1]+tv[1]*interval;
    next_p[2] = pos[2]+tv[2]*interval;
    //cout<<next_p[0]<<" "<<next_p[1]<<" "<<next_p[2]<<" the vague position"<<endl;
    cout<<"get next position success"<<endl;
    double *pcoord = new double[3];
    //vtkIdType PointId = data->FindPoint(next_p);
  
    int subId;
    double pcoords[3];
    double weights[8];
    vtkIdType CellId = data->FindCell(pos,NULL,0,0.01,subId,pcoords,weights);
    //cout<<" weights0 "<<weights[0]<<" "<<weights[1]<<" "<<weights[2]<<" "<<weights[3]<<" "<<weights[4]<<" "<<weights[5]<<" "<<weights[6]<<" "<<weights[7]<<endl;
   
    cout<<"check begin"<<endl;
    CellId = data->FindCell(next_p,NULL,0,0.01,subId,pcoords,weights);
    //cout<<" weights0 "<<weights[0]<<" "<<weights[1]<<" "<<weights[2]<<" "<<weights[3]<<" "<<weights[4]<<" "<<weights[5]<<" "<<weights[6]<<" "<<weights[7]<<endl;
    cout<<next_p[0]<<" "<<next_p[1]<<" "<<next_p[2];
        
    cell = data->GetCell(CellId);
    double* tup = cell->GetPoints()->GetData()->GetTuple(0);
    cout<<tup[0]<<" "<<tup[1]<<" "<<tup[2]<<endl;
    double* tup2 = cell->GetPoints()->GetData()->GetTuple(1);
    cout<<tup2[0]<<" "<<tup2[1]<<" "<<tup2[2]<<endl;
    double* tup3 = cell->GetPoints()->GetData()->GetTuple(2);
    cout<<tup3[0]<<" "<<tup3[1]<<" "<<tup3[2]<<endl;
    double* tup4 = cell->GetPoints()->GetData()->GetTuple(4);
    cout<<tup4[0]<<" "<<tup4[1]<<" "<<tup4[2]<<endl;
    cout<<"check ends"<<endl;
    //cout<<cell->GetPointId(0)<<" "<<cell->GetPointId(1)<<" "<<cell->GetPointId(2)<<" "<<cell->GetPointId(3)<<" "<<cell->GetPointId(4)<<" "<<cell->GetPointId(5)<<" "<<cell->GetPointId(6)<<" "<<cell->GetPointId(7)<<endl;
    //pcoord[0] = (cell->GetPoints()->GetData()->GetTuple(0)[0]+cell->GetPoints()->GetData()->GetTuple(1)[0]+cell->GetPoints()->GetData()->GetTuple(2)[0]+cell->GetPoints()->GetData()->GetTuple(4)[0])/4;
    //pcoord[1] = (cell->GetPoints()->GetData()->GetTuple(0)[1]+cell->GetPoints()->GetData()->GetTuple(1)[1]+cell->GetPoints()->GetData()->GetTuple(2)[1]+cell->GetPoints()->GetData()->GetTuple(4)[1])/4;
    //pcoord[2] = (cell->GetPoints()->GetData()->GetTuple(0)[2]+cell->GetPoints()->GetData()->GetTuple(1)[2]+cell->GetPoints()->GetData()->GetTuple(2)[2]+cell->GetPoints()->GetData()->GetTuple(4)[2])/4;
    pcoord = next_p;
    cout<<"find and get cell success"<<endl;
    //pcoord = data->GetPoint(PointId);
    
    double r1, r2, r3, r4;
    double d1, d2, d3, d4;
    point_array->InsertNextPoint(pcoord);
    vtkIdType count = 2;
    double bound[6];
    data->GetBounds(bound);
    cout<<"bound is "<<bound[0]<<" "<<bound[1]<<" "<<bound[2]<<" "<<bound[3]<<" "<<bound[4]<<" "<<bound[5]<<endl;
    cout<<"point2 "<<pcoord[0]<<" "<<pcoord[1]<<" "<<pcoord[2]<<endl;
    while(count<=5000){

        pos = pcoord;
    //    int index = (int)PointId;
        
       // tv[0] = (u->GetTuple(cell->GetPointId(0))[0]+u->GetTuple(cell->GetPointId(1))[0]+u->GetTuple(cell->GetPointId(2))[0]+u->GetTuple(cell->GetPointId(4))[0])/4;
       // tv[1] = (v->GetTuple(cell->GetPointId(0))[0]+v->GetTuple(cell->GetPointId(1))[0]+v->GetTuple(cell->GetPointId(2))[0]+v->GetTuple(cell->GetPointId(4))[0])/4;
       // tv[2] = (w->GetTuple(cell->GetPointId(0))[0]+w->GetTuple(cell->GetPointId(1))[0]+w->GetTuple(cell->GetPointId(2))[0]+w->GetTuple(cell->GetPointId(4))[0])/4;
        //tv[0] = u->GetTuple(cell->GetPointId(0))[0]*weights[0]+u->GetTuple(cell->GetPointId(1))[0]*weights[1]+u->GetTuple(cell->GetPointId(2))[0]*(weights[2]+weights[3])+u->GetTuple(cell->GetPointId(4))[0]*(weights[4]+weights[5]+weights[6]+weights[7]);
        //tv[1] = v->GetTuple(cell->GetPointId(0))[0]*weights[0]+v->GetTuple(cell->GetPointId(1))[0]*weights[1]+v->GetTuple(cell->GetPointId(2))[0]*(weights[2]+weights[3])+v->GetTuple(cell->GetPointId(4))[0]*(weights[4]+weights[5]+weights[6]+weights[7]);
        //tv[2] = w->GetTuple(cell->GetPointId(0))[0]*weights[0]+w->GetTuple(cell->GetPointId(1))[0]*weights[1]+w->GetTuple(cell->GetPointId(2))[0]*(weights[2]+weights[3])+w->GetTuple(cell->GetPointId(4))[0]*(weights[4]+weights[5]+weights[6]+weights[7]);

        d1 = sqrt(pow((cell->GetPoints()->GetData()->GetTuple(0)[0]-pos[0]),2)+pow((cell->GetPoints()->GetData()->GetTuple(0)[1]-pos[1]),2)+pow((cell->GetPoints()->GetData()->GetTuple(0)[2]-pos[2]),2)); 
        d2 = sqrt(pow((cell->GetPoints()->GetData()->GetTuple(1)[0]-pos[0]),2)+pow((cell->GetPoints()->GetData()->GetTuple(1)[1]-pos[1]),2)+pow((cell->GetPoints()->GetData()->GetTuple(1)[2]-pos[2]),2));         
        d3 = sqrt(pow((cell->GetPoints()->GetData()->GetTuple(2)[0]-pos[0]),2)+pow((cell->GetPoints()->GetData()->GetTuple(2)[1]-pos[1]),2)+pow((cell->GetPoints()->GetData()->GetTuple(2)[2]-pos[2]),2));        
        d4 = sqrt(pow((cell->GetPoints()->GetData()->GetTuple(4)[0]-pos[0]),2)+pow((cell->GetPoints()->GetData()->GetTuple(4)[1]-pos[1]),2)+pow((cell->GetPoints()->GetData()->GetTuple(4)[2]-pos[2]),2));

        r1 = (1/d1)/(1/d1+1/d2+1/d3+1/d4);
        r2 = (1/d2)/(1/d1+1/d2+1/d3+1/d4);
        r3 = (1/d3)/(1/d1+1/d2+1/d3+1/d4);
        r4 = (1/d4)/(1/d1+1/d2+1/d3+1/d4);
        
        cout<<"caluculation ends"<<count<<endl;
        
        tv[0] = u->GetTuple(cell->GetPointId(0))[0]*r1+u->GetTuple(cell->GetPointId(1))[0]*r2+u->GetTuple(cell->GetPointId(2))[0]*r3+u->GetTuple(cell->GetPointId(4))[0]*r4;
        tv[1] = v->GetTuple(cell->GetPointId(0))[0]*r1+v->GetTuple(cell->GetPointId(1))[0]*r2+v->GetTuple(cell->GetPointId(2))[0]*r3+v->GetTuple(cell->GetPointId(4))[0]*r4;
        tv[2] = w->GetTuple(cell->GetPointId(0))[0]*r1+w->GetTuple(cell->GetPointId(1))[0]*r2+w->GetTuple(cell->GetPointId(2))[0]*r3+w->GetTuple(cell->GetPointId(4))[0]*r4;
 
        
        next_p[0] = pos[0]+tv[0]*interval;
        next_p[1] = pos[1]+tv[1]*interval;
        next_p[2] = pos[2]+tv[2]*interval;
        cout<<"np "<<next_p[0]<<" "<<next_p[1]<<" "<<next_p[2]<<endl;
   //     PointId = data->FindPoint(next_p);
   //     if(index==(int)PointId){
   //         interval = interval*1.5;
   //         continue;
   //     }
   //     if(PointId<0){
   //         interval = interval*0.1;
   //         continue;
   //     }
   //     pcoord = data->GetPoint(PointId);
   //     point_array->InsertNextPoint(pcoord);
    
    vtkIdType P_CellId = CellId;
    cout<<"cell id is "<<CellId<<endl;
  
    CellId = data->FindCell(next_p,NULL,0,0.1,subId,pcoords,weights);
  
   //cout<<" weights "<<weights[0]<<" "<<weights[1]<<" "<<weights[2]<<" "<<weights[3]<<" "<<weights[4]<<" "<<weights[5]<<" "<<weights[6]<<" "<<weights[7]<<endl;
  
//   if(CellId==P_CellId){
//   	interval = interval*2;
//   	continue;
//   }
   if(CellId<0){
        cout<<"cell id is "<<CellId<<endl;
   	interval = interval*0.1;
        break;
    }
   
    cell = data->GetCell(CellId);
    //pcoord[0] = (cell->GetPoints()->GetData()->GetTuple(0)[0]+cell->GetPoints()->GetData()->GetTuple(1)[0]+cell->GetPoints()->GetData()->GetTuple(2)[0]+cell->GetPoints()->GetData()->GetTuple(4)[0])/4;
    //pcoord[1] = (cell->GetPoints()->GetData()->GetTuple(0)[1]+cell->GetPoints()->GetData()->GetTuple(1)[1]+cell->GetPoints()->GetData()->GetTuple(2)[1]+cell->GetPoints()->GetData()->GetTuple(4)[1])/4;
    //pcoord[2] = (cell->GetPoints()->GetData()->GetTuple(0)[2]+cell->GetPoints()->GetData()->GetTuple(1)[2]+cell->GetPoints()->GetData()->GetTuple(2)[2]+cell->GetPoints()->GetData()->GetTuple(4)[2])/4;
   //cout<<"location"<<"count"<<" "<<pcoord[0]<<" "<<pcoord[1]<<" "<<pcoord[2]<<endl;
   // pcoord = data->GetPoint(PointId);
    pcoord = next_p;
    point_array->InsertNextPoint(pcoord);
    count++;
    cout<<count<<endl;
    interval=0.01;
    }

    vtkSmartPointer<vtkLineSource> line = vtkSmartPointer<vtkLineSource>::New();
    line->SetPoints(point_array);
    line->Update();
    line->SetResolution((int)count-1);
    line->Update();
 
    //vtkSmartPointer<vtkPloyLineSource> PolyLine= vtkSmartPointer<vtkPolyLineSource>::New();
    //PolyLine->SetPoints();
    //PolyLine->SetResolution();
    //PolyLine->Updata();


    
    vtkSmartPointer<vtkPolyDataMapper> LineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    LineMapper->SetInputConnection(line->GetOutputPort());
    vtkSmartPointer<vtkActor> LineActor = vtkSmartPointer<vtkActor>::New();
    LineActor->SetMapper(LineMapper);
    LineActor->GetProperty()->SetLineWidth(5);

        



 
    //data->GetPointData()->RemoveArray(1);  // delete the data that we do not want to visualize.
    //data->GetPointData()->RemoveArray(1);
    //data->GetPointData()->RemoveArray(1);
    //data->GetPointData()->RemoveArray(1);
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
