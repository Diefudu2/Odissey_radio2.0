#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtMultimedia>
#include <QMediaPlayer>
#include <QtWidgets>
#include <QFileDialog>
#include <QStringListModel>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


struct SongData {
    QString title;
    QString artist;
    QString album;
    QString genre;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_Previous_clicked();

    void on_PlayButton_clicked();

    void on_Next_clicked();

    void on_Info_clicked();

    void on_Load_Library_clicked();

    void updateMemoryUsage();

    void loadData(const QModelIndex &index);

    void loadAndDisplayCSVData(const QString &csvFilePath);

<<<<<<< HEAD
    void playAudio(const QModelIndex &index);


=======
    void updateProgressBar();

    void onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
>>>>>>> f05ef26 (.)

private:
    Ui::MainWindow *ui;
    QMediaPlayer *M_Player;
    QProgressBar *progressBar_2;
    QLabel *memor;
    QTimer *timer;
    QStringList folderList;
    QStringListModel *folderListModel;
    QStandardItemModel *metadataModel;
<<<<<<< HEAD

=======
    QTimer *progressTimer;
>>>>>>> f05ef26 (.)
};
#endif // MAINWINDOW_H
