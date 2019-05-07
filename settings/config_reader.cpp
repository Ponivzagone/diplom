#include <settings/config_reader.h>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include <settings/settings.h>

#ifdef QT_DEBUG
#include <QDebug>
#endif

constexpr char SETTINGS_FILE[] = "diplomato_desu.conf";

ConfigReader::~ConfigReader()
{
    if(!save(SETTINGS_FILE))
    {
#ifdef QT_DEBUG
        qDebug() << "Can't save settings";
#endif
    }
}

ConfigReader &ConfigReader::instance()
{
    static ConfigReader instance;
    return instance;
}

void ConfigReader::setValue(uint key, const QVariant &val)
{
    mConfig[key] = val;
    emit configChanged(key);
}

ConfigReader::ConfigReader(QObject *parent) : QObject(parent)
{
    if(!load(SETTINGS_FILE))
    {
#ifdef QT_DEBUG
        qDebug() << "Can't load settings";
#endif
    }
}

bool ConfigReader::save(const QString &filename)
{
    QFile saveFile(filename);

    if(!saveFile.open(QIODevice::WriteOnly)) return false;

    QJsonObject obj;

    for(VariantMap::iterator it = mConfig.begin(); it != mConfig.end(); ++it)
    {
        QString key_str;
        if(Settings::keyToString(static_cast<CoreSettings>(it.key()), key_str))
        {
            obj[key_str] = QJsonValue::fromVariant(it.value());
        }
    }

    QJsonDocument saveDoc(obj);

#ifdef QT_DEBUG
    saveFile.write(saveDoc.toJson());
#else
    saveFile.write(saveDoc.toBinaryData());
#endif

    return true;
}

bool ConfigReader::load(const QString &filename)
{
    QFile loadFile(filename);

    if(!loadFile.open(QIODevice::ReadOnly)) return false;

    QByteArray data = loadFile.readAll();

#ifdef QT_DEBUG
    QJsonDocument loadDoc(QJsonDocument::fromJson(data));
#else
    QJsonDocument loadDoc(QJsonDocument::fromBinaryData(data));
#endif

    QJsonObject obj(loadDoc.object());

    for(const QString& key : obj.keys())
    {
        uint id;
        if(Settings::stringToKey(key, id))
        {
            setValue(id, obj[key].toVariant());
        }
    }

    return true;
}
