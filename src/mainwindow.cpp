#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

#include <vtkDataSetReader.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Next are some personalized functions: including openfile/a few dialogs

void MainWindow::aboutDialog()
{
    QMessageBox::information(this, "About", "By Yuqi Chai in ENGN2912B class");
}

void MainWindow::openFileDialog() // a wrapper function
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File:"), "","VTK unstructured grid files (*.vtu");
    // now open file.
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    if(!file.exists()){
        return;
    }
    openFile(fileName);
}

void MainWindow::openFile(const QString &fileName)
{
}
