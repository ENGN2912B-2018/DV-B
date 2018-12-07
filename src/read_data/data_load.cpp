#include <vtkSmartPointer.h>
#include <vtkXMLReader.h>
#include <vtkXMLUnstructuredGridReader.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLStructuredGridReader.h>
#include <vtkXMLRectilinearGridReader.h>
#include <vtkXMLCompositeDataReader.h>
#include <vtkXMLStructuredGridReader.h>
#include <vtkXMLImageDataReader.h>
#include <vtkDataSetReader.h>
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
#include <vtkWarpVector.h>
#include <map>

#include "data_load.h"
using namespace std;

template <class TReader> vtkAlgorithmOutput *ReadAnXMLFile(const char*fileName, vtkDataSet* &dataSet)  //get this part of the code from vtk code examples
{
      vtkSmartPointer<TReader> reader = vtkSmartPointer<TReader>::New();
      reader->SetFileName(fileName);
      reader->Update();
      reader->GetOutput()->Register(reader);
      //data_pipe = reader->GetOutputPort();
     // vtkDataSet* res = vtkDataSet :: New();
      //vtkDataSet::SafeDownCast(reader->GetOutput());
      dataSet = vtkDataSet::SafeDownCast(reader->GetOutput());
      return reader->GetOutputPort();
}

int data_load :: load_data(string filename){    //get this part of code from vtk code examples with slight modification 
    //if (filename.size()<2){
    //    std::cerr << "Usage: " << " XMLFile1 XMLFile2 ..." << std::endl;
    //}

    string extension = vtksys::SystemTools::GetFilenameLastExtension(filename);
    cout<<extension<<endl;
    vtkAlgorithmOutput* data_pipe;
    vtkDataSet *dataSet;
    if (extension == ".vtu")
    {
      data_pipe = ReadAnXMLFile<vtkXMLUnstructuredGridReader> (filename.c_str(),dataSet); //read the vtu file with vtkXMLUnstructuredGridTrader
    }
    else if (extension == ".vtp")
    {
      data_pipe = ReadAnXMLFile<vtkXMLPolyDataReader> (filename.c_str(),dataSet);
    }
    else if (extension == ".vts")
    {
      data_pipe = ReadAnXMLFile<vtkXMLStructuredGridReader> (filename.c_str(),dataSet);
    }
    else if (extension == ".vtr")
    {
      data_pipe = ReadAnXMLFile<vtkXMLRectilinearGridReader> (filename.c_str(), dataSet);
    }
    else if (extension == ".vti")
    {
      data_pipe = ReadAnXMLFile<vtkXMLImageDataReader> (filename.c_str(), dataSet);
    }
    else if (extension == ".vtk")
    {
      data_pipe = ReadAnXMLFile<vtkDataSetReader> (filename.c_str(), dataSet);
    }
    else
    {
      std::cerr << filename << " Unknown extension: " << extension << std::endl;
      return EXIT_FAILURE;
    }
    

    int numberOfCells = dataSet->GetNumberOfCells();
    int numberOfPoints = dataSet->GetNumberOfPoints();

    // Generate a report
    std::cout << "------------------------" << std::endl;  //valid the data that has been read
    std::cout << filename << std::endl
         << " contains a " << std::endl
         << dataSet->GetClassName()
         <<  " that has " << numberOfCells << " cells"
         << " and " << numberOfPoints << " points." << std::endl;

    this->data_ = dataSet;
    this->data_pipe_ = data_pipe;
  return EXIT_SUCCESS;
} 

vtkDataSet* data_load::get_data(){
    return this->data_;
}
vtkAlgorithmOutput* data_load::get_data_pipe(){
    return this->data_pipe_;
}
data_load::~data_load(){
  //  this->data_->Delete();
}
