#ifndef eventqtslotconnect_H
#define eventqtslotconnect_H

#include "ui_eventqtslotconnect.h"

#include <QMainWindow>
#include <data_load.h>
#include <vtkSmartPointer.h>
#include <vtkObject.h>
class vtkEventQtSlotConnect;

class EventQtSlotConnect : public QMainWindow, private Ui::EventQtSlotConnect
{
  Q_OBJECT

public:

  EventQtSlotConnect();


private slots:
  void on_radioButton_P_toggled(bool checked);


  void on_radioButton_U_toggled(bool checked);



  void on_radioButton_Q_toggled(bool checked);

private:

  vtkSmartPointer<vtkEventQtSlotConnect> Connections;
  data_load input;
  vtkSmartPointer<vtkRenderer> ren1;
};

#endif
