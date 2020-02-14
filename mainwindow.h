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
#include "connectdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void print(QString text);
    protocol prot;

private slots:

    void on_actionHelp_triggered();


private:
    Ui::MainWindow *ui;
    QDialog *dialogue;
    HANDLE ThreadHandle;
    DWORD ThreadId;
    static DWORD WINAPI ServerThread(LPVOID lpParameter);
    static DWORD WINAPI ClientThread(LPVOID lpParameter);
    QString filename;
    void start();
    ConnectDialog *connectDialog = nullptr;
    Server *server = nullptr;
    Client *client = nullptr;
    bool validateSettings();
    void stop();
};
#endif // MAINWINDOW_H
