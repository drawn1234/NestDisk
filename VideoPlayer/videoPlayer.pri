
QT += core gui widgets


SOURCES += \
    $$PWD/PacketQueue.cpp \
    $$PWD/playerdialog.cpp \
    $$PWD/videoplayer.cpp


HEADERS += \
    $$PWD/PacketQueue.h \
    $$PWD/playerdialog.h \
    $$PWD/videoplayer.h


FORMS += \
    $$PWD/playerdialog.ui


INCLUDEPATH += $$PWD/ffmpeg-4.2.2/include \
               $$PWD/SDL2-2.26.5/include


LIBS += $$PWD/ffmpeg-4.2.2/lib/avcodec.lib \
        $$PWD/ffmpeg-4.2.2/lib/avdevice.lib \
        $$PWD/ffmpeg-4.2.2/lib/avfilter.lib \
        $$PWD/ffmpeg-4.2.2/lib/avformat.lib \
        $$PWD/ffmpeg-4.2.2/lib/avutil.lib \
        $$PWD/ffmpeg-4.2.2/lib/postproc.lib \
        $$PWD/ffmpeg-4.2.2/lib/swresample.lib \
        $$PWD/ffmpeg-4.2.2/lib/swscale.lib


LIBS += $$PWD/SDL2-2.26.5/bin/SDL2.dll




DEPENDPATH += $$PWD
