#ifndef DEFINITIONS_H
#define DEFINITIONS_H
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include "filemanager.h"
#define PORT 5150
#define MAX_LEN 60100

enum protocol {TCP, UDP};
enum role {CLIENT, SERVER};

class Client;

typedef struct _SOCKET_INFORMATION {
   OVERLAPPED Overlapped;
   SOCKET Socket;
   CHAR Buffer[MAX_LEN];
   WSABUF DataBuf;
   DWORD BytesSEND;
   DWORD BytesRECV;
   DWORD Error;
   SYSTEMTIME firstPacketTime;
   SYSTEMTIME lastPacketTime;
} SOCKET_INFORMATION, * LPSOCKET_INFORMATION;

typedef struct _ACCEPT_INFORMATION {
    WSAEVENT AcceptEvent;
    SOCKET AcceptSocket;
    bool stopped;
} ACCEPT_INFORMATION, * LPACCEPT_INFORMATION;

typedef struct _CONNECT_INFORMATION {
    char ipAddress[50];
    int port;
    protocol protocolSelected;
    role roleSelected;
    int packetSize;
    int timesToSend;
} CONNECT_INFORMATION, * LPCONNECT_INFORMATION;

static long calculateTimeDelay (SYSTEMTIME t1, SYSTEMTIME t2)
{
    long d;

    d = (t2.wSecond - t1.wSecond) * 1000;
    d += (t2.wMilliseconds - t1.wMilliseconds);
    return(d);
}
#endif // DEFINITIONS_H
