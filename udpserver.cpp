#include "udpserver.h"

//override for protocol
bool UDPServer::createSocket() {
    if ((ListenSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0,
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
    int recvBufSize = 60000*100;
    int err = setsockopt(ListenSocket, SOL_SOCKET, SO_RCVBUF,(const char*)&recvBufSize, sizeof(recvBufSize));
    delete[] optval;
    return true;
}

bool UDPServer::start() {
    if (setUp() == false) {
        qDebug() << "setup failed";
        return false;
    }
    DWORD Flags;
    LPACCEPT_INFORMATION AcceptInfo;
    LPSOCKET_INFORMATION SocketInfo;
    WSAEVENT EventArray[1];
    DWORD Index;
    DWORD RecvBytes;

    SocketInfo = new SOCKET_INFORMATION;
    // Fill in the details of our accepted socket.
    ZeroMemory(&(SocketInfo->Overlapped), sizeof(WSAOVERLAPPED));
    SocketInfo->BytesSEND = 0;
    SocketInfo->BytesRECV = 0;

    SocketInfo->DataBuf.buf = SocketInfo->Buffer;
    SocketInfo->DataBuf.len = info->packetSize;
    SocketInfo->Socket = ListenSocket;
    Flags = 0;
    FileManager::clearFile();
    if (WSARecvFrom(ListenSocket, &(SocketInfo->DataBuf), 1, &RecvBytes, &Flags,
        NULL, NULL, &(SocketInfo->Overlapped), UDPWorkerRoutine) == SOCKET_ERROR) {
        if (WSAGetLastError() != WSA_IO_PENDING)
        {
            qDebug() << "WSARecv() failed with error" << WSAGetLastError();
            return FALSE;
        }
        //check for WSAEDISCON || WSAECONNRESET
    }

    while(true) {
        SleepEx(INFINITE, true);
        //FileManager::printToFile(SocketInfo->Buffer);
    }
}

   void UDPServer::UDPWorkerRoutine(DWORD Error, DWORD BytesTransferred,
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
      }
      memset(SI->Buffer, 0, sizeof(SI->Buffer));

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
         closesocket(SI->Socket);
         GlobalFree(SI);
         return;
      }
      if (WSARecvFrom(SI->Socket, &(SI->DataBuf), 1, &RecvBytes, &Flags,
          NULL, NULL, &(SI->Overlapped), UDPWorkerRoutine) == SOCKET_ERROR) {
          if (WSAGetLastError() != WSA_IO_PENDING)
          {
              qDebug() << "WSARecv() failed with error" << WSAGetLastError();
              return;
          }
          //check for WSAEDISCON || WSAECONNRESET
      }

}



