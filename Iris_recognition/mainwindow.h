#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QFileDialog>
#include "displayimage.h"
#include "database.h"
#include <QtCore/QWaitCondition>
#include "iris.cpp"
#include <QtSQL/QSqlQuery>

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
};

#endif // MAINWINDOW_H
