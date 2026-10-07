#include "UNmsdkI2cHubProtocolPlugin.h"

#include "USensorLabFrameDecoder.h"

namespace RDK {

void UNmsdkI2cHubProtocolPlugin::negotiate(UArduinoPluginHost* host, int protocolVersion)
{
    IArduinoProtocolPlugin::negotiate(host, protocolVersion);
    if (host)
        host->enqueueCommand(QStringLiteral("PROTO 2"));
}

void UNmsdkI2cHubProtocolPlugin::onHealthCheck(UArduinoPluginHost* host)
{
    if (host)
        host->enqueueCommand(QStringLiteral("PING"));
}

void UNmsdkI2cHubProtocolPlugin::onBinaryFrame(UArduinoPluginHost* host, uint8_t type,
                                               const QByteArray& payload)
{
    if (!host)
        return;
    host->appendFrameLog(QStringLiteral("i2c hub type=0x%1 len=%2")
                             .arg(type, 2, 16, QChar('0'))
                             .arg(payload.size()));
    if (type == 0x01) {
        const auto decoded = USensorLabFrameDecoder::decodeSensors(payload);
        if (!decoded.ok)
            return;
        QVector<double> row;
        row << decoded.paramCount;
        for (int i = 0; i < decoded.paramCount && i < 8; ++i)
            row << decoded.values[i];
        host->publishSensorMatrixRow(row);
        if (decoded.paramCount >= 1)
            host->publishNamedFloat(QStringLiteral("t"), decoded.values[0]);
        if (decoded.paramCount >= 2)
            host->publishNamedFloat(QStringLiteral("h"), decoded.values[1]);
        if (decoded.paramCount >= 3)
            host->publishNamedFloat(QStringLiteral("pressure_hpa"), decoded.values[2]);
        host->setProtocolReady(true);
    } else if (type == 0x7F) {
        host->setProtocolReady(true);
        host->publishNamedFloat(QStringLiteral("pong"), 1.f);
    }
}

} // namespace RDK
