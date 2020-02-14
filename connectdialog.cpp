#include "connectdialog.h"
#include "ui_connectdialog.h"

ConnectDialog::ConnectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectDialog)
{
    ui->setupUi(this);
}


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
