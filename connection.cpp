#include "connection.h"

Connection::Connection(qintptr handle, QObject *parent) : QObject(parent){
    socket = new QTcpSocket(this);
    socket->setSocketDescriptor(handle);

    connect(socket, SIGNAL(readyRead()), SLOT(controller()));
    connect(socket, SIGNAL(disconnected()), SLOT(disconnecting()));
}

void Connection::send(QByteArray request){
    socket->write(request);
}

QString Connection::getExtension() const{
    return extension;
}

void Connection::setFilePath(QString filePath){
    canPost = true;
    this->filePath = filePath;
}

void Connection::serverTryPost(QJsonObject request){
    extension = request.value("Extension").toString();
    request.erase(request.find("Extension"));
    request.erase(request.find("Size"));
    request.insert("Socket handle", socket->socketDescriptor());
    emit sendToServer(QJsonDocument(request).toJson());
}

void Connection::controller(){
    QByteArray receivedObject = socket->readAll();

    QJsonParseError error;

    QJsonObject response;
    QJsonObject request = QJsonDocument::fromJson(receivedObject, &error).object();

    if(error.error == QJsonParseError::NoError){
        if(request.value("Target").toString() == "Post" && request.contains("Size") && request.value("Size").toInt() <= MAX_AFFIX_SIZE)
            return serverTryPost(request);

        response.insert("Target", "Post");
        response.insert("Value", "Deny");
    }
    else if(canPost){

    }

    socket->write(QJsonDocument(response).toJson());
}

void Connection::disconnecting(){
    emit disconnected(socket->socketDescriptor());
}
