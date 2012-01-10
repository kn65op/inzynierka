#include "dbdialog.h"
#include "ui_dbdialog.h"

#include "QMessageBox"

DbDialog::DbDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DbDialog)
{
    ui->setupUi(this);
    ui->tabWidget->fillFaculties();
}

DbDialog::~DbDialog()
{
    delete ui;
}

#define TAB_FACULTY 0
#define TAB_SUBJECT 1
#define TAB_TOPIC 2
#define TAB_GROUP 3
#define TAB_CLASS 4

void DbDialog::on_tabWidget_currentChanged(int index)
{
    QMessageBox box;
    switch(index)
    {
        case TAB_FACULTY:
            ui->tabWidget->fillFaculties();
            break;
        case TAB_TOPIC:
            ui->tabWidget->fillTopics();
            break;
        case TAB_SUBJECT:
            ui->tabWidget->fillSubjects();
            break;
        case TAB_GROUP:
            ui->tabWidget->fillGroups();
            break;
        case TAB_CLASS:
            ui->tabWidget->fillClasses();
            break;
        default:
            break;
    }
}

void DbDialog::setDatabase(Database *d)
{
    ui->tabWidget->setDatabase(d);
}


void DbDialog::on_okButton_clicked()
{

}

void DbDialog::on_applyButton_clicked()
{

}

void DbDialog::on_cancelButton_clicked()
{
    done(0);
}
