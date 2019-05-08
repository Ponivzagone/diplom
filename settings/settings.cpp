#include <settings/settings.h>

#include <QStringList>

static QStringList key_strings
{
    "sample_rate",
    "frame_size",
    "bit_rate",
    "fft_type"
};

bool Settings::keyToString(CoreSettings key, QString& key_str)
{
    int id = static_cast<int>(key);
    if(key_strings.size() > id && id >= 0)
    {
        key_str = key_strings[id];
        return true;
    }
    return false;
}

bool Settings::stringToKey(const QString &key_str, uint &key_id)
{
    int index = key_strings.indexOf(key_str);
    if(index != -1)
    {
        key_id = static_cast<uint>(key_id);
        return true;
    }
    return false;
}
