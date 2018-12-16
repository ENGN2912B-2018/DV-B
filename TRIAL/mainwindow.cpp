#include "mainwindow.h"

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
#include <vtkTextActor.h>

#include <QFileDialog>
#include <QMessageBox>
#include <QDir>

struct no_data : public std::exception {
    const char * what () const throw () {
        return "the current dataset has no selected data";}
};
void mainwindow::deleteActors()
{
    for(int i = 0; i<actors.size(); i++){
        this->ren1->RemoveActor(actors[i]);
    }
    for(int i = 0; i<actor2ds.size(); i++){
        this->ren1->RemoveActor2D(actor2ds[i]);
    }
    for(int i = 0; i<textactors.size(); i++){
        this->ren1->RemoveActor(textactors[i]);
    }
    actors.resize(0);
    actor2ds.resize(0);
    textactors.resize(0);
}
mainwindow::mainwindow()
{
    this->setupUi(this);
    connect(this->actionOpen, &QAction::triggered, this, &mainwindow::open);
    connect(this->actionAbout, &QAction::triggered, this, &mainwindow::openAbout);


    vector<string> paths_obj;
    vector<string> air_path;
    while(1)
    {
        QDir directory_name = QFileDialog::getExistingDirectory(this,"Open a folder", "../../../../../");
        directory_name.makeAbsolute();
        QString qsdirPath = directory_name.absolutePath();
        string dirPath = qsdirPath.toStdString().c_str();
        /*****      load files starts here        *****/
        QStringList airImage = directory_name.entryList(QStringList() << "*.vtu", QDir::Files);
        foreach(QString airPathName, airImage)
        {
            string filePath = airPathName.toStdString().c_str();
            string temp_path = dirPath + '/' + filePath;
            cout << temp_path << endl;
            air_path.push_back(temp_path);
        }
        QStringList bladeImages = directory_name.entryList(QStringList() << "*.vtp", QDir::Files);
        foreach(QString file_name, bladeImages)
        {
            string filePath = file_name.toStdString().c_str();
            cout << dirPath + '/' + filePath << endl;
            string temp_path = dirPath + '/' + filePath;
            paths_obj.push_back(temp_path);
        }
        // if the dir is empty:
        if(air_path.size() == 0 && paths_obj.size() == 0){
            // it means there's nothing to read.
            QMessageBox msgBox;
            msgBox.setText("There's no appropriate files to read in. Please select another folder!");
            msgBox.exec();
            continue;
        }
        else{
            break;
        }
    }
    void(mainwindow::*read_data_p)(vector<string>, vector<vtkSmartPointer<vtkDataSet>> &);
    read_data_p = &mainwindow::read_data;
    std::thread t_air(ref(read_data_p), this, paths_obj, ref(this->objects));
    read_data(air_path, this->air);

    for(int i = 0; i < air.size(); i++){
        cout<<"air_file: "<<i<<" "<<endl;
        cout<< air[i]->GetPointData()->GetNumberOfArrays()<<endl;
    }
    t_air.join();

    for( int i =0; i< objects.size(); i++){
        cout << "object_file: " << i << " " << endl;
        cout << objects[i]->GetPointData()->GetNumberOfArrays() << endl;
    }
    vtkSmartPointer<vtkUnstructuredGrid> data = vtkSmartPointer<vtkUnstructuredGrid>::New();

    data->DeepCopy(this->air[0]);
    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
    this->qvtkWidget->SetRenderWindow(renderWindow);
    //cout << "renderwindow" << endl;


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
}

