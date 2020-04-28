#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->sampleBox->setValue(100);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QElapsedTimer timer;
    timer.start();

    ui->graphicsView->NUM_SAMPLES = ui->sampleBox->value();
    ui->graphicsView->render();

    qint64 elp = timer.elapsed();
    qDebug().nospace().noquote() << alignString("Total Rendering") << elp/1000 << "." << elp%1000 << "s";
}
