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


using  std::string;
class vtk_io{
    private:
        vtkDataSet* data_;
    public:
        int load_data(string file_path);
        vtkDataSet* get_data();
        ~vtk_io();
};

