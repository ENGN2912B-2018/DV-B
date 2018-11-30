#include <iostream>

using namespace std;

vtkStructureGridReader vtkStructureGridReader;
reader->SetFileName("data/*.vtk");
reader->Update();

vtkRungeKutta4 integ;
vtkStreamTracer streamer;
streamer->SetInputConnection(reader->GetOutputPort());
streamer->SetStartPosition(0.1, 2.1, 0.5);
streamer->SetMaximumPropagationUnitToTimeUnit();
streamer->SetInitialIntegrationStep(0.05);
streamer->SetInitialIntegrationStepUnitToCellLengthUnit();
streamer->SetInterationDirectionToBoth();
streamer->SetIntegrator(integ);

vtkTubeFilter streamTube;
streamTube SetInputConnection(streamer->GetOutputPort());
streamTube SetInputArrayToProcess(1, 0, 0);
vtkDataObject::FIELD_ASSOCIATION_POINTS vectors
streamTube->SetRadius(0.02);
streamTube->SetNumberOfSides(12);
streamTube->SetVaryRadiusToVaryRadiusByVector();

vtkPolyDataMapper mapStreamTube
mapStreamTube->SetInputConnection(streamTube->GetOutputPort);
eval mapStreamTube->SetScalarRange(reader->GetOutput()->GetPointData()->GetScalars()->GetRange());

vtkActor streamTubeActor;
streamTubeActor->SetMapper(mapStreamTube);
streamTubeActor->GetProperty(BackfaceCullingOn);

vtkPointSource seeds 
seeds->SetRadius(0.15);
seeds->SetCenter(0.1, 2.1, 0.5);
seeds->SetNumberOfPoints(6);
vtkRungeKutta4 integ;
vtkStreamTracer streamer 
streamer->SetInputConnection(reader->GetOutputPort());
streamer->SetSourceConnection(seeds->GetOutput());
streamer->SetMaximumPropagationTime(500);
streamer->SetMaximumPropagationUnitToTimeUnit();
streamer->SetInitialIntegrationStep(0.05);
streamer->SetInitialIntegrationStepUnitToCellLengthUnit();
streamer->SetIntegrationDirectionToBoth();
streamer->SetIntegrator(integ);



// class vtkMyCallback: public vtkCommand{
// static myCallback *New() {return new vtkMyCallback;}
// virtual void Execute(vtkObject *caller, unsigned long, void *){
// 	vtkRenderer *renderer = reinterpret_cast<vtkRenderer*>(caller);
// 	cout<<renderer->GetActiveCamera()->GetPosition()[0]<<" "
// 	    <<renderer->GetActiveCamera()->GetPosition()[1]<<" "
// 	    <<renderer->GetActiveCamera()->GetPosition()[2]<<"\n";
// }

// };