void mainwindow::read_data(vector<string> paths, vector<vtkSmartPointer<vtkDataSet>> &data){
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

void mainwindow::open()
{
    //QString file_name = QFileDialog::getOpenFileName(this, "Open file(s)", "~");
    //QMessageBox::information(this, "..", file_name);
    deleteActors();
    air.resize(0);
    objects.resize(0);
    this->qvtkWidget->GetRenderWindow()->RemoveRenderer(ren1);
    vector<string> air_path;
    vector<string> paths_obj;
    while(1)
    {
        QDir directory_name = QFileDialog::getExistingDirectory(this,"Open a folder", "../../../../../");
        directory_name.makeAbsolute();
        QString qsdirPath = directory_name.absolutePath();
        string dirPath = qsdirPath.toStdString().c_str();
        /*****      load files starts here        *****/
        QStringList airImage = directory_name.entryList(QStringList() << "*.vtu", QDir::Files);

        foreach(QString airPathName, airImage)
        {
            string filePath = airPathName.toStdString().c_str();
            string temp_path = dirPath + '/' + filePath;
            cout << temp_path << endl;
            air_path.push_back(temp_path);
        }
        QStringList bladeImages = directory_name.entryList(QStringList() << "*.vtp", QDir::Files);

        foreach(QString file_name, bladeImages)
        {
            string filePath = file_name.toStdString().c_str();
            cout << dirPath + '/' + filePath << endl;
            string temp_path = dirPath + '/' + filePath;
            paths_obj.push_back(temp_path);
        }
        if(air_path.size() == 0 && paths_obj.size() == 0){
            // it means there's nothing to read.
            QMessageBox msgBox;
            msgBox.setText("There's no appropriate files to read in. Please select another folder!");
            msgBox.exec();
            continue;
        }
        else{
            break;
        }
    }
    void(mainwindow::*read_data_p)(vector<string>, vector<vtkSmartPointer<vtkDataSet>> &);
    read_data_p = &mainwindow::read_data;
    std::thread t_air(ref(read_data_p), this, paths_obj, ref(this->objects));
    read_data(air_path, this->air);

    for(int i = 0; i < air.size(); i++){
        cout<<"air_file: "<<i<<" "<<endl;
        cout<< air[i]->GetPointData()->GetNumberOfArrays()<<endl;
    }
    t_air.join();

    for( int i =0; i< objects.size(); i++){
        cout << "object_file: " << i << " " << endl;
        cout << objects[i]->GetPointData()->GetNumberOfArrays() << endl;
    }
    vtkSmartPointer<vtkUnstructuredGrid> data = vtkSmartPointer<vtkUnstructuredGrid>::New();

    data->DeepCopy(this->air[0]);
    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
    this->qvtkWidget->SetRenderWindow(renderWindow);
    //cout << "renderwindow" << endl;


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
}


void mainwindow::on_radioButton_P_toggled(bool checked)
{
    if(checked){
        cout << "P is on!" << endl;

        deleteActors();
        this->qvtkWidget->GetRenderWindow()->RemoveRenderer(ren1);

        //this->setupUi(this);


        vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
        this->qvtkWidget->SetRenderWindow(renderWindow);

        vtkSmartPointer<vtkUnstructuredGrid> data = vtkSmartPointer<vtkUnstructuredGrid>::New();
        data->DeepCopy(this->air[0]);

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

            cout << "vtk complete_P" << endl;
            this->qvtkWidget->GetRenderWindow()->AddRenderer(ren1);
        }

        //        vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
        //        lut->SetNumberOfColors(256);
        //        //lut->SetTableRange(-100,350);
        //        lut->Build();

        //        double r = 0, g = 0, b = 0;
        //        for(int i = 0; i< 253 ; i+=4){
        //            lut->SetTableValue(i,r,g,b);
        //            lut->SetTableValue(i+1,r+1,g,b);
        //            lut->SetTableValue(i+2,r+2,g,b);
        //            lut->SetTableValue(i+3,r+3,g,b);
        //            r += 4;
        //            b += 4;
        //            g += 4;
        //        }

        vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();

        double n1[] = {9,1,1}; // coordinates, needs projection. -4 is set here for uniformity.
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

        cout << "vtkrenderer_P" << endl;

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
        cout << "vtk complete_Q" << endl;
        this->qvtkWidget->GetRenderWindow()->AddRenderer(ren1);
        //cout << "add renderer complete" << endl;
    }
}


