#ifndef SERVER_H
#define SERVER_H
#include "Definitions.h"
#include <QPlainTextEdit>

class Server
{
private:

protected:
    WSADATA wsaData;
    SOCKET ListenSocket;
    SOCKADDR_IN InternetAddr;
    INT Ret;
    HANDLE ThreadHandle;
    DWORD ThreadId;
    QPlainTextEdit *editor;
    virtual bool createSocket() = 0;
    bool wsaStartup();
    bool bindSocket();
    bool createThread();
    bool setUp();
    static void printToFile(std::string);
public:
    Server() {};
    virtual ~Server() {
    };
    virtual bool start() = 0;

};

#endif // SERVER_H
