#-------------------------------------------------
#
# Project created by QtCreator 2015-09-27T11:13:06
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Demo
TEMPLATE = app

FORMS += \
    demo.ui

HEADERS += \
    demo.h \
    ../qjfastfir.h

SOURCES += \
    demo.cpp \
    main.cpp \
    ../qjfastfir.cpp

DISTFILES += \
    ../../kiss_fft130_jfork1.0.0/CHANGELOG \
    ../../kiss_fft130_jfork1.0.0/COPYING \
    ../LICENSE \
    ../../README.md \
    screenshot.png


