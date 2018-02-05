#include <QCoreApplication>
#include <QDateTime>
#include "ftpserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qsrand(QTime(0,0,0).msecsTo(QTime::currentTime()));

    FTPServer server;

    return a.exec();
}
