#include "Control.h"

#include <QApplication>
#include <QDir>
#include <QFileDialog>

Control::Control() : end(this)
{
    this->save = NULL;

    this->end.setText("Beenden");

    this->menuList.setTitle("Aufnahme von:");

    this->trayMenu.addAction(&end);

    this->sysIcon.setIcon(QIcon(":/icons/icon"));
    this->sysIcon.show();
    this->sysIcon.setContextMenu(&trayMenu);


    connect(&end, SIGNAL(triggered()), this, SLOT(closeApplication()));

    connect(&this->signalMapper, SIGNAL(mapped(QString)), this, SLOT(openStreamSelectFile(QString)));

    //connect(&sysIcon, &QSystemTrayIcon::activated, this, &Control::trayIconClicked);
}

Control::~Control()
{
    foreach (QAction* item, this->actionsList)
    {
        delete item;
    }
}

void Control::deviceListWanted()
{
    emit sendDeviceList(this->rec->getInputDeviceList());
}

void Control::startRecording(QString deviceName, QString fileName)
{
    int index = this->rec->getInputDeviceIndex(deviceName);
    if (index > -1)
    {
        QString path = fileName.section('/', 0, -2);
        if (QDir(path).mkpath(path))
        {
            openStream(deviceName, fileName);
        }
    }
}

void Control::closeApplication()
{
    if (this->save != NULL && this->save->isRunning())
    {
        connect(this->save, SIGNAL(finished()), qApp, SLOT(quit()));
        this->rec->stop();
        this->save->end();
    }
    else
    {
        qApp->quit();
    }
}

void Control::openStreamSelectFile(QString name)
{
    int index = this->rec->getInputDeviceIndex(name);

    if (index > -1)
    {
        QString fileName = QFileDialog::getSaveFileName(NULL, QApplication::applicationName(), QDir::homePath(), "audio (*.wav *.w64 *.flac *.ogg)");
        if (fileName != "")
        {
            if (!fileName.endsWith(".wav", Qt::CaseInsensitive) && !fileName.endsWith(".ogg", Qt::CaseInsensitive) && !fileName.endsWith(".flac", Qt::CaseInsensitive) && !fileName.endsWith(".w64", Qt::CaseInsensitive))
            {
                fileName.append(".wav");
            }
            openStream(name, fileName);
        }
    }
}

void Control::createMenu()
{
    QStringList list = this->rec->getInputDeviceList();

    foreach (QString item, list)
    {
        QAction* action = new QAction(item, this);
        this->actionsList.append(action);
        this->menuList.addAction(action);

        this->signalMapper.setMapping(action, item);
        connect(action, SIGNAL(triggered()), &this->signalMapper, SLOT(map()));
    }

    this->trayMenu.insertMenu(&this->end, &this->menuList);
}

void Control::openStream(QString name, QString fileName)
{
    int index = this->rec->getInputDeviceIndex(name);

    if (index > -1)
    {
        this->menuList.setEnabled(false);

        this->save = new SaveThread();
        this->save->setFileName(fileName);
        this->save->setRingBuffer(this->rec->getRingBuffer());
        this->save->start();

        this->rec->startRec(this->rec->getInputDeviceIndex(name));

        this->sysIcon.setToolTip(fileName + "\n" + name);

        emit signalRecordingStarted();
    }
}

void Control::trayIconClicked(QSystemTrayIcon::ActivationReason reason)
{
    Q_UNUSED(reason);
}
