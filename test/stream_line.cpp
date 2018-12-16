#include "read_data/data_load.h"
#include <iostream>

#include <vtkPolyLine.h>
#include <unordered_map>
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
//#include <vtkStreamer.h>
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

    double i = 0, y_min = INT_MAX;
    vector<int> ids;
    while(i < data->GetNumberOfPoints()){
        double *pos;
        pos = data->GetPoint(i);
        if(pos[1]==y_min){
            ids.push_back(i);
          //  cout<<"x: "<<pos[0]<<" y: "<< pos[1]<<" z: "<<pos[2]<<endl;
        }
        else if(pos[1] < y_min){
        y_min = pos[1];
        ids.resize(0);
        ids.push_back(i);
        //cout<<"fresh min; clear"<<endl;
        //cout<<"x: "<<pos[0]<<" y: "<< pos[1]<<" z: "<<pos[2]<<endl;
        }
        i++;
    }
    cout<< "ids_get y_min:"<<y_min<<endl;
    vector<vector<int>> x_lines;
    unordered_map<double, int> x_dic;
    for (int i =0; i< ids.size(); i++){
        double *pos;
        pos = data->GetPoint(ids[i]);
        if(x_dic.count(pos[0])){
            x_lines[x_dic[pos[0]]].push_back(ids[i]);
        }
        else{
            vector<int> line;
            line.push_back(ids[i]);
            x_lines.push_back(line);
            x_dic[pos[0]] = x_lines.size()-1;
        }
    }        
    cout<<"x_lines get "<<x_lines.size()<<endl;
    cout<<"x_lines 0 0 "<<x_lines[0][0]<<endl;
    cout<<"ids 0 0"<<ids[0]<<endl;
    vector<vector<int>> z_lines;
    unordered_map<double, int> z_dic;
    for (int i =0; i< ids.size(); i++){
        double *pos;
        pos = data->GetPoint(ids[i]);
        if(z_dic.count(pos[2])){
            z_lines[z_dic[pos[2]]].push_back(ids[i]);
        }
        else{
            vector<int> line;
            line.push_back(ids[i]);
            z_lines.push_back(line);
            z_dic[pos[2]] = z_lines.size()-1;
        }
    }        
    cout<<"get z"<<endl;
    double* pos;

    double r1, r2, r3, r4;
    double d1, d2, d3, d4;
    double tv[3];
    double interval = 0.001;
    double *next_p = new double[3];
    int subId;
    double pcoords[3];
    double weights[5];
    vtkSmartPointer<vtkRenderer> ren1 = vtkSmartPointer<vtkRenderer> :: New();

    vtkDataArray *u = data->GetPointData()->GetArray("U"); 
    vtkDataArray *v = data->GetPointData()->GetArray("V");
    vtkDataArray *w = data->GetPointData()->GetArray("W");

    vector<int> cur_lines= x_lines[x_dic[1.2532]];
    for (int j =0 ; j<cur_lines.size(); j++){
    int origin = cur_lines[j];
    pos = data->GetPoint(origin);
    vtkSmartPointer<vtkPoints> point_array = vtkSmartPointer<vtkPoints>::New();
    //point_array->SetNumberOfPoints(52000);
    point_array->InsertNextPoint(pos);    

    cout<<origin<<" the initial location"<<endl;

    //caculate second point=============================
    tv[0] = u->GetTuple(origin)[0];
    tv[1] = v->GetTuple(origin)[0];
    tv[2] = w->GetTuple(origin)[0];
    
    next_p[0] = pos[0]+tv[0]*interval;
    next_p[1] = pos[1]+tv[1]*interval;
    next_p[2] = pos[2]+tv[2]*interval;
    
    point_array->InsertNextPoint(next_p);   

    vtkIdType CellId = data->FindCell(next_p,NULL,0,0.01,subId,pcoords,weights);
    vtkSmartPointer<vtkCell> cell = data->GetCell(CellId);
    cout<<"cell_get: "<< CellId<<endl;
    vtkIdType count = 1;
    for (int i = 0; i< 3 ;i++){
        pos[i] = next_p[i];
    }
    point_array->InsertNextPoint(pos);

    while(count<=50000){
    //    cout<<"loop start"<<endl;
        d1 = sqrt(pow((cell->GetPoints()->GetData()->GetTuple(0)[0]-pos[0]),2)+pow((cell->GetPoints()->GetData()->GetTuple(0)[1]-pos[1]),2)+pow((cell->GetPoints()->GetData()->GetTuple(0)[2]-pos[2]),2)); 
        d2 = sqrt(pow((cell->GetPoints()->GetData()->GetTuple(1)[0]-pos[0]),2)+pow((cell->GetPoints()->GetData()->GetTuple(1)[1]-pos[1]),2)+pow((cell->GetPoints()->GetData()->GetTuple(1)[2]-pos[2]),2));         
        d3 = sqrt(pow((cell->GetPoints()->GetData()->GetTuple(2)[0]-pos[0]),2)+pow((cell->GetPoints()->GetData()->GetTuple(2)[1]-pos[1]),2)+pow((cell->GetPoints()->GetData()->GetTuple(2)[2]-pos[2]),2));        
        d4 = sqrt(pow((cell->GetPoints()->GetData()->GetTuple(4)[0]-pos[0]),2)+pow((cell->GetPoints()->GetData()->GetTuple(4)[1]-pos[1]),2)+pow((cell->GetPoints()->GetData()->GetTuple(4)[2]-pos[2]),2));

        r1 = (1/d1)/(1/d1+1/d2+1/d3+1/d4);
        r2 = (1/d2)/(1/d1+1/d2+1/d3+1/d4);
        r3 = (1/d3)/(1/d1+1/d2+1/d3+1/d4);
        r4 = (1/d4)/(1/d1+1/d2+1/d3+1/d4);

        tv[0] = u->GetTuple(cell->GetPointId(0))[0]*r1+u->GetTuple(cell->GetPointId(1))[0]*r2+u->GetTuple(cell->GetPointId(2))[0]*r3+u->GetTuple(cell->GetPointId(4))[0]*r4;
        tv[1] = v->GetTuple(cell->GetPointId(0))[0]*r1+v->GetTuple(cell->GetPointId(1))[0]*r2+v->GetTuple(cell->GetPointId(2))[0]*r3+v->GetTuple(cell->GetPointId(4))[0]*r4;
        tv[2] = w->GetTuple(cell->GetPointId(0))[0]*r1+w->GetTuple(cell->GetPointId(1))[0]*r2+w->GetTuple(cell->GetPointId(2))[0]*r3+w->GetTuple(cell->GetPointId(4))[0]*r4;

        next_p[0] = pos[0]+tv[0]*interval;
        next_p[1] = pos[1]+tv[1]*interval;
        next_p[2] = pos[2]+tv[2]*interval;
    
    //vtkIdType P_CellId = CellId;
  
    CellId = data->FindCell(next_p,NULL,0,0.01,subId,pcoords,  weights);
   // cout<<"next cell_id"<< CellId<<endl;
   //if(CellId==P_CellId){
  // 	interval = interval*2;
  /// 	continue;
  // }
   if(CellId<0){
        break;
    }
   
    cell = data->GetCell(CellId);
    for(int i =0; i< 3; i++){
        pos[i] = next_p[i];
    }
   point_array->InsertNextPoint(next_p);
        count++;
 //     interval=0.01;
     // cout<<count<<endl;
    }
    vtkSmartPointer<vtkPolyLine> PolyLine= vtkSmartPointer<vtkPolyLine>::New();
    PolyLine->GetPointIds()->SetNumberOfIds(count);
    for( int i = 0; i <(int)count ; i++)
    {
      PolyLine->GetPointIds()->SetId(i,i);
    }
    vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
    cells->InsertNextCell(PolyLine);
    vtkSmartPointer<vtkPolyData> linesPolyData =vtkSmartPointer<vtkPolyData>::New();
    linesPolyData->SetPoints(point_array);
    linesPolyData->SetLines(cells);
    
    vtkSmartPointer<vtkPolyDataMapper> LineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    //LineMapper->SetInputConnection(line->GetOutputPort());
    LineMapper->SetInputData(linesPolyData);
    vtkSmartPointer<vtkActor> LineActor = vtkSmartPointer<vtkActor>::New();
    LineActor->SetMapper(LineMapper);
    LineActor->GetProperty()->SetLineWidth(5);
    
    ren1->AddActor(LineActor);
    
    }
    //cout<<point_array->GetNumberOfPoints()<<endl;


 
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

    planeMapper->SetInputData(ploydata);
    planeMapper -> ScalarVisibilityOn();
    planeMapper -> SetScalarModeToUsePointData();
    planeMapper -> SetColorModeToMapScalars();


    cout<< "actor" <<endl;
    vtkScalarBarActor *scalarActor = vtkScalarBarActor :: New();
    scalarActor->SetLookupTable(planeMapper->GetLookupTable());

    vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor> :: New();
    planeActor ->SetMapper(planeMapper);
    cout<< "vtkrender"<<endl;

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
    
    iren->Initialize();
    iren->Start();
    return 0;
}
