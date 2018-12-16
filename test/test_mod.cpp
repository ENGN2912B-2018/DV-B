//#include "eventqtslotconnect.h"
#include "read_data/data_load.h"
#include <iostream>
#include <thread>
#include <vtkCutter.h>
//#include "vtkGenericOpenGLRenderWindow.h"
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
//#include <vtkSphereSource.h>
//#include <vtkEventQtSlotConnect.h>
//#include <vtkInteractorStyleTrackballActor.h>
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
#include <vtkPlane.h>
#include <vtkTextProperty.h>
#include <vtkTextActor.h>
//#include <>


using namespace std;

struct no_data : public std::exception {
    const char * what () const throw () {
        return "the current dataset has no selected data";}
};


void read_data(vector<string> paths, vector<vtkSmartPointer<vtkDataSet>> &data){
    for (int i = 0; i<paths.size();i++){
        data_load input;
        if(!input.load_data(paths[i])){
            cout<<"load data at: "<<paths[i]<<" successfully"<<endl;
        }
        //vtkSmartPointer<vtkDataSet> inputData = vtkSmartPointer<vtkDataSet> :: New();
        vtkSmartPointer<vtkDataSet> inputData;
        inputData = vtkDataSet::SafeDownCast(input.get_data());

        data.push_back(inputData);
    }
}

