#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>

enum class CoreSettings : uint
{
    sample_rate = 0,
    frame_size = 1,
    bit_rate = 2,
    fft_type = 3
};

namespace Settings
{
    bool keyToString(CoreSettings key, QString& key_str);

    bool stringToKey(const QString& key_str, uint& key_id);
}

#endif
