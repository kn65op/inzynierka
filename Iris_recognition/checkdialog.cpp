#include "checkdialog.h"
#include "ui_checkdialog.h"

checkDialog::checkDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::checkDialog)
{
    ui->setupUi(this);
}

checkDialog::~checkDialog()
{
    delete ui;
}

void checkDialog::setImage(QString file)
{
    QPixmap img = QPixmap(file);
    ui->image->setPixmap(img.scaled(800, 600));
}
