QT = core network

TARGET = qctools
TEMPLATE = lib
CONFIG += c++11
CONFIG += staticlib

include(../brew.pri)
include(../ffmpeg.pri)

SOURCES_PATH = $$PWD/../../../Source
message("qctools-lib: SOURCES_PATH = " $$absolute_path($$SOURCES_PATH))

THIRD_PARTY_PATH = $$absolute_path($$SOURCES_PATH/../..)
message("qctools-lib: THIRD_PARTY_PATH = " $$absolute_path($$THIRD_PARTY_PATH))

INCLUDEPATH += $$SOURCES_PATH
INCLUDEPATH += $$SOURCES_PATH/ThirdParty/tinyxml2

QMAKE_CXXFLAGS += -DWITH_SYSTEM_FFMPEG=1

HEADERS = \
    $$SOURCES_PATH/ThirdParty/tinyxml2/tinyxml2.h \
    $$SOURCES_PATH/Core/AudioCore.h \
    $$SOURCES_PATH/Core/AudioStats.h \
    $$SOURCES_PATH/Core/CommonStats.h \
    $$SOURCES_PATH/Core/Core.h \
    $$SOURCES_PATH/Core/FFmpeg_Glue.h \
    $$SOURCES_PATH/Core/VideoCore.h \
    $$SOURCES_PATH/Core/VideoStats.h \
    $$SOURCES_PATH/Core/Timecode.h \
    $$SOURCES_PATH/Core/FileInformation.h \
    $$SOURCES_PATH/Core/SignalServerConnectionChecker.h \
    $$SOURCES_PATH/Core/SignalServer.h


SOURCES = \
    $$SOURCES_PATH/ThirdParty/tinyxml2/tinyxml2.cpp \
    $$SOURCES_PATH/Core/AudioCore.cpp \
    $$SOURCES_PATH/Core/AudioStats.cpp \
    $$SOURCES_PATH/Core/CommonStats.cpp \
    $$SOURCES_PATH/Core/Core.cpp \
    $$SOURCES_PATH/Core/FFmpeg_Glue.cpp \
    $$SOURCES_PATH/Core/VideoCore.cpp \
    $$SOURCES_PATH/Core/VideoStats.cpp \
    $$SOURCES_PATH/Core/Timecode.cpp \
    $$SOURCES_PATH/Core/FileInformation.cpp \
    $$SOURCES_PATH/Core/SignalServerConnectionChecker.cpp \
    $$SOURCES_PATH/Core/SignalServer.cpp

win32 {
    INCLUDEPATH += $$[QT_INSTALL_PREFIX]/../src/qtbase/src/3rdparty/zlib
}

include(../blackmagic.pri)
