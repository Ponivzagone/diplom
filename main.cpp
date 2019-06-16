#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QApplication>

#include <QQmlContext>

#include <ImageProvider.h>
#include <settings/config_reader.h>
#include <QtQml>

#include <QQmlContext>
#include <QQmlFileSelector>

#include <QQuickStyle>

#include <QFontDatabase>


#include <QCoreApplication>
#include <aubio/aubio.h>
#include <iostream>
#include <stdio.h>

#include <unistd.h>

#include <settings/config_reader.h>


#include "AudioInput.h"

int main(int argc, char * argv[]) {
    ConfigReader::instance();

//    ConfigReader::instance().setValue<uint>(CoreSettings::frame_size, 1024);
//    ConfigReader::instance().setValue<uint>(CoreSettings::bit_rate, 16);
//    ConfigReader::instance().setValue<uint>(CoreSettings::sample_rate, 0);
//    ConfigReader::instance().setValue<QString>(CoreSettings::source_path, "/srv/download/Test1.wav");


//    QCoreApplication app(argc, argv);
//    AubioReader jopa;
//    QObject::connect(&jopa, &AudioInput::finishAlgo, &app, QCoreApplication::quit);
//    QTimer::singleShot(500, &jopa, &AudioInput::startRecord);
//    //QTimer::singleShot(800, &jopa, &AudioInput::stopRecord);
//    return app.exec();



    (void)argc;
    (void)argv;
  //  ConfigReader::instance().setValue<uint>(CoreSettings::frame_size, 1024);
  //  ConfigReader::instance().setValue<uint>(CoreSettings::bit_rate, 16);
   // ConfigReader::instance().setValue<uint>(CoreSettings::sample_rate, 0);
    //ConfigReader::instance().setValue<QString>(CoreSettings::source_path, "/srv/download/Test1.wav");

    QGuiApplication::setApplicationName("Audio");
    QGuiApplication::setOrganizationName("Diplom");
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);



    QQuickStyle::setStyle("Material");

    QApplication app(argc, argv);

    QFontDatabase fontDatabase;
    if (fontDatabase.addApplicationFont(":/fonts/fontello.ttf") == -1)
        qWarning() << "Failed to load fontello.ttf";

    AubioReader jopa;


    LiveImageProvider * liveImageProvider(new LiveImageProvider());
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("liveImageProvider", liveImageProvider);
    engine.addImageProvider("live", liveImageProvider);


    QObject::connect(&jopa, &AudioInput::finishAlgo, liveImageProvider, &LiveImageProvider::updateImage);
    //QTimer::singleShot(2000, &jopa, &AudioInput::startRecord);

    jopa.Image();


    QSettings settings;
    QString style = QQuickStyle::name();
    if (!style.isEmpty())
        settings.setValue("style", style);
    else
        QQuickStyle::setStyle(settings.value("style").toString());

    engine.rootContext()->setContextProperty("availableStyles", QQuickStyle::availableStyles());
    engine.rootContext()->setContextProperty("aubioReader", &jopa);

    const QUrl url(QStringLiteral("qrc:/view/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}


