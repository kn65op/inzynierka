#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "checkdialog.h"
#include "dbdialog.h"
#include <QMessageBox>
#include <sstream>
#include <QDir>
#include <fstream>

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
        ui->cameraWidget->setVisible(false);
        //dodanie kierunkow
        QSqlQuery *query = db.getSpecialisations();
        while (query->next())
        {
            ui->Specialisation->addItem(query->value(2).toString() + " " + db.getFacultyById(query->value(1).toInt()));
            specialisation_map.push_back(query->value(0).toInt());
        }
        delete query;
        on_Specialisation_activated(0);
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
   if (image_loaded)
   {
       if (checkSegmentation())
       {
           addToDB();
       }
   }
   return;
   static int file_nr = 0;
    m_netwManager = new QNetworkAccessManager(this);
    connect(m_netwManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slot_netwManagerFinished(QNetworkReply*)));


   // QUrl urlSave("http://usteni.gliczarow.info.pl/webcam.jpeg?1293535363894");
    QUrl urlSave("http://192.168.0.214/now.jpg?snap=pre?ww=1600?wh=1200");

   QNetworkRequest request(urlSave);
   m_netwManager->get(request);

   QString name    = ui->nameField->text();
   QString surname = ui->surnameField->text();
//   QString group   = ui->groupField->text();
//   QString faculty = ui->facultyField->text();
   stringstream ss;
   ss << file_nr++;
   QString file_no = ss.str().c_str();
//   QString filename = group + "-" + name + "-" + surname + "-" + faculty + file_no + ".jpg";
   QString catalog  = "capture/";
//   path = catalog + filename;

   //cvSaveImage(path.toStdString().c_str(), image);

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
    qDebug() << "re[ly\
                ";
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
    pixmap.save("tmp111.jpg");

    QString name    = ui->nameField->text();
    QString surname = ui->surnameField->text();
//    QString group   = ui->groupField->text();
//    QString faculty = ui->facultyField->text();
    stringstream ss;
//odkomentowaæ!!!
    //ss << file_nr++;
//odkomentowaæ!!!
    QString file_no = ss.str().c_str();
//    QString filename = group + "-" + name + "-" + surname + "-" + faculty + file_no + ".jpg";
    QString catalog  = "capture/";
//    path = catalog + filename;

    qDebug() << "path: " << path;

    //sprawdzenie
    if (!checkSegmentation())
    {
        return;
    }
    //dodanie do bazy
    addToDB();

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
    //startTimer(40);
    //ui->cameraWidget->setVisible(true);
    connect(&thread, SIGNAL(setPixmap(const QPixmap&)), ui->imageLabel, SLOT(setPixmap(const QPixmap&)));
    thread.start();
}

