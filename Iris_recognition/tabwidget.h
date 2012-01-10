#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>
#include <QComboBox>

#include <database.h>

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

    void fillFaculties();
    void fillSubjects();
    void fillTopics();
    void fillGroups();
    void fillClasses();
    void setDatabase(Database *d);

private slots:
    void on_TabWidget_currentChanged(int index);

private:
    Ui::TabWidget *ui;
    void fillFaculties(QComboBox *to);
    void fillSubjects(QComboBox *to);
    void fillTopics(QComboBox *to);
    void fillGroups(QComboBox *to);
    void fillClasses(QComboBox *to);

    Database *db;
};

#endif // TABWIDGET_H
