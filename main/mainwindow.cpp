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
#include <vtkDataArray.h>
#include <vtkPoints.h>
#include <unordered_map>
#include <vtkPolyLine.h>
#include <vtkProperty.h>
#include <vtkDoubleArray.h>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <vtkCellLocator.h>
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
    bool flag = 1;
    streamActor = vtkSmartPointer<vtkActor>::New();
    vector<string> paths_obj;
    vector<string> air_path;
    vector<string> line_path;
    while(flag)
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
        QStringList streamlineImages = directory_name.entryList(QStringList() << "*.vtk", QDir::Files);
        foreach(QString file_name, streamlineImages)
        {
            string filePath = file_name.toStdString().c_str();
            cout << dirPath + '/' + filePath << endl;
            string temp_path = dirPath + '/' + filePath;
            line_path.push_back(temp_path);
        }
        //=======================BELOW IS THE METHOD TO READ IN FILES INSTEAD OF DIR=========================//
        //        QStringList stringList = QFileDialog::getOpenFileNames(this, "Selete file(s) to open",
        //                                                               "../../../../..", "VTK files (*.vtu *.vtp *.vtk)");
        //        QStringList airImage = (static_cast<void>(QStringList() << "*.vtu"), stringList);

        //        foreach(QString airPathName, airImage)
        //        {
        //            string filePath = airPathName.toStdString().c_str();
        //            cout << filePath << endl;
        //            air_path.push_back(filePath);
        //        }
        //        QStringList bladeImages = (static_cast<void>(QStringList() << "*.vtp"), stringList);
        //        foreach(QString file_name, bladeImages)
        //        {
        //            string filePath = file_name.toStdString().c_str();
        //            cout << filePath << endl;
        //            paths_obj.push_back(filePath);
        //        }
        //        QStringList streamlineImages = (static_cast<void>(QStringList() << "*.vtk"), stringList);
        //        foreach(QString file_name, streamlineImages)
        //        {
        //            string filePath = file_name.toStdString().c_str();
        //            cout << filePath << endl;
        //            line_path.push_back(filePath);
        //        }
        //======================================ABOVE=====================================================//


        if(directory_name == QDir::currentPath())
        {
            flag = 0;
        }
        else if(air_path.size() == 0)
        {
            QMessageBox msgBox;
            msgBox.setText("There's no appropriate .vtu files to read in. Please select another folder!");
            msgBox.exec();
            continue;
        }
        else{
            break;
        }
    }
    // DATA READ-IN
    if(flag == 1)
    {
        void(mainwindow::*read_data_p)(vector<string>, vector<vtkSmartPointer<vtkDataSet>> &);
        read_data_p = &mainwindow::read_data;
        std::thread t_air(ref(read_data_p), this, paths_obj, ref(this->objects));
        read_data(air_path, this->air);
        t_air.join();
        read_data(line_path, this->lines);

        vtkSmartPointer<vtkUnstructuredGrid> data = vtkSmartPointer<vtkUnstructuredGrid>::New();
        data->DeepCopy(this->air[0]);

        //==============================PREPROCESSING======================================//

        if(data->GetPointData()->GetArray("U") != nullptr && data->GetPointData()->GetArray("V") != nullptr &&data->GetPointData()->GetArray("W") != nullptr  )
        {
            vtkDataArray *u = data->GetPointData()->GetArray("U");
            vtkDataArray *v = data->GetPointData()->GetArray("V");
            vtkDataArray *w = data->GetPointData()->GetArray("W");

            this->vlo_array = vtkSmartPointer<vtkDoubleArray>::New();
            this->vlo_array->SetNumberOfComponents(1);
            this->vlo_array->SetNumberOfTuples(data->GetNumberOfPoints());
            double component;
            for(vtkIdType i = 0; i <= data->GetNumberOfPoints(); i++){
                component = sqrt(pow(u->GetTuple(i)[0],2)+pow(v->GetTuple(i)[0],2)+pow(w->GetTuple(i)[0],2));
                this->vlo_array->SetTuple1(i,component);
            }
        }

        //=================================END=============================================//
        vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
        this->qvtkWidget->SetRenderWindow(renderWindow);
        vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
        vertexFilter->SetInputData(data);
        vertexFilter->Update();
        vtkSmartPointer<vtkPolyData> ploydata = vtkSmartPointer<vtkPolyData>::New();
        ploydata->ShallowCopy(vertexFilter->GetOutput());
        vtkSmartPointer<vtkPolyDataMapper> planeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        planeMapper -> SetInputData(ploydata);
        vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor> :: New();
        planeActor ->SetMapper(planeMapper);
        vtkRenderer* ren1 = vtkRenderer :: New();
        this->ren1 = ren1;
        ren1 -> SetBackground(0.1,0.2,0.4);
        ren1 -> AddActor(planeActor);
        this->actors.push_back(planeActor);
        cout << "default constructor" << endl;
        this->qvtkWidget->GetRenderWindow()->AddRenderer(ren1);
    }
    else
    {
        //=========SHOULD CONSTRUCT A DEFAULT VTK RENDERER===========

        vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
        this->qvtkWidget->SetRenderWindow(renderWindow);

    }
}

