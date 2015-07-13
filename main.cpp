#include "LsMainWindow.h"
#include <QApplication>

#include <pa_ringbuffer.h>
#include "LsPaConnection.h"
#include "SaveThread.h"
#include "ThreatedServerSocket.h"

int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        QCoreApplication ca(argc, argv);

        if (ca.arguments().at(1) == "-s")
        {
            LsPaConnection(NULL).printDeviceList();
        }
        return 0;
    }

    QApplication a(argc, argv);

    PaUtilRingBuffer rb;
    char buffer[8 * 1024 * 16];
    ring_buffer_size_t size = PaUtil_InitializeRingBuffer(&rb, 8, 1024 * 16, &buffer[0]);
    Q_UNUSED(size);

    PaUtil_FlushRingBuffer(&rb);

    int ret = -1;

    LsPaConnection connection(&rb);

    Control control;

    control.setPortAudioRec(&connection);

    if (a.arguments().count() > 2)
    {
        bool ok = false;
        int nr = a.arguments().at(1).toInt(&ok);
        QString deviceName;
        if ( ! ok )
        {
            deviceName = "Line 3/4 (M-Audio Delta 1010LT)[Windows WASAPI]";
        }
        else
        {
            deviceName = connection.getDeviceName(nr);
        }
        if (deviceName != "")
        {
            control.startRecording(deviceName, a.arguments().at(2));
        }

        ret = a.exec();
    }
    else
    {
        ThreatedServerSocket server(&control);
        server.start();

        ret = a.exec();
    }

    return ret;
}
