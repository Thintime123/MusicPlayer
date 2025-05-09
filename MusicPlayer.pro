QT       += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    lyricparser.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    lyricparser.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    MusicPlayer_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

DISTFILES += \
    res/Audio/M800001ziKgJ3o5Ipp.mp3 \
    res/Audio/M800004HtbeG1hrKLv.mp3
