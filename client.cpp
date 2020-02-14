#include "client.h"

void Client::start() {
    emit printToScreen("client started");
       SOCKET writeSocket;
       SOCKADDR_IN server;
       INT Ret;
       struct hostent* hp;
       DWORD SendBytes, RecvBytes, Index;
       WSAOVERLAPPED overlapped;

       writeBuffer = static_cast<char*> (malloc(sizeof(char)*(info->packetSize+1)));
       memset(writeBuffer, 97, info->packetSize);
       WSABUF DataBuf;
       DataBuf.buf = writeBuffer;
       DataBuf.len = info->packetSize;

       if ((Ret = WSAStartup(0x0202,&wsaData)) != 0)
       {
          printf("WSAStartup failed with error %d\n", Ret);
          WSACleanup();
          return;
       }
       if ((hp = gethostbyname(info->ipAddress)) == NULL)
       {
           emit printToScreen(GetLastError() + "Unknown server address");
           return;
       }

       if (info->protocolSelected == protocol::TCP) {
           if ((writeSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0,
              WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
           {
               emit printToScreen("Failed to get a socket: " + WSAGetLastError());
               return;
           }
       } else {
           if ((writeSocket = WSASocket(AF_INET, SOCK_DGRAM, 0, NULL, 0,
              WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
           {
               emit printToScreen("Failed to get a socket: " + WSAGetLastError());
               return;
           }
       }

       server.sin_family = AF_INET;
       server.sin_addr.s_addr = htonl(INADDR_ANY);
       server.sin_port = htons(info->port);
       memcpy((char*)& server.sin_addr, hp->h_addr, hp->h_length);

       if (WSAConnect(writeSocket,(struct sockaddr*) &server, sizeof(server), NULL, NULL, NULL, NULL) == -1) {
           emit printToScreen("Can't connect to server");
           return;
       };

       LPSOCKET_INFORMATION SocketInfo = new SOCKET_INFORMATION;
       SocketInfo->Socket = writeSocket;
       SocketInfo->Overlapped = overlapped;
       SocketInfo->BytesSEND = 0;


       WSAEVENT EventArray[1];
       int times = 0;
       while(times < info->timesToSend) {

           if (WSASend(writeSocket, &DataBuf, 1, &SendBytes, 0,
               (WSAOVERLAPPED*)SocketInfo, ClientWorkerRoutine) == SOCKET_ERROR)
           {
               if (WSAGetLastError() != WSA_IO_PENDING)
               {
                   printf("WSASend() failed with error %d\n", WSAGetLastError());
                   return;
               }
           }
;           emit printToScreen("Sent packet " + QString::number(times) + ": " + SocketInfo->BytesSEND);
           times++;

           //place thread in alertable state
           SleepEx(INFINITE, TRUE);
       }
       delete SocketInfo;
       closesocket(writeSocket);
       emit printToScreen("Socket closed.");
}


void CALLBACK Client:: ClientWorkerRoutine(DWORD Error, DWORD BytesTransferred,
                            LPWSAOVERLAPPED Overlapped, DWORD InFlags) {
    LPSOCKET_INFORMATION SI = (LPSOCKET_INFORMATION)Overlapped;
    SI->BytesSEND += BytesTransferred;
    if (Error != 0)
    {
      SI->Error = Error;
    }

    if (Error != 0 || BytesTransferred == 0)
    {
       closesocket(SI->Socket);
       GlobalFree(SI);
       return;
    }
}

