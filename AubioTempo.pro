TEMPLATE = app
QT += gui quick quickcontrols2

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
        AudioFFT.cpp \
        AudioInput.cpp \
        AudioTempo.cpp \
        ImageProvider.cpp \
        NeuroNet/net.cpp \
        NeuroNet/neuron.cpp \
        NeuroNet/recognition.cpp \
        NeuroNet/trainingSet.cpp \
        NoteDescription.cpp \
        main.cpp

LIBS += -L/usr/local/lib -laubio
LIBS += -L/usr/lib -lpoppler-qt5

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    AudioFFT.h \
    AudioInput.h \
    AudioTempo.h \
    ImageProvider.h \
    NeuroNet/net.h \
    NeuroNet/neuron.h \
    NeuroNet/recognition.h \
    NeuroNet/trainingSet.h \
    NoteDescription.h

RESOURCES += \
    view/HomeForm.qml \
    view/Page1Form.qml \
    view/Page2Form.qml \
    view/main.qml \
    view/qtquickcontrols2.conf
