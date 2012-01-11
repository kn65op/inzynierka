#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QFileDialog>
#include "displayimage.h"
#include "database.h"
#include <QtCore/QWaitCondition>
#include "iris.cpp"
#include <QtSQL/QSqlQuery>
#include "QOpenCVWidget.h"
#include <highgui.h>
#include <cv.h>
#include "camera.h"
#include "vector"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    QString path;
    displayImage thread;
    QNetworkAccessManager *m_netwManager;
    Iris    eye;
    QString filepath;
    Database db;
    bool image_loaded;
//    CvCapture *camera;
    static Camera c;
    IplImage *image;
    bool checkSegmentation();
    void addToDB();
    std::vector<int> specialisation_map;
    std::vector<int> group_map;

private slots:
    void on_searchButton_clicked();
    void on_makeMaskButton_clicked();
    void on_divideIrisButton_clicked();
    void on_findIrisButton_clicked();
    void on_findPupilButton_clicked();
    void on_choosePhotoButton_clicked();
    void on_activeCameraButton_clicked();
    void on_submitButton_clicked();
    void slot_netwManagerFinished(QNetworkReply *reply);
    void timerEvent(QTimerEvent *);
    void on_actionTestuj_folder_triggered();
    void on_actionTestuj_baz_triggered();
    void on_actionEdycja_bazy_danych_triggered();
    void on_actionStw_rz_baz_danych_usunie_istniej_ce_dane_triggered();
    void on_Specialisation_currentIndexChanged(int index);
    void on_Specialisation_activated(int index);
};

#endif // MAINWINDOW_H
