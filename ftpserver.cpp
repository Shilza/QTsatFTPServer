#include "ftpserver.h"

FTPServer::FTPServer(QObject *parent) : QTcpServer(parent){
    socket = new QTcpSocket;
    listen(QHostAddress::LocalHost, SERVER_PORT);

    connections.clear();

    connect(socket, SIGNAL(readyRead()), SLOT(serverSocketReading()));
}

void FTPServer::incomingConnection(qintptr handle){
    static bool isServerConected = false;
    if(!isServerConected){
        socket->setSocketDescriptor(handle);
        isServerConected = true;
        close();
        listen(QHostAddress::Any, CLIENTS_PORT);
    }
    else{
        connections.insert(handle, new Connection(handle));
        connect(connections.value(handle), SIGNAL(disconnected(qintptr)), SLOT(deleteConnection(qintptr)));
        connect(connections.value(handle), SIGNAL(sendToServer(QByteArray)), SLOT(sendToServer(QByteArray)));
    }
}

void FTPServer::deleteConnection(qintptr handle){
    connections.erase(connections.find(handle));
}

void FTPServer::sendToServer(QByteArray message){
    socket->write(message);
}

void FTPServer::serverSocketReading(){
    QByteArray receivedObject = socket->readAll();

    QJsonObject response;
    QJsonObject request = QJsonDocument::fromJson(receivedObject).object();

    if(request.value("Target").toString() == "Post"){
        Connection *connection = connections.value(request.value("Socket handle").toInt());
        response.insert("Target", "Post");
        response.insert("Value", request.value("Value").toString());
        if(request.value("Value").toString() == "Allow"){
            response.insert("Location", request.value("Location").toString());

            QString filePath = request.value("Location").toString() + '/';
            QString extension = connection->getExtension();
            if(extension == "jpg" || extension == "png" || extension == "bmp"
                    || extension == "jpeg" || extension == "jpe" || extension == "image")
                filePath += "Images/";
            else if(extension == "mp3")
                filePath += "Audio/";
            else
                filePath += "Documents/";
            filePath += QString::number(request.value("ID").toInt()) + '_' +
                    QString::number(QDateTime::currentDateTime().toTime_t()) + '.' + connection->getExtension();

            if(filePath.indexOf("/Avatars") != -1){
                QStringList list = filePath.split("/");
                filePath = list.at(0) + "/" + list.at(1) + "/Images/Avatars/" + list.back();
            }
            connection->setFilePath(filePath);
        }
        connection->send(QJsonDocument(response).toJson());
    }
    else if(request.value("Target").toString() == "Get"){
        Connection *connection = connections.value(request.value("Socket handle").toInt());

        if(request.value("Value").toString() == "Deny"){
            response.insert("Target", "Get");
            response.insert("Value", request.value("Value").toString());
            connection->send(QJsonDocument(response).toJson());
        }
        else{
            QFile file(request.value("Reference").toString());
            file.open(QIODevice::ReadOnly);
            connection->send(file.readAll());
            file.close();
        }
    }
}







