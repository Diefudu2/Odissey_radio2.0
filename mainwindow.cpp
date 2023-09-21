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

    // Inicializa variables para paginación
    totalRows = 0; // Agrega esta línea para asegurarte de que totalRows esté inicializado
    currentPage = 0; // Agrega esta línea para asegurarte de que currentPage esté inicializado
    paginationEnabled = false; // Inicializa la paginación como deshabilitada
    recalculateItemsPerPage(); // Calcula el número de elementos por página


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


    connect(ui->metadataTableView, &QTableView::clicked, this, &MainWindow::playAudio);

    // Conecta el estado cambiado del QCheckBox al slot correspondiente
    connect(ui->checkBox, &QCheckBox::stateChanged, this, &MainWindow::on_checkBox_stateChanged);

    // Calcula el número de elementos por página en función del tamaño de la ventana
    int windowHeight = this->height(); // Altura de la ventana principal
    int estimatedItemHeight = 30; // Supongamos una altura estimada de un elemento

    // Inicialmente, en modo de paginación, ajusta el número de elementos por página en función del tamaño de la ventana
    itemsPerPage = windowHeight / estimatedItemHeight;

    // Restringe itemsPerPage a un valor mínimo para evitar problemas si la ventana es muy pequeña
    int minItemsPerPage = 1; // Establece el mínimo a 1
    if (itemsPerPage < minItemsPerPage) {
        itemsPerPage = minItemsPerPage;
    }

    connect(ui->metadataTableView->verticalScrollBar(), &QScrollBar::valueChanged, this, &MainWindow::onScrollValueChanged);
    // Conecta el estado cambiado del QCheckBox al slot correspondiente
    connect(ui->checkBox, &QCheckBox::stateChanged, this, &MainWindow::on_checkBox_stateChanged);
}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadData(const QModelIndex &index)
{
    if (index.isValid()) {
        QString folderName = folderListModel->data(index, Qt::DisplayRole).toString();
        QString csvFilePath = folderName + "/" + folderName + ".csv"; // Ruta relativa desde el directorio de trabajo actual
        loadAndDisplayCSVData();
    }
}
void MainWindow::recalculateItemsPerPage()
{
    // Calcula el número de elementos por página en función del tamaño de la ventana
    int estimatedItemHeight = 30; // Supongamos una altura estimada de un elemento

    // Restringe itemsPerPage a un valor mínimo para evitar problemas si la ventana es muy pequeña
    int minItemsPerPage = 1; // Establece el mínimo a 1
    itemsPerPage = ui->metadataTableView->height() / estimatedItemHeight;
    if (itemsPerPage < minItemsPerPage) {
        itemsPerPage = minItemsPerPage;
    }
}

void MainWindow::loadPages(int startIndex, int endIndex)
{
    // Limpia el modelo actual
    metadataModel->removeRows(0, metadataModel->rowCount());

    // Carga las filas para el rango de páginas especificado
    for (int row = startIndex; row <= endIndex; row++) {
        QStandardItem* item = new QStandardItem(metadataModel->item(row, 0)->text());
        metadataModel->appendRow(item);
    }
}


void MainWindow::loadAndDisplayCSVData()
{
    // Borra el modelo actual
    metadataModel->clear();
    metadataModel->setHorizontalHeaderLabels({"Name", "Artist", "Length", "Genre"});

    // Obtén la carpeta actualmente seleccionada
    QString folderName = folderListModel->data(ui->folderListView->currentIndex(), Qt::DisplayRole).toString();

    // Construye la ruta completa del archivo CSV
    QString csvFilePath = folderName + "/" + folderName + ".csv";

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
                QString genre = fields[3];

                QList<QStandardItem*> row;
                row << new QStandardItem(title)
                    << new QStandardItem(artist)
                    << new QStandardItem(album)
                    << new QStandardItem(genre);

                metadataModel->appendRow(row);
            }
        }

        file.close();

        // Actualiza la variable totalRows después de cargar los datos
        totalRows = metadataModel->rowCount();

        if (paginationEnabled) {
            // Recalcula el número total de páginas
            totalPages = (totalRows + itemsPerPage - 1) / itemsPerPage; // Redondea hacia arriba

            // Si la paginación está habilitada, carga la página actual
            loadPages(0, itemsPerPage * 3 - 1);

        }
    }
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




// Función para actualizar la barra de progreso
void MainWindow::updateProgressBar()
{

    QMediaPlayer::State playbackState = M_Player->state();
    // Verifica si hay una canción en reproducción
    if (playbackState == QMediaPlayer::PlayingState || playbackState == QMediaPlayer::PausedState) {
        // Obtiene la duración total de la canción en milisegundos
        qint64 duration = M_Player->duration();

        // Obtiene el tiempo actual de reproducción en milisegundos
        qint64 position = M_Player->position();

        // Calcula el progreso como un porcentaje
        int progress = static_cast<int>((position * 100) / duration);
        // Actualiza la barra de progreso
        ui->progressBar->setValue(progress);
    }else{
        ui->progressBar->setValue(0);
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
    qint64 newPosition = M_Player->position() - 5000; // 5000 milisegundos = 5 segundos
    M_Player->setPosition(qMax(newPosition, qint64(0))); // Asegura que la posición no sea negativa

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
    qint64 newPosition = M_Player->position() + 5000; // 5000 milisegundos = 5 segundos
    M_Player->setPosition(qMin(newPosition, M_Player->duration())); // Asegura que la posición no supere la duración total


}


void MainWindow::on_Info_clicked()
{

}

void MainWindow::on_checkBox_stateChanged(int state)
{
    paginationEnabled = (state != Qt::Checked); // Desactivar paginación si el checkbox está marcado
    if (paginationEnabled) {
        loadAndDisplayCSVData(); // Cargar todos los datos
    }
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

void MainWindow::onScrollValueChanged(int value)
{
    // Calcula las páginas que se deben cargar en función de la posición actual del scroll
    int startIndex = (value / itemsPerPage) * itemsPerPage;
    int endIndex = qMin(startIndex + (itemsPerPage * 3) - 1, totalRows - 1);

    // Verifica si es necesario cargar más páginas
    if (startIndex != lastLoadedPage) {
        currentPage = startIndex / itemsPerPage;
        loadPages(startIndex, endIndex);
        lastLoadedPage = startIndex;
        qDebug() << "Página actual:" << currentPage + 1;
    }
}

