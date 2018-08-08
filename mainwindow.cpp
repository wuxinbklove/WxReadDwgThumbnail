#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "wxreaddwgthumbnail.h"
#include <QFileDialog>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QString fileName = QFileDialog::getOpenFileName(nullptr, windowTitle(), QString(), "Dwgs (*.dwg)");
    if (!fileName.isEmpty())
    {
        QImage image = WxReadDwgThumbnail::ReadThumbnail(fileName);
        ui->label->setPixmap(QPixmap::fromImage(image));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
