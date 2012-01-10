#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>

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

private slots:
    void on_TabWidget_currentChanged(int index);

private:
    Ui::TabWidget *ui;

};

#endif // TABWIDGET_H
