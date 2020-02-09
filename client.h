#ifndef CLIENT_H
#define CLIENT_H

#define PORT 5150
#define DATA_BUFSIZE 8192
#include "Definitions.h"
#include <QDebug>

class Client
{
public:
    Client();
    ~Client(){};
    void start();
    WSADATA wsaData;

    static void CALLBACK ClientWorkerRoutine(DWORD Error, DWORD BytesTransferred,
       LPWSAOVERLAPPED Overlapped, DWORD InFlags);

    static DWORD WINAPI ClientWorkerThread(LPVOID lpParameter);
};

#endif // CLIENT_H
