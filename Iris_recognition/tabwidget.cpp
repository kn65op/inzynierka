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

}

void TabWidget::fillClasses()
{

}

void TabWidget::fillGroups()
{

}

void TabWidget::fillSubjects()
{

}

void TabWidget::fillTopics()
{

}
