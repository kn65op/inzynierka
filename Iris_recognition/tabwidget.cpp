#include "tabwidget.h"
#include "ui_tabwidget.h"
#include "QTime"

#include "QMessageBox"

#include <stdexcept>

TabWidget::TabWidget(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::TabWidget)
{
    ui->setupUi(this);
    QStringList list;
    list.append("poniedzia³ek");
    list.append("wtorek");
    list.append("œroda");
    list.append("czwartek");
    list.append("pi¹tek");
    list.append("sobota");
    list.append("niedziela");
    ui->groups_day->addItems(list);
}

TabWidget::~TabWidget()
{
    delete ui;
}

void TabWidget::fillFaculties(int index)
{
    fillFaculties(ui->faculties_all);
    ui->faculties_specialisation->setEditable(false);
    ui->faculties_all->setCurrentIndex(index);
}

void TabWidget::fillClasses()
{
 //   fillClasses(ui->classes_all);
    fillFaculties(ui->classes_faculty);
    //fillGroups(ui->classes_group);
    //ui->classes_faculty->setCurrentIndex(indexf);
    //ui->cl
//    fillSubjects(ui->classes_subject);
}

void TabWidget::fillGroups(int indexf, int indexs)
{
    fillFaculties(ui->groups_faculty);
    ui->groups_faculty->setCurrentIndex(indexf);
    ui->groups_specialisation->setCurrentIndex(indexs);
}

void TabWidget::fillSubjects(int indexf, int indexs)
{
    fillFaculties(ui->subjects_faculty);
    ui->subjects_faculty->setCurrentIndex(indexf);
    ui->subjects_specialisation->setCurrentIndex(indexs);
}

void TabWidget::fillTopics(int indexf, int indexs, int indexsub)
{
    fillFaculties(ui->topics_faculty);
    ui->topics_faculty->setCurrentIndex(indexf);
    ui->topics_specialisation->setCurrentIndex(indexs);
    ui->topics_subject->setCurrentIndex(indexsub);
}

void TabWidget::fillFaculties(QComboBox *to, bool empty_space)
{
    faculties_map.clear();
    if (empty_space) faculties_map.push_back(-1);
    resetComboBox(to, empty_space);
    QSqlQuery *query = db->getFaculties();
    //zapisanie wszystkich
    while(query->next())
    {
        faculties_map.push_back(query->value(0).toInt());
        to->addItem(query->value(1).toString());
    }
    if (query) delete query;
}

void TabWidget::fillSpecialisations(QComboBox *to, int faculty_id, bool empty_space)
{
    resetComboBox(to, empty_space);
    specialisations_map.clear();
    if (empty_space) specialisations_map.push_back(-1);
    QSqlQuery *query = db->getSpecialisationsByFacultyId(faculty_id);
    while(query->next())
    {
        to->addItem(query->value(2).toString());
        specialisations_map.push_back(query->value(0).toInt());
    }
    if (query) delete query;
}

void TabWidget::fillClasses(QComboBox *to, bool empty_space)
{

}

void TabWidget::fillGroups(QComboBox *to, int subject_id, bool empty_space)
{
    resetComboBox(to, empty_space);
    groups_map.clear();
    if (empty_space) groups_map.push_back(-1);
    QSqlQuery *query = db->getGroupsBySpecialisationId(subject_id);
    while(query->next())
    {
        to->addItem(query->value(1).toString());
        groups_map.push_back(query->value(0).toInt());
    }
    if (query) delete query;
}

void TabWidget::fillSubjects(QComboBox *to, int specialisation_id, bool empty_space)
{
    resetComboBox(to, empty_space);
    subjects_map.clear();
    if (empty_space) subjects_map.push_back(-1);
    QSqlQuery *query = db->getSubjectsBySpecialisationId(specialisation_id);
    while(query->next())
    {
        to->addItem(query->value(3).toString());
        subjects_map.push_back(query->value(0).toInt());
    }
    if (query) delete query;
}

