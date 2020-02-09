#ifndef SERVER_H
#define SERVER_H
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <QPlainTextEdit>

#define PORT 6150
#define DATA_BUFSIZE 8192

typedef struct _SOCKET_INFORMATION {
   OVERLAPPED Overlapped;
   SOCKET Socket;
   CHAR Buffer[DATA_BUFSIZE];
   WSABUF DataBuf;
   DWORD BytesSEND;
   DWORD BytesRECV;
} SOCKET_INFORMATION, * LPSOCKET_INFORMATION;

class Server
{
private:

protected:
    WSADATA wsaData;
    SOCKET ListenSocket;
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

public:
    Server() {};
    virtual ~Server() {
    };
    virtual bool start() = 0;

};

#endif // SERVER_H
