//#include "udpserver.h"
//UDPServer::UDPServer()
//{

//}

//bool UDPServer::createSocket() {
//    if ((ListenSocket = WSASocket(AF_INET, SOCK_DGRAM, 0, NULL, 0,
//       WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
//    {
//       printf("Failed to get a socket %d\n", WSAGetLastError());
//       return false;
//    }
//    return true;
//}

//bool UDPServer::start() {
//    if (setUp() == false) {
//        qDebug() << "setup failed";
//        return false;
//    }

//    if ((SocketInfo->AcceptEvent = WSACreateEvent()) == WSA_INVALID_EVENT)
//    {
//        printf("WSACreateEvent() failed with error %d\n", WSAGetLastError());
//        return false;
//    }

//        // Create a worker thread to service completed I/O requests.

//    if ((ThreadHandle = CreateThread(NULL, 0, WorkerThread, (LPVOID)SocketInfo, 0, &ThreadId)) == NULL)
//    {
//        printf("CreateThread failed with error %d\n", GetLastError());
//        return false;
//    }

//    WSASetEvent(SocketInfo->AcceptEvent);
//    WaitForSingleObject(ThreadHandle, INFINITE);
//    return true;
//}
