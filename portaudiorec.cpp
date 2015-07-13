#include "portaudiorec.h"

#include <QTextStream>

#include "PointerRingBuffer.h"

PortaudioRec::PortaudioRec(PointerRingBuffer *rb)
{
    this->rb = rb;

    PaError err = Pa_Initialize();
    if( err != paNoError )
    {
        this->error = err;
    }
}

PortaudioRec::~PortaudioRec()
{
    if (Pa_IsStreamActive(this->stream))
    {
        Pa_StopStream(this->stream);
    }

    /*PaError err = */ Pa_Terminate();
}


QStringList PortaudioRec::getHostApiList()
{
    QStringList hostInfoList;

    for (int i = 0; i < Pa_GetHostApiCount(); ++i)
        hostInfoList << QString(Pa_GetHostApiInfo(i)->name);

    return hostInfoList;
}

int PortaudioRec::getInputDeviceIndex(QString name)
{
    QStringList hostInfoList = getHostApiList();

    for (int i = 0; i < Pa_GetDeviceCount(); ++i)
    {
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
        if ( deviceInfo->maxInputChannels > 1 && QString(deviceInfo->name) + "[" + hostInfoList.value(deviceInfo->hostApi) + "]" == name )
        {
            return i;
        }
    }

    return -1;
}

QStringList PortaudioRec::getInputDeviceList()
{
    QStringList ret;
    QStringList hostInfoList = getHostApiList();

    for (int i = 0; i < Pa_GetDeviceCount(); ++i)
    {
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo->maxInputChannels > 1)
        {
            ret << QString(deviceInfo->name) + "[" + hostInfoList.value(deviceInfo->hostApi) + "]";
        }
    }
    return ret;
}

int PortaudioRec::getStandardInputDeviceIndex()
{
    return Pa_GetDefaultInputDevice();
}

QString PortaudioRec::getDeviceName(int index)
{
    const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(index);
    return QString(deviceInfo->name) + "[" + QString(Pa_GetHostApiInfo(deviceInfo->hostApi)->name) + "]";
}


int PortaudioRec::myMemberCallback(const void *input, void *, unsigned long frameCount, const PaStreamCallbackTimeInfo *, PaStreamCallbackFlags)
{
    if (this->rb != NULL)
        this->rb->put((char*)input, frameCount * 8);
        //this->rb->Put((samplePtr)input, int24Sample, frameCount * 2);
    // Do what you need (having access to every part of MyPortaudioClass)

    return paContinue;
}

void PortaudioRec::printDeviceList()
{
    QStringList hostApiList = getHostApiList();

    QTextStream out(stdout);

    for (int i = 0; i < Pa_GetDeviceCount(); ++i)
    {
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo->maxInputChannels > 1)
        {
            out << i << " = " << QString(deviceInfo->name) << ":" << hostApiList.at(deviceInfo->hostApi) << ":" << deviceInfo->maxInputChannels << "\n";
        }
    }
}

void PortaudioRec::run()
{
    PaError err;

    const PaDeviceInfo* info = Pa_GetDeviceInfo(this->deviceIndex);

    /* Open an audio I/O stream. */

    PaStreamParameters parameters;
    parameters.device = this->deviceIndex;
    parameters.channelCount = 2;
    parameters.sampleFormat = paInt32;
    parameters.hostApiSpecificStreamInfo = NULL;
    parameters.suggestedLatency = info->defaultHighInputLatency;

    err = Pa_OpenStream( &this->stream, &parameters, NULL, SAMPLE_RATE, paFramesPerBufferUnspecified, paNoFlag, NULL, this);
    if (err != paNoError)
    {
        this->lastError = QString(Pa_GetErrorText(err));
        return;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError)
    {
        this->lastError = QString(Pa_GetErrorText(err));
        return;
    }

    int buffer[4096];

    while (this != NULL && this->rb != NULL)
    {
        long len = Pa_GetStreamReadAvailable(stream);
        if (len < 0)
        {
            this->lastError = QString(Pa_GetErrorText(len));
        }
        if (len > 512)
        {
            len = 512;
        }
        err = Pa_ReadStream(this->stream, &buffer[0], len);
        if (err != paNoError)
        {
            this->lastError = QString(Pa_GetErrorText(err));
        }
        this->rb->put((char*)&buffer[0], len * 8);
    }
}

int PortaudioRec::startRec(int deviceIndex)
{
    if (deviceIndex < 0 || deviceIndex >= Pa_GetDeviceCount())
    {
        return 0;
    }

    this->deviceIndex = deviceIndex;

    this->start(QThread::TimeCriticalPriority);

    return -1;
}
