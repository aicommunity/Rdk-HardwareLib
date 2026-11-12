#include "USensorLabProtocolPlugin.h"

#include "USensorLabFrameDecoder.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace RDK {

void USensorLabProtocolPlugin::negotiate(UArduinoPluginHost* host, int protocolVersion)
{
    IArduinoProtocolPlugin::negotiate(host, protocolVersion);
    if (host && protocolVersion < 2)
        host->setProtocolReady(true);
}

void USensorLabProtocolPlugin::onBinaryFrame(UArduinoPluginHost* host, uint8_t type,
                                             const QByteArray& payload)
{
    if (!host)
        return;
    host->appendFrameLog(QStringLiteral("frame type=0x%1 len=%2")
                             .arg(type, 2, 16, QChar('0'))
                             .arg(payload.size()));
    if (type == 0x01) {
        const auto decoded = USensorLabFrameDecoder::decodeSensors(payload);
        if (!decoded.ok)
            return;
        QVector<double> row;
        row << decoded.paramCount;
        for (int i = 0; i < decoded.paramCount && i < 4; ++i)
            row << decoded.values[i];
        host->publishSensorMatrixRow(row);
        if (decoded.paramCount >= 1)
            host->publishNamedFloat(QStringLiteral("t"), decoded.values[0]);
        if (decoded.paramCount >= 4)
            host->publishNamedFloat(QStringLiteral("speed"), decoded.values[3]);
        host->setProtocolReady(true);
    } else if (type == 0x04) {
        const auto decoded =
            USensorLabFrameDecoder::decodePinStatus(payload, host->boardProfile());
        if (!decoded.ok)
            return;
        QJsonObject obj;
        QJsonArray analog;
        for (const QString& p : decoded.analogPins)
            analog.append(p);
        obj.insert(QStringLiteral("analog"), analog);
        obj.insert(QStringLiteral("dht"), decoded.dhtPin);
        obj.insert(QStringLiteral("servo"), decoded.servoPin);
        host->publishPinStatusJson(
            QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
        host->setProtocolReady(true);
    }
}

} // namespace RDK
