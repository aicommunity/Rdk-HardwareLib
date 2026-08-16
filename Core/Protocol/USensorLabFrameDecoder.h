#ifndef USENSORLABFRAMEDECODER_H
#define USENSORLABFRAMEDECODER_H

#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QVector>

namespace RDK {

struct USensorLabDecodedSensors {
    uint8_t paramCount = 0;
    float values[4] = {0, 0, 0, 0};
    bool ok = false;
};

struct USensorLabDecodedPins {
    QStringList analogPins;
    QString dhtPin;
    QString servoPin;
    bool ok = false;
};

class USensorLabFrameDecoder {
public:
    static QString pinToString(int pin, int boardProfile);
    static USensorLabDecodedSensors decodeSensors(const QByteArray& payload);
    static USensorLabDecodedPins decodePinStatus(const QByteArray& payload, int boardProfile);
};

} // namespace RDK

#endif
