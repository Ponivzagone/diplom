
QT += gui quick multimedia core quickcontrols2 widgets

cross_compile: DEFINES += QT_EXTRA_FILE_SELECTOR=\\\"touch\\\"
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
        AlgorithManager.cpp \
        AudioDevice.cpp \
        AudioFFT.cpp \
        AudioInput.cpp \
        AudioTempo.cpp \
        ImageProvider.cpp \
        NeuroNet/net.cpp \
        NeuroNet/neuron.cpp \
        NeuroNet/recognition.cpp \
        NeuroNet/trainingSet.cpp \
        NoteDescription.cpp \
        NoteListBuilder.cpp \
        main.cpp \
        settings/config_reader.cpp \
        settings/settings.cpp

LIBS += -L/usr/local/lib -laubio
LIBS += -L/usr/lib -lpoppler-qt5

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    AlgorithManager.h \
    AudioDevice.h \
    AudioFFT.h \
    AudioInput.h \
    AudioTempo.h \
    ImageProvider.h \
    NeuroNet/net.h \
    NeuroNet/neuron.h \
    NeuroNet/recognition.h \
    NeuroNet/trainingSet.h \
    settings/config_reader.h \
    settings/settings.h \
    NoteListBuilder.h \
    NoteDescription.h

RESOURCES += \
    audio.qrc


