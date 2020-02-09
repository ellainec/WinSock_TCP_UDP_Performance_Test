#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "server.h"
#include <QDebug>

typedef struct _ACCEPT_INFORMATION {
    WSAEVENT AcceptEvent;
    SOCKET AcceptSocket;
} ACCEPT_INFORMATION, * LPACCEPT_INFORMATION;

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
