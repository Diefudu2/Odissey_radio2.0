#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_Previous_clicked()
{

}


void MainWindow::on_PlayButton_clicked()
{

}


void MainWindow::on_Next_clicked()
{

}


void MainWindow::on_Info_clicked()
{

}


void MainWindow::on_Load_Library_clicked()
{

}


void MainWindow::on_checkBox_stateChanged(int arg1)
{

}

