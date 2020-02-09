#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QMessageBox>
#include "ui_mainwindow.h"
#include "tcpserver.h"

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

private slots:

    void on_actionHelp_triggered();

    void on_action_Send_As_Client_triggered();

    void on_action_Receive_As_Server_triggered();

private:
    Ui::MainWindow *ui;
    QDialog *dialogue;
    static DWORD WINAPI ServerThread(LPVOID lpParameter);
};
#endif // MAINWINDOW_H
