#ifndef UDPSERVER_H
#define UDPSERVER_H

#include "server.h"
#include <QDebug>

class UDPServer : public Server
{
public:
    UDPServer();
    bool start() override;
    bool createSocket() override;
    static void CALLBACK UDPWorkerRoutine(DWORD Error, DWORD BytesTransferred,
       LPWSAOVERLAPPED Overlapped, DWORD InFlags);
    static DWORD WINAPI UDPWorkerThread(LPVOID lpParameter);
};

#endif // UDPSERVER_H
