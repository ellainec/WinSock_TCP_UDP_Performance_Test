#include "server.h"

/*--------------------------------------------------------------------------------
-- SOURCE FILE:       Server.cpp
--
-- PROGRAM:           Abstract class server
--
-- FUNCTIONS:         bool wsaStartup()
                      bool bindSocket()
                      bool setUp()
--
-- DATE:              January 18, 2020
--
--
-- DESIGNER:          Ellaine Chan
--
-- PROGRAMMER:        Ellaine Chan
--
-- NOTES:
-- Server class is an abstract class that contains methods that both a TCP and UDP
-- server will require.
-------------------------------------------------------------------------------- */

/*--------------------------------------------------------------------------------
-- FUNCTION: bool wsaStartup()
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
-- Makes sure that all information required to start receiving/sending data is
-- input by the user. Role, port number, protocol and packet size are always required.
-- IPAddress, and number of times to send are used only by the client.
-------------------------------------------------------------------------------- */
bool Server::wsaStartup() {
    if ((Ret = WSAStartup(0x0202,&wsaData)) != 0) {
        emit printToScreen("WSAStartup failed: " + QString::number(Ret));
        WSACleanup();
        return false;
    }
    return true;
}

/*--------------------------------------------------------------------------------
-- FUNCTION: bool bindSocket()
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
-- Binds created socket to the port. Returns true if successful, else false.
-------------------------------------------------------------------------------- */
bool Server::bindSocket() {
    InternetAddr.sin_family = AF_INET;
    InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    InternetAddr.sin_port = htons(info->port);

    if (bind(ListenSocket, (PSOCKADDR) &InternetAddr,
       sizeof(InternetAddr)) == SOCKET_ERROR) {
        emit printToScreen("bind() failed with error " + QString::number(WSAGetLastError()));
        return false;
    }
    return true;
}

/*--------------------------------------------------------------------------------
-- FUNCTION: bool setUp()
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
-- Starts up WSA, creates socket and binds the socket to a port. The
-- createSocket is a virtual method overidden so that the TCPServer will create a
-- SOCK_STREAM socket while a UDPServer will create a SOCK_DGRAM socket.
-------------------------------------------------------------------------------- */
bool Server::setUp() {
    bool startup = wsaStartup();
    bool socket = createSocket();
    bool bound = bindSocket();
    return startup && socket && bound;
}
