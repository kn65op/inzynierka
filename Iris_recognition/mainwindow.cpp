#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <sstream>
#include <QDir>

Camera MainWindow::c = Camera(0);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
    {
        ui->setupUi(this);
        image_loaded = false;
//        camera = cvCaptureFromCAM(-1);
  //      camera = cvCreateCameraCapture(-1);
    //    assert(camera);
        image = 0;
    }

MainWindow::~MainWindow()
{
    delete ui;
    thread.terminate();
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::on_submitButton_clicked()
{
   if (!image)
   {
       return;
   }
   static int file_nr = 0;
   // m_netwManager = new QNetworkAccessManager(this);
   // connect(m_netwManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slot_netwManagerFinished(QNetworkReply*)));

   QString name    = ui->nameField->text();
   QString surname = ui->surnameField->text();
   QString group   = ui->groupField->text();
   QString faculty = ui->facultyField->text();
   stringstream ss;
   ss << file_nr++;
   QString file_no = ss.str().c_str();
   QString filename = group + "-" + name + "-" + surname + "-" + faculty + file_no + ".bmp";
   QString catalog  = "capture/";
   path = catalog + filename;

   cvSaveImage(path.toStdString().c_str(), image);

   // QUrl urlSave("http://usteni.gliczarow.info.pl/webcam.jpeg?1293535363894");
   // QUrl urlSave("http://192.168.0.1/now.jpg?snap=pre?ww=1600?wh=1200");

   //QNetworkRequest request(urlSave);
   //m_netwManager->get(request);
   /* moje
    eye.init(path);
   QPixmap img = QPixmap(path);
   ui->imageLabel->setPixmap(img.scaled(700, 525));
   image_loaded = true;
*/
  /* eye.pupil(ui->binaryEdit->text().toInt());
   eye.iris();
   eye.masking();

   if(db.insertUser(ui->nameField->text(), ui->surnameField->text(), ui->groupField->text(), ui->facultyField->text(), eye.get_mask())) {
     ui->resultLabel->setText("User was added!");
     qDebug("Zapisano do bazy.");
   } else {
     qDebug("Wyst¹pi³ b³¹d przy zapisywaniu.");
   }*/
}

void MainWindow::slot_netwManagerFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Error in" << reply->url() << ":" << reply->errorString();
        return;
    }

    QVariant attribute = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    if (attribute.isValid()) {
        QUrl url = attribute.toUrl();
        qDebug() << "must go to:" << url;
        return;
    }

    QByteArray jpegData = reply->readAll();
    QPixmap pixmap;
    pixmap.loadFromData(jpegData);

    if(pixmap.save(path, "jpg")) {
        qDebug() << "File was saved!";

        /*eye.init(path);
        //eye.pupil(ui->binaryEdit->text().toInt());
        //eye.iris();
        //eye.masking();

        if(db.insertUser(ui->nameField->text(), ui->surnameField->text(), ui->groupField->text(), ui->facultyField->text(), eye.get_mask())) {
            ui->resultLabel->setText("User was added!");
            qDebug("Zapisano do bazy.");
        } else {
            qDebug("Wyst¹pi³ b³¹d przy zapisywaniu.");
        }*/

    } else {
        qDebug() << "Error: Didn't saved!";
    }
}

void MainWindow::on_activeCameraButton_clicked()
{
    startTimer(40);
    //connect(&thread, SIGNAL(setPixmap(const QPixmap&)), ui->imageLabel, SLOT(setPixmap(const QPixmap&)));
    //thread.start();
}

void MainWindow::on_choosePhotoButton_clicked()
{
    ui->resultLabel->setText("");
    filepath = QFileDialog::getOpenFileName(this, tr("Wybierz plik..."), "./photos", tr("Obrazki (*.jpg *.png *.bmp)") );
    if (!filepath.isEmpty ())
    {
        eye.init(filepath);
        QPixmap img = QPixmap(filepath);
        ui->imageLabel->setPixmap(img.scaled(700, 525));
        image_loaded = true;
    }
}

