#include "ThreatedServerSocket.h"

#include <QBuffer>
#include <QDataStream>

ThreatedServerSocket::ThreatedServerSocket(Control *ctrl, QObject *parent) :
    QThread(parent)
{
    this->save = NULL;
    this->socket = NULL;

    this->ctrl = ctrl;

    connect(this, SIGNAL(wantEnd()), this->ctrl, SLOT(closeApplication()));
    connect(this, SIGNAL(wantDeviceList()), this->ctrl, SLOT(deviceListWanted()));
    connect(this->ctrl, SIGNAL(sendDeviceList(QStringList)), this, SLOT(send(QStringList)));

    connect(this, SIGNAL(startRecording(QString,QString)), this->ctrl, SLOT(startRecording(QString,QString)));
}

void ThreatedServerSocket::newConnection()
{
    if (this->socket)
    {
        this->socket->deleteLater();
    }
    this->socket = this->tcpServer->nextPendingConnection();
    connect(this->socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

QString ThreatedServerSocket::readString( QDataStream& ds)
{
    qint16 len;
    ds >> len;
    QByteArray ba((int)len, '\0');
    ds.readRawData(ba.data(), (uint)len);
    QString text = QString::fromUtf8(ba);

    return text;
}

void ThreatedServerSocket::readyRead()
{
    QByteArray ba = this->socket->readAll();
    QDataStream ds(ba);

    while (!ds.atEnd())
    {
        qint8 cmd;
        ds >> cmd;
        switch (cmd)
        {
        case -1:
            emit wantEnd();
            break;

        case 1:
            emit wantDeviceList();
            break;

        case 2:
        {
            QString deviceName = readString(ds);

            QString fileName = readString(ds);

            emit startRecording(deviceName, fileName);
        }
            break;

        case 3:
            emit wantBuffer();

        }
    }
}

void ThreatedServerSocket::send(QByteArray data)
{
    QDataStream ds(this->socket);
    ds << (qint8)3;
    ds.writeBytes(data.data(), data.count());
}

void ThreatedServerSocket::send(int pos, qint16 left1, qint16 left2, qint16 right1, qint16 right2)
{
    QDataStream ds(this->socket);
    ds << (qint8)1;
    ds << pos;
    ds << left1;
    ds << left2;
    ds << right1;
    ds << right2;
}

void ThreatedServerSocket::send(QStringList deviceList)
{
    QDataStream ds(this->socket);
    ds << (qint8)2;
    ds << (qint16) deviceList.count();
    foreach (QString text, deviceList)
    {
        ds << (qint16) text.count();
        QByteArray ba = text.toUtf8();
        ds.writeRawData(ba.data(), ba.count());
    }
}

void ThreatedServerSocket::slotRecordingStarted()
{
    this->save = ctrl->getSaveThread();

    connect(this->save, SIGNAL(signalPeaks(int,qint16,qint16,qint16,qint16)), this, SLOT(send(int,qint16,qint16,qint16,qint16)));

    connect(this, SIGNAL(wantBuffer()), this->save, SLOT(bufferWanted()));
    connect(this->save, SIGNAL(signalBuffer(QByteArray)), this, SLOT(send(QByteArray)));
}

void ThreatedServerSocket::run()
{
    QTcpServer svr;
    this->tcpServer = &svr;

    connect(&svr, SIGNAL(newConnection()), this, SLOT(newConnection()));

    svr.listen(QHostAddress::LocalHost, 61435);

    this->exec();
}
