TARGET = camera
QT = quickcontrols2

equals(QT_ARCH, "arm") {
    QMAKE_CXXFLAGS += -mfp16-format=ieee
}

HEADERS += \
    camera.h

SOURCES += \
    main.cpp \
    camera.cpp

LIBS += -lopencv_core -lopencv_videoio

RESOURCES += \
    main.qrc \
    images/images.qrc

include(app.pri)
