#include "SaveThread.h"

#include "sndfile.hh"

#include <pa_ringbuffer.h>
#include <QFile>
#include <QDataStream>

SaveThread::SaveThread() : QThread()
{
    this->toEnd = false;
    this->shuldSendBuffer = false;
}

void SaveThread::run()
{
    int buffer[4096];
    char* tempBuffer = new char[4096 * 4];

    short maxBuffer[512];

    QList<qint16> obuf;

    int pos = 0;

    int peakIndex = 0;

    int format = SF_FORMAT_PCM_24 | SF_FORMAT_RAW;
    QString ending = this->fileName.section('.', -1, -1);
    if (ending == "wav")
    {
        format = SF_FORMAT_PCM_24 | SF_FORMAT_WAV;
    }
    else if (ending == "w64")
    {
        format = SF_FORMAT_PCM_24 | SF_FORMAT_W64;
    }
    else if (ending == "ogg")
    {
        format = SF_FORMAT_OGG | SF_FORMAT_VORBIS;
    }
    else if (ending == "flac")
    {
        format = SF_FORMAT_PCM_24 | SF_FORMAT_FLAC;
    }
    SndfileHandle file(this->fileName.toStdString(), SFM_WRITE, format, 2, 44100);

    QFile peakFile(this->fileName + ".pk");
    peakFile.open(QFile::WriteOnly);

    short towrite[512];

    while (!this->toEnd && this->rb != NULL)
    {
        ring_buffer_size_t len = PaUtil_GetRingBufferReadAvailable(this->rb);
        if (len >= 128)
        {
            PaUtil_ReadRingBuffer(this->rb, tempBuffer, 128);
//            this->rb->get(tempBuffer, 256 * 4);

            qint16 maxLeft1 = 0;
            qint16 maxLeft2 = 0;
            qint16 maxRihgt1 = 0;
            qint16 maxRihgt2 = 0;

            memcpy(&maxBuffer[0], tempBuffer, 128 * 4 * 2);
            for (int i = 1; i < 512; i += 4)
            {
                if (maxBuffer[i] > maxLeft1)
                {
                    maxLeft1 = maxBuffer[i];
                }
                if (maxBuffer[i] < maxLeft2)
                {
                    maxLeft2 = maxBuffer[i];
                }
                if (maxBuffer[i + 2] > maxRihgt1)
                {
                    maxRihgt1 = maxBuffer[i + 2];
                }
                if (maxBuffer[i + 2] < maxRihgt2)
                {
                    maxRihgt2 = maxBuffer[i + 2];
                }
            }
            obuf.append(maxLeft1);
            obuf.append(maxLeft2);
            obuf.append(maxRihgt1);
            obuf.append(maxRihgt2);

            emit signalPeaks(peakIndex, maxLeft1, maxLeft2, maxRihgt1, maxRihgt2);
            ++peakIndex;

            memcpy(&buffer[pos], tempBuffer, 128 * 4 * 2);
            pos += 256;

            if (pos >= 4096)
            {
                file.writef((int*)buffer, 2048);
                pos = 0;
            }

            if (obuf.count() >= 512)
            {
                for (int i = 0; i < 512; ++i)
                {
                    towrite[i] = obuf.at(i);
                }
                peakFile.write((char*)&towrite[0], 1024);
                obuf.clear();
            }
            if (this->shuldSendBuffer && obuf.count() > 0)
            {
                this->shuldSendBuffer = false;
                for (int i = 0; i < obuf.count(); ++i)
                {
                    towrite[i] = obuf.at(i);
                }
                QByteArray ba(obuf.count() * 2, '\0');
                memcpy(ba.data(), &towrite[0], obuf.count() * 2);
                emit signalBuffer(ba);
            }
        }
        else
        {
            msleep(100);
        }
    }

    if (pos > 0)
    {
        file.writef((int*)buffer, pos / 2);
    }

    if (obuf.count() > 0)
    {
        for (int i = 0; i < obuf.count(); ++i)
        {
            towrite[i] = obuf.at(i);
        }
        peakFile.write((char*)&towrite[0], obuf.count() * 2);
        obuf.clear();
    }

    delete [] tempBuffer;

    file.writeSync();

//    sf_close(file.rawHandle());

//    QFile wavfile(this->fileName);
//    wavfile.open(QIODevice::ReadWrite);
//    wavfile.seek(4);
//    int len = wavfile.size() - 8;
//    wavfile.write((char*)&len, 4);
//    wavfile.seek(40);
//    len -= 36;
//    wavfile.write((char*)&len, 4);
//    wavfile.close();

    peakFile.flush();
    peakFile.close();
}


void SaveThread::bufferWanted()
{
    this->shuldSendBuffer = true;
}
