/*--------------------------------------------------------------------------------
-- SOURCE FILE:       mainwindow.cpp
--
-- PROGRAM:           application GUI
--
-- FUNCTIONS:         MainWindow(QWidget *parent);
                      DWORD WINAPI MainWindow::ServerThread(LPVOID lpParameter);
                      DWORD WINAPI MainWindow::ClientThread(LPVOID lpParameter);
                      void start();
                      bool validateSettings();
                      void getServiceName(std::string portAndProtocol);
                      void print(QString text);
--
-- DATE:              January 18, 2020
--
--
-- DESIGNER:          Ellaine Chan
--
-- PROGRAMMER:        Ellaine Chan
--
-- NOTES:
-- GUI, calls winsock class and displays results on screen
-------------------------------------------------------------------------------- */
#include "mainwindow.h"

/*--------------------------------------------------------------------------------
-- FUNCTION: MainWindow(QWidget *parent)
--
-- DATE:    February 16, 2020
--
-- DESIGNER: Ellaine Chan
--
-- PROGRAMMER: Ellaine Chan
--
-- PARAMETERS: void
--
-- RETURNS: void
--
-- NOTES:
-- Constructor for a MainWindow instance. It calls setupUi which is a
-- QT implemented method for QWidgets objects. The menu items for Connect and Settings
-- are hooked up to call methods when triggered.
-------------------------------------------------------------------------------- */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , connectDialog(new ConnectDialog)
{
    ui->setupUi(this);
    connect(ui->action_Connect_Settings, &QAction::triggered, connectDialog, &ConnectDialog::show);
    connect(ui->action_Start, &QAction::triggered, this, &MainWindow::start);
}

/*--------------------------------------------------------------------------------
-- FUNCTION: DWORD WINAPI ServerThread(LPVOID lpParameter)
--
-- DATE:    February 16, 2020
--
-- DESIGNER: Ellaine Chan
--
-- PROGRAMMER: Ellaine Chan
--
-- PARAMETERS: void
--
-- RETURNS: true after server's start method returns
--
-- NOTES:
-- Function run by CreateThread to call start on the server.
-------------------------------------------------------------------------------- */
DWORD WINAPI MainWindow::ServerThread(LPVOID lpParameter) {
    Server* server = (Server*) lpParameter;
    server->start();
    return true;
}

/*--------------------------------------------------------------------------------
-- FUNCTION: DWORD WINAPI ClientThread(LPVOID lpParameter)
--
-- DATE:    February 16, 2020
--
-- DESIGNER: Ellaine Chan
--
-- PROGRAMMER: Ellaine Chan
--
-- PARAMETERS: void
--
-- RETURNS: true after client's start method returns.
--
-- NOTES:
-- Function run by CreateThread to call start on the client.
-------------------------------------------------------------------------------- */
DWORD WINAPI MainWindow::ClientThread(LPVOID lpParameter) {
    Client *client = (Client *) lpParameter;
    client->start();
    return true;
}

/*--------------------------------------------------------------------------------
-- FUNCTION: void start(void)
--
-- DATE:    February 16, 2020
--
-- DESIGNER: Ellaine Chan
--
-- PROGRAMMER: Ellaine Chan
--
-- PARAMETERS: void
--
-- RETURNS: void
--
-- NOTES:
-- Validates that all required settings are input by the user. Depending on
-- the user selection, will create a client or server instance, and pass in
-- the user settings. If the user selects server, it will create either a TCPServer
-- or UDPServer. The MainWindow's print function is also connected as a slot to a
-- client and server signal to allow them to print out to the main window.
-- A new thread is created to start the client/server as to now freeze up the UI.
-------------------------------------------------------------------------------- */
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
            print("CreateThread failed with error " + QString::number(GetLastError()));
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

/*--------------------------------------------------------------------------------
-- FUNCTION: bool validateSettings() {
--
-- DATE:    February 16, 2020
--
-- DESIGNER: Ellaine Chan
--
-- PROGRAMMER: Ellaine Chan
--
-- PARAMETERS: void
--
-- RETURNS: true if all user input is filled in, else false
--
-- NOTES:
-- Makes sure that all information required to start receiving/sending data is
-- input by the user. Role, port number, protocol and packet size are always required.
-- IPAddress, and number of times to send are used only by the client.
-------------------------------------------------------------------------------- */
bool MainWindow::validateSettings() {

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

/*--------------------------------------------------------------------------------
-- FUNCTION: void print(QString text)
--
-- DATE:    February 16, 2020
--
-- DESIGNER: Ellaine Chan
--
-- PROGRAMMER: Ellaine Chan
--
-- PARAMETERS: QString - string to print out
--
-- RETURNS: void
--
-- NOTES:
-- Prints out passed in QString to main window for user to see.
-------------------------------------------------------------------------------- */
void MainWindow::print(QString text) {
    ui->main_text->insertPlainText(text);
    ui->main_text->insertPlainText("\n");
}

/*--------------------------------------------------------------------------------
-- FUNCTION: ~MainWindow()
--
-- DATE:    February 16, 2020
--
-- DESIGNER: Ellaine Chan
--
-- PROGRAMMER: Ellaine Chan
--
-- PARAMETERS: void
--
-- RETURNS: void
--
-- NOTES:
-- Deconstructor for MainWindow class. Deletes the dynamically allocated server, client
-- and ui.
-------------------------------------------------------------------------------- */
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
                       tr("This application can be a client or server to transfer a desired amount of data. To get started, "
                          "click on Settings and fill in the settings such as ip address, port number and packet size. To connect"
                          " and start sending/receiving, click connect"));
}
