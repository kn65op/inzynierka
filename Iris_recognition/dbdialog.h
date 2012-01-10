#ifndef DBDIALOG_H
#define DBDIALOG_H

#include <QDialog>

namespace Ui {
    class DbDialog;
}

class DbDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DbDialog(QWidget *parent = 0);
    ~DbDialog();

private slots:
    void on_tabWidget_currentChanged(int index);

private:
    Ui::DbDialog *ui;
};

#endif // DBDIALOG_H