void MainWindow::on_choosePhotoButton_clicked()
{
    ui->resultLabel->setText("");
    filepath = QFileDialog::getOpenFileName(this, tr("Wybierz plik..."), "./photos", tr("Obrazki (*.jpg *.png *.bmp)") );
    if (!filepath.isEmpty ())
    {
        eye.init(filepath);
        QPixmap img = QPixmap(filepath);
        if (img.isNull())
        {
            QMessageBox box;
            box.setText("Zdjêcie jest niepoprawne");
            box.exec();
            return;
        }
        //tmp
        img.save("tmp.jpg");
        //tmp
        ui->imageLabel->setPixmap(img.scaled(700, 525));
        image_loaded = true;
        //ui->cameraWidget->setVisible(false);
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
    eye.pupil();
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
    IplImage * bin = eye.getMaskImage();
    Image::showBinaryImage(bin, "4. Mask");
    cvReleaseImage(&bin);
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

    eye.pupil();
    eye.iris();
    eye.masking();

    QSqlQuery *query = db.searchStudents();

    QString result = "Nie znaleziono osoby";
    while (query->next())
    {
        qDebug() << query->value(1).toString() << " " << eye.compare(query->value(2).toString());
        if (eye.compare(query->value(2).toString()))
        {
            //QMessageBox box;
            //box.setText(
            result = "Osoba jest podobna do osoby " + query->value(0).toString() + " " + query->value(1).toString();//);
            //box.exec();
            break;
        }
        /*else
        {
            file_out << "0;";
            QMessageBox box;
            box.setText("Osoba " + list.at(i).fileName() + " NIE jest podobna do osoby z id " + query->value(0).toString());
            box.exec();
        }*/
    }
    delete query;

/*    QString iris_code;
    QSqlQuery query("SELECT `id`, `name`, `surname`, `iris_code`, `group` FROM users");
    query.exec();

    QString result;

    while(query.next()) {
        iris_code = query.value(3).toString();
        if(eye.compare(iris_code)) {
             result = "You are: " + query.value(1).toString() + " " + query.value(2).toString() + ".\nWelcome!";
             break;
        } else {
             result = "Don't find You.";
        }

        qDebug() << query.value(1).toString() << query.value(2).toString() << query.value(4).toString() << ", H: " << eye.hamming;
    }
*/
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
    db.createDB();
    filepath = QFileDialog::getExistingDirectory(this, tr("Wybierz katalog do dodania do bazy"), ".");
    QStringList images_extensions;
    images_extensions << "*.jpg" << "*.bmp";
    QDir dir(filepath);
    dir.setFilter(QDir::Files);
    QFileInfoList list = dir.entryInfoList(images_extensions);
    for (int i=0; i<list.size(); i++)
    {
        string file_name = "zrenica\\";
        qDebug() << list.at(i).fileName();
        Iris tmp;
        tmp.init(list.at(i).absoluteFilePath());
        try
        {
            if (tmp.pupil()) //szukanie Ÿrenicy, jeœli nie znajzdiemy to nie idziemy dalej
            {
                file_name += "ok";
                //wyœwietlanie zdjêcia z zaznaczon¹ Ÿrenic¹
                //cvDestroyAllWindows();
                //Image::showImage(tmp.img, "1. Find pupil");
                //Image::showImage(tmp.img, list.at(i).fileName().toStdString().c_str());
                //while (cvWaitKey(100) < 0);

                //zapisanie obrazu
                //cvSaveImage(list.at(i).fileName().toStdString().c_str(), tmp.img);

                //wyszukiwanie granic têczówki
                tmp.iris();

                //wyœwietlenie granic têczówki oraz Ÿrenicy
                //cvDestroyAllWindows();
                //Image::showImage(tmp.img, "2. Find iris");
                //while (cvWaitKey(1000) < 0);

                //Tworzenie maski têczówki
                tmp.masking();

                //Zapisanie do bazy
                QStringList slist = list.at(i).fileName().split("-");
                db.insertUser(slist.at(1), slist.at(2), slist.at(0), slist.at(3), tmp.get_mask());

            }
            else
            {
                file_name += "nok";
                /*QMessageBox box;
                box.setText("Dla zdjecia nie znaleziono zrenicy");
                box.exec();*/
                qDebug() << "Zle zdjecie";
            }
            file_name +=list.at(i).fileName().toStdString();
            cvSaveImage(file_name.c_str(), tmp.img);
        }
        catch (...)
        {

        }

        //Tworzenie kodów i porównywanie ich
        //qDebug() << list.at(i).absoluteFilePath();
    }
    QMessageBox box;
    box.setText("Koniec");
    box.exec();
}

void MainWindow::on_actionTestuj_baz_triggered()
{
    //tmp
    std::string tmp_file_name = "wynik.csv";
    QSqlQuery *query = db.searchUsers();
    ofstream file_out(tmp_file_name.c_str(), ios::trunc);
    file_out << ";";
    while (query->next())
    {
        file_out << query->value(2).toString().toStdString() << " " << query->value(3).toString().toStdString() << ";";
    }
    file_out << "\n";
    delete query;
    //tmp

    filepath = QFileDialog::getExistingDirectory(this, tr("Wybierz katalog do testowania bazy"), ".");
    QStringList images_extensions;
    images_extensions << "*.jpg" << "*.bmp";
    QDir dir(filepath);
    dir.setFilter(QDir::Files);
    QFileInfoList list = dir.entryInfoList(images_extensions);
    for (int i=0; i<list.size(); i++)
    {
        qDebug() << list.at(i).fileName();
        Iris tmp;
        tmp.init(list.at(i).absoluteFilePath());
        try
        {
            if (tmp.pupil()) //szukanie Ÿrenicy, jeœli nie znajzdiemy to nie idziemy dalej
            {
                //wyœwietlanie zdjêcia z zaznaczon¹ Ÿrenic¹
                //cvDestroyAllWindows();
                //Image::showImage(tmp.img, "1. Find pupil");
                //Image::showImage(tmp.img, list.at(i).fileName().toStdString().c_str());
                //while (cvWaitKey(100) < 0);

                //zapisanie obrazu
                //cvSaveImage(list.at(i).fileName().toStdString().c_str(), tmp.img);

                //wyszukiwanie granic têczówki
                tmp.iris();

                //wyœwietlenie granic têczówki oraz Ÿrenicy
                //cvDestroyAllWindows();
                //Image::showImage(tmp.img, "2. Find iris");
                //while (cvWaitKey(1000) < 0);

                //Tworzenie maski têczówki
                tmp.masking();

                //Zapisanie do bazy
                /*QStringList slist = list.at(i).fileName().split("-");
                db.insertUser(slist.at(1), slist.at(2), slist.at(0), slist.at(3), tmp.get_mask());*/

                //tmp porównanie z tym co jest w bazie
                QSqlQuery *query = db.searchUsers();
                file_out << list.at(i).fileName().toStdString() << ";";

                while (query->next())
                {
                    file_out << tmp.compare(query->value(1).toString()) << ";";
/*                    if (tmp.compare(query->value(1).toString()))
                    {
                        file_out << "1;";
                        /*QMessageBox box;
                        box.setText("Osoba " + list.at(i).fileName() + " jest podobna do osoby z id " + query->value(0).toString());
                        box.exec();///
                    }
                    else
                    {
                        file_out << "0;";
                        /*QMessageBox box;
                        box.setText("Osoba " + list.at(i).fileName() + " NIE jest podobna do osoby z id " + query->value(0).toString());
                        box.exec();///
                    }
  */              }
                file_out << "\n";
                delete query;
            }
            else
            {
                /*QMessageBox box;
                box.setText("Dla zdjecia nie znaleziono zrenicy");
                box.exec();*/
                qDebug() << "Zle zdjecie";
            }
        }
        catch (...)
        {

        }

        //Tworzenie kodów i porównywanie ich
        //qDebug() << list.at(i).absoluteFilePath();
    }

    file_out.close();
    QMessageBox box;
    box.setText("Koniec");
    box.exec();
}

bool MainWindow::checkSegmentation()
{
    eye.init(QString("tmp.jpg"));
    if (!eye.pupil())
    {
        QMessageBox box;
        box.setText("Segmentacja têczówki nie powiod³a siê.");
        box.exec();
        return false;
    }
    eye.iris();
    cvSaveImage("tmpwc.jpg", eye.img);
    checkDialog dialog(this);
    dialog.setImage(QString("tmpwc.jpg"));
    dialog.show();
    return dialog.exec() == QDialog::Accepted;
}

void MainWindow::addToDB()
{
    eye.masking();
    //db.insertUser(ui->nameField->text(), ui->surnameField->text(), ui->groupField->text(), ui->facultyField->text(), eye.get_mask());
    db.insertStudent(ui->nameField->text(), ui->surnameField->text(), specialisation_map[ui->Specialisation->currentIndex()], db.insertEyeCode(eye.get_mask()));
    db.addLastStudentToGroup(group_map[ui->group->currentIndex()]);
}

void MainWindow::on_actionEdycja_bazy_danych_triggered()
{
    DbDialog dbdialog(this);
    dbdialog.setDatabase(&db);
    dbdialog.show();
    dbdialog.exec();
}

void MainWindow::on_actionStw_rz_baz_danych_usunie_istniej_ce_dane_triggered()
{
    db.createDB();
}

void MainWindow::on_Specialisation_currentIndexChanged(int index)
{

}

void MainWindow::on_Specialisation_activated(int index)
{
    if (index < 0)
    {
        return;
    }
    QSqlQuery *query = db.getGroupsBySpecialisationId(specialisation_map[index]);
    ui->group->clear();
    group_map.clear();
    while (query->next())
    {
        group_map.push_back(query->value(0).toInt());
        ui->group->addItem(query->value(1).toString());
    }
}
