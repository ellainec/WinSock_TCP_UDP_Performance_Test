#ifndef SERVER_H
#define SERVER_H
#include "Definitions.h"
#include <QPlainTextEdit>
#include <QObject>

class Server: public QObject  {
    Q_OBJECT

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
    CONNECT_INFORMATION* info;
public:
    Server(CONNECT_INFORMATION* info) : info(info) {};
    virtual ~Server() {
    };
    virtual bool start() = 0;
signals:
    virtual void printToScreen(QString);
};

#endif // SERVER_H