void TabWidget::fillTopics(QComboBox *to, int subject_id, bool empty_space)
{
    resetComboBox(to, empty_space);
    topics_map.clear();
    if (empty_space) topics_map.push_back(-1);
    QSqlQuery *query = db->getTopicsBySubjectId(subject_id);
    while(query->next())
    {
        to->addItem(query->value(2).toString());
        topics_map.push_back(query->value(0).toInt());
    }
    if (query) delete query;
}

void TabWidget::setDatabase(Database *d)
{
    db = d;
}

void TabWidget::resetComboBox(QComboBox *what, bool empty_space)
{
    what->clear();
    if (empty_space)
    {
        what->addItem("");
    }
}

void TabWidget::on_faculties_all_currentIndexChanged(int index)
{
    on_faculties_all_activated(index);
}

void TabWidget::on_faculties_all_activated(int index)
{
    try
    {
        if (faculties_map.at(index) != -1)
        {
            fillSpecialisations(ui->faculties_specialisation, faculties_map.at(index));
            ui->faculties_specialisation->setEditable(true);
        }
        else
        {
            ui->faculties_specialisation->setEditable(false);
            resetComboBox(ui->faculties_specialisation, false);
        }
    }
    catch (std::out_of_range)
    {
        ui->faculties_specialisation->setEditable(false);
        resetComboBox(ui->faculties_specialisation, false);
    }
}

bool TabWidget::save()
{
    bool ret = true;
    switch (currentIndex())
    {
        case TAB_FACULTY:
            // jest wpisany kierunek, zapisujemy kierunek
            if (ui->faculties_specialisation->currentText() != "")
            {
                //edycja kierunku
                if (ui->faculties_specialisation->currentIndex())
                {
                    ret = db->changeSpecialisationName(specialisations_map[ui->faculties_specialisation->currentIndex()], ui->faculties_specialisation->currentText());
                }
                //dodanie kierunku
                else
                {
                    ret = db->addSpecialisation(ui->faculties_specialisation->currentText(), faculties_map[ui->faculties_all->currentIndex()]);
                }
            }
            //zmiana wydzia³u
            else
            {
                //edycja wydzia³u
                if (ui->faculties_all->currentIndex())
                {
                    ret = db->changeFacultyName(faculties_map[ui->faculties_all->currentIndex()], ui->faculties_all->currentText());
                }
                //dodanie wydzialu
                else
                {
                    ret = db->addFaculty(ui->faculties_all->currentText());
                }
            }
            fillFaculties(ui->faculties_all->currentIndex() == 0 ? ui->faculties_all->count() : ui->faculties_all->currentIndex() );
            break;
        case TAB_CLASS:
        ret = db->addClass(subjects_map[ui->classes_subject->currentIndex()], topics_map[ui->classes_topic->currentIndex()], groups_map[ui->classes_group->currentIndex()], ui->classes_date->date());
            break;
        case TAB_GROUP:
        //edycja grupy
            if(ui->groups_all->currentIndex() > 0)
            {
                ret = db->changeGroup(groups_map[ui->groups_all->currentIndex()], ui->groups_all->currentText(), ui->groups_day->currentText(), ui->groups_hour->time());
            }
            //dodanie grupy
            else
            {
                if (ui->groups_all->currentText() == "")
                {
                    break;
                }
                ret = db->addGroup(specialisations_map[ui->groups_specialisation->currentIndex()], ui->groups_all->currentText(), ui->groups_day->currentText(), ui->groups_hour->time());
            }
            fillGroups(ui->groups_faculty->currentIndex(), ui->groups_specialisation->currentIndex());
            break;
        case TAB_SUBJECT:
            //edycja starego przedmiotu
            if (ui->subjects_all->currentIndex() > 0)
            {
                ret = db->changeSubject(subjects_map[ui->subjects_all->currentIndex()], ui->subjects_year->text().toInt(), ui->subjects_all->currentText());
            }
            //dodawanie nowego
            else
            {
                if (ui->subjects_all->currentText() == "")
                {
                    break;
                }
                ret = db->addSubject(specialisations_map[ui->subjects_specialisation->currentIndex()], ui->subjects_year->text().toInt(), ui->subjects_all->currentText());
            }
            fillSubjects(ui->subjects_faculty->currentIndex(), ui->subjects_specialisation->currentIndex());
        break;
        case TAB_TOPIC:
            //edycja
            if(ui->topics_all->currentIndex() > 0)
            {
                ret = db->changeTopic(topics_map[ui->topics_all->currentIndex()], ui->topics_all->currentText());
            }
            //dodawanie nowego
            else
            {
                qDebug() << "!" << ui->topics_all->currentText() << "!" << (ui->topics_all->currentText() == "");
                if (ui->topics_all->currentText() == "")
                {
                    break;
                }
                ret = db->addTopic(subjects_map[ui->topics_subject->currentIndex()], ui->topics_all->currentText());
            }
            fillTopics(ui->topics_faculty->currentIndex(), ui->topics_specialisation->currentIndex(), ui->topics_subject->currentIndex());
            break;
        default:
            break;
    }
    return ret;
}

