#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QTcpSocket>
#include <QFile>
#include <QDir>
#include "def.h"

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(qintptr handle, QObject *parent = nullptr);
    void send(QByteArray request);
    QString getExtension() const;
    void setFilePath(QString filePath);

private:
    QTcpSocket *socket;
    QString extension;
    QString filePath;
    quint32 sizeOfAttachment;
    bool canPost = false;

    void serverTryPost(QJsonObject request);

signals:
    void disconnected(qintptr handle);
    void sendToServer(QByteArray message);

private slots:
    void controller();
    void disconnecting();
};

#endif // CONNECTION_H
