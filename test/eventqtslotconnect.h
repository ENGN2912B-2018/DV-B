#ifndef eventqtslotconnect_H
#define eventqtslotconnect_H

#include "ui_eventqtslotconnect.h"

#include <QMainWindow>
#include <data_load.h>
#include <vtkSmartPointer.h>
#include <vtkObject.h>
#include <vtkActor.h>
#include <vtkActor2D.h>

using namespace std;
class vtkEventQtSlotConnect;

class EventQtSlotConnect : public QMainWindow, private Ui::EventQtSlotConnect
{
  Q_OBJECT

public:

  EventQtSlotConnect();
  void read_data(vector<string> paths, vector<vtkSmartPointer<vtkDataSet>> &data);


private slots:


  void on_radioButton_P_toggled(bool checked);

  void on_radioButton_Q_toggled(bool checked);

private:

  vtkSmartPointer<vtkEventQtSlotConnect> Connections;
  vector<vtkSmartPointer<vtkDataSet>> air;
  vector<vtkSmartPointer<vtkDataSet>> objects;
  vtkRenderer* ren1;
  vector<vtkSmartPointer<vtkActor>> actors;
  vector<vtkSmartPointer<vtkActor2D>> actor2ds;

};

#endif
