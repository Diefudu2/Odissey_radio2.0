#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QTimer>
#include <QProcess>
#include <cstdlib>
bool a = true;



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    M_Player = new QMediaPlayer;

    // Obtiene los punteros a los widgets del archivo .ui
    progressBar_2 = ui->progressBar_2;
    memor = ui->memor;
    timer = new QTimer(this);

    // Configura el temporizador para actualizar el uso de memoria (como se mostró anteriormente)
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateMemoryUsage);
    timer->start(1000); // Intervalo de actualización de 1 segundo

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateMemoryUsage()
{
    // Ejecuta el comando "free" para obtener información sobre el uso de memoria.
    QProcess process;
    process.start("free", QStringList() << "-m");

    if (process.waitForFinished()) {
        QByteArray output = process.readAllStandardOutput();
        QString outputStr(output);

        // Analiza la salida para obtener la información deseada.
        QStringList lines = outputStr.split("\n");
        if (lines.length() >= 2) {
            QString memoryInfo = lines[1].simplified();

            // Extrae los valores de memoria total y en uso en MB.
            QStringList values = memoryInfo.split(" ");
            if (values.length() >= 7) {  // Asumimos que los valores están en las posiciones adecuadas
                int totalMemoryMB = values[1].toInt();
                int usedMemoryMB = values[2].toInt();

                // Convierte de MB a GB (1 GB = 1024 MB)
                double totalMemoryGB = static_cast<double>(totalMemoryMB) / 1024.0;
                double usedMemoryGB = static_cast<double>(usedMemoryMB) / 1024.0;

                // Actualiza el QLabel con el uso de memoria en GB.
                QString memoryUsageText = "Memoria en uso: " + QString::number(usedMemoryGB, 'f', 2) + " GB";
                ui->progressBar_2->setValue(static_cast<int>((usedMemoryGB / totalMemoryGB)*100));
                ui->memor->setText(memoryUsageText);
            }
        }
    }
}




void MainWindow::on_Previous_clicked()
{

}


void MainWindow::on_PlayButton_clicked()
{
    if (a==true){
        M_Player->play();
        a=false;
    }
    else{
        M_Player->pause();
        a=true;
    }

}


void MainWindow::on_Next_clicked()
{

}


void MainWindow::on_Info_clicked()
{

}


void MainWindow::on_Load_Library_clicked()
{

    QString FileName = QFileDialog::getOpenFileName(this,tr("Selec audio file"),"",tr("MP3 file (*.mp3)"));
    M_Player->setMedia(QUrl::fromLocalFile(FileName));
    QFileInfo File(FileName);
    ui->label->setText("Cancion: "+File.fileName());

}


void MainWindow::on_checkBox_stateChanged(int arg1)
{

}

void MainWindow::on_progressBar_2_valueChanged(int value)
{

}

