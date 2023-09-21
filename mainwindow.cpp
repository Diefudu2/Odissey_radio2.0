#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QProcess>
#include <cstdlib>
#include <QMessageBox>
#include <QFileDialog>
#include <QStringListModel>
#include <QStandardItem>
#include <iostream>
#include <QItemSelectionModel>
#include <QDebug>

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
    progressBar = ui->progressBar;
    timer = new QTimer(this);

    // Configura el temporizador para actualizar el uso de memoria (como se mostró anteriormente)
    timerb = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateMemoryUsage);
    progressBar = ui->progressBar;

    folderListModel = new QStringListModel(this);
    ui->folderListView->setModel(folderListModel);

    // Inicializa folderList con las carpetas existentes en el proyecto (si las hay)
    folderList = folderListModel->stringList();

    metadataModel = new QStandardItemModel(this);
    metadataModel->setHorizontalHeaderLabels({"Name", "Artist", "Length","Genre"});
    ui->metadataTableView->setModel(metadataModel);
    // Conexión para cargar datos al seleccionar una carpeta
    connect(ui->folderListView->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::loadData);

    //Conexion para cargar datos al seleccionar uns libreria
    connect(ui->metadataTableView, &QTableView::clicked, this, &MainWindow::playAudio);


}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    QModelIndexList selectedIndexes = selected.indexes();
    if (!selectedIndexes.isEmpty()) {
        // Obtener el contenido de la primera columna de la fila seleccionada
        QModelIndex firstColumnIndex = selectedIndexes.first();
        QVariant data = ui->metadataTableView->model()->data(firstColumnIndex);
        QString firstColumnValue = data.toString();
        qDebug() << "Fila seleccionada: " << firstColumnIndex.row() << ", Valor de la primera columna: " << firstColumnValue;
    }
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




void MainWindow::loadData(const QModelIndex &index)
{
    if (index.isValid()) {
        QString folderName = folderListModel->data(index, Qt::DisplayRole).toString();
        QString csvFilePath = folderName + "/" + folderName + ".csv"; // Ruta relativa desde el directorio de trabajo actual

        loadAndDisplayCSVData(csvFilePath);
    }
}

void MainWindow::loadAndDisplayCSVData(const QString &csvFilePath)
{
    // Borra el modelo actual
    metadataModel->clear();
    metadataModel->setHorizontalHeaderLabels({"Name", "Artist", "Length","Genre"});

    // Carga los metadatos desde el archivo CSV
    QFile file(csvFilePath);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);

        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList fields = line.split(',');

            if (fields.size() >= 4) {
                QString title = fields[0];
                QString artist = fields[1];
                QString album = fields[2];
                QString genre = fields [3];

                QList<QStandardItem*> row;
                row << new QStandardItem(title)
                    << new QStandardItem(artist)
                    << new QStandardItem(album)
                    << new QStandardItem(genre)
                    ;


                metadataModel->appendRow(row);
            }
        }

        file.close();
    }
}


void MainWindow::playAudio(const QModelIndex &index)
{
    if (index.isValid()) {
        QString title = metadataModel->item(index.row(), 0)->text();
        QString folderName = folderListModel->data(ui->folderListView->currentIndex(), Qt::DisplayRole).toString();
        QString audioFilePath = QDir::currentPath() + "/" + folderName + "/" + title + ".mp3";

        M_Player->setMedia(QUrl::fromLocalFile(audioFilePath));

        // Conecta la señal positionChanged del reproductor de medios a una ranura para actualizar el progreso
        connect(M_Player, &QMediaPlayer::positionChanged, this, &MainWindow::updateProgressBar);

        M_Player->play();
        ui->label->setText("Cancion: " + title);
    }
}

// Función para actualizar la barra de progreso
void MainWindow::updateProgressBar(qint64 position)
{
    if (M_Player->duration() > 0) {
        int progress = static_cast<int>((position * 100) / M_Player->duration());
        ui->progressBar->setValue(progress);
    }
}


void MainWindow::on_Previous_clicked()
{

}


void MainWindow::on_PlayButton_clicked()
{
    //M_Player->setMedia(QUrl::fromLocalFile("/home/andresc/Escritorio/Odissey_radio/Canciones/believer.mp3"));
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
    QString folderPath = QFileDialog::getExistingDirectory(this, "Seleccionar carpeta", "", QFileDialog::ShowDirsOnly);

    if (!folderPath.isEmpty()) {
        QString folderName = QFileInfo(folderPath).fileName();

        // Agregar el nombre de la carpeta al modelo de datos
        QStringList folderList = folderListModel->stringList();
        folderList.append(folderName);
        folderListModel->setStringList(folderList);

        // Copiar la carpeta seleccionada al directorio del proyecto
        QString projectDir = QDir::currentPath(); // Obtener el directorio actual del proyecto
        QString destinationPath = projectDir + "/" + folderName;

        QDir destinationDir(destinationPath);

        if (!destinationDir.exists()) {
            if (!QDir().mkpath(destinationPath)) {
                QMessageBox::warning(this, tr("Error"), tr("No se pudo crear la carpeta en el proyecto."));
                return;
            }
        }

        // Copiar los archivos y subdirectorios de la carpeta seleccionada
        QDir sourceDir(folderPath);
        QStringList files = sourceDir.entryList(QDir::Files);
        foreach (const QString &file, files) {
            QString srcFilePath = folderPath + "/" + file;
            QString destFilePath = destinationPath + "/" + file;
            QFile::copy(srcFilePath, destFilePath);
        }

        // Puedes mostrar un mensaje de éxito si lo deseas
        QMessageBox::information(this, tr("Éxito"), tr("La carpeta se ha copiado y cargado en el proyecto."));
    }
}



