#include "connectdialog.h"
#include "ui_connectdialog.h"
/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: ConnectDialog.cpp
--
-- PROGRAM: Connect Dialog Widget
--
-- FUNCTIONS:
-- ConnectDialog(QWidget *parent)
--void on_buttonBox_accepted(void)
--
-- DATE: February 16, 2020
--
-- DESIGNER: Ellaine Chan
--
-- PROGRAMMER: Ellaine Chan
--
-- NOTES:
-- The connect dialog is the widget that is used to get user settings for:
-- ipAddress, port, packetSize, times to send, role, and protocol.
----------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------
-- FUNCTION: ConnectDialog(QWidget *parent)
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
-- Constructor for a ConnectDialog instance. It calls setupUi which is a
-- QT implemented method for QWidgets objects.
-------------------------------------------------------------------------------- */
ConnectDialog::ConnectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectDialog)
{
    ui->setupUi(this);
}

/*--------------------------------------------------------------------------------
-- FUNCTION: on_buttonBox_accepted()
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
-- Saves the user input settings for ip address, port number, packet size, times to send,
-- role, protocol into the CONNECT_INFORMATION struct member info
-------------------------------------------------------------------------------- */
void ConnectDialog::on_buttonBox_accepted()
{
    sprintf(info.ipAddress, ui->ipAddress->text().toLocal8Bit());
    int port = ui->portNumber->text().toInt();
    int packetSize = ui->packetSize->text().toInt();
    int timesToSend = ui->timesToSend->text().toInt();
    role r = ui->clientSelected->isChecked() ? role::CLIENT : role::SERVER;
    protocol p = ui->tcpSelected->isChecked() ? protocol::TCP : protocol::UDP;
    info.roleSelected = r;
    info.protocolSelected = p;
    info.packetSize = packetSize;
    //info.ipAddress = ui->ipAddress->text().toLocal8Bit();
    info.timesToSend = timesToSend;
    info.port = port;
}