void mainwindow::on_radioButton_Q_toggled(bool checked)
{
    if(checked){
        cout << "Q is on!" << endl;

        deleteActors();
        this->qvtkWidget->GetRenderWindow()->RemoveRenderer(ren1);


        //this->setupUi(this);

        vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
        this->qvtkWidget->SetRenderWindow(renderWindow);
        //cout << "renderwindow2" << endl;

        vtkSmartPointer<vtkUnstructuredGrid> data = vtkSmartPointer<vtkUnstructuredGrid>::New();
        data->DeepCopy(this->air[0]);
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
        //============================================================================================

        vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
        vertexFilter->SetInputData(data);
        vertexFilter->Update();

        vtkSmartPointer<vtkPolyData> ploydata = vtkSmartPointer<vtkPolyData>::New(); //make the data into vertex
        ploydata->ShallowCopy(vertexFilter->GetOutput());

        ploydata->GetPointData()->SetScalars(ploydata->GetPointData()->GetArray("Q")); // set the scalar value for visualize

        vtkSmartPointer<vtkPolyDataMapper> planeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        planeMapper->SetInputData(ploydata);
        planeMapper->SetScalarRange(-500, 0);  //set map range

        vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
        lut->SetNumberOfColors(256);
        //lut->SetTableRange(0,50);
        lut->Build();
        double r = 128 , g = 128, b = 128;
        for(int i = 0; i< 256 ; i+=1){
            lut->SetTableValue(i,r,g,b);
        }
        lut->SetTableValue(0,0,0,0,0);  // hide the value that is lower than low_bound
        lut->SetTableValue(255,0,0,0,0);

        vtkSmartPointer<vtkScalarBarActor> scalarActor = vtkSmartPointer<vtkScalarBarActor> :: New();
        scalarActor->SetMaximumNumberOfColors(256);
        scalarActor->SetLookupTable(lut);
        scalarActor->SetHeight(0.25);
        scalarActor->SetWidth(0.12);
        scalarActor->SetPosition(0.04, 0.02);
        //scalarActor->SetPosition2(0.90, 0.20);
        scalarActor->SetNumberOfLabels(2);
        scalarActor->SetLabelFormat("%.4g");
        scalarActor->VisibilityOn();
        vtkSmartPointer<vtkTextProperty> labeltype = vtkSmartPointer<vtkTextProperty>:: New();
        labeltype->SetFontSize(5);
        scalarActor->SetLabelTextProperty(labeltype);

        planeMapper->SetLookupTable(lut);
        vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor> :: New();
        planeActor ->SetMapper(planeMapper);

        cout << "vtkrender_Q"<< endl;

        //render=============================================
        ren1 -> SetBackground(0.1,0.2,0.4);
        ren1 -> AddActor(planeActor);
        this->actors.push_back(planeActor);
        ren1 -> AddActor2D(scalarActor);
        this->actor2ds.push_back(scalarActor);


        //add_air_complete==================================================

        for (int j =0 ; j< this->objects.size(); j++){
            //vtkSmartPointer<vtkDataSet> data = obj_datasets[j];
            vtkSmartPointer<vtkDataSetMapper> planeMapper = vtkSmartPointer<vtkDataSetMapper>::New();
            planeMapper->SetInputData(objects[j]);
            planeMapper->ScalarVisibilityOff();
            cout << "visibility off" << endl;
            vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor> :: New();
            planeActor ->SetMapper(planeMapper);
            this->ren1->AddActor(planeActor);
            this->actors.push_back(planeActor);
        }
        // render
        cout << "vtk complete_Q" << endl;
        this->qvtkWidget->GetRenderWindow()->AddRenderer(ren1);
    }
}


