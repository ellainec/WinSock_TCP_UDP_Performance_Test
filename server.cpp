#include "server.h"
#include <QDebug>


bool Server::wsaStartup() {
    if ((Ret = WSAStartup(0x0202,&wsaData)) != 0) {
        emit printToScreen("WSAStartup failed: " + QString::number(Ret));
        WSACleanup();
        return false;
    }
    return true;
}

bool Server::bindSocket() {
    InternetAddr.sin_family = AF_INET;
    InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    InternetAddr.sin_port = htons(info->port);

    if (bind(ListenSocket, (PSOCKADDR) &InternetAddr,
       sizeof(InternetAddr)) == SOCKET_ERROR) {
        emit printToScreen("bind() failed with error " + QString::number(WSAGetLastError()));
        return false;
    }
    return true;
}

bool Server::setUp() {
    bool startup = wsaStartup();
    bool socket = createSocket();
    bool bound = bindSocket();
    return startup && socket && bound;
}
