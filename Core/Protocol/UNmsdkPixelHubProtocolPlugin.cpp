#include "UNmsdkPixelHubProtocolPlugin.h"

#include "USensorLabFrameDecoder.h"

namespace RDK {

void UNmsdkPixelHubProtocolPlugin::negotiate(UArduinoPluginHost* host, int protocolVersion)
{
    IArduinoProtocolPlugin::negotiate(host, protocolVersion);
}

void UNmsdkPixelHubProtocolPlugin::onHealthCheck(UArduinoPluginHost* host)
{
    if (host)
        host->enqueueCommand(QStringLiteral("PING"));
}

void UNmsdkPixelHubProtocolPlugin::onBinaryFrame(UArduinoPluginHost* host, uint8_t type,
                                                 const QByteArray& payload)
{
    if (!host)
        return;
    host->appendFrameLog(QStringLiteral("pixel hub type=0x%1 len=%2")
                             .arg(type, 2, 16, QChar('0'))
                             .arg(payload.size()));
    if (type == 0x7F) {
        host->setProtocolReady(true);
        host->publishNamedFloat(QStringLiteral("pong"), 1.f);
        return;
    }
    if (type == 0x41) {
        const auto decoded = USensorLabFrameDecoder::decodeSensors(payload);
        if (!decoded.ok)
            return;
        if (decoded.paramCount >= 1)
            host->publishNamedFloat(QStringLiteral("led_count"), decoded.values[0]);
        if (decoded.paramCount >= 2)
            host->publishNamedFloat(QStringLiteral("last_ack"), decoded.values[1]);
        host->setProtocolReady(true);
    }
}

} // namespace RDK
