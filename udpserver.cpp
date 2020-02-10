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
    delete[] optval;
    return true;
}

bool UDPServer::start() {
    if (setUp() == false) {
        qDebug() << "setup failed";
        return false;
    }

    // Create a worker thread to service completed I/O requests.

    if ((ThreadHandle = CreateThread(NULL, 0, UDPWorkerThread, &ListenSocket, 0, &ThreadId)) == NULL)
    {
        qDebug() << "thread creation failed";
       printf("CreateThread failed with error %d\n", GetLastError());
       return false;
    }

    WaitForSingleObject(ThreadHandle, INFINITE);
}

DWORD WINAPI UDPServer::UDPWorkerThread(LPVOID lpParameter)
   {
      DWORD Flags;
      LPACCEPT_INFORMATION AcceptInfo;
      LPSOCKET_INFORMATION SocketInfo;
      WSAEVENT EventArray[1];
      DWORD Index;
      DWORD RecvBytes;

      SOCKET* socket = (SOCKET*) lpParameter;
      SocketInfo = new SOCKET_INFORMATION;


         // Fill in the details of our accepted socket.
         ZeroMemory(&(SocketInfo->Overlapped), sizeof(WSAOVERLAPPED));
         SocketInfo->BytesSEND = 0;
         SocketInfo->BytesRECV = 0;
         SocketInfo->DataBuf.buf = SocketInfo->Buffer;
         SocketInfo->DataBuf.len = sizeof(SocketInfo->Buffer);
         Flags = 0;
         FileManager::clearFile();
         while(true) {
             if (WSARecvFrom(*socket, &(SocketInfo->DataBuf), 1, &RecvBytes, &Flags,
                 NULL, NULL, &(SocketInfo->Overlapped), UDPWorkerRoutine) == SOCKET_ERROR) {
                 if (WSAGetLastError() != WSA_IO_PENDING)
                 {
                     qDebug() << "WSARecv() failed with error" << WSAGetLastError();
                     return FALSE;
                 }
                 //check for WSAEDISCON || WSAECONNRESET
             }
             SleepEx(INFINITE, true);
         }
          printf("Socket %d connected\n", AcceptInfo->AcceptSocket);

      return TRUE;
   }

   void UDPServer::UDPWorkerRoutine(DWORD Error, DWORD BytesTransferred,
      LPWSAOVERLAPPED Overlapped, DWORD InFlags)
   {
      DWORD SendBytes, RecvBytes;
      DWORD Flags;

      // Reference the WSAOVERLAPPED structure as a SOCKET_INFORMATION structure
      LPSOCKET_INFORMATION SI = (LPSOCKET_INFORMATION) Overlapped;

      FileManager::printToFile(SI->DataBuf.buf);
      qDebug() << SI->DataBuf.buf;
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

}



