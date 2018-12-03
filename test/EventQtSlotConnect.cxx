#include "EventQtSlotConnect.h"
#include "data_load.h"

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
  data_load input;
  if(!input.load_data(file_path)){
    cout << "Loading succeeded." << endl;
  }

  vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
  this->qvtkWidget->SetRenderWindow(renderWindow);

  vtkDataSet *data = input.get_data();
  vtkAbstractArray *q_array = data->GetPointData()->GetArray(1);

  vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
  lut->SetNumberOfColors(64);
  lut->SetTableRange(-100,350);
  lut->Build();

  vtkDataArray *dis_arr = data->GetFieldData()->GetArray(0);
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
  //planeMapper->SetLookupTable(lut);
  //planeMapper->DebugOn();
  planeMapper->SetInputData(ploydata);
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
  ren1->AddActor2D(scalarActor);

  //cout << "vtkrender" << endl;
  //vtkSmartPointer<vtkRenderWindow> renWin  = vtkSmartPointer<vtkRenderWindow> :: New();
  //renWin->AddRenderer(ren1);
  //renWin->SetSize(300,150);

  // render
  this->qvtkWidget->GetRenderWindow()->AddRenderer(ren1);

  this->Connections->Connect(this->qvtkWidget->GetRenderWindow()->GetInteractor(),
  vtkCommand::LeftButtonPressEvent,
  this,
  SLOT(slot_clicked(vtkObject*, unsigned long, void*, void*)));

};

void EventQtSlotConnect::slot_clicked(vtkObject*, unsigned long, void*, void*)
{
  std::cout << "Clicked." << std::endl;
}
