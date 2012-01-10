#include "tabwidget.h"
#include "ui_tabwidget.h"

#include "QMessageBox"

TabWidget::TabWidget(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::TabWidget)
{
    ui->setupUi(this);
}

TabWidget::~TabWidget()
{
    delete ui;
}

void TabWidget::on_TabWidget_currentChanged(int index)
{
}

void TabWidget::fillFaculties()
{
    fillFaculties(ui->faculties_all);
}

void TabWidget::fillClasses()
{
    fillClasses(ui->classes_all);
    fillFaculties(ui->classes_faculty);
    fillGroups(ui->classes_group);
    fillSubjects(ui->classes_subject);
}

void TabWidget::fillGroups()
{
    fillGroups(ui->groups_all);
    fillFaculties(ui->groups_faculty);
}

void TabWidget::fillSubjects()
{
    fillSubjects(ui->subjects_all);
    fillFaculties(ui->faculties_all);
}

void TabWidget::fillTopics()
{
    fillTopics(ui->topics_all);
    fillSubjects(ui->topics_subject);
}

void TabWidget::fillFaculties(QComboBox *to)
{

}

void TabWidget::fillClasses(QComboBox *to)
{

}

void TabWidget::fillGroups(QComboBox *to)
{

}

void TabWidget::fillSubjects(QComboBox *to)
{

}

void TabWidget::fillTopics(QComboBox *to)
{

}

void TabWidget::setDatabase(Database *d)
{
    db = d;
}
