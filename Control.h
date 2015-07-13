#ifndef CONTROL_H
#define CONTROL_H

#include <QObject>

#include "LsPaConnection.h"
#include "SaveThread.h"

#include <QAction>
#include <QList>
#include <QMenu>
#include <QSignalMapper>
#include <QSystemTrayIcon>

class Control : public QObject
{
    Q_OBJECT

signals:
    void sendDeviceList(QStringList deviceList);

    void signalRecordingStarted();

public:
    Control();
    ~Control();

    void setPortAudioRec(LsPaConnection* rec) { this->rec = rec; this->createMenu(); }

    SaveThread* getSaveThread() { return this->save; }

public slots:
    void closeApplication();

    void deviceListWanted();

    void startRecording(QString deviceName, QString fileName);

private slots:
    void openStreamSelectFile(QString name);

private:
    void createMenu();

    void openStream(QString name, QString fileName);

    void trayIconClicked(QSystemTrayIcon::ActivationReason reason);

private:
    QSystemTrayIcon sysIcon;

    QSignalMapper signalMapper;

    QMenu trayMenu;

    QMenu menuList;

    QList<QAction*> actionsList;

    QAction end;

    LsPaConnection* rec;

    SaveThread* save;
};

#endif // CONTROL_H
