#include "server.h"
#include <QDebug>


bool Server::wsaStartup() {
    qDebug() << "starting...";
    if ((Ret = WSAStartup(0x0202,&wsaData)) != 0)
      {
        qDebug() << "WSAStartup failed" << Ret;
         printf("WSAStartup failed with error %d\n", Ret);
         WSACleanup();
         return false;
      }
    return true;
}

bool Server::bindSocket() {
    InternetAddr.sin_family = AF_INET;
    InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    InternetAddr.sin_port = htons(PORT);

    if (bind(ListenSocket, (PSOCKADDR) &InternetAddr,
       sizeof(InternetAddr)) == SOCKET_ERROR)
    {
       qDebug() << "bind failed, " << WSAGetLastError();
       printf("bind() failed with error %d\n", WSAGetLastError());
       return false;
    }
    return true;
}

bool Server::allocAI() {
    if ((AI = (LPACCEPT_INFORMATION) GlobalAlloc(GPTR,
       sizeof(LPACCEPT_INFORMATION))) == NULL)
    {
        qDebug() << "globalalloc failed";
       printf("GlobalAlloc() failed with error %d\n", GetLastError());
       return false;
    }
    return true;
}

bool Server::setUp() {
    bool startup = wsaStartup();
    bool socket = createSocket();
    bool bound = bindSocket();
    bool AIAlloc = allocAI();
    return startup && socket && bound && AIAlloc;
}



DWORD WINAPI Server::WorkerThread(LPVOID lpParameter)
   {
      DWORD Flags;
      LPSOCKET_INFORMATION SocketInfo;
      WSAEVENT EventArray[1];
      DWORD Index;
      DWORD RecvBytes;

      LPACCEPT_INFORMATION AI = (LPACCEPT_INFORMATION) lpParameter;

      // Save the accept event in the event array.

      EventArray[0] = (WSAEVENT) AI->AcceptEvent;
      qDebug() << "inside worker thread!";
      while(TRUE)
      {
         // Wait for accept() to signal an event and also process WorkerRoutine() returns.

         while(TRUE)
         {
            Index = WSAWaitForMultipleEvents(1, EventArray, FALSE, WSA_INFINITE, TRUE);

            if (Index == WSA_WAIT_FAILED)
            {
               printf("WSAWaitForMultipleEvents failed with error %d\n", WSAGetLastError());
               return FALSE;
            }

            if (Index != WAIT_IO_COMPLETION)
            {
               // An accept() call event is ready - break the wait loop
               break;
            }
         }

         WSAResetEvent(EventArray[Index - WSA_WAIT_EVENT_0]);

         // Create a socket information structure to associate with the accepted socket.

         if ((SocketInfo = (LPSOCKET_INFORMATION) GlobalAlloc(GPTR,
            sizeof(SOCKET_INFORMATION))) == NULL)
         {
            printf("GlobalAlloc() failed with error %d\n", GetLastError());
            return FALSE;
         }

         // Fill in the details of our accepted socket.

         SocketInfo->Socket = AI->AcceptSocket;
         ZeroMemory(&(SocketInfo->Overlapped), sizeof(WSAOVERLAPPED));
         SocketInfo->BytesSEND = 0;
         SocketInfo->BytesRECV = 0;
         SocketInfo->DataBuf.len = DATA_BUFSIZE;
         SocketInfo->DataBuf.buf = SocketInfo->Buffer;

         Flags = 0;
         Server* thisServer = (Server*) AI->server;
         qDebug() << "right after cast";
         if (WSARecv(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &RecvBytes, &Flags,
            &(SocketInfo->Overlapped), WorkerRoutine) == SOCKET_ERROR)
         {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
               printf("WSARecv() failed with error %d\n", WSAGetLastError());
               return FALSE;
            }
         }

         printf("Socket %d connected\n", AI->AcceptSocket);
      }

      return TRUE;
   }

   void Server::WorkerRoutine(DWORD Error, DWORD BytesTransferred,
      LPWSAOVERLAPPED Overlapped, DWORD InFlags)
   {
      DWORD SendBytes, RecvBytes;
      DWORD Flags;

      // Reference the WSAOVERLAPPED structure as a SOCKET_INFORMATION structure
      LPSOCKET_INFORMATION SI = (LPSOCKET_INFORMATION) Overlapped;

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

      if (WSARecv(SI->Socket, &(SI->DataBuf), 1, &RecvBytes, &Flags,
         &(SI->Overlapped), WorkerRoutine) == SOCKET_ERROR)
      {
         if (WSAGetLastError() != WSA_IO_PENDING )
         {
            printf("WSARecv() failed with error %d\n", WSAGetLastError());
            return;
         }
      }

      // Check to see if the BytesRECV field equals zero. If this is so, then
      // this means a WSARecv call just completed so update the BytesRECV field
      // with the BytesTransferred value from the completed WSARecv() call.

//      if (SI->BytesRECV == 0)
//      {
//         SI->BytesRECV = BytesTransferred;
//         SI->BytesSEND = 0;
//      }
//      else
//      {
//         SI->BytesSEND += BytesTransferred;
//      }

//      if (SI->BytesRECV > SI->BytesSEND)
//      {

//         // Post another WSASend() request.
//         // Since WSASend() is not gauranteed to send all of the bytes requested,
//         // continue posting WSASend() calls until all received bytes are sent.

//         ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));

//         SI->DataBuf.buf = SI->Buffer + SI->BytesSEND;
//         SI->DataBuf.len = SI->BytesRECV - SI->BytesSEND;

//         if (WSASend(SI->Socket, &(SI->DataBuf), 1, &SendBytes, 0,
//            &(SI->Overlapped), WorkerRoutine) == SOCKET_ERROR)
//         {
//            if (WSAGetLastError() != WSA_IO_PENDING)
//            {
//               printf("WSASend() failed with error %d\n", WSAGetLastError());
//               return;
//            }
//         }
//      }
//      else
//      {
//         SI->BytesRECV = 0;

//         // Now that there are no more bytes to send post another WSARecv() request.

//         Flags = 0;
//         ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));

//         SI->DataBuf.len = DATA_BUFSIZE;
//         SI->DataBuf.buf = SI->Buffer;

//         if (WSARecv(SI->Socket, &(SI->DataBuf), 1, &RecvBytes, &Flags,
//            &(SI->Overlapped), WorkerRoutine) == SOCKET_ERROR)
//         {
//            if (WSAGetLastError() != WSA_IO_PENDING )
//            {
//               printf("WSARecv() failed with error %d\n", WSAGetLastError());
//               return;
//            }
//         }
//      }
}
