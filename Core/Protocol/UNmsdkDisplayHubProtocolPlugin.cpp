#include "UNmsdkDisplayHubProtocolPlugin.h"

#include "USensorLabFrameDecoder.h"

namespace RDK {

void UNmsdkDisplayHubProtocolPlugin::negotiate(UArduinoPluginHost* host, int protocolVersion)
{
    IArduinoProtocolPlugin::negotiate(host, protocolVersion);
    if (host)
        host->enqueueCommand(QStringLiteral("PROTO 2"));
}

void UNmsdkDisplayHubProtocolPlugin::onHealthCheck(UArduinoPluginHost* host)
{
    if (host)
        host->enqueueCommand(QStringLiteral("PING"));
}

void UNmsdkDisplayHubProtocolPlugin::onBinaryFrame(UArduinoPluginHost* host, uint8_t type,
                                                   const QByteArray& payload)
{
    if (!host)
        return;
    host->appendFrameLog(QStringLiteral("display hub type=0x%1 len=%2")
                             .arg(type, 2, 16, QChar('0'))
                             .arg(payload.size()));
    if (type == 0x7F) {
        host->setProtocolReady(true);
        host->publishNamedFloat(QStringLiteral("pong"), 1.f);
        return;
    }
    if (type == 0x40) {
        const auto decoded = USensorLabFrameDecoder::decodeSensors(payload);
        if (!decoded.ok)
            return;
        if (decoded.paramCount >= 1)
            host->publishNamedFloat(QStringLiteral("rows"), decoded.values[0]);
        if (decoded.paramCount >= 2)
            host->publishNamedFloat(QStringLiteral("cols"), decoded.values[1]);
        if (decoded.paramCount >= 3)
            host->publishNamedFloat(QStringLiteral("driver_id"), decoded.values[2]);
        host->setProtocolReady(true);
    }
}

} // namespace RDK
