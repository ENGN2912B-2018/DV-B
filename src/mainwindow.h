#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {     // Switch the context with the scope
class MainWindow;  // MainWindow only exits in the namespace Ui
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
// add some public functions
public slots:
    void aboutDialog();
    void openFileDialog();
protected:
    void openFile(const QString &fileName);
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