void mainwindow::on_radioButton_PO_toggled(bool checked)
{
    if(checked){
        cout << "PO is on!" << endl;

        deleteActors();

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


        cout<< "vtkrender"<<endl;
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
                textactors.push_back(textActor);
                ren1 -> SetBackground(0.1,0.2,0.4);

                cout << "vtk complete_P" << endl;
                this->qvtkWidget->GetRenderWindow()->AddRenderer(ren1);
                break;
            }
            vtkSmartPointer<vtkWarpVector> warp = vtkSmartPointer<vtkWarpVector>::New();
            warp->SetInputData(data);

            vtkSmartPointer<vtkDataSetMapper> planeMapper = vtkSmartPointer<vtkDataSetMapper>::New();
            planeMapper->SetInputData(data);
            planeMapper -> ScalarVisibilityOn();
            planeMapper-> SetLookupTable(lut);
            double* arr_range = data->GetPointData()->GetArray("P")->GetRange();
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



void mainwindow::on_horizontalSlider_valueChanged(int value)
{
    cout << "value now: " << value << endl;
    if(radioButton_P->isChecked()){
        cout << "P is on!" << endl;

        deleteActors();
        this->qvtkWidget->GetRenderWindow()->RemoveRenderer(ren1);


        vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
        this->qvtkWidget->SetRenderWindow(renderWindow);
        vtkSmartPointer<vtkUnstructuredGrid> data = vtkSmartPointer<vtkUnstructuredGrid>::New();
        data->DeepCopy(this->air[0]);

        data->GetPointData()->RemoveArray(1);
        data->GetPointData()->RemoveArray(1);
        data->GetPointData()->RemoveArray(1);
        data->GetPointData()->RemoveArray(1);

        vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
        lut->SetNumberOfColors(256);
        lut->Build();

        double r = 0, g = 0, b = 0;
        for(int i = 0; i< 253 ; i+=4){
            lut->SetTableValue(i,r,g,b);
            lut->SetTableValue(i+1,r+1,g,b);
            lut->SetTableValue(i+2,r+2,g,b);
            lut->SetTableValue(i+3,r+3,g,b);
            r += 4;
            b += 4;
            g += 4;
        }

        vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();

        // HERE: we want to compress 0~100 integers to the scale the dataset complys.
        double z_value = static_cast<double>(value) / 20.0 - 2.0;
        double n1[] = {9,1,z_value}; // coordinates, needs projection
        double n2[] = {0,0,1}; // choose which surface to display (0,0,1) is x-y surface.
        plane -> SetOrigin(n1);
        plane -> SetNormal(n2);

        vtkSmartPointer<vtkCutter> planecut = vtkSmartPointer<vtkCutter>::New();
        planecut -> SetInputData(data);
        planecut -> SetCutFunction(plane);
        planecut -> Update();

        double* arr_range = data->GetPointData()->GetArray("P")->GetRange();

        vtkSmartPointer<vtkPolyDataMapper> planeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        planeMapper->SetInputData(planecut->GetOutput());
        planeMapper->SetScalarRange(arr_range[0],arr_range[1]);  //set map range

        vtkSmartPointer<vtkScalarBarActor> scalarActor = vtkSmartPointer<vtkScalarBarActor> :: New();
        scalarActor->SetMaximumNumberOfColors(256);
        scalarActor->SetLookupTable(lut);
        scalarActor->SetHeight(0.5);
        scalarActor->SetWidth(0.12);
        scalarActor->SetPosition(0.04, 0.02);
        scalarActor->SetPosition2(0.90, 0.20);
        scalarActor->SetNumberOfLabels(5);
        scalarActor->SetLabelFormat("%.4g");
        scalarActor->VisibilityOn();
        vtkSmartPointer<vtkTextProperty> labeltype = vtkSmartPointer<vtkTextProperty>:: New();
        labeltype->SetFontSize(5);
        scalarActor->SetLabelTextProperty(labeltype);

        vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor> :: New();
        planeActor ->SetMapper(planeMapper);
        this->ren1 -> SetBackground(0.1,0.2,0.4);
        this->ren1 -> AddActor(planeActor);
        this->actors.push_back(planeActor);
        this->ren1 -> AddActor2D(scalarActor);
        this->actor2ds.push_back(scalarActor);
        for (int j =0 ; j< objects.size(); j++){
            vtkSmartPointer<vtkDataSetMapper> planeMapper = vtkSmartPointer<vtkDataSetMapper>::New();
            planeMapper->SetInputData(objects[j]);
            vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor> :: New();
            planeActor ->SetMapper(planeMapper);
            this->ren1->AddActor(planeActor);
            this->actors.push_back(planeActor);
        }
        cout << "surface position changed." << endl;
        this->qvtkWidget->GetRenderWindow()->AddRenderer(ren1);
    }
}

void mainwindow::openAbout()
{
    QMessageBox msgBox;
    msgBox.setText("This program is made by Yuqi Chai, Yabo Yan and Zhiyu Wang, for ENGN 2912B of Brown University. "
                   "\n\nAll rights reserved.");
    msgBox.exec();
}

void mainwindow::on_checkBox_clicked(bool checked)
{
    if(checked)
    {

    }
}
