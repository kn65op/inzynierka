#include "dbdialog.h"
#include "ui_dbdialog.h"

DbDialog::DbDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DbDialog)
{
    ui->setupUi(this);
}

DbDialog::~DbDialog()
{
    delete ui;
}
