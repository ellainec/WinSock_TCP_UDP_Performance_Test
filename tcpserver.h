#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "server.h"
#include "Definitions.h"
#include <QDebug>


class TCPServer : public Server
{
        Q_OBJECT
private:

    LPACCEPT_INFORMATION AcceptInfo;
public:
    TCPServer(CONNECT_INFORMATION *info) : Server(info){};
    ~TCPServer() override {};
    static void CALLBACK TCPWorkerRoutine(DWORD Error, DWORD BytesTransferred,
       LPWSAOVERLAPPED Overlapped, DWORD InFlags);
    static DWORD WINAPI TCPWorkerThread(LPVOID lpParameter);
    bool createSocket() override;
    bool start() override;
signals:

    //void tcpPrint(QString text);
};
#endif // TCPSERVER_H
