#include "client.h"
/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: client.cpp
--
-- PROGRAM: Client
--
-- FUNCTIONS:
-- void start()
-- void CALLBACK ClientWorkerRoutine(DWORD Error, DWORD BytesTransferred,
--     LPWSAOVERLAPPED Overlapped, DWORD InFlags)
--
--
-- DATE: February 16, 2020
--
-- DESIGNER: Ellaine Chan
--
-- PROGRAMMER: Ellaine Chan
--
-- NOTES:
-- Client interfaces with Windows Socket API to create a TCP or UDP socket,
-- connect and send data to host. The packet size and number of times client sends to host server is
-- specified by the user. This class uses Completion I/O, with completion routine to track
-- the number of bytes successfully sent. It also records how long, in ms, it takes to send all the
-- desired packets.
----------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------
-- FUNCTION: start
--
-- DATE:    February 16, 2020
--
-- DESIGNER: Ellaine Chan
--
-- PROGRAMMER: Ellaine Chan
--
-- PARAMETERS: void
--
-- RETURNS: void
--
-- NOTES:
-- Creates the socket and gets host by name. If it is a TCP socket it starts connection to
-- server, if it is UDP it calls connect to set the server host address as default.
-- Creates a buffer allocating it's size to the amount set by the user. The WSARecv
-- method is looped to send the data stored in the buffer n amount of times, set by the user.
-- System time is stored right before and after the send loop to calculate the total amount
-- of time the data transfer took. The result, and total amount of bytes sent is printed
-- out.
-------------------------------------------------------------------------------- */
void Client::start() {
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
           emit printToScreen("WSAStartup failed with error %d\n" + Ret);
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
       memset((char*)&server, 0, sizeof(struct sockaddr_in));
       server.sin_family = AF_INET;
       server.sin_addr.s_addr = htonl(INADDR_ANY);
       server.sin_port = htons(info->port);
       memcpy((char*)& server.sin_addr, hp->h_addr, hp->h_length);

       if (WSAConnect(writeSocket,(struct sockaddr*) &server, sizeof(server), NULL, NULL, NULL, NULL) == -1) {
           emit printToScreen("Can't connect to server " + WSAGetLastError());
           return;
       };

       LPSOCKET_INFORMATION SocketInfo = new SOCKET_INFORMATION;
       SocketInfo->Socket = writeSocket;
       SocketInfo->Overlapped = overlapped;
       SocketInfo->BytesSEND = 0;


       WSAEVENT EventArray[1];
       int times = 0;
       emit printToScreen("Connected. Sending...");
       GetSystemTime(&stStartTime);
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
           times++;

           //place thread in alertable state
           SleepEx(INFINITE, TRUE);
       }
       GetSystemTime(&stEndTime);
       long totalTime = calculateTimeDelay(stStartTime, stEndTime);
       emit printToScreen(QString::number(info->packetSize) + "B packets sent " + QString::number(times) + " times.");
       emit printToScreen("Took a total of: " + QString::number(calculateTimeDelay(stStartTime, stEndTime)) + " ms");
       delete SocketInfo;
       closesocket(writeSocket);
}

/*--------------------------------------------------------------------------------
-- FUNCTION: ClientWorkerRoutine
--
-- DATE:    February 16, 2020
--
-- DESIGNER: Ellaine Chan
--
-- PROGRAMMER: Ellaine Chan
--
-- PARAMETERS: void
--
-- RETURNS: void
--
-- NOTES:
-- This method is the callback when WSARecv finishes reading. The pointer to
-- the overlapped struct is cast to a pointer to SOCKET_INFORMATION which holds
-- both the overlapped struct and BytesSEND which is used to keep track of how many
-- bytes has been sent. If an error occurs or no bytes were transferred
-- the socket is closed.
-------------------------------------------------------------------------------- */
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

