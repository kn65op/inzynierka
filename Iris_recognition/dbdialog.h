#ifndef DBDIALOG_H
#define DBDIALOG_H

#include <QDialog>
#include <database.h>

namespace Ui {
    class DbDialog;
}

class DbDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DbDialog(QWidget *parent = 0);
    ~DbDialog();

    void setDatabase(Database *d);

private slots:
    void on_tabWidget_currentChanged(int index);

private:
    Ui::DbDialog *ui;
};

#endif // DBDIALOG_H