void TabWidget::on_subjects_faculty_activated(int index)
{
    fillSpecialisations(ui->subjects_specialisation, faculties_map[index]);
}

void TabWidget::on_subjects_specialisation_activated(int index)
{
    fillSubjects(ui->subjects_all, specialisations_map[index]);
}

void TabWidget::on_subjects_faculty_currentIndexChanged(int index)
{
    on_subjects_faculty_activated(index);
}

void TabWidget::on_subjects_specialisation_currentIndexChanged(int index)
{
    if (index > 0) on_subjects_specialisation_activated(index);
}

void TabWidget::on_subjects_all_currentIndexChanged(int index)
{
    if (index > 0)
    {
        ui->subjects_year->setText(db->getSubjectYearAsString(subjects_map[index]));
    }
}

void TabWidget::on_topics_faculty_currentIndexChanged(int index)
{
    if (index > 0)
    {
        fillSpecialisations(ui->topics_specialisation, faculties_map[index]);
    }
}

void TabWidget::on_topics_specialisation_currentIndexChanged(int index)
{
    if (index > 0)
    {
        fillSubjects(ui->topics_subject, specialisations_map[index]);
    }
}

void TabWidget::on_topics_subject_currentIndexChanged(int index)
{
    if (index > 0)
    {
        fillTopics(ui->topics_all, subjects_map[index]);
    }
}

void TabWidget::on_groups_faculty_currentIndexChanged(int index)
{
    if (index > 0)
    {
        fillSpecialisations(ui->groups_specialisation, faculties_map[index]);
    }
}

void TabWidget::on_groups_specialisation_currentIndexChanged(int index)
{
    if (index > 0)
    {
        fillGroups(ui->groups_all, specialisations_map[index]);
    }
}

void TabWidget::on_groups_all_currentIndexChanged(int index)
{
    if (index > 0)
    {
        ui->groups_day->setCurrentIndex(db->getGroupDayAsInt(groups_map[index]));
        ui->groups_hour->setTime(db->getGroupTime(groups_map[index]));
        qDebug() << db->getGroupTime(groups_map[index]);
    }
}

void TabWidget::on_classes_faculty_currentIndexChanged(int index)
{
    if (index > 0)
    {
        fillSpecialisations(ui->classes_specialisations, index);
    }
}

void TabWidget::on_classes_specialisations_currentIndexChanged(int index)
{
    if (index > 0)
    {
        fillSubjects(ui->classes_subject, index);
        fillGroups(ui->classes_group, index);
    }
}

void TabWidget::on_classes_subject_currentIndexChanged(int index)
{
    if (index > 0)
    {
        fillTopics(ui->classes_topic, index);
    }
}
