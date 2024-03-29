QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        Demo.cpp \
        api/BitConverter.cpp \
        api/Dashboard.cpp \
        api/DobotClient.cpp \
        api/DobotMove.cpp \
        api/ErrorInfoBean.cpp \
        api/ErrorInfoHelper.cpp \
        api/Feedback.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    Demo.h \
    api/BitConverter.h \
    api/Dashboard.h \
    api/DescartesPoint.h \
    api/DobotClient.h \
    api/DobotMove.h \
    api/ErrorInfoBean.h \
    api/ErrorInfoHelper.h \
    api/Feedback.h \
    api/FeedbackData.h \
    api/JointPoint.h \
    api/rapidjson/allocators.h \
    api/rapidjson/cursorstreamwrapper.h \
    api/rapidjson/document.h \
    api/rapidjson/encodedstream.h \
    api/rapidjson/encodings.h \
    api/rapidjson/error/en.h \
    api/rapidjson/error/error.h \
    api/rapidjson/filereadstream.h \
    api/rapidjson/filewritestream.h \
    api/rapidjson/fwd.h \
    api/rapidjson/internal/biginteger.h \
    api/rapidjson/internal/clzll.h \
    api/rapidjson/internal/diyfp.h \
    api/rapidjson/internal/dtoa.h \
    api/rapidjson/internal/ieee754.h \
    api/rapidjson/internal/itoa.h \
    api/rapidjson/internal/meta.h \
    api/rapidjson/internal/pow10.h \
    api/rapidjson/internal/regex.h \
    api/rapidjson/internal/stack.h \
    api/rapidjson/internal/strfunc.h \
    api/rapidjson/internal/strtod.h \
    api/rapidjson/internal/swap.h \
    api/rapidjson/istreamwrapper.h \
    api/rapidjson/memorybuffer.h \
    api/rapidjson/memorystream.h \
    api/rapidjson/msinttypes/inttypes.h \
    api/rapidjson/msinttypes/stdint.h \
    api/rapidjson/ostreamwrapper.h \
    api/rapidjson/pointer.h \
    api/rapidjson/prettywriter.h \
    api/rapidjson/rapidjson.h \
    api/rapidjson/reader.h \
    api/rapidjson/schema.h \
    api/rapidjson/stream.h \
    api/rapidjson/stringbuffer.h \
    api/rapidjson/uri.h \
    api/rapidjson/writer.h

win32 {
    LIBS += -lWs2_32
}
