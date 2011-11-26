#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
        ui->setupUi(this);
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
   // m_netwManager = new QNetworkAccessManager(this);
   // connect(m_netwManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slot_netwManagerFinished(QNetworkReply*)));

   QString name    = ui->nameField->text();
   QString surname = ui->surnameField->text();
   QString group   = ui->groupField->text();
   QString faculty = ui->facultyField->text();

   QString filename = group + "-" + name + "-" + surname + "-" + faculty + ".jpg";
   QString catalog  = "capture/";
   path = catalog + filename;

   // QUrl urlSave("http://usteni.gliczarow.info.pl/webcam.jpeg?1293535363894");
   // QUrl urlSave("http://192.168.0.1/now.jpg?snap=pre?ww=1600?wh=1200");

   //QNetworkRequest request(urlSave);
   //m_netwManager->get(request);

   eye.pupil(ui->binaryEdit->text().toInt());
   eye.iris();
   eye.masking();

   if(db.insertUser(ui->nameField->text(), ui->surnameField->text(), ui->groupField->text(), ui->facultyField->text(), eye.get_mask())) {
     ui->resultLabel->setText("User was added!");
     qDebug("Zapisano do bazy.");
   } else {
     qDebug("Wyst�pi� b��d przy zapisywaniu.");
   }
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
            qDebug("Wyst�pi� b��d przy zapisywaniu.");
        }*/

    } else {
        qDebug() << "Error: Didn't saved!";
    }
}

void MainWindow::on_activeCameraButton_clicked()
{
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
        ui->imageLabel->setPixmap(img.scaled(700, 525));
    }
}

void MainWindow::on_findPupilButton_clicked()
{
    eye.pupil(ui->binaryEdit->text().toInt());
    cvDestroyAllWindows();
    Image::showImage(eye.img, "1. Find pupil");
}

void MainWindow::on_findIrisButton_clicked()
{
    eye.iris();
    cvDestroyAllWindows();
    Image::showImage(eye.img, "2. Find iris");
}

void MainWindow::on_divideIrisButton_clicked()
{
    eye.draw_semicircles();
    cvDestroyAllWindows();
    Image::showImage(eye.img, "3. Divide the areas");
}

void MainWindow::on_makeMaskButton_clicked()
{
    eye.masking();
    Image::showBinaryImage(eye.getMaskImage(), "4. Mask");
}

void MainWindow::on_searchButton_clicked()
{
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