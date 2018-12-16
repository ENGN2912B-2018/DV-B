#include "eventqtslotconnect.h"
#include <thread>
#include <vtkCutter.h>
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
#include <vtkPlane.h>
#include <vtkTextProperty.h>

#include <QFileDialog>
#include <QMessageBox>
using namespace std;
// Constructor
struct no_data : public std::exception {
        const char * what () const throw () {
                    return "the current dataset has no selected data";}
};

void EventQtSlotConnect::read_data(vector<string> paths, vector<vtkSmartPointer<vtkDataSet>> &data){
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

EventQtSlotConnect::EventQtSlotConnect()
{

    /*******************************/
    connect(this->actionOpen_file, SIGNAL(triggered()), this, SLOT(open()));
    /*******************************/
    string air_path_str = "../../../../../../data/air.vtu";
    // data_load input;
    // sealed globally
    vector<string> paths_obj;
    paths_obj.push_back("../../../../../../data/blade21.vtp");
    paths_obj.push_back("../../../../../../data/blade22.vtp");
    paths_obj.push_back("../../../../../../data/blade23.vtp");
    paths_obj.push_back("../../../../../../data/blade24.vtp");
    paths_obj.push_back("../../../../../../data/blade25.vtp");
    paths_obj.push_back("../../../../../../data/blade26.vtp");

    paths_obj.push_back("../../../../../../data/blade44.vtp");
    paths_obj.push_back("../../../../../../data/blade46.vtp");
    paths_obj.push_back("../../../../../../data/blade43.vtp");
    paths_obj.push_back("../../../../../../data/blade45.vtp");
    void (EventQtSlotConnect::*read_data_p)(vector<string>, vector<vtkSmartPointer<vtkDataSet>>&);
    read_data_p = &EventQtSlotConnect::read_data;
    std::thread t_air(ref(read_data_p), this, paths_obj, ref(this->objects));
    vector<string> air_path;
    air_path.push_back(air_path_str);
    read_data(air_path,this->air);

    for( int i =0; i< air.size(); i++){
        cout<<"air_file: "<<i<<" "<<endl;
        cout<< air[i]->GetPointData()->GetNumberOfArrays()<<endl;
    }

    t_air.join();

    for( int i =0; i< objects.size(); i++){
        cout<<"object_file: "<<i<<" "<<endl;
        cout<<objects[i]->GetPointData()->GetNumberOfArrays()<<endl;
    }

    /**********************************/
    
    this->setupUi(this);
    //connect( this->ui.radioButton_P , SIGNAL( clicked() ), this, SLOT(pushButton_SetLabelText()));



    vtkSmartPointer<vtkUnstructuredGrid> data = vtkSmartPointer<vtkUnstructuredGrid>::New();

    data->DeepCopy(this->air[0]);
    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
    this->qvtkWidget->SetRenderWindow(renderWindow);
    //cout << "renderwindow" << endl;


    //cout<<data->GetFieldData()->GetArray(0)->GetName()<<endl;
    cout<<"number of array"<<data->GetPointData()->GetNumberOfArrays()<<endl;
    data->GetPointData()->RemoveArray(0);
    data->GetPointData()->RemoveArray(1);
    data->GetPointData()->RemoveArray(1);
    data->GetPointData()->RemoveArray(1);
    cout<<"number of array"<<data->GetPointData()->GetNumberOfArrays()<<endl;


    vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    //cout<<"set_input"<<endl;
    vertexFilter->SetInputData(data);
    vertexFilter->Update();
    //cout<<"input_over"<<endl;

    vtkSmartPointer<vtkPolyData> ploydata = vtkSmartPointer<vtkPolyData>::New();
    ploydata->ShallowCopy(vertexFilter->GetOutput());


    vtkSmartPointer<vtkPolyDataMapper> planeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();

    planeMapper -> SetInputData(ploydata);

    //cout << "actor" << endl;
    vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor> :: New();
    planeActor ->SetMapper(planeMapper);
    cout << "vtkrender_initial"<< endl;
    vtkRenderer* ren1 = vtkRenderer :: New();
    this->ren1 = ren1;
    ren1 -> SetBackground(0.1,0.2,0.4);
    ren1 -> AddActor(planeActor);
    this->actors.push_back(planeActor);


    // render
    this->qvtkWidget->GetRenderWindow()->AddRenderer(ren1);
    cout << "add renderer complete_initial" << endl;

    /**************************************/

    // slot and signal
    vtkNew<vtkEventQtSlotConnect> slotConnector;
    this->Connections = slotConnector;

    connect(this->actionOpen_file, &QAction::triggered, this, &EventQtSlotConnect::open);

};

void EventQtSlotConnect::on_radioButton_P_toggled(bool checked)
{

    if(checked){
        cout << "Q is on!" << endl;

        for(int i = 0; i<actors.size(); i++){
            this->ren1->RemoveActor(actors[i]);
        }
        for(int i = 0; i<actor2ds.size(); i++){
            this->ren1->RemoveActor2D(actor2ds[i]);
        }
        actors.resize(0);
        actor2ds.resize(0);
        this->qvtkWidget->GetRenderWindow()->RemoveRenderer(ren1);

        //this->setupUi(this);


        vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
        this->qvtkWidget->SetRenderWindow(renderWindow);
        //cout << "renderwindow2" << endl;

        vtkSmartPointer<vtkUnstructuredGrid> data = vtkSmartPointer<vtkUnstructuredGrid>::New();
        data->DeepCopy(this->air[0]);

        //data->GetPointData()->RemoveArray(1);

        
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

        this->ren1 -> AddViewProp(textActor);
        this->ren1 -> SetBackground(0.1,0.2,0.4);
        
        cout << "vtk complete_Q" << endl;
        this->qvtkWidget->GetRenderWindow()->AddRenderer(ren1);
        }

        vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();

        double n1[] = {9,1,1}; // coordinates, needs projection
        double n2[] = {0,0,1}; // choose which surface to display (0,0,1) is x-y surface.
        plane -> SetOrigin(n1);
        plane -> SetNormal(n2);

        vtkSmartPointer<vtkCutter> planecut = vtkSmartPointer<vtkCutter>::New();
        planecut -> SetInputData(data);
        planecut -> SetCutFunction(plane);
        planecut -> Update();


        vtkSmartPointer<vtkPolyDataMapper> planeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        planeMapper->SetInputData(planecut->GetOutput());
        planeMapper->SetScalarRange(-10, 10);  //set map range

        vtkSmartPointer<vtkScalarBarActor> scalarActor = vtkSmartPointer<vtkScalarBarActor> :: New();
        scalarActor->SetMaximumNumberOfColors(256);
        scalarActor->SetLookupTable(planeMapper->GetLookupTable());
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


        vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor> :: New();
        planeActor ->SetMapper(planeMapper);

        cout << "vtkrenderer_Q" << endl;

        //    this->ren1 -> Clear();
        this->ren1 -> SetBackground(0.1,0.2,0.4);
        this->ren1 -> AddActor(planeActor);
        this->actors.push_back(planeActor);
        this->ren1 -> AddActor2D(scalarActor);
        this->actor2ds.push_back(scalarActor);

        //add_air_complete==================================================

        //     vector<vtkDataSet*> obj_datasets(objects.size());
        //     for (int j=0; j < objects.size();j++){
        //        obj_datasets[j] = this->objects[j].get_data();

        //     }
        for (int j =0 ; j< objects.size(); j++){
            //vtkDataSet* data = obj_datasets[j];

            vtkSmartPointer<vtkDataSetMapper> planeMapper = vtkSmartPointer<vtkDataSetMapper>::New();
            planeMapper->SetInputData(objects[j]);
            planeMapper->ScalarVisibilityOff();
            vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor> :: New();
            planeActor ->SetMapper(planeMapper);
            this->ren1->AddActor(planeActor);
            this->actors.push_back(planeActor);

        }
        cout << "vtk complete_P" << endl;
        this->qvtkWidget->GetRenderWindow()->AddRenderer(ren1);
        //cout << "add renderer complete" << endl;
    }

}

