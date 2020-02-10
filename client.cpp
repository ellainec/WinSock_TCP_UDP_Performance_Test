#include "client.h"

void Client::start() {
       SOCKET writeSocket;
       SOCKADDR_IN server;
       INT Ret;
       HANDLE ThreadHandle;
       DWORD ThreadId;
       WSAEVENT AcceptEvent;
       struct hostent* hp;
       char localHost[] = "127.0.0.1";

       if ((Ret = WSAStartup(0x0202,&wsaData)) != 0)
       {
          printf("WSAStartup failed with error %d\n", Ret);
          WSACleanup();
          return;
       }
       if ((hp = gethostbyname(localHost)) == NULL)
           {
               qDebug() << GetLastError() << " Unknown server address";
               exit(1);
           }

       if (client_protocol == protocol::TCP) {
           if ((writeSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0,
              WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
           {
              printf("Failed to get a socket %d\n", WSAGetLastError());
              return;
           }
       } else {
           if ((writeSocket = WSASocket(AF_INET, SOCK_DGRAM, 0, NULL, 0,
              WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
           {
              printf("Failed to get a socket %d\n", WSAGetLastError());
              return;
           }
       }

       server.sin_family = AF_INET;
       server.sin_addr.s_addr = htonl(INADDR_ANY);
       server.sin_port = htons(PORT);
       memcpy((char*)& server.sin_addr, hp->h_addr, hp->h_length);

       if (connect(writeSocket,(struct sockaddr*) &server, sizeof(server)) == -1) {
            qDebug() << "can't connect to server";
       };

       // Create a worker thread to service completed I/O requests.

       if ((ThreadHandle = CreateThread(NULL, 0, ClientWorkerThread, (LPVOID) &writeSocket, 0, &ThreadId)) == NULL)
       {
          printf("CreateThread failed with error %d\n", GetLastError());
          return;
       }

       WaitForSingleObject(ThreadHandle, INFINITE);
}

DWORD WINAPI Client::ClientWorkerThread(LPVOID lpParameter){
    CRITICAL_SECTION CriticalSection;
    InitializeCriticalSection(&CriticalSection);
    EnterCriticalSection(&CriticalSection);
    DWORD Flags, SendBytes, RecvBytes, Index;
    LPCLIENT_INFORMATION SocketInfo;
    WSAEVENT EventArray[1];

    SocketInfo = new CLIENT_INFORMATION;

    SOCKET *writeSocket = (SOCKET*) lpParameter;
    // Save the accept event in the event array.
    SocketInfo->Socket = *writeSocket;
    ZeroMemory(&(SocketInfo->Overlapped), sizeof(WSAOVERLAPPED));
    SocketInfo->BytesToSend = DATA_BUFSIZE;
    SocketInfo->BytesLeft = 0;
    SocketInfo->DataBuf.len = sizeof(SocketInfo->Buffer);
    SocketInfo->DataBuf.buf = SocketInfo->Buffer;
    Flags = 0;
    int fileSize = FileManager::getFileSize();
    volatile int remaining = fileSize;
    qDebug() << "reading file with length " << remaining;
    while(remaining > 0) {
        int packetSize = DATA_BUFSIZE;
        if(remaining < DATA_BUFSIZE) {
            packetSize = remaining;
            SocketInfo->DataBuf.len = packetSize;
            memset(SocketInfo->DataBuf.buf, 0, sizeof(SocketInfo->DataBuf.buf));
        }
        int position = fileSize - remaining;

        FileManager::readFromFile(SocketInfo->DataBuf.buf, position, packetSize);
        qDebug() << SocketInfo->DataBuf.buf;
        if (WSASend(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &SendBytes, 0,
            &(SocketInfo->Overlapped), ClientWorkerRoutine) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                printf("WSASend() failed with error %d\n", WSAGetLastError());
                return false;
            }
        }
        remaining = remaining - packetSize;

        //place thread in alertable state
        SleepEx(INFINITE, TRUE);
    }
    LeaveCriticalSection(&CriticalSection);

    return TRUE;
};


void CALLBACK Client:: ClientWorkerRoutine(DWORD Error, DWORD BytesTransferred,
                            LPWSAOVERLAPPED Overlapped, DWORD InFlags) {
    DWORD SendBytes, RecvBytes;
    DWORD Flags;

    // Reference the WSAOVERLAPPED structure as a SOCKET_INFORMATION structure
    LPCLIENT_INFORMATION SI = (LPCLIENT_INFORMATION) Overlapped;

    //qDebug() << SI->DataBuf.buf;
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
//    if (BytesTransferred < SI->BytesToSend) {
//        SI->BytesLeft = SI->BytesToSend - BytesTransferred;
//        //do it again
//        if (WSASend(SI->Socket, &(SI->DataBuf), 1, &SendBytes, 0,
//            &(SI->Overlapped), ClientWorkerRoutine) == SOCKET_ERROR)
//        {
//            if (WSAGetLastError() != WSA_IO_PENDING)
//            {
//                printf("WSASend() failed with error %d\n", WSAGetLastError());
//                return;
//            }
//        }
//    }


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

