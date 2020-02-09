#include "tcpserver.h"

//override for protocol
bool TCPServer::createSocket() {
    if ((ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0,
       WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
    {
       qDebug() << WSAGetLastError();
       printf("Failed to get a socket %d\n", WSAGetLastError());
       return false;
    }
    return true;
}

bool TCPServer::receiveFunc(LPSOCKET_INFORMATION SI, DWORD* RecvBytes, DWORD* Flags) {
    if (WSARecv(SI->Socket, &(SI->DataBuf), 1, RecvBytes, Flags,
       &(SI->Overlapped), WorkerRoutine) == SOCKET_ERROR) {
        return false;
    }
    return true;
}
bool TCPServer::start() {
    if (setUp() == false) {
        qDebug() << "setup failed";
        return false;
    }

    AI->server = this;
    if(listen(ListenSocket, 5)){
         qDebug() << "can't listen";
         printf("listen() failed with error %d\n", WSAGetLastError());
         return false;
    }

    if ((AI->AcceptEvent = WSACreateEvent()) == WSA_INVALID_EVENT)
    {
        qDebug()<< "acceptEvent creation failed";
       printf("WSACreateEvent() failed with error %d\n", WSAGetLastError());
       return false;
    }


    // Create a worker thread to service completed I/O requests.

    if ((ThreadHandle = CreateThread(NULL, 0, WorkerThread, (LPVOID) AI, 0, &ThreadId)) == NULL)
    {
        qDebug() << "thread creation failed";
       printf("CreateThread failed with error %d\n", GetLastError());
       return false;
    }

    while(TRUE)
    {
       AI->AcceptSocket = accept(ListenSocket, NULL, NULL);

       if (WSASetEvent(AI->AcceptEvent) == FALSE)
       {
           qDebug() << "set signal failed";
          printf("WSASetEvent failed with error %d\n", WSAGetLastError());
          return false;
       }
    }
}
