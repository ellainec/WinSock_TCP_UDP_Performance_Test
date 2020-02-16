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
    const char *optval = new char[100];
    if (setsockopt(ListenSocket,SOL_SOCKET, SO_REUSEADDR, optval, sizeof(optval)) < 0) {
        qDebug() << "set sock opt failed" ;
    };
    return true;
}

bool TCPServer::start() {
    WSAEVENT EventArray[MAXIMUM_WAIT_OBJECTS];
    DWORD Flags, RecvBytes, Index;
    char buffer[MAX_LEN];
    LPSOCKET_INFORMATION SocketInfo;

    if (setUp() == false) {
        return false;
    }

    AcceptInfo = new ACCEPT_INFORMATION;
    AcceptInfo->stopped = false;
    if(listen(ListenSocket, 5)){
        // emit tcpPrint("listen() failed with error %d\n" + WSAGetLastError());
         return false;
    }

    if ((AcceptInfo->AcceptEvent = WSACreateEvent()) == WSA_INVALID_EVENT)
    {
        // emit tcpPrint("WSACreateEvent() failed with error %d\n" + WSAGetLastError());
       return false;
    }
    emit printToScreen("Connected and ready to accept incoming connection...");
    AcceptInfo->AcceptSocket = accept(ListenSocket, NULL, NULL);
    //start timer
    Flags = 0;

    //ZeroMemory(&Overlapped, sizeof(WSAOVERLAPPED));

    SocketInfo = new SOCKET_INFORMATION;
    SocketInfo->Socket = AcceptInfo->AcceptSocket;
    ZeroMemory(&(SocketInfo->Overlapped), sizeof(WSAOVERLAPPED));
    SocketInfo->BytesSEND = 0;
    SocketInfo->BytesRECV = 0;
    SocketInfo->DataBuf.len = info->packetSize;
    SocketInfo->DataBuf.buf = SocketInfo->Buffer;
    memset(SocketInfo->Buffer, 0, sizeof(SocketInfo->Buffer));

    FileManager::clearFile();
    GetSystemTime(&stStartTime);
    emit printToScreen("Reading...");
    if (WSARecv(AcceptInfo->AcceptSocket, &SocketInfo->DataBuf, 1, &RecvBytes, &Flags,
        &SocketInfo->Overlapped, TCPWorkerRoutine) == SOCKET_ERROR) {
        if (WSAGetLastError() != WSA_IO_PENDING) {
            printf("WSARecv() failed with error %d\n", WSAGetLastError());
            return false;
        }
          //check for WSAEDISCON || WSAECONNRESET
    }

    if ((EventArray[1] = WSACreateEvent()) == WSA_INVALID_EVENT) {
            qDebug()<< "incoming event creation failed: " << WSAGetLastError();
           return false;
    }
    bool keepGoing = true;
    Index = 0;
    while(!serverStopped) {
        SleepEx(1000, TRUE);
    }
    GetSystemTime(&stEndTime);
    long totalTime = calculateTimeDelay(stStartTime, stEndTime);
    emit printToScreen("Total of " + QString::number(SocketInfo->BytesRECV) + "B received. Total time: " + QString::number(totalTime) + " ms");
    closesocket(AcceptInfo->AcceptSocket);
    closesocket(ListenSocket);
    delete AcceptInfo;
    delete SocketInfo;
}

   void TCPServer::TCPWorkerRoutine(DWORD Error, DWORD BytesTransferred,
      LPWSAOVERLAPPED Overlapped, DWORD InFlags)
   {
      DWORD SendBytes, RecvBytes;
      DWORD Flags;
      Flags = 0;

      // Reference the WSAOVERLAPPED structure as a SOCKET_INFORMATION structure
      LPSOCKET_INFORMATION SI = (LPSOCKET_INFORMATION) Overlapped;
      SI->BytesRECV += BytesTransferred;
      if (BytesTransferred > 0) {
          FileManager::printToFile(SI->Buffer);
          qDebug() << "Bytes received: " << SI->BytesRECV;
      }

      if (Error != 0)
      {
        printf("I/O operation failed with error %d\n", Error);
      }

      if (BytesTransferred == 0)
      {
         printf("Closing socket %d\n", SI->Socket);
      }

      if (Error != 0 || BytesTransferred == 0)
      {
         qDebug() << "error: " << Error;
         return;
      }
      memset(SI->Buffer, 0, sizeof(SI->Buffer));
      if (WSARecv(SI->Socket, &SI->DataBuf, 1, &RecvBytes, &Flags,
         &SI->Overlapped, TCPWorkerRoutine) == SOCKET_ERROR) {
         if (WSAGetLastError() != WSA_IO_PENDING)
         {
            printf("WSARecv() failed with error %d\n", WSAGetLastError());
            return;
         }
         //check for WSAEDISCON || WSAECONNRESET
      }
}



