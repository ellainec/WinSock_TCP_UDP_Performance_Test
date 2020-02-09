#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "server.h"
#include <QDebug>

class TCPServer : public Server
{
private:
    bool createSocket() override;
    bool start() override;
    bool receiveFunc(LPSOCKET_INFORMATION SI, DWORD* RecvBytes, DWORD* Flags) override;
public:
    TCPServer(){};
};

#endif // TCPSERVER_H
