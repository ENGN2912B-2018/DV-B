#include "eventqtslotconnect.h"

#include "vtkGenericOpenGLRenderWindow.h"
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSphereSource.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkInteractorStyleTrackballActor.h>
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
#include <vtkCommand.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkScalarBarActor.h>

using namespace std;
// Constructor
EventQtSlotConnect::EventQtSlotConnect()
{
  string file_path = "../../data/air.vtu";
  // data_load input;
  // sealed globally
  if(!this->input.load_data(file_path)){
    cout << "Loading succeeded." << endl;
  }
  vector<string> paths;
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
    
  for(int i=0 ; i < paths.size(); i++){
    string file_path = paths[i];
    data_load input;
    if(!input.load_data(file_path)){
        cout<<"load success"<<endl;
    }
    this->objects.push_back(input);
  }

  /**********************************/
    
  this->setupUi(this);
  vtkSmartPointer<vtkUnstructuredGrid> data = vtkSmartPointer<vtkUnstructuredGrid>::New();

  data->DeepCopy(input.get_data());
  vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
  this->qvtkWidget->SetRenderWindow(renderWindow);
  cout << "renderwindow" << endl;

  // vtkDataSet *data = input.get_data();

  vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
  lut->SetNumberOfColors(64);
  lut->SetTableRange(-100,350);
  lut->Build();

  cout<<data->GetFieldData()->GetArray(0)->GetName()<<endl;
  cout<<"number of array"<<data->GetPointData()->GetNumberOfArrays()<<endl;
  data->GetPointData()->RemoveArray(0);
  data->GetPointData()->RemoveArray(1);
  data->GetPointData()->RemoveArray(1);
  data->GetPointData()->RemoveArray(1);
  cout<<"number of array"<<data->GetPointData()->GetNumberOfArrays()<<endl;

  double r = 0, g = 0, b = 0;
  for(int i = 0; i < 61 ; i += 4){
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

  vtkSmartPointer<vtkPolyData> ploydata = vtkSmartPointer<vtkPolyData>::New();
  ploydata->ShallowCopy(vertexFilter->GetOutput());

  ploydata->GetPointData()->SetScalars(ploydata->GetPointData()->GetArray(0));

  cout<<"ploy_data"<<ploydata->GetPointData()->GetNumberOfArrays()<<endl;
  vtkSmartPointer<vtkPolyDataMapper> planeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  cout << "mapper" << endl;
  //planeMapper->SetLookupTable(lut);
  //planeMapper->DebugOn();

  // CRASHED HERE.
  planeMapper -> SetInputData(ploydata);

  planeMapper -> ScalarVisibilityOn();
  planeMapper -> SetScalarModeToUsePointData();
  planeMapper -> SetColorModeToMapScalars();

  cout << "actor" << endl;
  vtkScalarBarActor *scalarActor = vtkScalarBarActor :: New();
  scalarActor->SetLookupTable(planeMapper->GetLookupTable());
  vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor> :: New();
  planeActor ->SetMapper(planeMapper);
  cout << "vtkrender"<< endl;
  vtkSmartPointer<vtkRenderer> ren1 = vtkSmartPointer<vtkRenderer> :: New();
  this->ren1 = ren1;
  ren1 -> SetBackground(0.1,0.2,0.4);
  ren1 -> AddActor(planeActor);
  ren1 -> AddActor2D(scalarActor);

  //cout << "vtkrender" << endl;
  //vtkSmartPointer<vtkRenderWindow> renWin  = vtkSmartPointer<vtkRenderWindow> :: New();
  //renWin->AddRenderer(ren1);
  //renWin->SetSize(300,150);

  // render
  this->qvtkWidget->GetRenderWindow()->AddRenderer(ren1);
  cout << "add renderer complete" << endl;

  /**************************************/

  // slot and signal
  vtkNew<vtkEventQtSlotConnect> slotConnector;
  this->Connections = slotConnector;

//  connect(checkBox_P,
//    SIGNAL(stateChanged(int)), //
//    this,
//    SLOT(on_checkBox_P_clicked()));

};

void EventQtSlotConnect::on_checkBox_P_clicked()
{
    if(checkBox_P->isChecked()){
        cout << "P is on!" << endl;
    }

    this->setupUi(this);
    cout << "renderwindow1" << endl;

    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
    this->qvtkWidget->SetRenderWindow(renderWindow);
    cout << "renderwindow2" << endl;

    vtkSmartPointer<vtkUnstructuredGrid> data = vtkSmartPointer<vtkUnstructuredGrid>::New();
    data->DeepCopy(input.get_data());

    //============================================================================================
    data->GetPointData()->RemoveArray(0);  // delete the data that we do not want to visualize.
    data->GetPointData()->RemoveArray(1);
    data->GetPointData()->RemoveArray(1);
    data->GetPointData()->RemoveArray(1);

    vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    vertexFilter->SetInputData(data);
    vertexFilter->Update();

    vtkSmartPointer<vtkPolyData> ploydata = vtkSmartPointer<vtkPolyData>::New(); //make the data into vertex
    ploydata->ShallowCopy(vertexFilter->GetOutput());

    ploydata->GetPointData()->SetScalars(ploydata->GetPointData()->GetArray(0)); // set the scalar value for visualize

    cout<<"ploy_data"<<ploydata->GetPointData()->GetNumberOfArrays()<<endl;
    vtkSmartPointer<vtkPolyDataMapper> planeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    planeMapper->SetInputData(ploydata);

    double* arr_range = data->GetPointData()->GetArray(0)->GetRange();
    cout<<arr_range[0]<<" arr_range0"<<endl;
    cout<<arr_range[1]<<" arr_range1"<<endl;

    planeMapper->SetScalarRange(0,arr_range[1]);  //set map range

    vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
    lut->SetNumberOfColors(256);
    lut->SetTableRange(0,50);
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

    double* range = lut->GetRange();
    cout<<range[0]<<" range0"<<endl;
    cout<<range[1]<<" range1"<<endl;

    cout<< "actor" <<endl;
    vtkSmartPointer<vtkScalarBarActor> scalarActor = vtkSmartPointer<vtkScalarBarActor> :: New();
    scalarActor->SetMaximumNumberOfColors(256);
    scalarActor->SetLookupTable(lut);
    scalarActor->SetHeight(0.12);
    scalarActor->SetWidth(0.90);
    scalarActor->SetPosition(0.04, 0.02);
    scalarActor->SetPosition2(0.90, 0.20);
    scalarActor->SetOrientationToHorizontal();
    scalarActor->SetNumberOfLabels(3);
    scalarActor->SetLabelFormat("%.4g");
    scalarActor->VisibilityOn();
    vtkSmartPointer<vtkTextProperty> labeltype = vtkSmartPointer<vtkTextProperty>:: New();
    labeltype->SetFontSize(15);
    scalarActor->SetLabelTextProperty(labeltype);

    planeMapper->SetLookupTable(lut);
    vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor> :: New();
    planeActor ->SetMapper(planeMapper);

    cout << "actor" << endl;
    vtkScalarBarActor *scalarActor = vtkScalarBarActor :: New();
    scalarActor->SetLookupTable(planeMapper->GetLookupTable());
    vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor> :: New();
    planeActor ->SetMapper(planeMapper);
    cout << "vtkrender"<< endl;

    //render=============================================
    this -> render -> clear();
    ren1 -> SetBackground(0.1,0.2,0.4);
    ren1 -> AddActor(planeActor);
    ren1 -> AddActor2D(scalarActor);

    //add_air_complete==================================================

     vector<vtkDataSet*> obj_datasets(objects.size());
     for (int j=0; j < paths.size(),j++){
        obj_datasets[j] = this->objects.get_data();
        j++;
     }
     for (int j =0 ; j< obj_datasets.size(); j++){
        vtkSmartPointer<vtkDataSet> data = datasets[j];

        vtkSmartPointer<vtkDataSetMapper> planeMapper = vtkSmartPointer<vtkDataSetMapper>::New();
        planeMapper->SetInputData(data);

        vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor> :: New();
        planeActor ->SetMapper(planeMapper);
        this->ren1->AddActor(planeActor);
    }

    
    // render
    cout << "vtk complete" << endl;
    //this->qvtkWidget->GetRenderWindow()->AddRenderer(ren1);
    cout << "add renderer complete" << endl;
}

void EventQtSlotConnect::on_checkBox_Q_stateChanged()
{
    if(checkBox_Q->isChecked()){
        cout << "Q is on!" << endl;
    }

    this->setupUi(this);
    cout << "renderwindow1" << endl;


    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
    this->qvtkWidget->SetRenderWindow(renderWindow);
    cout << "renderwindow2" << endl;
    
    avtkSmartPointer<vtkUnstructuredGrid> data = vtkSmartPointer<vtkUnstructuredGrid>::New();
    data->DeepCopy(air_input.get_data());

    data->GetPointData()->RemoveArray(1);
    data->GetPointData()->RemoveArray(1);
    data->GetPointData()->RemoveArray(1);
    data->GetPointData()->RemoveArray(1);

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

    vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    cout<<"set_input"<<endl;
    vertexFilter->SetInputData(data);
    vertexFilter->Update();
    cout<<"input_over"<<endl;
    vtkSmartPointer<vtkPolyData> ploydata = vtkSmartPointer<vtkPolyData>::New(); //make the data into vertex
    ploydata->ShallowCopy(vertexFilter->GetOutput());
    ploydata->GetPointData()->SetScalars(ploydata->GetPointData()->GetArray(0));    

    vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();

    double n1[] = {9,1,1};
    double n2[] = {1,0,0};
    plane -> SetOrigin(n1);
    plane -> SetNormal(n2);

    vtkSmartPointer<vtkCutter> planecut = vtkSmartPointer<vtkCutter>::New();
    planecut -> SetInputData(data);
    planecut -> SetCutFunction(plane);
    planecut -> Update();

    vtkSmartPointer<vtkPolyDataMapper> planeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    planeMapper->SetInputData(planecut->GetOutput());

    vtkSmartPointer<vtkScalarBarActor> scalarActor = vtkSmartPointer<vtkScalarBarActor> :: New();
    scalarActor->SetLookupTable(planeMapper->GetLookupTable());

    vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor> :: New();
    planeActor ->SetMapper(planeMapper);
    
    this->ren1->Clear();
    this->ren1 -> SetBackground(0.1,0.2,0.4);
    this->ren1 -> AddActor(planeActor);
    this->ren1 -> AddActor2D(scalarActor);
    
    //add_air_complete==================================================

     vector<vtkDataSet*> obj_datasets(objects.size());
     for (int j=0; j < paths.size(),j++){
        obj_datasets[j] = this->objects.get_data();
        j++;
     }
     for (int j =0 ; j< obj_datasets.size(); j++){
        vtkSmartPointer<vtkDataSet> data = datasets[j];

        vtkSmartPointer<vtkDataSetMapper> planeMapper = vtkSmartPointer<vtkDataSetMapper>::New();
        planeMapper->SetInputData(data);

        vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor> :: New();
        planeActor ->SetMapper(planeMapper);
        this->ren1->AddActor(planeActor);
    }


    cout << "vtk complete" << endl;
    //this->qvtkWidget->GetRenderWindow()->AddRenderer(ren1);
    cout << "add renderer complete" << endl;
}
