#include "tabwidget.h"
#include "ui_tabwidget.h"

#include "QMessageBox"

#include <stdexcept>

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

void TabWidget::fillFaculties()
{
    fillFaculties(ui->faculties_all);
    ui->faculties_specialisation->setEditable(false);
}

void TabWidget::fillClasses()
{
 //   fillClasses(ui->classes_all);
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
    fillFaculties(ui->subjects_faculty);
}

void TabWidget::fillTopics()
{
    fillTopics(ui->topics_all);
    fillSubjects(ui->topics_subject);
}

void TabWidget::fillFaculties(QComboBox *to)
{
    faculties_map.clear();
    resetComboBox(to);
    QSqlQuery *query = db->getFaculties();
    //zapisanie wszystkich
    while(query->next())
    {
        faculties_map.push_back(query->value(0).toInt());
        to->addItem(query->value(1).toString());
    }
    if (query) delete query;
}

void TabWidget::fillSpecialisations(QComboBox *to, int faculty_id)
{
    resetComboBox(to);
    QSqlQuery *query = db->getSpecialisationsByFacultyId(faculty_id);
    while(query->next())
    {
        to->addItem(query->value(2).toString());
    }
    if (query) delete query;
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

void TabWidget::resetComboBox(QComboBox *what)
{
    what->clear();
    what->addItem("");
}

void TabWidget::on_faculties_all_currentIndexChanged(int index)
{

}

void TabWidget::on_faculties_all_activated(int index)
{
    try
    {
        fillSpecialisations(ui->faculties_specialisation, faculties_map.at(index-1));
        ui->faculties_specialisation->setEditable(true);
    }
    catch (std::out_of_range)
    {
        ui->faculties_specialisation->setEditable(false);
        resetComboBox(ui->faculties_specialisation);
    }
}
