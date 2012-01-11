#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>
#include <QComboBox>

#include <database.h>
#include <vector>

#define TAB_FACULTY 0
#define TAB_SUBJECT 1
#define TAB_TOPIC 2
#define TAB_GROUP 3
#define TAB_CLASS 4


namespace Ui {
    class TabWidget;
}

class TabWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit TabWidget(QWidget *parent = 0);
    ~TabWidget();

    void fillFaculties(int index = 0);
    void fillSubjects(int indexf = 0, int indexs = 0);
    void fillTopics(int indexf = 0, int indexs = 0, int indexsub = 0);
    void fillGroups(int indexf = 0, int indexs = 0);
    void fillClasses();
    void setDatabase(Database *d);
    bool save();

private slots:
    void on_faculties_all_currentIndexChanged(int index);

    void on_faculties_all_activated(int index);

    void on_subjects_faculty_activated(int index);

    void on_subjects_specialisation_activated(int index);

    void on_subjects_faculty_currentIndexChanged(int index);

    void on_subjects_specialisation_currentIndexChanged(int index);

    void on_subjects_all_currentIndexChanged(int index);

    void on_topics_faculty_currentIndexChanged(int index);

    void on_topics_specialisation_currentIndexChanged(int index);

    void on_topics_subject_currentIndexChanged(int index);

    void on_groups_faculty_currentIndexChanged(int index);

    void on_groups_specialisation_currentIndexChanged(int index);

    void on_groups_all_currentIndexChanged(int index);

    void on_classes_faculty_currentIndexChanged(int index);

    void on_classes_specialisations_currentIndexChanged(int index);

    void on_classes_subject_currentIndexChanged(int index);

private:
    Ui::TabWidget *ui;
    void fillFaculties(QComboBox *to, bool empty_space = true);
    void fillSpecialisations(QComboBox *to, int faculty_id, bool empty_space = true);
    void fillSubjects(QComboBox *to, int specialisation_id, bool empty_space = true);
    void fillTopics(QComboBox *to, int subject_id, bool empty_space = true);
    void fillGroups(QComboBox *to, int subject_id, bool empty_space = true);
    void fillClasses(QComboBox *to, bool empty_space = true);
    void resetComboBox(QComboBox *what, bool empty_space);

    Database *db;
    std::vector<int> faculties_map;
    std::vector<int> specialisations_map;
    std::vector<int> subjects_map;
    std::vector<int> topics_map;
    std::vector<int> groups_map;
};

#endif // TABWIDGET_H
