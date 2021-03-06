#include "udpserver.h"

/*--------------------------------------------------------------------------------
-- SOURCE FILE:       UDPServer.cpp
--
-- PROGRAM:           UDPServer
--
-- FUNCTIONS:         bool createSocket()
                      bool start()
                      void UDPWorkerRoutine(DWORD Error, DWORD BytesTransferred,
      LPWSAOVERLAPPED Overlapped, DWORD InFlags)
--
-- DATE:              February 16, 2020
--
--
-- DESIGNER:          Ellaine Chan
--
-- PROGRAMMER:        Ellaine Chan
--
-- NOTES:
-- UDPServer inherits from the base Server class to implement a UDPServer.
-- It opens up a streaming socket and starts receiving using completion I/O.
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
-- Creates a socket with datagram option for UDP protocol. Sets socket option
-- to enable address reuse to prevent address in use errors. The receive buffer is
-- also set to be able to hold the maximum total data size of what this application
-- can expect.
-------------------------------------------------------------------------------- */
bool UDPServer::createSocket() {
    if ((ListenSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0,
       WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET) {
       emit printToScreen("Failed to get socket: " + QString::number(WSAGetLastError()));
       return false;
    }
    const char *optval = new char[100];
    if (setsockopt(ListenSocket,SOL_SOCKET, SO_REUSEADDR, optval, sizeof(optval)) < 0) {
        emit printToScreen("Failed to set socket option address reuse.");
    };
    int recvBufSize = 60000*100;
    if (setsockopt(ListenSocket, SOL_SOCKET, SO_RCVBUF,(const char*)&recvBufSize, sizeof(recvBufSize)) < 0) {
        emit printToScreen("Failed to set socket size.");
    }
    delete[] optval;
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
-- Calls WSARecv to start receiving.
-- Once the packet is received, the completion callback will be called.
-------------------------------------------------------------------------------- */
bool UDPServer::start() {
    if (setUp() == false) {
        return false;
    }
    DWORD Flags;
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
        if (WSAGetLastError() != WSA_IO_PENDING) {
            emit printToScreen("WSARecv() failed with error " +  QString::number(WSAGetLastError()));
            return FALSE;
        }
    }
    emit printToScreen("UDP Server ready to receive...");
    while(!serverStopped) {
        SleepEx(1000, true);
    }
    if (SocketInfo->BytesRECV != 0) {
        long totalTime = calculateTimeDelay(SocketInfo->firstPacketTime, SocketInfo->lastPacketTime);
        emit printToScreen("Total of " + QString::number(SocketInfo->BytesRECV) + "B received. Total time: " + QString::number(totalTime) + " ms");
    } else {
        emit printToScreen("No Bytes received.");
    }
    closesocket(ListenSocket);
    SocketInfo = {0};
    serverStopped = false;
    delete SocketInfo;
}

/*--------------------------------------------------------------------------------
-- FUNCTION: void UDPWorkerRoutine(DWORD Error, DWORD BytesTransferred,
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
void UDPServer::UDPWorkerRoutine(DWORD Error, DWORD BytesTransferred,
      LPWSAOVERLAPPED Overlapped, DWORD InFlags)
   {
      DWORD SendBytes, RecvBytes;
      DWORD Flags;
      Flags = 0;

      // Reference the WSAOVERLAPPED structure as a SOCKET_INFORMATION structure
      LPSOCKET_INFORMATION SI = (LPSOCKET_INFORMATION) Overlapped;
      if (SI->BytesRECV == 0) {
          GetSystemTime(&SI->firstPacketTime);
      } else {
          GetSystemTime(&SI->lastPacketTime);
      }
      SI->BytesRECV += BytesTransferred;
      if (BytesTransferred > 0) {
          FileManager::printToFile(SI->Buffer);
      }
      memset(SI->Buffer, 0, sizeof(SI->Buffer));

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
              return;
          }
      }

}



