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
};

#endif // UDPSERVER_H
