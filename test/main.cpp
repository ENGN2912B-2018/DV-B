#include "vtkAutoInit.h"
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType);

#include <QApplication>
#include <QSurfaceFormat>
#include "eventqtslotconnect.h"

int main(int argc, char** argv)
{
  // needed to ensure appropriate OpenGL context is created for VTK rendering.
  QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat());

  QApplication app(argc, argv);

  EventQtSlotConnect eventQtSlotConnect;
  eventQtSlotConnect.show();

  return app.exec();
}
