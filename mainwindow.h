#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QMessageBox>
#include "ui_mainwindow.h"
#include "tcpserver.h"
#include "udpserver.h"
#include "client.h"
#include <qfiledialog.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void print(std::string text);
    protocol prot;

private slots:

    void on_actionHelp_triggered();

    void on_action_Send_As_Client_triggered();

    void on_action_Receive_As_Server_triggered();

    void on_serverStartBtn_clicked();

    void on_clientStartBtn_clicked();

    void on_tcpRadioBtn_clicked() {
        prot = protocol::TCP;
    }

    void on_udpRadioBtn_clicked(){
        prot = protocol::UDP;
    }

    void on_selectFileBtn_clicked(){
        filename = QFileDialog::getOpenFileName(this, tr("Open File to send"));
        qDebug() << filename << " selected";
    }

private:
    Ui::MainWindow *ui;
    QDialog *dialogue;
    HANDLE ThreadHandle;
    DWORD ThreadId;
    static DWORD WINAPI ServerThread(LPVOID lpParameter);
    static DWORD WINAPI ClientThread(LPVOID lpParameter);
    QString filename;
};
#endif // MAINWINDOW_H
