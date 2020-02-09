#include "server.h"
#include <QDebug>


bool Server::wsaStartup() {
    qDebug() << "starting...";
    if ((Ret = WSAStartup(0x0202,&wsaData)) != 0)
      {
        qDebug() << "WSAStartup failed" << Ret;
         printf("WSAStartup failed with error %d\n", Ret);
         WSACleanup();
         return false;
      }
    return true;
}

bool Server::bindSocket() {
    InternetAddr.sin_family = AF_INET;
    InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    InternetAddr.sin_port = htons(PORT);

    if (bind(ListenSocket, (PSOCKADDR) &InternetAddr,
       sizeof(InternetAddr)) == SOCKET_ERROR)
    {
       qDebug() << "bind failed, " << WSAGetLastError();
       printf("bind() failed with error %d\n", WSAGetLastError());
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

