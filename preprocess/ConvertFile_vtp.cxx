#include <vtkSmartPointer.h>
#include <string>
#include <vtkGenericDataObjectReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkUnstructuredGrid.h>

int main(int argc, char *argv[])
{
  if(argc < 3)
  {
    std::cerr << "Required arguments: input.vtk output.vtu" << std::endl;
    return EXIT_FAILURE;
  }

  std::string inputFileName = argv[1];
  std::string outputFileName = argv[2];

  vtkSmartPointer<vtkGenericDataObjectReader> reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
  reader->SetFileName(inputFileName.c_str());
  reader->Update();

  vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
  writer->SetFileName(outputFileName.c_str());
  writer->SetInputConnection(reader->GetOutputPort());
  writer->Update();

  return EXIT_SUCCESS;
}
