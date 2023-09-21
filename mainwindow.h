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
    void loadAndDisplayCSVData();
    void playAudio(const QModelIndex &index);
<<<<<<< HEAD



    void updateProgressBar(qint64 position);

=======
    void updateProgressBar();
>>>>>>> 6cb5334 (.)
    void onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void loadPages(int startIndex, int endIndex);
    void recalculateItemsPerPage();
    void onScrollValueChanged(int value);
    void on_checkBox_stateChanged(int state);

private:
    Ui::MainWindow *ui;
    QMediaPlayer *M_Player;
    QProgressBar *progressBar_2;
    QProgressBar *progressBar;
    QLabel *memor;
    QTimer *timer;
    QTimer *timerb;
    QStringList folderList;
    QStringListModel *folderListModel;
    QStandardItemModel *metadataModel;
    QTimer *progressTimer;
    int currentPage = 0;
    int itemsPerPage = 0;
    int totalPages = 0;
    bool paginationEnabled = true;
    int totalRows = 0;
    int lastLoadedPage = -1;
};

#endif // MAINWINDOW_H
