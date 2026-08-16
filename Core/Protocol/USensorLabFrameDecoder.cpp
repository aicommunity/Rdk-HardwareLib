#include "USensorLabFrameDecoder.h"

#include "Transport/UArduinoPinMap.h"

#include <cstring>

namespace RDK {

QString USensorLabFrameDecoder::pinToString(int pin, int boardProfile)
{
    const QString label = UArduinoPinMap::labelForFirmataPin(pin, boardProfile);
    if (!label.isEmpty())
        return label;
    return QString::number(pin);
}

USensorLabDecodedSensors USensorLabFrameDecoder::decodeSensors(const QByteArray& payload)
{
    USensorLabDecodedSensors out;
    if (payload.size() < 2)
        return out;
    out.paramCount = static_cast<uint8_t>(payload[1]);
    if (payload.size() < 2 + out.paramCount * static_cast<int>(sizeof(float)))
        return out;
    for (int i = 0; i < out.paramCount && i < 4; ++i)
        memcpy(&out.values[i], payload.constData() + 2 + i * sizeof(float), sizeof(float));
    out.ok = true;
    return out;
}

USensorLabDecodedPins USensorLabFrameDecoder::decodePinStatus(const QByteArray& payload, int boardProfile)
{
    USensorLabDecodedPins out;
    if (payload.size() < 3)
        return out;
    const uint8_t analog_pin_count = static_cast<uint8_t>(payload[0]);
    const int need = 1 + analog_pin_count + 2;
    if (payload.size() < need)
        return out;
    int idx = 1;
    for (int i = 0; i < analog_pin_count; ++i)
        out.analogPins.append(pinToString(static_cast<uint8_t>(payload[idx++]), boardProfile));
    out.dhtPin = pinToString(static_cast<uint8_t>(payload[idx++]), boardProfile);
    out.servoPin = pinToString(static_cast<uint8_t>(payload[idx++]), boardProfile);
    out.ok = true;
    return out;
}

} // namespace RDK