void EventQtSlotConnect::on_radioButton_Q_toggled(bool checked)
{
    if(checked){
        cout << "P is on!" << endl;

        for(int i = 0; i<actors.size(); i++){
            this->ren1->RemoveActor(actors[i]);
        }
        for(int i = 0; i<actor2ds.size(); i++){
            this->ren1->RemoveActor2D(actor2ds[i]);
        }
        actors.resize(0);
        actor2ds.resize(0);
        this->qvtkWidget->GetRenderWindow()->RemoveRenderer(ren1);


        //this->setupUi(this);

        vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
        this->qvtkWidget->SetRenderWindow(renderWindow);
        //cout << "renderwindow2" << endl;

        vtkSmartPointer<vtkUnstructuredGrid> data = vtkSmartPointer<vtkUnstructuredGrid>::New();
        data->DeepCopy(this->air[0]);

        //============================================================================================
        //data->GetPointData()->RemoveArray(0);  // delete the data that we do not want to visualize.
        try{
        if(data->GetPointData()->GetAbstractArray("Q") == nullptr){
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

        cout << "vtk complete_Q" << endl;
        this->qvtkWidget->GetRenderWindow()->AddRenderer(ren1);


    }
        

        vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
        vertexFilter->SetInputData(data);
        vertexFilter->Update();

        vtkSmartPointer<vtkPolyData> ploydata = vtkSmartPointer<vtkPolyData>::New(); //make the data into vertex
        ploydata->ShallowCopy(vertexFilter->GetOutput());

        ploydata->GetPointData()->SetScalars(ploydata->GetPointData()->GetArray("Q")); // set the scalar value for visualize

        //cout<<"ploy_data"<<ploydata->GetPointData()->GetNumberOfArrays()<<endl;
        vtkSmartPointer<vtkPolyDataMapper> planeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        planeMapper->SetInputData(ploydata);

        double* arr_range = ploydata->GetPointData()->GetArray("Q")->GetRange();
        //cout<<arr_range[0]<<" arr_range0"<<endl;
        //cout<<arr_range[1]<<" arr_range1"<<endl;
        planeMapper->SetScalarRange(-500,0);  //set map range

        vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
        lut->SetNumberOfColors(256);
        //lut->SetTableRange(0,50);
        lut->Build();
        double r = 128 , g = 128, b = 128;
        for(int i = 0; i< 256 ; i+=1){
            lut->SetTableValue(i,r,g,b);
        }
        lut->SetTableValue(0,0,0,0,0);  //hide the value that is lower than low_bound
        lut->SetTableValue(255,0,0,0,0);

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

        planeMapper->SetLookupTable(lut);
        vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor> :: New();
        planeActor ->SetMapper(planeMapper);

        cout << "vtkrender_P"<< endl;

        //render=============================================
        ren1 -> SetBackground(0.1,0.2,0.4);
        ren1 -> AddActor(planeActor);
        this->actors.push_back(planeActor);
        ren1-> AddActor2D(scalarActor);
        this->actor2ds.push_back(scalarActor);

        for (int j =0 ; j< this->objects.size(); j++){
            //vtkSmartPointer<vtkDataSet> data = obj_datasets[j];
            vtkSmartPointer<vtkDataSetMapper> planeMapper = vtkSmartPointer<vtkDataSetMapper>::New();
            planeMapper->SetInputData(objects[j]);
            planeMapper->ScalarVisibilityOff();
            vtkSmartPointer<vtkActor> planeActor1 = vtkSmartPointer<vtkActor> :: New();
            planeActor ->SetMapper(planeMapper);
            this->ren1->AddActor(planeActor);
            this->actors.push_back(planeActor);
        }


        // render
        cout << "vtk complete_P" << endl;
        this->qvtkWidget->GetRenderWindow()->AddRenderer(ren1);
    }
}

void EventQtSlotConnect::on_radioButton_PO_toggled(bool checked)
{
    if(checked){
        cout << "PO is on!" << endl;

        for(int i = 0; i<actors.size(); i++){
            this->ren1->RemoveActor(actors[i]);
        }
        for(int i = 0; i<actor2ds.size(); i++){
            this->ren1->RemoveActor2D(actor2ds[i]);
        }
        actors.resize(0);
        actor2ds.resize(0);
        this->qvtkWidget->GetRenderWindow()->RemoveRenderer(ren1);

        //this->setupUi(this);
        //cout << "renderwindow1" << endl;

        vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
        this->qvtkWidget->SetRenderWindow(renderWindow);

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
        
        cout << "vtk complete_P" << endl;
        this->qvtkWidget->GetRenderWindow()->AddRenderer(ren1);
}


            vtkSmartPointer<vtkDataSetMapper> planeMapper = vtkSmartPointer<vtkDataSetMapper>::New();
            planeMapper->SetInputData(data);
            planeMapper -> ScalarVisibilityOn();
            planeMapper-> SetLookupTable(lut);
            double* arr_range = data->GetPointData()->GetArray(0)->GetRange();
            planeMapper->SetScalarRange(arr_range[0],arr_range[1]);  //set map range

            cout<< "actor" <<endl;


            vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor> :: New();
            planeActor ->SetMapper(planeMapper);
            this->ren1 -> AddActor(planeActor);
            this->actors.push_back(planeActor);
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

        this->ren1->AddActor2D(scalarActor);
        this->actor2ds.push_back(scalarActor);

        cout << "vtk complete_PO" << endl;
        this->qvtkWidget->GetRenderWindow()->AddRenderer(this->ren1);
    }
}

void EventQtSlotConnect::on_horizontalSlider_valueChanged(int value)
{
    cout << "value now: " << value << endl;

}

void EventQtSlotConnect::open()
{
    QString file_name = QFileDialog::getOpenFileName(this, "Open file(s)", "~");
    QMessageBox::information(this, "..", file_name);
}
