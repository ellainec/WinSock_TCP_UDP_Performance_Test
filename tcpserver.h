#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "server.h"
#include "Definitions.h"
#include <QDebug>


class TCPServer : public Server
{
private:
    bool createSocket() override;
    bool start() override;
    LPACCEPT_INFORMATION AcceptInfo;
public:
    TCPServer(){};
    static void CALLBACK TCPWorkerRoutine(DWORD Error, DWORD BytesTransferred,
       LPWSAOVERLAPPED Overlapped, DWORD InFlags);
    static DWORD WINAPI TCPWorkerThread(LPVOID lpParameter);
};

#endif // TCPSERVER_H
