#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <ui_mainwindow.h>
#include <vtkSmartPointer.h>
#include <vtkObject.h>
#include <data_load.h>
#include <vtkSmartPointer.h>
#include <vtkObject.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkTextActor.h>
using namespace std;
class mainwindow;

class mainwindow : public QMainWindow, private Ui::mainwindow
{
    Q_OBJECT

public:
    mainwindow();
    void read_data(vector<string> paths, vector<vtkSmartPointer<vtkDataSet>> &data);


private slots:
    void open();

    void on_radioButton_P_toggled(bool checked);

    void on_radioButton_Q_toggled(bool checked);

    void on_radioButton_PO_toggled(bool checked);

    void on_horizontalSlider_valueChanged(int value);

    void openAbout();

    void on_checkBox_clicked(bool checked);

private:
    //Ui::mainwindow *ui;
    vector<vtkSmartPointer<vtkDataSet>> air;
    vector<vtkSmartPointer<vtkDataSet>> objects;
    vtkRenderer* ren1;
    vector<vtkSmartPointer<vtkActor>> actors;
    vector<vtkSmartPointer<vtkActor2D>> actor2ds;
    vector<vtkSmartPointer<vtkTextActor>> textactors;
    void deleteActors();
};

#endif // MAINWINDOW_H
