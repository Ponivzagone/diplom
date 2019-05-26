QT += charts multimedia

HEADERS += \
    widget.h \
    xyseriesiodevice.h \
    FindNotes.h \
    NeuroNet/neuron.h \
    NeuroNet/net.h \
    NeuroNet/trainingSet.h \
    NeuroNet/recognition.h

SOURCES += \
    main.cpp\
    widget.cpp \
    xyseriesiodevice.cpp \
    findnotes.cpp \
    NeuroNet/net.cpp \
    NeuroNet/neuron.cpp \
    NeuroNet/trainingSet.cpp \
    NeuroNet/recognition.cpp

CONFIG += c++14

#target.path = $$[QT_INSTALL_EXAMPLES]/charts/audio
#INSTALLS += target
