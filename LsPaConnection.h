#ifndef LSPACONNECTION_H
#define LSPACONNECTION_H

#include <QThread>

struct PaUtilRingBuffer;

class LsPaConnection : public QThread
{
    Q_OBJECT
public:
    LsPaConnection(PaUtilRingBuffer* buffer, QObject *parent = 0);

    ~LsPaConnection();

    void printDeviceList();

    void startRec( int deviceIndex );

    void stop() { this->rb = NULL; }

    QString getDeviceName( int deviceIndex );

    int getInputDeviceIndex( QString name );

    QStringList getInputDeviceList();

    PaUtilRingBuffer* getRingBuffer() { return rb; }

private:
    void run();

private:
    PaUtilRingBuffer* rb;

    int deviceIndex;
};

#endif // LSPACONNECTION_H
