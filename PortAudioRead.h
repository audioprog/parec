#ifndef PORTAUDIOREAD_H
#define PORTAUDIOREAD_H

#include <QThread>

#include <portaudio.h>
#include "PointerRingBuffer.h"

class PortAudioRead : public QThread
{
    Q_OBJECT
public:
    explicit PortAudioRead(QObject *parent = 0);

    void end() { this->toEnd = true; }
    void setDevice(int newDevice) { this->index = newDevice; }
    void setRingBuffer(PointerRingBuffer* buffer) { this->rb = buffer; }

private:
    void run();

private:
    bool toEnd;
    int index;
    PointerRingBuffer* rb;
};

#endif // PORTAUDIOREAD_H
