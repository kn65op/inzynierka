#ifndef CHECKDIALOG_H
#define CHECKDIALOG_H

#include <QDialog>

namespace Ui {
    class checkDialog;
}

class checkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit checkDialog(QWidget *parent = 0);
    ~checkDialog();

    void setImage(QString file);

private:
    Ui::checkDialog *ui;
};

#endif // CHECKDIALOG_H
