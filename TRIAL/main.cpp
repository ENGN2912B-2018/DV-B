#include "vtkAutoInit.h"
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType);

#include <QApplication>
#include <QSurfaceFormat>
#include "mainwindow.h"
#include <QVTKOpenGLWidget.h>
#include <QCoreApplication>

int main(int argc, char** argv)
{
  QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);
  // needed to ensure appropriate OpenGL context is created for VTK rendering.
  QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat());

  QApplication app(argc, argv);

  mainwindow MainWindow;
  MainWindow.show();

  return app.exec();
}
