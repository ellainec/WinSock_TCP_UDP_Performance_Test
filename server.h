#ifndef SERVER_H
#define SERVER_H
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <QPlainTextEdit>

#define PORT 6150
#define DATA_BUFSIZE 8192

class Server;

typedef struct _SOCKET_INFORMATION {
   OVERLAPPED Overlapped;
   SOCKET Socket;
   CHAR Buffer[DATA_BUFSIZE];
   WSABUF DataBuf;
   DWORD BytesSEND;
   DWORD BytesRECV;
} SOCKET_INFORMATION, * LPSOCKET_INFORMATION;

typedef struct _ACCEPT_INFORMATION {
    Server* server;
    WSAEVENT AcceptEvent;
    SOCKET AcceptSocket;
} ACCEPT_INFORMATION, * LPACCEPT_INFORMATION;

enum protocol {TCP, UDP};
class Server
{
private:

protected:
    static void CALLBACK WorkerRoutine(DWORD Error, DWORD BytesTransferred,
       LPWSAOVERLAPPED Overlapped, DWORD InFlags);
    static DWORD WINAPI WorkerThread(LPVOID lpParameter);
    WSADATA wsaData;
    SOCKET ListenSocket;
    LPACCEPT_INFORMATION AI;
    SOCKADDR_IN InternetAddr;
    INT Ret;
    HANDLE ThreadHandle;
    DWORD ThreadId;
    QPlainTextEdit *editor;
    virtual bool createSocket() = 0;
    bool wsaStartup();
    bool bindSocket();
    bool createThread();
    bool setUp();
    bool allocAI();
    virtual bool receiveFunc(LPSOCKET_INFORMATION SI, DWORD* RecvBytes, DWORD* Flags) = 0;

public:
    Server() {};
    virtual bool start() = 0;

};

#endif // SERVER_H
