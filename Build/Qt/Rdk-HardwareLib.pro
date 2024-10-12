#-------------------------------------------------
#
# Project created by QtCreator 2024-09-22T15:17:55
#
#-------------------------------------------------

QT       -= gui

TARGET = Rdk-HardwareLib.qt
TEMPLATE = lib
CONFIG += staticlib

include($$PWD/../../../../Rdk/Build/Lib/Qt/RdkDefines.pri)

DEFINES += LIBRDK_LIBRARY_EXPORT
DEFINES += RDK_UNICODE_RUN
DEFINES += RDK_QT

VERSION = 0 # $$system(hg parents --template '{rev}')
DEFINES += RDK_LIB_CVS_REVISION=$$VERSION

unix {
    DESTDIR = $$PWD/../../../../Bin/Platform/Linux/Lib.Qt
}

windows {
    DESTDIR = $$PWD/../../../../Bin/Platform/Win/Lib.Qt
}

HEADERS += \
    ../../Core/UHardwareLibrary.h

SOURCES += \
    ../../Core/UHardwareLibrary.cpp
