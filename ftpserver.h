#ifndef FTPSERVER_H
#define FTPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QDateTime>
#include <QHash>
#include <QFile>
#include "connection.h"
#include "def.h"

class FTPServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit FTPServer(QObject *parent = nullptr);

private:
    QTcpSocket *socket;
    QHash<qintptr, Connection*> connections;
    void incomingConnection(qintptr handle);

private slots:
    void deleteConnection(qintptr handle);
    void sendToServer(QByteArray message);
    void serverSocketReading();
};

#endif // FTPSERVER_H