int main(){
    string air_path_str = "../../dataset/air.vtu";
    // data_load input;
    // sealed globally
    vector<string> paths_obj;
    paths_obj.push_back("../../dataset/blade21.vtp");
    paths_obj.push_back("../../dataset/blade22.vtp");
    paths_obj.push_back("../../dataset/blade23.vtp");
    paths_obj.push_back("../../dataset/blade24.vtp");
    paths_obj.push_back("../../dataset/blade25.vtp");
    paths_obj.push_back("../../dataset/blade26.vtp");

    paths_obj.push_back("../../dataset/blade44.vtp");
    paths_obj.push_back("../../dataset/blade46.vtp");
    paths_obj.push_back("../../dataset/blade43.vtp");
    paths_obj.push_back("../../dataset/blade45.vtp");

    vector<vtkSmartPointer<vtkDataSet>> air;
    vector<vtkSmartPointer<vtkDataSet>> objects;
    vtkSmartPointer<vtkRenderer> ren1 = vtkSmartPointer<vtkRenderer> :: New();
    
    void (*read_data_p)(vector<string>, vector<vtkSmartPointer<vtkDataSet>>&);
    read_data_p = &read_data;
    thread t_air(ref(read_data_p), paths_obj, ref(objects));
    
    vector<string> air_path;
    air_path.push_back(air_path_str);
    read_data(air_path,air);

    for( int i =0; i< air.size(); i++){
        cout<<"air_file: "<<i<<" "<<endl;
        cout<< air[i]->GetPointData()->GetNumberOfArrays()<<endl;
    }

    t_air.join();

    for( int i =0; i< objects.size(); i++){
        cout<<"object_file: "<<i<<" "<<endl;
        cout<<objects[i]->GetPointData()->GetNumberOfArrays()<<endl;
    }



    vtkSmartPointer<vtkUnstructuredGrid> data = vtkSmartPointer<vtkUnstructuredGrid>::New();
    data->DeepCopy(air[0]);
    //data->GetPointData()->RemoveArray("Q");
    
    vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
        lut->SetNumberOfColors(256);
        //lut->SetTableRange(-100,350);
        lut->Build();

        double r = 0, g = 0, b = 0;
        for(int i = 0; i< 125 ; i+=4){
            lut->SetTableValue(i,r,g,b);
            lut->SetTableValue(i+1,r+2,g,b);
            lut->SetTableValue(i+2,r+4,g,b);
            lut->SetTableValue(i+3,r+6,g,b);
            r += 8;
            //b += 4;
            //g -= 4;
        }
        r = 0;
        for(int i = 128; i< 253 ; i+=4){
            lut->SetTableValue(i,r,g,b);
            lut->SetTableValue(i+1,r,g,b+2);
            lut->SetTableValue(i+2,r,g,b+4);
            lut->SetTableValue(i+3,r,g,b+8);
            //r -= 8;
            b += 8;
            //g -= 4;
        }




     for(int j =0; j < objects.size();j++){
            vtkSmartPointer<vtkDataSet> data = objects[j];


             try{
        if(data->GetPointData()->GetAbstractArray("P") == nullptr){
            throw no_data();
        }
    }


    catch (const no_data &e){
        cerr<<e.what()<<endl;

        vtkSmartPointer<vtkTextActor> textActor = vtkSmartPointer<vtkTextActor> :: New();
        textActor->SetTextScaleModeToProp();
        textActor -> SetInput(e.what());
        textActor-> GetPosition2Coordinate()->SetCoordinateSystemToNormalizedViewport();
        textActor-> GetPosition2Coordinate()->SetValue(1,1);
        textActor-> GetTextProperty()->SetFontSize(20);
        textActor-> GetTextProperty()-> SetFontFamilyToArial();
        textActor-> GetTextProperty()-> SetJustificationToCentered();
        textActor-> GetTextProperty()-> BoldOn();
        textActor-> GetTextProperty()-> ItalicOn();
        textActor-> GetTextProperty()-> SetColor(1,0,0);

        ren1 -> AddViewProp(textActor);
        ren1 -> SetBackground(0.1,0.2,0.4);

         cout<<"vtkrender_catch"<<endl;
        vtkSmartPointer<vtkRenderWindow> renWin  = vtkSmartPointer<vtkRenderWindow> :: New();
        renWin->AddRenderer(ren1);
        renWin->SetSize(450,450);


        cout<<"window_catch"<<endl;
        vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor> :: New();
        iren -> SetRenderWindow (renWin);


        iren->Initialize();
        iren->Start();

}


            vtkSmartPointer<vtkDataSetMapper> planeMapper = vtkSmartPointer<vtkDataSetMapper>::New();
            planeMapper->SetInputData(data);
            planeMapper -> ScalarVisibilityOn();
            planeMapper-> SetLookupTable(lut);
            double* arr_range = data->GetPointData()->GetArray(0)->GetRange();
            if(abs(arr_range[0])> abs(arr_range[1])){
            planeMapper->SetScalarRange(-abs(arr_range[0]),abs(arr_range[0]));  //set map range
            }
            else{
            planeMapper->SetScalarRange(-abs(arr_range[1]),abs(arr_range[1]));
            }
            cout<< "actor" <<endl;


            vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor> :: New();
            planeActor ->SetMapper(planeMapper);
            ren1 -> AddActor(planeActor);
           // this->actors.push_back(planeActor);
        }

        vtkSmartPointer<vtkScalarBarActor> scalarActor = vtkSmartPointer<vtkScalarBarActor> :: New();
        scalarActor->SetMaximumNumberOfColors(256);
        scalarActor->SetLookupTable(lut);
        scalarActor->SetHeight(0.5);
        scalarActor->SetWidth(0.12);
        scalarActor->SetPosition(0.04, 0.02);
        //scalarActor->SetPosition2(0.90, 0.20);
        scalarActor->SetNumberOfLabels(5);
        scalarActor->SetLabelFormat("%.4g");
        scalarActor->VisibilityOn();
        vtkSmartPointer<vtkTextProperty> labeltype = vtkSmartPointer<vtkTextProperty>:: New();
        labeltype->SetFontSize(5);
        scalarActor->SetLabelTextProperty(labeltype);


        ren1 -> SetBackground(0.1,0.2,0.4);
     //   ren1 -> AddActor(planeActor);
        ren1 -> AddActor2D(scalarActor);
        
    cout<<"vtkrender"<<endl;
    vtkSmartPointer<vtkRenderWindow> renWin  = vtkSmartPointer<vtkRenderWindow> :: New();
    renWin->AddRenderer(ren1);
    renWin->SetSize(450,450);


    cout<< "window"<<endl;
    vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor> :: New();
    iren -> SetRenderWindow (renWin);


    iren->Initialize();
    iren->Start();

    return 0;
}