void MainWindow::on_findPupilButton_clicked()
{
    if (!image_loaded)
    {
        QMessageBox box;
        box.setText("Najpierw wczytaj zdjecie");
        box.exec();
        return;
    }
    eye.pupil(ui->binaryEdit->text().toInt());
    cvDestroyAllWindows();
    Image::showImage(eye.img, "1. Find pupil");
}

void MainWindow::on_findIrisButton_clicked()
{
    if (!image_loaded)
    {
        QMessageBox box;
        box.setText("Najpierw wczytaj zdjecie");
        box.exec();
        return;
    }
    eye.iris();
    cvDestroyAllWindows();
    Image::showImage(eye.img, "2. Find iris");
}

void MainWindow::on_divideIrisButton_clicked()
{
    if (!image_loaded)
    {
        QMessageBox box;
        box.setText("Najpierw wczytaj zdjecie");
        box.exec();
        return;
    }
    eye.draw_semicircles();
    cvDestroyAllWindows();
    Image::showImage(eye.img, "3. Divide the areas");
}

void MainWindow::on_makeMaskButton_clicked()
{
    if (!image_loaded)
    {
        QMessageBox box;
        box.setText("Najpierw wczytaj zdjecie");
        box.exec();
        return;
    }
    eye.masking();
    Image::showBinaryImage(eye.getMaskImage(), "4. Mask");
}

void MainWindow::on_searchButton_clicked()
{
    if (!image_loaded)
    {
        QMessageBox box;
        box.setText("Najpierw wczytaj zdjecie");
        box.exec();
        return;
    }
    ui->resultLabel->setText("");
    QString binary = ui->binaryEdit->text();
    eye.pupil(binary.toInt());
    eye.iris();
    eye.masking();

    QString iris_code;
    QSqlQuery query("SELECT `id`, `name`, `surname`, `iris_code`, `group` FROM users");
    query.exec();

    QString result;

    while(query.next()) {
        iris_code = query.value(3).toString();
        if(eye.compare(iris_code)) {
            // result = "You are: " + query.value(1).toString() + " " + query.value(2).toString() + ".\nWelcome!";
            // break;
        } else {
            // result = "Don't find You.";
        }

        qDebug() << query.value(1).toString() << query.value(2).toString() << query.value(4).toString() << ", H: " << eye.hamming;
    }

    ui->resultLabel->setText(result);
}

void MainWindow::timerEvent(QTimerEvent *)
{
    //c.getFrame();
    //IplImage *image=cvQueryFrame(camera);
    if (image) delete image;
    MainWindow::c.getFrame(&image);
    //image = cvQueryFrame(camera);
    if (image) ui->cameraWidget->putImage(image);
}


void MainWindow::on_actionTestuj_folder_triggered()
{
    filepath = QFileDialog::getExistingDirectory(this, tr("Wybierz katalog do sprawdzenia"), ".");
    QStringList images_extensions;
    images_extensions << "*.jpg" << "*.bmp";
    QDir dir(filepath);
    dir.setFilter(QDir::Files);
    QFileInfoList list = dir.entryInfoList(images_extensions);
    for (int i=0; i<list.size(); i++)
    {
        Iris tmp;
        tmp.init(list.at(i).absoluteFilePath());
        try
        {
            if (tmp.pupil())
            {
                cvDestroyAllWindows();
                //Image::showImage(tmp.img, "1. Find pupil");
                Image::showImage(tmp.img, list.at(i).fileName().toStdString().c_str());
                while (cvWaitKey(1000) < 0);
                /*tmp.iris();
                cvDestroyAllWindows();
                Image::showImage(tmp.img, "2. Find iris");
                while (cvWaitKey(1000) < 0);*/
            }
            else
            {
                QMessageBox box;
                box.setText("Dla zdjecia nie znaleziono zrenicy");
                box.exec();
            }
        }
        catch (...)
        {

        }

        //Tworzenie kodów i porównywanie ich
        //qDebug() << list.at(i).absoluteFilePath();
    }
}
