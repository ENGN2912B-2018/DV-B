#ifndef FIRSTTEST_H
#define FIRSTTEST_H

#include "ui_firsttest.h"

#include <QMainWindow>

#include <vtkSmartPointer.h>
#include <vtkObject.h>
class vtkEventQtSlotConnect;

class firsttest : public QMainWindow, private Ui::firsttest
{
  Q_OBJECT
public:
  firsttest(string fileName);

public slots:
  void slot_clicked(vtkObject*, unsigned long, void*, void*);

private:
  vtkSmartPointer<vtkEventQtSlotConnect> Connections;
};

#endif
