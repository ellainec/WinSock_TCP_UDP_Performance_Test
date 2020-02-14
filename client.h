#ifndef CLIENT_H
#define CLIENT_H

#include "Definitions.h"
#include <QDebug>
#include <fstream>
#include <iostream>
#include <QObject>

class Client : public QObject  {
    Q_OBJECT
private:
    CONNECT_INFORMATION* info;
public:
    Client(CONNECT_INFORMATION* info) : info(info) {};
    ~Client(){};
    void start();
    WSADATA wsaData;
    char *writeBuffer;

    static void CALLBACK ClientWorkerRoutine(DWORD Error, DWORD BytesTransferred,
       LPWSAOVERLAPPED Overlapped, DWORD InFlags);

    static DWORD WINAPI ClientWorkerThread(LPVOID lpParameter);

signals:
    void printToScreen(QString text);

};
#endif // CLIENT_H
