#include "LsPaConnection.h"

#include <portaudio.h>
#include <pa_ringbuffer.h>
#include <QStringList>
#include <QTextStream>

LsPaConnection::LsPaConnection(PaUtilRingBuffer *buffer, QObject *parent) :
    QThread(parent)
{
    this->rb = buffer;
    this->deviceIndex = -1;
}

LsPaConnection::~LsPaConnection()
{
    Pa_Terminate();
}

void LsPaConnection::printDeviceList()
{    
    if (Pa_Initialize() == pa_
    int count = Pa_GetDeviceCount();

    QTextStream out(stdout);

    for (int i = 0; i < count; ++i)
    {
        const PaDeviceInfo* devInfo = Pa_GetDeviceInfo(deviceIndex);
        if (devInfo != NULL && devInfo->maxInputChannels > 1)
        {
            const PaHostApiInfo* apiInfo = Pa_GetHostApiInfo(devInfo->hostApi);
            if (apiInfo != NULL)
            {
                out << QString(devInfo->name) + "[" + QString(apiInfo->name) + "]\n";
            }
        }
    }
}

void LsPaConnection::startRec(int deviceIndex)
{
    this->deviceIndex = deviceIndex;
    QThread::start(QThread::TimeCriticalPriority);
}

QString LsPaConnection::getDeviceName(int deviceIndex)
{
    const PaDeviceInfo* devInfo = Pa_GetDeviceInfo(deviceIndex);
    if (devInfo == NULL)
    {
        return "";
    }
    const PaHostApiInfo* apiInfo = Pa_GetHostApiInfo(devInfo->hostApi);
    if (apiInfo == NULL)
    {
        return "";
    }
    QString ret = QString(devInfo->name) + "[" + QString(apiInfo->name) + "]";
    return ret;
}

int LsPaConnection::getInputDeviceIndex(QString name)
{
    int count = Pa_GetDeviceCount();

    for (int i = 0; i < count; ++i)
    {
        if (this->getDeviceName(i) == name)
        {
            return i;
        }
    }
    return -1;
}

QStringList LsPaConnection::getInputDeviceList()
{
    int count = Pa_GetDeviceCount();

    QStringList ret;

    for (int i = 0; i < count; ++i)
    {
        const PaDeviceInfo* devInfo = Pa_GetDeviceInfo(deviceIndex);
        if (devInfo != NULL && devInfo->maxInputChannels > 1)
        {
            const PaHostApiInfo* apiInfo = Pa_GetHostApiInfo(devInfo->hostApi);
            if (apiInfo != NULL)
            {
                ret << QString(devInfo->name) + "[" + QString(apiInfo->name) + "]";
            }
        }
    }

    return ret;
}

void LsPaConnection::run()
{
    PaStreamParameters inputParameters;
    PaStream *stream;
    PaError err;

    err = Pa_Initialize();
    if( err != paNoError ) return;

    if (this->deviceIndex == -1)
    {
        inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
    }
    else
    {
        inputParameters.device = this->deviceIndex;
    }
    if (inputParameters.device == paNoDevice)
    {
        return;
    }
    inputParameters.channelCount = 2;
    inputParameters.sampleFormat = paInt32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    /* Record some audio. -------------------------------------------- */
    err = Pa_OpenStream(
              &stream,
              &inputParameters,
              NULL,                  /* &outputParameters, */
              44100,
              1024,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              NULL, /* no callback, use blocking API */
              NULL ); /* no callback, so no callback userData */
    if( err != paNoError ) this->rb = NULL;

    err = Pa_StartStream( stream );
    if( err != paNoError ) this->rb = NULL;

    while (this != NULL && this->rb != NULL)
    {
        long len = Pa_GetStreamReadAvailable( stream );
        if (len > this->rb->bufferSize )
        {
            len = this->rb->bufferSize;
        }

        if (this->rb != NULL)
        {
            err = Pa_ReadStream( stream, this->rb->buffer, len );
            if( err != paNoError ) this->rb = NULL;
        }
    }
    err = Pa_CloseStream( stream );
    Pa_Terminate();
}
