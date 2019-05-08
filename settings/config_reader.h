#ifndef CONFIG_READER_H
#define CONFIG_READER_H

#include <QObject>
#include <QVariant>
#include <settings/settings.h>

using VariantMap = QMap<uint, QVariant>;

class ConfigReader : public QObject
{
    Q_OBJECT
public:
    virtual ~ConfigReader();

    static ConfigReader& instance();

    template<typename T>
    void setValue(CoreSettings key, const T& val)
    {
        uint key_id = static_cast<uint>(key);
        mConfig[key_id] = QVariant::fromValue(val);
        emit configChanged(key_id);
    }


    template<typename T>
    T getValue(CoreSettings key, const T& defVal = T())
    {
        uint key_id = static_cast<uint>(key);
        if(mConfig.contains(key_id) && mConfig[key_id].isValid() && mConfig[key_id].canConvert<T>())
        {
            return mConfig[key_id].value<T>();
        }
        return defVal;
    }

signals:
    void configChanged(uint key);

private:
    explicit ConfigReader(QObject *parent = nullptr);

    void setValue(uint key, const QVariant& val);

    bool save(const QString& filename);
    bool load(const QString& filename);

private:
    VariantMap mConfig;
};

#endif // CONFIG_READER_H
