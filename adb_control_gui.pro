#-------------------------------------------------
#
# Project created by QtCreator 2015-09-16T11:13:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=gnu++0x

TARGET = adb_control_gui
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    version.h

FORMS    += \
    mainwindow.ui

RC_FILE = myapp.rc

OTHER_FILES += \
    myapp.rc \
    icon_adb.ico

RESOURCES += \
    resources.qrc
