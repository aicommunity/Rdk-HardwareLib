#include "UNmsdkUartDeviceHubProtocolPlugin.h"

#include "USensorLabFrameDecoder.h"

#include <QRegularExpression>

namespace RDK {

void UNmsdkUartDeviceHubProtocolPlugin::negotiate(UArduinoPluginHost* host, int protocolVersion)
{
    IArduinoProtocolPlugin::negotiate(host, protocolVersion);
    if (host)
        host->enqueueCommand(QStringLiteral("PROTO 2"));
}

void UNmsdkUartDeviceHubProtocolPlugin::onHealthCheck(UArduinoPluginHost* host)
{
    if (host)
        host->enqueueCommand(QStringLiteral("PING"));
}

void UNmsdkUartDeviceHubProtocolPlugin::onBinaryFrame(UArduinoPluginHost* host, uint8_t type,
                                                      const QByteArray& payload)
{
    if (!host)
        return;
    host->appendFrameLog(QStringLiteral("uart hub type=0x%1 len=%2")
                             .arg(type, 2, 16, QChar('0'))
                             .arg(payload.size()));
    if (type == 0x7F) {
        host->setProtocolReady(true);
        host->publishNamedFloat(QStringLiteral("pong"), 1.f);
        return;
    }
    if (type == 0x60) {
        const QString line = QString::fromUtf8(payload).trimmed();
        host->appendFrameLog(QStringLiteral("RX: %1").arg(line));
        host->publishNamedString(QStringLiteral("last_line"), line);
        host->publishNamedFloat(QStringLiteral("last_line_len"),
                                static_cast<float>(line.size()));
        static const QRegularExpression gga(
            QStringLiteral(R"(\$GPGGA,[^,]*,([0-9.]+),([NS]),([0-9.]+),([EW]))"));
        const auto m = gga.match(line);
        if (m.hasMatch()) {
            host->publishNamedFloat(QStringLiteral("gps_lat"), m.captured(1).toFloat());
            host->publishNamedFloat(QStringLiteral("gps_lon"), m.captured(3).toFloat());
        }
        host->setProtocolReady(true);
        return;
    }
    if (type == 0x61) {
        const auto decoded = USensorLabFrameDecoder::decodeSensors(payload);
        if (decoded.ok) {
            if (decoded.paramCount >= 1)
                host->publishNamedFloat(QStringLiteral("gps_lat"), decoded.values[0]);
            if (decoded.paramCount >= 2)
                host->publishNamedFloat(QStringLiteral("gps_lon"), decoded.values[1]);
            host->setProtocolReady(true);
        }
    }
}

} // namespace RDK
