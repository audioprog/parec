#ifndef SAVETHREAD_H
#define SAVETHREAD_H

#include <QThread>

struct PaUtilRingBuffer;

class SaveThread : public QThread
{
    Q_OBJECT

public slots:
    void bufferWanted();

public:
    SaveThread();

    void end() { this->toEnd = true; }

    QString getFileName() { return this->fileName; }

    void setFileName(QString newFileName) { this->fileName = newFileName; }

    void setRingBuffer(PaUtilRingBuffer* rb) { this->rb = rb; }

signals:
    void signalPeaks(int index, qint16 left1, qint16 left2, qint16 right1, qint16 right2);

    void signalBuffer( QByteArray buffer );

private:
    void run();

private:
    QString fileName;

    PaUtilRingBuffer* rb;

    bool toEnd;

    bool shuldSendBuffer;
};

#endif // SAVETHREAD_H
