#ifndef PORTAUDIOREC_H
#define PORTAUDIOREC_H

#include <portaudio.h>

#include <QStringList>
#include <QThread>

class PointerRingBuffer;

#define SAMPLE_RATE (44100)

class PortaudioRec : public QThread
{
public:
    PortaudioRec(PointerRingBuffer* rb);
    ~PortaudioRec();

    int startRec(int deviceIndex);

    QStringList getInputDeviceList();

    int getInputDeviceIndex(QString name);

    int getStandardInputDeviceIndex();

    QString getDeviceName(int index);

    PointerRingBuffer* getRingBuffer() { return this->rb; }

    void stop() { Pa_StopStream(stream); this->rb = NULL; }

    int myMemberCallback(const void *input, void *,
      unsigned long frameCount, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags);

    static int myPaCallback(
      const void *input, void *output,
      unsigned long frameCount,
      const PaStreamCallbackTimeInfo* timeInfo,
      PaStreamCallbackFlags statusFlags,
      void *userData )
    {
      return ((PortaudioRec*)userData)
         ->myMemberCallback(input, output, frameCount, timeInfo, statusFlags);
    }

    QStringList getHostApiList();

    void printDeviceList();

private:
    void run();

    PaError error;

    QString lastError;

    PointerRingBuffer* rb;

    PaStream* stream;

    PaDeviceIndex deviceIndex;
};

#endif // PORTAUDIOREC_H
