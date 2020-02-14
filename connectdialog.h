#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>
#include "Definitions.h"
#include <QDebug>

namespace Ui {
class ConnectDialog;
}

class ConnectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectDialog(QWidget *parent = nullptr);
    CONNECT_INFORMATION info ;
    ~ConnectDialog() { delete ui; }

private slots:

    void on_buttonBox_accepted();

private:
    Ui::ConnectDialog *ui;

};

#endif // CONNECTDIALOG_H
