#ifndef THREATEDSERVERSOCKET_H
#define THREATEDSERVERSOCKET_H

#include <QByteArray>
#include <QStringList>
#include <QThread>
#include <QTcpSocket>
#include <QTcpServer>

#include "Control.h"
#include "SaveThread.h"

class ThreatedServerSocket : public QThread
{
    Q_OBJECT

signals:
    void startRecording(QString deviceName, QString fileName);

    void wantBuffer();

    void wantDeviceList();

    void wantEnd();

public:
    explicit ThreatedServerSocket(Control* ctrl, QObject *parent = 0);

private slots:
    void newConnection();

    void readyRead();

    void send(QByteArray data);

    void send(int pos, qint16 left1, qint16 left2, qint16 right1, qint16 right2);

    void send(QStringList deviceList);

    void slotRecordingStarted();

private:
    void run();

    QString readString(QDataStream &ds);

private:
    Control* ctrl;
    SaveThread* save;
    QTcpServer* tcpServer;
    QTcpSocket* socket;
};

#endif // THREATEDSERVERSOCKET_H