void mainwindow::read_data(vector<string> paths, vector<vtkSmartPointer<vtkDataSet>> &data){
    for (int i = 0; i < paths.size(); i++){
        data_load input;
        if(!input.load_data(paths[i])){
            cout << "load data at: "<< paths[i] <<" successfully" << endl;
        }
        vtkSmartPointer<vtkDataSet> inputData;
        inputData = vtkDataSet::SafeDownCast(input.get_data());
        data.push_back(inputData);
    }
}

void mainwindow::open()
{

    vector<string> air_path;
    vector<string> paths_obj;
    vector<string> line_path;
    bool flag = 1;
    while(flag)
    {
        QDir directory_name = QFileDialog::getExistingDirectory(this,"Open a folder", "../../../../../");
        directory_name.makeAbsolute();
        QString qsdirPath = directory_name.absolutePath();
        string dirPath = qsdirPath.toStdString().c_str();
        //==========LOAD FILES==========//
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
        QStringList streamlineImages = directory_name.entryList(QStringList() << "*.vtk", QDir::Files);

        foreach(QString file_name, streamlineImages)
        {
            string filePath = file_name.toStdString().c_str();
            cout << dirPath + '/' + filePath << endl;
            string temp_path = dirPath + '/' + filePath;
            line_path.push_back(temp_path);
        }
        if(directory_name == QDir::currentPath())
        {
            flag = 0;
        }
        else if(air_path.size() == 0 && paths_obj.size() == 0){
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
    if(flag == 1)
    {

        deleteActors();
        if(air.size() != 0){
            air.resize(0);
            objects.resize(0);
            lines.resize(0);
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

        for(int i = 0; i < objects.size(); i++){
            cout << "object_file: " << i << " " << endl;
            cout << objects[i]->GetPointData()->GetNumberOfArrays() << endl;
        }
        read_data(line_path, this->lines);

        for(int i = 0; i < lines.size(); i++){
            cout << "streamline file: " << i << " " << endl;
        }

        vtkSmartPointer<vtkUnstructuredGrid> data = vtkSmartPointer<vtkUnstructuredGrid>::New();

        data->DeepCopy(this->air[0]);
        vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
        this->qvtkWidget->SetRenderWindow(renderWindow);
        vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
        vertexFilter->SetInputData(data);
        vertexFilter->Update();

        vtkSmartPointer<vtkPolyData> ploydata = vtkSmartPointer<vtkPolyData>::New();
        ploydata->ShallowCopy(vertexFilter->GetOutput());

        vtkSmartPointer<vtkPolyDataMapper> planeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        planeMapper -> SetInputData(ploydata);

        vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor> :: New();
        planeActor ->SetMapper(planeMapper);
        vtkRenderer* ren1 = vtkRenderer :: New();
        this -> ren1 = ren1;
        ren1 -> SetBackground(0.1,0.2,0.4);
        ren1 -> AddActor(planeActor);
        this -> actors.push_back(planeActor);

        // THEN RENDER:
        this->qvtkWidget->GetRenderWindow()->AddRenderer(ren1);
    }
    else
    {

        this->qvtkWidget->GetRenderWindow()->RemoveRenderer(ren1);

        this->qvtkWidget->GetRenderWindow()->AddRenderer(ren1);

    }
}


void mainwindow::on_radioButton_P_toggled(bool checked)
{
    if(checked){
        cout << "P is on!" << endl;

        deleteActors();

        vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
        this->qvtkWidget->SetRenderWindow(renderWindow);

        vtkSmartPointer<vtkUnstructuredGrid> data = vtkSmartPointer<vtkUnstructuredGrid>::New();
        if(air.size() == 0){
            return;
        }
        data->DeepCopy(this->air[0]);
        vtkSmartPointer<vtkDataArray> data_array;
        data_array = data->GetPointData()->GetArray("P");
        if(data_array == nullptr){
            data_array = data->GetPointData()->GetArray("p");
        }
        try{
            if(data_array == nullptr){
                throw no_data();
            }
        }
        catch (const no_data &e){
            cerr<<e.what()<<endl;
            QMessageBox msgBox;
            msgBox.setText("No pressure data!");
            msgBox.exec();
            radioButton_P->setChecked(false);
            return;
        }

        vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();

        double n1[] = {9,1,1}; // coordinates, needs projection. -4 is set here for uniformity.
        double n2[] = {0,0,1}; // choose which surface to display (0,0,1) is x-y surface.
        plane -> SetOrigin(n1);
        plane -> SetNormal(n2);

        data->GetPointData()->SetScalars(data_array); // set the scalar value for visualize
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
        scalarActor->SetNumberOfLabels(5);
        scalarActor->SetLabelFormat("%.4g");
        scalarActor->VisibilityOn();
        vtkSmartPointer<vtkTextProperty> labeltype = vtkSmartPointer<vtkTextProperty>:: New();
        labeltype->SetFontSize(5);
        scalarActor->SetLabelTextProperty(labeltype);

        vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor> :: New();
        planeActor ->SetMapper(planeMapper);

        this->ren1 -> AddActor(planeActor);
        this->actors.push_back(planeActor);
        this->ren1 -> AddActor2D(scalarActor);
        this->actor2ds.push_back(scalarActor);

        //==================================add_air_complete==================================================

        for (int j =0 ; j< objects.size(); j++){
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
    }
}


void mainwindow::on_radioButton_Q_toggled(bool checked)
{
    if(checked){
        cout << "Q is on!" << endl;

        deleteActors();
        vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
        this->qvtkWidget->SetRenderWindow(renderWindow);
        vtkSmartPointer<vtkUnstructuredGrid> data = vtkSmartPointer<vtkUnstructuredGrid>::New();
        if(air.size() == 0){
            return;
        }
        data->DeepCopy(this->air[0]);
        vtkSmartPointer<vtkDataArray> data_array;
        data_array = data->GetPointData()->GetArray("Q");
        if(data_array == nullptr){
            data_array = data->GetPointData()->GetArray("q");
        }
        try{
            if(data->GetPointData()->GetArray("Q") == nullptr){
                throw no_data();
            }
        }
        catch (const no_data &e){
            cerr<<e.what()<<endl;
            QMessageBox msgBox;
            msgBox.setText("NO Q!");
            msgBox.exec();
            radioButton_Q->setChecked(false);

            return;
        }
        //============================================================================================

        vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
        vertexFilter->SetInputData(data);
        vertexFilter->Update();

        vtkSmartPointer<vtkPolyData> ploydata = vtkSmartPointer<vtkPolyData>::New(); //make the data into vertex
        ploydata->ShallowCopy(vertexFilter->GetOutput());

        ploydata->GetPointData()->SetScalars(data_array); // set the scalar value for visualize

        vtkSmartPointer<vtkPolyDataMapper> planeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        planeMapper->SetInputData(ploydata);
        planeMapper->SetScalarRange(-500, 0);  //set map range

        vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
        lut->SetNumberOfColors(256);
        lut->Build();
        double r = 128 , g = 128, b = 128;
        for(int i = 0; i< 256 ; i+=1){
            lut->SetTableValue(i,r,g,b);
        }
        lut->SetTableValue(0,0,0,0,0);  // hide the value that is lower than low_bound
        lut->SetTableValue(255,0,0,0,0);

        planeMapper->SetLookupTable(lut);
        vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor> :: New();
        planeActor ->SetMapper(planeMapper);

        //============================render===================================
        ren1 -> SetBackground(0.1,0.2,0.4);
        ren1 -> AddActor(planeActor);
        this->actors.push_back(planeActor);


        //=======================add_air_complete===========================
        for (int j =0 ; j< this->objects.size(); j++){
            //vtkSmartPointer<vtkDataSet> data = obj_datasets[j];
            vtkSmartPointer<vtkDataSetMapper> planeMapper = vtkSmartPointer<vtkDataSetMapper>::New();
            planeMapper->SetInputData(objects[j]);
            planeMapper->ScalarVisibilityOff();
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
        vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
        this->qvtkWidget->SetRenderWindow(renderWindow);

        vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
        lut->SetNumberOfColors(256);
        lut->Build();

        double r = 255, g = 0, b = 0;
        for(int i = 0; i< 125 ; i+=4){
            lut->SetTableValue(i,r,g,b);
            lut->SetTableValue(i+1,r-2,g,b);
            lut->SetTableValue(i+2,r-4,g,b);
            lut->SetTableValue(i+3,r-6,g,b);
            r -= 8;
        }

        for(int i = 128; i< 253 ; i+=4){
            lut->SetTableValue(i,r,g,b);
            lut->SetTableValue(i+1,r,g,b+2);
            lut->SetTableValue(i+2,r,g,b+4);
            lut->SetTableValue(i+3,r,g,b+8);
            b += 8;
        }
        try {
            if(objects.size() == 0)
            {
                throw no_data();
            }
        } catch (const no_data &e) {
            cerr<<e.what()<<endl;
            QMessageBox msgBox;
            msgBox.setText("No objects!");
            msgBox.exec();
            radioButton_PO->setChecked(false);
            return;
        }
        for(int j =0; j < objects.size();j++){

            vtkSmartPointer<vtkDataSet> data = objects[j];
            vtkSmartPointer<vtkDataArray> data_array;
            data_array = data->GetPointData()->GetArray("P");
            if(data_array == nullptr){
                data_array = data->GetPointData()->GetArray("p");
            }
//            try{
//                if(data_array == nullptr){
//                    throw no_data();
//                }
//            }
//            catch (const no_data &e){
//                QMessageBox msgBox;
//                msgBox.setText("No pressure data on the objects!");
//                msgBox.exec();
//                return;
//            }
            vtkSmartPointer<vtkWarpVector> warp = vtkSmartPointer<vtkWarpVector>::New();
            warp->SetInputData(data);

            vtkSmartPointer<vtkDataSetMapper> planeMapper = vtkSmartPointer<vtkDataSetMapper>::New();
            planeMapper->SetInputData(data);
            planeMapper -> ScalarVisibilityOn();
            planeMapper-> SetLookupTable(lut);
            double* arr_range = data_array->GetRange();
            double bound= max(fabs(arr_range[0]),fabs(arr_range[1]));

            planeMapper->SetScalarRange(-bound,bound);  //set map range

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
        scalarActor->SetNumberOfLabels(5);
        scalarActor->SetLabelFormat("%.4g");
        scalarActor->VisibilityOn();
        vtkSmartPointer<vtkTextProperty> labeltype = vtkSmartPointer<vtkTextProperty>:: New();
        labeltype->SetFontSize(5);
        scalarActor->SetLabelTextProperty(labeltype);

        this->ren1->AddActor2D(scalarActor);
        this->actor2ds.push_back(scalarActor);

        this->qvtkWidget->GetRenderWindow()->AddRenderer(this->ren1);
    }
}



void mainwindow::on_horizontalSlider_valueChanged(int value)
{
    if(radioButton_P->isChecked()){
        cout << "P is on!" << endl;

        deleteActors();
        vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
        this->qvtkWidget->SetRenderWindow(renderWindow);
        vtkSmartPointer<vtkUnstructuredGrid> data = vtkSmartPointer<vtkUnstructuredGrid>::New();
        if(air.size() == 0){
            return;
        }
        data->DeepCopy(this->air[0]);
        vtkSmartPointer<vtkDataArray> data_array;
        data_array = data->GetPointData()->GetArray("P");
        if(data_array == nullptr){
            data_array = data->GetPointData()->GetArray("p");
        }
        try{
            if(data_array == nullptr){
                throw no_data();
            }
        }
        catch (const no_data &e){
            cerr<<e.what()<<endl;
            QMessageBox msgBox;
            msgBox.setText("No pressure data!");
            msgBox.exec();
            horizontalSlider->setValue(0);
            return;
        }

        vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();

        double* bounds = air[0] -> GetBounds();
        double z_range = fabs(bounds[5] - bounds[4]);
        double z_value = static_cast<double>(value + 1) * (bounds[5] - bounds[4]) / 100 + bounds[4];
        double n1[] = {9,1,z_value}; // coordinates, needs projection
        double n2[] = {0,0,1}; // choose which surface to display (0,0,1) is x-y surface.
        plane -> SetOrigin(n1);
        plane -> SetNormal(n2);
        data->GetPointData()->SetScalars(data_array);
        vtkSmartPointer<vtkCutter> planecut = vtkSmartPointer<vtkCutter>::New();
        planecut -> SetInputData(data);
        planecut -> SetCutFunction(plane);
        planecut -> Update();

        vtkSmartPointer<vtkPolyDataMapper> planeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        planeMapper->SetInputData(planecut->GetOutput());
        planeMapper->SetScalarRange(-10,10);  //set map range

        vtkSmartPointer<vtkScalarBarActor> scalarActor = vtkSmartPointer<vtkScalarBarActor> :: New();
        scalarActor->SetMaximumNumberOfColors(256);
        scalarActor->SetLookupTable(planeMapper->GetLookupTable());
        scalarActor->SetHeight(0.5);
        scalarActor->SetWidth(0.12);
        scalarActor->SetPosition(0.04, 0.02);
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
            planeMapper->ScalarVisibilityOff();
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
    if(air.size() == 0){
        checkBox->setChecked(false);
        return;
    }
    try {
        if(checked && (air[0]->GetPointData()->GetArray("U") != nullptr || air[0]->GetPointData()->GetArray("u") != nullptr) &&
                (air[0]->GetPointData()->GetArray("v") != nullptr || air[0]->GetPointData()->GetArray("V") != nullptr) &&
                (air[0]->GetPointData()->GetArray("w") != nullptr || air[0]->GetPointData()->GetArray("W") != nullptr))
        {
            throw no_data();
        }
    } catch (const no_data &e) {
        cerr << e.what() << endl;
        QMessageBox msgBox;
        msgBox.setText("Data contains NO vector field!");
        msgBox.exec();
        checkBox->setChecked(false);
        return;
    }
    if(checked)
    {
        // FIRST PART: DEAL WITH EXCEPTIONS
        try {
            if(lines.size() == 0){
                throw no_data();
            }
        } catch (const no_data &e) {
            cerr<<e.what()<<endl;
            QMessageBox msgBox;
            msgBox.setText("Data contains NO vector field!");
            msgBox.exec();
            return;
        }

        //==================================//

        vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
        for(int i = 0; i < lines[0]->GetNumberOfPoints(); i++)
        {
            points->InsertNextPoint(lines[0]->GetPoint(i));
        }
        vtkSmartPointer<vtkCellArray> cellArray = vtkSmartPointer<vtkCellArray>::New();
        for(int i = 0; i < lines.size(); i++)
        {
            cout << "i: " << i << endl;
            for(int j = 0; j < lines[i]->GetNumberOfCells(); j++)
            {
                cellArray->InsertNextCell(lines[i]->GetCell(j));
            }
        }
        vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
        polyData->SetPoints(points);
        polyData->SetLines(cellArray);
        vtkSmartPointer<vtkPolyDataMapper> polyDataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        polyDataMapper->SetInputData(polyData);
        streamActor->SetMapper(polyDataMapper);
        this->ren1->AddActor(streamActor);
        this->qvtkWidget->GetRenderWindow()->AddRenderer(ren1);
    }
    else{
        this->ren1->RemoveActor(streamActor);
    }
}

void mainwindow::on_radioButton_U_toggled(bool checked)
{
    if(air.size() == 0){
        return;
    }
    try {
        if(checked && (air[0]->GetPointData()->GetArray("U") != nullptr || air[0]->GetPointData()->GetArray("u") != nullptr) &&
                (air[0]->GetPointData()->GetArray("v") != nullptr || air[0]->GetPointData()->GetArray("V") != nullptr) &&
                (air[0]->GetPointData()->GetArray("w") != nullptr || air[0]->GetPointData()->GetArray("W") != nullptr))
        {
            throw no_data();
        }
    } catch (const no_data &e) {
        cerr << e.what() << endl;
        QMessageBox msgBox;
        msgBox.setText("Data contains NO vector field!");
        msgBox.exec();
        radioButton_U->setChecked(false);
        return;
    }
    if(checked)
    {
        cout << "U is on!" << endl;
        deleteActors();
        vtkSmartPointer<vtkUnstructuredGrid> data = vtkSmartPointer<vtkUnstructuredGrid>::New();
        data->DeepCopy(this->air[0]);

        vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
        vertexFilter->SetInputData(data);
        vertexFilter->Update();
        vtkSmartPointer<vtkPolyData> ploydata = vtkSmartPointer<vtkPolyData>::New(); //make the data into vertex
        ploydata->ShallowCopy(vertexFilter->GetOutput());
        ploydata->GetPointData()->SetScalars(ploydata->GetPointData()->GetArray("U")); // set the scalar value for visualize
        vtkSmartPointer<vtkPolyDataMapper> planeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        planeMapper->SetScalarRange(ploydata->GetPointData()->GetArray("U")->GetRange());
        planeMapper->SetInputData(ploydata);
        vtkSmartPointer<vtkScalarBarActor> scalarActor = vtkSmartPointer<vtkScalarBarActor> :: New();
        scalarActor->SetLookupTable(planeMapper->GetLookupTable());
        vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor> :: New();
        planeActor ->SetMapper(planeMapper);

        this->ren1 -> SetBackground(0.1,0.2,0.4);
        ren1 -> AddActor(planeActor);
        actors.push_back(planeActor);
        ren1 -> AddActor2D(scalarActor);
        actor2ds.push_back(scalarActor);
        this->qvtkWidget->GetRenderWindow()->AddRenderer(ren1);
    }
}

void mainwindow::on_horizontalSlider_2_valueChanged(int value)
{
    deleteActors();
    vtkSmartPointer<vtkUnstructuredGrid> data = vtkSmartPointer<vtkUnstructuredGrid>::New();
    if(air.size() == 0){
        return;
    }
    data->DeepCopy(this->air[0]);
    try {
        if(data->GetPointData()->GetArray("W") == nullptr){
            throw no_data();
        }
    } catch (const no_data &e) {
        cerr<<e.what()<<endl;
        QMessageBox msgBox;
        msgBox.setText("Data contains NO scalar field velocity!");
        msgBox.exec();
        //horizontalSlider_2->setValue(0);
        return;
    }
    double i = 0, y_min = INT_MAX;
    vector<int> ids;
    while(i < data->GetNumberOfPoints()){
        double *pos;
        pos = data->GetPoint(i);
        if(pos[1]==y_min){
            ids.push_back(i);
        }
        else if(pos[1] < y_min){
            y_min = pos[1];
            ids.resize(0);
            ids.push_back(i);
        }
        i++;
    }
    vector<vector<int>> x_lines;
    unordered_map<int, int> x_dic;
    for (int i =0; i< ids.size(); i++){
        double *pos;
        pos = data->GetPoint(ids[i]);
        if(x_dic.count(static_cast<int>(pos[0]))){
            x_lines[x_dic[static_cast<int>(pos[0])]].push_back(ids[i]);
        }
        else{
            vector<int> line;
            line.push_back(ids[i]);
            x_lines.push_back(line);
            x_dic[static_cast<int>(pos[0])] = x_lines.size()-1;
        }
    }
    vector<vector<int>> z_lines;
    unordered_map<int, int> z_dic;
    for (int i =0; i< ids.size(); i++){
        double *pos;
        pos = data->GetPoint(ids[i]);
        if(z_dic.count(static_cast<int>(pos[2]))){
            z_lines[z_dic[static_cast<int>(pos[2])]].push_back(ids[i]);
        }
        else{
            vector<int> line;
            line.push_back(ids[i]);
            z_lines.push_back(line);
            z_dic[static_cast<int>(pos[2])] = z_lines.size()-1;
            cout<<"dic_pos: "<<static_cast<int>(pos[2])<<endl;
        }
    }

    double* pos;

    double r1, r2, r3, r4;
    double d1, d2, d3, d4;
    double tv[3];
    double interval = 0.01;
    double *next_p = new double[3];
    int subId;
    double pcoords[3];
    double weights[5];
    vtkSmartPointer<vtkCellLocator> cellLocator = vtkSmartPointer<vtkCellLocator>::New();
    cellLocator->SetDataSet(data);
    cellLocator->BuildLocator();
    vtkDataArray *u = data->GetPointData()->GetArray("U");
    vtkDataArray *v = data->GetPointData()->GetArray("V");
    vtkDataArray *w = data->GetPointData()->GetArray("W");
    double* bounds = data -> GetBounds();
    int low = bounds[4], high = bounds[5];
    cout<<"low: "<<low<<"high: "<<high<<endl;
    double dis = (high+1 - low)/100.0;
    cout<<"dis: "<<dis<<endl;
    //if (z_dic.count(value) == 0)
     //   return;
    cout<< " value: "<<static_cast<int>(dis*value + low)<<endl;
    try {
        if(z_dic.count(static_cast<int>(dis*value +low)) == 0){
            throw no_data();
        }
    } catch (const no_data &e) {
        cerr << e.what() << endl;
        QMessageBox msgBox;
        msgBox.setText("No start point on the selected surface! Please try again.");
        msgBox.exec();
    }
    vector<int> cur_lines= z_lines[z_dic[static_cast<int>(dis*value +low)]];
    for (int j =0 ; j<cur_lines.size(); j++){

        int origin = cur_lines[j];
        pos = data->GetPoint(origin);
        vtkSmartPointer<vtkPoints> point_array = vtkSmartPointer<vtkPoints>::New();
        //point_array->SetNumberOfPoints(52000);
        point_array->InsertNextPoint(pos);

        //================caculate second point=============================
        tv[0] = u->GetTuple(origin)[0];
        tv[1] = v->GetTuple(origin)[0];
        tv[2] = w->GetTuple(origin)[0];

        next_p[0] = pos[0]+tv[0]*interval;
        next_p[1] = pos[1]+tv[1]*interval;
        next_p[2] = pos[2]+tv[2]*interval;

        point_array->InsertNextPoint(next_p);

        vtkIdType CellId = data->FindCell(next_p,NULL,0,0.0001,subId,pcoords,weights);
        vtkSmartPointer<vtkCell> cell = data->GetCell(CellId);
        vtkIdType count = 1;
        for (int i = 0; i< 3 ;i++){
            pos[i] = next_p[i];
        }
        point_array->InsertNextPoint(pos);

        while(1){
            d1 = sqrt(pow((cell->GetPoints()->GetData()->GetTuple(0)[0]-pos[0]),2)+pow((cell->GetPoints()->GetData()->GetTuple(0)[1]-pos[1]),2)+pow((cell->GetPoints()->GetData()->GetTuple(0)[2]-pos[2]),2));
            d2 = sqrt(pow((cell->GetPoints()->GetData()->GetTuple(1)[0]-pos[0]),2)+pow((cell->GetPoints()->GetData()->GetTuple(1)[1]-pos[1]),2)+pow((cell->GetPoints()->GetData()->GetTuple(1)[2]-pos[2]),2));
            d3 = sqrt(pow((cell->GetPoints()->GetData()->GetTuple(2)[0]-pos[0]),2)+pow((cell->GetPoints()->GetData()->GetTuple(2)[1]-pos[1]),2)+pow((cell->GetPoints()->GetData()->GetTuple(2)[2]-pos[2]),2));
            d4 = sqrt(pow((cell->GetPoints()->GetData()->GetTuple(4)[0]-pos[0]),2)+pow((cell->GetPoints()->GetData()->GetTuple(4)[1]-pos[1]),2)+pow((cell->GetPoints()->GetData()->GetTuple(4)[2]-pos[2]),2));

            r1 = (1/d1)/(1/d1+1/d2+1/d3+1/d4);
            r2 = (1/d2)/(1/d1+1/d2+1/d3+1/d4);
            r3 = (1/d3)/(1/d1+1/d2+1/d3+1/d4);
            r4 = (1/d4)/(1/d1+1/d2+1/d3+1/d4);

            tv[0] = u->GetTuple(cell->GetPointId(0))[0]*r1+u->GetTuple(cell->GetPointId(1))[0]*r2+u->GetTuple(cell->GetPointId(2))[0]*r3+u->GetTuple(cell->GetPointId(4))[0]*r4;
            tv[1] = v->GetTuple(cell->GetPointId(0))[0]*r1+v->GetTuple(cell->GetPointId(1))[0]*r2+v->GetTuple(cell->GetPointId(2))[0]*r3+v->GetTuple(cell->GetPointId(4))[0]*r4;
            tv[2] = w->GetTuple(cell->GetPointId(0))[0]*r1+w->GetTuple(cell->GetPointId(1))[0]*r2+w->GetTuple(cell->GetPointId(2))[0]*r3+w->GetTuple(cell->GetPointId(4))[0]*r4;

            next_p[0] = pos[0]+tv[0]*interval;
            next_p[1] = pos[1]+tv[1]*interval;
            next_p[2] = pos[2]+tv[2]*interval;
            CellId = cellLocator->FindCell(next_p);
            //CellId = data->FindCell(next_p,cell,CellId,0.0001,subId,pcoords, weights);

            //cout<<" subId:"<<subId<<endl;
            if(CellId<0){
                for(int i =0; i< 3; i++){
                    cout<< " "<< next_p[i];
                    //cout<< " "<< pcoords[i];
                }
                cout<<" break point"<<endl;
                cout<< count << "count at break"<<endl;
                break;
            }

            cell = data->GetCell(CellId);
            for(int i =0; i< 3; i++){
                pos[i] = next_p[i];
            }
            point_array->InsertNextPoint(next_p);
            count++;
        }
        //cout<<"number of points"<< count<<endl;
        vtkSmartPointer<vtkPolyLine> PolyLine= vtkSmartPointer<vtkPolyLine>::New();
        PolyLine->GetPointIds()->SetNumberOfIds(count);
        for( int i = 0; i <(int)count ; i++)
        {
            PolyLine->GetPointIds()->SetId(i,i);
        }
        vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
        cells->InsertNextCell(PolyLine);
        vtkSmartPointer<vtkPolyData> linesPolyData =vtkSmartPointer<vtkPolyData>::New();
        linesPolyData->SetPoints(point_array);
        linesPolyData->SetLines(cells);
        vtkSmartPointer<vtkPolyDataMapper> LineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        LineMapper->SetInputData(linesPolyData);
        vtkSmartPointer<vtkActor> LineActor = vtkSmartPointer<vtkActor>::New();
        LineActor->SetMapper(LineMapper);
        LineActor->GetProperty()->SetLineWidth(5);

        ren1->AddActor(LineActor);
        actors.push_back(LineActor);
    }
    vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    vertexFilter->SetInputData(data);
    vertexFilter->Update();

    vtkSmartPointer<vtkPolyData> ploydata = vtkSmartPointer<vtkPolyData>::New(); //make the data into vertex
    ploydata->ShallowCopy(vertexFilter->GetOutput());
    ploydata->GetPointData()->SetScalars(this->vlo_array);

    double* arr_range = this->vlo_array->GetRange();
    vtkSmartPointer<vtkPolyDataMapper> planeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    planeMapper->SetInputData(ploydata);
    planeMapper -> ScalarVisibilityOn();
    planeMapper -> SetScalarModeToUsePointData();
    planeMapper -> SetColorModeToMapScalars();
    planeMapper ->SetScalarRange(0,30);

    vtkSmartPointer<vtkScalarBarActor> scalarActor = vtkSmartPointer<vtkScalarBarActor> :: New();
    scalarActor->SetMaximumNumberOfColors(256);
    scalarActor->SetLookupTable(planeMapper->GetLookupTable());
    scalarActor->SetHeight(0.5);
    scalarActor->SetWidth(0.12);
    scalarActor->SetPosition(0.04, 0.02);
    scalarActor->SetNumberOfLabels(5);
    scalarActor->SetLabelFormat("%.4g");
    scalarActor->VisibilityOn();
    vtkSmartPointer<vtkTextProperty> labeltype = vtkSmartPointer<vtkTextProperty>:: New();
    labeltype->SetFontSize(5);
    scalarActor->SetLabelTextProperty(labeltype);
    this->ren1->AddActor2D(scalarActor);
    this->actor2ds.push_back(scalarActor);

    vtkSmartPointer<vtkActor> planeActor = vtkSmartPointer<vtkActor> :: New();
    planeActor ->SetMapper(planeMapper);
    ren1->AddActor(planeActor);
    actors.push_back(planeActor);
    this->qvtkWidget->GetRenderWindow()->AddRenderer(ren1);
}
