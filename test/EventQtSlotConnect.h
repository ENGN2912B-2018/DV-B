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

public slots:

  void on_checkBox_P_clicked();

private slots:
  void on_checkBox_Q_stateChanged();

private:

  vtkSmartPointer<vtkEventQtSlotConnect> Connections;
  data_load input;

};

#endif
