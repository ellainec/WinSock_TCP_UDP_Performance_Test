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
    , connectDialog(new ConnectDialog)
{
    ui->setupUi(this);
    connect(ui->action_Connect_Settings, &QAction::triggered, connectDialog, &ConnectDialog::show);
    connect(ui->action_Start, &QAction::triggered, this, &MainWindow::start);
}

void MainWindow::stop() {
    print("Stopped.");
}

DWORD WINAPI MainWindow::ServerThread(LPVOID lpParameter) {
    Server* server = (Server*) lpParameter;
    server->start();
    qDebug() << "server finished";
    return true;
}

DWORD WINAPI MainWindow::ClientThread(LPVOID lpParameter) {
    Client *client = (Client *) lpParameter;
    client->start();
    qDebug() << "client finished";
    return true;
}

void MainWindow::start() {
    if (!validateSettings()) {
        print("Missing some Connection Settings");
        return;
    }

    if (connectDialog->info.roleSelected == role::CLIENT) {
        if (client == nullptr) {
            client = new Client(&connectDialog->info);
            connect(client, &Client::printToScreen, this, &MainWindow::print);
        }
        if ((ThreadHandle = CreateThread(NULL, 0, ClientThread, (LPVOID)client, 0, &ThreadId)) == NULL)
        {
            printf("CreateThread failed with error %d\n", GetLastError());
            return;
        }
    } else {
        if(server == nullptr) {
            if(connectDialog->info.protocolSelected == protocol::TCP) {
                server = new TCPServer(&connectDialog->info);
            } else {
                server = new UDPServer(&connectDialog->info);
            }
            connect(server, &Server::printToScreen, this, &MainWindow::print);
            connect(ui->action_On_Stop, &QAction::triggered, server, &Server::stopServer);
        }

        if ((ThreadHandle = CreateThread(NULL, 0, ServerThread, (LPVOID)server, 0, &ThreadId)) == NULL)
        {
            printf("CreateThread failed with error %d\n", GetLastError());
            return;
        }
    }
}

bool MainWindow::validateSettings() {
    //todo: remove later
//    connectDialog->info.port = 5150;
//    sprintf(connectDialog->info.ipAddress, "127.0.0.1");
//    connectDialog->info.packetSize = 1024;
//    connectDialog->info.timesToSend = 10;
//    connectDialog->info.protocolSelected = protocol::TCP;

    if (connectDialog->info.roleSelected != role::CLIENT && connectDialog->info.roleSelected != role::SERVER) {
        return false;
    }
    if (connectDialog->info.protocolSelected != protocol::TCP && connectDialog->info.protocolSelected != protocol::UDP) {
        return false;
    }

    if (connectDialog->info.port == 0) {
       return false;
    }
    if (connectDialog->info.packetSize == 0) {
       return false;
    }
    if (connectDialog->info.roleSelected == role::CLIENT && connectDialog->info.timesToSend <=0) {
       return false;
    }
    return true;

}
void MainWindow::print(QString text) {
    ui->main_text->insertPlainText(text);
    ui->main_text->insertPlainText("\n");
}

MainWindow::~MainWindow()
{
    delete server;
    delete client;
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
