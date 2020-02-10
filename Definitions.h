#ifndef DEFINITIONS_H
#define DEFINITIONS_H
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include "filemanager.h"
#define PORT 5150
#define DATA_BUFSIZE 1024

enum protocol {TCP, UDP};

typedef struct _SOCKET_INFORMATION {
   OVERLAPPED Overlapped;
   SOCKET Socket;
   CHAR Buffer[DATA_BUFSIZE+1];
   WSABUF DataBuf;
   DWORD BytesSEND;
   DWORD BytesRECV;
} SOCKET_INFORMATION, * LPSOCKET_INFORMATION;

typedef struct _ACCEPT_INFORMATION {
    WSAEVENT AcceptEvent;
    SOCKET AcceptSocket;
} ACCEPT_INFORMATION, * LPACCEPT_INFORMATION;

typedef struct _CLIENT_INFORMATION {
   OVERLAPPED Overlapped;
   SOCKET Socket;
   CHAR Buffer[DATA_BUFSIZE];
   WSABUF DataBuf;
   DWORD BytesToSend;
   DWORD BytesLeft;
} CLIENT_INFORMATION, * LPCLIENT_INFORMATION;

#endif // DEFINITIONS_H
