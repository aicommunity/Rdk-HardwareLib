#include "UNmsdkSensorHubProtocolPlugin.h"

#include "USensorLabFrameDecoder.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace RDK {

void UNmsdkSensorHubProtocolPlugin::negotiate(UArduinoPluginHost* host, int protocolVersion)
{
    IArduinoProtocolPlugin::negotiate(host, protocolVersion);
    if (host)
        host->enqueueCommand(QStringLiteral("PROTO 2"));
}

void UNmsdkSensorHubProtocolPlugin::onHealthCheck(UArduinoPluginHost* host)
{
    if (host)
        host->enqueueCommand(QStringLiteral("PING"));
}

void UNmsdkSensorHubProtocolPlugin::onBinaryFrame(UArduinoPluginHost* host, uint8_t type,
                                                  const QByteArray& payload)
{
    if (!host)
        return;
    host->appendFrameLog(QStringLiteral("hub frame type=0x%1 len=%2")
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
        if (decoded.paramCount >= 2)
            host->publishNamedFloat(QStringLiteral("h"), decoded.values[1]);
        if (decoded.paramCount >= 3)
            host->publishNamedFloat(QStringLiteral("distance_cm"), decoded.values[2]);
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
        obj.insert(QStringLiteral("echo"), decoded.servoPin);
        host->publishPinStatusJson(
            QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
        host->setProtocolReady(true);
    } else if (type == 0x22 && payload.size() >= 4) {
        host->publishNamedFloat(QStringLiteral("pin_dht"), static_cast<float>((uint8_t)payload[0]));
        host->publishNamedFloat(QStringLiteral("pin_trig"), static_cast<float>((uint8_t)payload[1]));
        host->publishNamedFloat(QStringLiteral("pin_echo"), static_cast<float>((uint8_t)payload[2]));
        host->publishNamedFloat(QStringLiteral("pin_hall"), static_cast<float>((uint8_t)payload[3]));
        host->setProtocolReady(true);
    } else if (type == 0x7F) {
        host->setProtocolReady(true);
        host->publishNamedFloat(QStringLiteral("pong"), 1.f);
    }
}

} // namespace RDK
