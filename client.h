#ifndef CLIENT_H
#define CLIENT_H

#define PORT 5150
#define DATA_BUFSIZE 1024
#include "Definitions.h"
#include <QDebug>

class Client
{
private:
    protocol client_protocol;
public:
    Client(protocol tcp_or_udp) : client_protocol(tcp_or_udp) {};
    ~Client(){};
    void start();
    WSADATA wsaData;

    static void CALLBACK ClientWorkerRoutine(DWORD Error, DWORD BytesTransferred,
       LPWSAOVERLAPPED Overlapped, DWORD InFlags);

    static DWORD WINAPI ClientWorkerThread(LPVOID lpParameter);
};

#endif // CLIENT_H
