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
    if (type == 0x01 || type == 0x30 || type == 0x31 || type == 0x32 || type == 0x33) {
        const auto decoded = USensorLabFrameDecoder::decodeSensors(payload);
        if (!decoded.ok)
            return;
        QVector<double> row;
        row << static_cast<double>(type) << decoded.paramCount;
        for (int i = 0; i < decoded.paramCount && i < 8; ++i)
            row << decoded.values[i];
        host->publishSensorMatrixRow(row);

        if (type == 0x01) {
            if (decoded.paramCount >= 1)
                host->publishNamedFloat(QStringLiteral("t"), decoded.values[0]);
            if (decoded.paramCount >= 2)
                host->publishNamedFloat(QStringLiteral("h"), decoded.values[1]);
            if (decoded.paramCount >= 3)
                host->publishNamedFloat(QStringLiteral("pressure_hpa"), decoded.values[2]);
        } else if (type == 0x30) {
            if (decoded.paramCount >= 1)
                host->publishNamedFloat(QStringLiteral("distance_mm"), decoded.values[0]);
        } else if (type == 0x31) {
            if (decoded.paramCount >= 1)
                host->publishNamedFloat(QStringLiteral("ax"), decoded.values[0]);
            if (decoded.paramCount >= 2)
                host->publishNamedFloat(QStringLiteral("ay"), decoded.values[1]);
            if (decoded.paramCount >= 3)
                host->publishNamedFloat(QStringLiteral("az"), decoded.values[2]);
            if (decoded.paramCount >= 4)
                host->publishNamedFloat(QStringLiteral("gx"), decoded.values[3]);
            if (decoded.paramCount >= 5)
                host->publishNamedFloat(QStringLiteral("gy"), decoded.values[4]);
            if (decoded.paramCount >= 6)
                host->publishNamedFloat(QStringLiteral("gz"), decoded.values[5]);
        } else if (type == 0x32) {
            if (decoded.paramCount >= 1)
                host->publishNamedFloat(QStringLiteral("bus_v"), decoded.values[0]);
            if (decoded.paramCount >= 2)
                host->publishNamedFloat(QStringLiteral("current_ma"), decoded.values[1]);
            if (decoded.paramCount >= 3)
                host->publishNamedFloat(QStringLiteral("power_mw"), decoded.values[2]);
        } else if (type == 0x33) {
            if (decoded.paramCount >= 1)
                host->publishNamedFloat(QStringLiteral("pca_ch"), decoded.values[0]);
            if (decoded.paramCount >= 2)
                host->publishNamedFloat(QStringLiteral("pca_duty"), decoded.values[1]);
        }
        host->setProtocolReady(true);
    } else if (type == 0x7F) {
        host->setProtocolReady(true);
        host->publishNamedFloat(QStringLiteral("pong"), 1.f);
    }
}

} // namespace RDK
