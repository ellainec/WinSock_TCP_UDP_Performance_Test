/*--------------------------------------------------------------------------------
-- SOURCE FILE:       mainwindow.cpp
--
-- PROGRAM:           application GUI
--
-- FUNCTIONS:         char* getUserInput();
                      void printNewLine(QString);
                      void getIpAddress(std::string hostname);
                      void getHostName(std::string ipaddress);
                      void getPortNumber(std::string nameAndProtocol);
                      void getServiceName(std::string portAndProtocol);
--
-- DATE:              January 18, 2020
--
-- DESIGNER:          Ellaine Chan
--
-- PROGRAMMER:        Ellaine Chan
--
-- NOTES:
-- GUI, calls winsock class and displays results on screen
-------------------------------------------------------------------------------- */
#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    HANDLE ThreadHandle;
    DWORD ThreadId;
    if ((ThreadHandle = CreateThread(NULL, 0, ServerThread, NULL, 0, &ThreadId)) == NULL)
    {
       printf("CreateThread failed with error %d\n", GetLastError());
       return;
    }
}

DWORD WINAPI MainWindow::ServerThread(LPVOID lpParameter) {
    Server* server = new TCPServer();
    server->start();
    return true;
}

void MainWindow::print(std::string text) {
    ui->status_text_edit->insertPlainText(QString::fromStdString(text));
}

MainWindow::~MainWindow()
{
    delete ui;
}





/*------------------------------------------------------------------------------------------------------------------
-- DATE: January 20, 2020
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ellaine Chan
--
-- PROGRAMMER: Ellaine Chan
--
-- INTERFACE: void on_actionHelp_triggered()
--
-- RETURNS: void
--
-- NOTES:
-- This method is triggered by user selecting the help menu option. It shows a message box with a brief summary of this
-- application.
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::on_actionHelp_triggered()
{
    QMessageBox::about(this, tr("About This Application"),
                       tr("This simple application demonstrates the use of the basic Winsock 2 API database lookup calls.\n "
                          "Select one of the menu options to look up a host name, ip address, port number or service name.\n "
                          "An attempt will be made to resolve the selected value given user input, and the result will be displayed. \n"
                          "Click on the clear button to clear the result box of all text."));
}

void MainWindow::on_action_Send_As_Client_triggered()
{
    ui->serverBox->setVisible(false);
    ui->clientBox->setVisible(true);
}

void MainWindow::on_action_Receive_As_Server_triggered()
{
    ui->serverBox->setVisible(true);
    ui->clientBox->setVisible(false);
    //ui->serverBox->setVisible(true);
}
