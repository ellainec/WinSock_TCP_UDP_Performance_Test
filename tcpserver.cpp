#include "tcpserver.h"

/*--------------------------------------------------------------------------------
-- SOURCE FILE:       TCPServer.cpp
--
-- PROGRAM:           TCPServer
--
-- FUNCTIONS:         bool createSocket()
                      bool start()
                      void TCPWorkerRoutine(DWORD Error, DWORD BytesTransferred,
      LPWSAOVERLAPPED Overlapped, DWORD InFlags)
--
-- DATE:              January 18, 2020
--
--
-- DESIGNER:          Ellaine Chan
--
-- PROGRAMMER:        Ellaine Chan
--
-- NOTES:
-- TCPServer inherits from the base Server class to implement a TCPServer.
-- It opens up a streaming socket, listens for an incoming connect, accepts, and
-- starts receiving using completion I/O.
-------------------------------------------------------------------------------- */

/*--------------------------------------------------------------------------------
-- FUNCTION: bool createSocket()
--
-- DATE:    February 16, 2020
--
-- DESIGNER: Ellaine Chan
--
-- PROGRAMMER: Ellaine Chan
--
-- PARAMETERS: void
--
-- RETURNS: true if successful, else false
--
-- NOTES:
-- Creates a socket with streaming option for TCP protocol. Sets socket option
-- to enable address reuse to prevent address in use errors.
-------------------------------------------------------------------------------- */
bool TCPServer::createSocket() {
    if ((ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0,
       WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
    {
       qDebug() << WSAGetLastError();
       emit printToScreen("Failed to get a socket " + QString::number(WSAGetLastError()));
       return false;
    }
    const char *optval = new char[100];
    if (setsockopt(ListenSocket,SOL_SOCKET, SO_REUSEADDR, optval, sizeof(optval)) < 0) {
        emit printToScreen("failed to set sock opt");
    };
    return true;
}

/*--------------------------------------------------------------------------------
-- FUNCTION: bool start()
--
-- DATE:    February 16, 2020
--
-- DESIGNER: Ellaine Chan
--
-- PROGRAMMER: Ellaine Chan
--
-- PARAMETERS: void
--
-- RETURNS: true if successful, else false
--
-- NOTES:
-- Calls setUp that will start WSA, create socket, and bind the socket to the port.
-- Listens to incoming connection, accepts the first one, and starts listening.
-- Once the packet is received, the completion callback will be called.
-------------------------------------------------------------------------------- */
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
         return false;
    }

    if ((AcceptInfo->AcceptEvent = WSACreateEvent()) == WSA_INVALID_EVENT)
    {
       return false;
    }
    emit printToScreen("Connected and ready to accept incoming connection...");
    AcceptInfo->AcceptSocket = accept(ListenSocket, NULL, NULL);
    //start timer
    Flags = 0;

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

/*--------------------------------------------------------------------------------
-- FUNCTION: void TCPWorkerRoutine(DWORD Error, DWORD BytesTransferred,
      LPWSAOVERLAPPED Overlapped, DWORD InFlags)
--
-- DATE:    February 16, 2020
--
-- DESIGNER: Ellaine Chan
--
-- PROGRAMMER: Ellaine Chan
--
-- PARAMETERS: DWORD Error, DWORD BytesTransferred,
      LPWSAOVERLAPPED Overlapped, DWORD InFlags
--
-- RETURNS: void
--
-- NOTES:
-- Function called when a WSARecv reads a packet. Increments the total of bytes
-- received and calls WSARecv again to receive the next packet. If there is an error,
-- or BytesTransferred is 0 meaning client has nothing more to send, it doesn't call
-- WSARecv.
-------------------------------------------------------------------------------- */
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



