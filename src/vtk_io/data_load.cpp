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

#include <map>

#include "data_load.h"
using namespace std;

template <class TReader> vtkDataSet *ReadAnXMLFile(const char*fileName)
{
      vtkSmartPointer<TReader> reader = vtkSmartPointer<TReader>::New();
      reader->SetFileName(fileName);
      reader->Update();
      reader->GetOutput()->Register(reader);
      return vtkDataSet::SafeDownCast(reader->GetOutput());
}

int vtk_io :: load_data(string filename){
    //if (filename.size()<2){
    //    std::cerr << "Usage: " << " XMLFile1 XMLFile2 ..." << std::endl;
    //}
    vtkDataSet *dataSet;
    string extension = vtksys::SystemTools::GetFilenameLastExtension(filename);
    cout<<extension<<endl;
    if (extension == ".vtu")
    {
      dataSet = ReadAnXMLFile<vtkXMLUnstructuredGridReader> (filename.c_str());
    }
    else if (extension == ".vtp")
    {
      dataSet = ReadAnXMLFile<vtkXMLPolyDataReader> (filename.c_str());
    }
    else if (extension == ".vts")
    {
      dataSet = ReadAnXMLFile<vtkXMLStructuredGridReader> (filename.c_str());
    }
    else if (extension == ".vtr")
    {
      dataSet = ReadAnXMLFile<vtkXMLRectilinearGridReader> (filename.c_str());
    }
    else if (extension == ".vti")
    {
      dataSet = ReadAnXMLFile<vtkXMLImageDataReader> (filename.c_str());
    }
    else if (extension == ".vtk")
    {
      dataSet = ReadAnXMLFile<vtkDataSetReader> (filename.c_str());
    }
    else
    {
      std::cerr << filename << " Unknown extension: " << extension << std::endl;
      return EXIT_FAILURE;
    }

    int numberOfCells = dataSet->GetNumberOfCells();
    int numberOfPoints = dataSet->GetNumberOfPoints();

    // Generate a report
    std::cout << "------------------------" << std::endl;
    std::cout << filename << std::endl
         << " contains a " << std::endl
         << dataSet->GetClassName()
         <<  " that has " << numberOfCells << " cells"
         << " and " << numberOfPoints << " points." << std::endl;
    typedef std::map<int,int> CellContainer;
    CellContainer cellMap;
    for (int i = 0; i < numberOfCells; i++)
    {
      cellMap[dataSet->GetCellType(i)]++;
    }

    CellContainer::const_iterator it = cellMap.begin();
    while (it != cellMap.end())
    {
      std::cout << "\tCell type "
           << vtkCellTypes::GetClassNameFromTypeId(it->first)
           << " occurs " << it->second << " times." << std::endl;
      ++it;
    }

    // Now check for point data
    vtkPointData *pd = dataSet->GetPointData();
    if (pd)
    {
      std::cout << " contains point data with "
           << pd->GetNumberOfArrays()
           << " arrays." << std::endl;
      for (int i = 0; i < pd->GetNumberOfArrays(); i++)
      {
        std::cout << "\tArray " << i
             << " is named "
             << (pd->GetArrayName(i) ? pd->GetArrayName(i) : "NULL")
             << std::endl;
      }
    }
    // Now check for cell data
    vtkCellData *cd = dataSet->GetCellData();
    if (cd)
    {
      std::cout << " contains cell data with "
           << cd->GetNumberOfArrays()
           << " arrays." << std::endl;
      for (int i = 0; i < cd->GetNumberOfArrays(); i++)
      {
        std::cout << "\tArray " << i
             << " is named "
             << (cd->GetArrayName(i) ? cd->GetArrayName(i) : "NULL")
             << std::endl;
      }
    }
    // Now check for field data
    if (dataSet->GetFieldData())
    {
      std::cout << " contains field data with "
           << dataSet->GetFieldData()->GetNumberOfArrays()
           << " arrays." << std::endl;
      for (int i = 0; i < dataSet->GetFieldData()->GetNumberOfArrays(); i++)
      {
        std::cout << "\tArray " << i
             << " is named " << dataSet->GetFieldData()->GetArray(i)->GetName()
             << std::endl;
      }
    }
    this->data_ = dataSet;
  return EXIT_SUCCESS;
} 

vtkDataSet* vtk_io::get_data(){
    return this->data_;
}
vtk_io::~vtk_io(){
    this->data_->Delete();
}
