QT       += core gui serialport charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DESTDIR += ../bin
OBJECTS_DIR = ../bin/obj
TARGET = ahrsgui
TEMPLATE = app
DEFINES -= UNICODE
CONFIG += c++11
CONFIG += warn_off  #不要啥都提示

INCLUDEPATH += ../common .
DEFINES += QT_DEPRECATED_WARNINGS
win32 {
	LIBS += -lwsock32
	DEFINES += WIN32
}

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    ahrsgui.cpp \
    ../common/com_fun.c \
    ../common/comm_rec.c

HEADERS += \
        mainwindow.h \
    main.h \
    ahrsgui.h \
    ../common/com_fun.h \
    ../common/comm_rec.h

FORMS += \
        mainwindow.ui
