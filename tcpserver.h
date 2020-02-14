#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "server.h"
#include "Definitions.h"
#include <QDebug>


class TCPServer : public Server
{
        Q_OBJECT
private:
    bool createSocket() override;
    bool start() override;
    LPACCEPT_INFORMATION AcceptInfo;
public:
    TCPServer(CONNECT_INFORMATION *info) : Server(info){};
    ~TCPServer() override {};
    static void CALLBACK TCPWorkerRoutine(DWORD Error, DWORD BytesTransferred,
       LPWSAOVERLAPPED Overlapped, DWORD InFlags);
    static DWORD WINAPI TCPWorkerThread(LPVOID lpParameter);
signals:
    void printToScreen(QString text);
};
#endif // TCPSERVER_H
