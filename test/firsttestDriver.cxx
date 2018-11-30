#include <QApplication>
#include <QSurfaceFormat>
#include "firsttest.h"
// should include read_data module

int main(int argc, char** argv)
{
  string file_path = "../";
  QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat());

  QApplication app(argc, argv);

  firsttest test1(file_path);
  test1.show()

  return app.exec();
}
