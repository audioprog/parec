#-------------------------------------------------
#
# Project created by QtCreator 2013-01-05T09:45:01
#
#-------------------------------------------------

QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = parec
TEMPLATE = app vcapp

win32{
INCLUDEPATH += $$PWD/../include

LIBS += -L$$PWD/../lib/

LIBS += -lportaudio_x86 -llibsndfile-1
} else {
LIBS += -lportaudio -libsndfile
}

SOURCES += main.cpp\
		LsMainWindow.cpp \
	LsPaConnection.cpp \
	../include/pa_ringbuffer.c \
	ThreatedServerSocket.cpp \
	SaveThread.cpp \
	Control.cpp

HEADERS  += LsMainWindow.h \
	LsPaConnection.h \
	ThreatedServerSocket.h \
	SaveThread.h \
	Control.h

FORMS    += LsMainWindow.ui
