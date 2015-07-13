#include "PortAudioRead.h"

PortAudioRead::PortAudioRead(QObject *parent) :
    QThread(parent)
{
    //this->setPriority(QThread::TimeCriticalPriority);
    this->toEnd = false;
}

void PortAudioRead::run()
{
    //this->index = Pa_GetDefaultInputDevice();
    const PaDeviceInfo* info = Pa_GetDeviceInfo(this->index);
    PaStream* stream = NULL;
    PaStreamParameters parameter;
    parameter.channelCount = 2;
    parameter.device = this->index;
    parameter.hostApiSpecificStreamInfo = NULL;
    parameter.sampleFormat = paInt24;
    parameter.suggestedLatency = info->defaultHighInputLatency;

    PaError err = Pa_OpenStream(&stream, &parameter, NULL, 44100, 1024, paNoFlag, NULL, NULL);
    //PaError err = Pa_OpenDefaultStream(&stream, 2, 0, paFloat32, 44100, paFramesPerBufferUnspecified, NULL, NULL);

    if (err == paNoError)
    {
        err = Pa_StartStream(stream);
    }

    char buffer[12288];

    while (!this->toEnd && err == paNoError)
    {
        long count = Pa_GetStreamReadAvailable(stream);
        if (count > 4096)
            count = 4096;
        if (count > 0)
        {
            err = Pa_ReadStream(stream, &buffer[0], count);
            if (err == paNoError)
            {
                this->rb->Put((samplePtr)&buffer, int24Sample, count * 2);
            }
        }
        else
        {
            msleep(100);
        }
    }

    Pa_StopStream(stream);

    Pa_CloseStream(stream);
}
