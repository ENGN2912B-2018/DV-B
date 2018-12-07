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
  string file_path = "../../../../../../data/air.vtu";
  // data_load input;
  // sealed globally
  if(!input.load_data(file_path)){
    cout << "Loading succeeded." << endl;
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

    cout << "setinputdata" << endl;
    planeMapper -> ScalarVisibilityOn();
    cout << "scalar visibility" << endl;
    planeMapper -> SetScalarModeToUsePointData();
    cout << "set scalar mode to use point data" << endl;
    planeMapper -> SetColorModeToMapScalars();
    cout << "set color mode to map scalars" << endl;

    cout << "actor" << endl;
    vtkScalarBarActor *scalarActor = vtkScalarBarActor :: New();
    scalarActor->SetLookupTable(planeMapper->GetLookupTable());
    vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor> :: New();
    planeActor ->SetMapper(planeMapper);
    cout << "vtkrender"<< endl;
    vtkSmartPointer<vtkRenderer> ren1 = vtkSmartPointer<vtkRenderer> :: New();
    ren1 -> SetBackground(0.1,0.2,0.4);
    ren1 -> AddActor(planeActor);
    ren1 -> AddActor2D(scalarActor);

    //cout << "vtkrender" << endl;
    //vtkSmartPointer<vtkRenderWindow> renWin  = vtkSmartPointer<vtkRenderWindow> :: New();
    //renWin->AddRenderer(ren1);
    //renWin->SetSize(300,150);

    // render
    cout << "vtk complete" << endl;
    this->qvtkWidget->GetRenderWindow()->AddRenderer(ren1);
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
    vtkSmartPointer<vtkUnstructuredGrid> data = vtkSmartPointer<vtkUnstructuredGrid>::New();

    data->DeepCopy(input.get_data());

    vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
    lut->SetNumberOfColors(64);
    lut->SetTableRange(-100,350);
    lut->Build();

    cout<<data->GetFieldData()->GetArray(1)->GetName()<<endl;
    cout<<"number of array"<<data->GetPointData()->GetNumberOfArrays()<<endl;
    data->GetPointData()->RemoveArray(1);
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

    cout << "setinputdata" << endl;
    planeMapper -> ScalarVisibilityOn();
    cout << "scalar visibility" << endl;
    planeMapper -> SetScalarModeToUsePointData();
    cout << "set scalar mode to use point data" << endl;
    planeMapper -> SetColorModeToMapScalars();
    cout << "set color mode to map scalars" << endl;

    cout << "actor" << endl;
    vtkScalarBarActor *scalarActor = vtkScalarBarActor :: New();
    scalarActor->SetLookupTable(planeMapper->GetLookupTable());
    vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor> :: New();
    planeActor ->SetMapper(planeMapper);
    cout << "vtkrender"<< endl;
    vtkSmartPointer<vtkRenderer> ren1 = vtkSmartPointer<vtkRenderer> :: New();
    ren1 -> SetBackground(0.1,0.2,0.4);
    ren1 -> AddActor(planeActor);
    ren1 -> AddActor2D(scalarActor);

    //cout << "vtkrender" << endl;
    //vtkSmartPointer<vtkRenderWindow> renWin  = vtkSmartPointer<vtkRenderWindow> :: New();
    //renWin->AddRenderer(ren1);
    //renWin->SetSize(300,150);

    // render
    cout << "vtk complete" << endl;
    this->qvtkWidget->GetRenderWindow()->AddRenderer(ren1);
    cout << "add renderer complete" << endl;
}
