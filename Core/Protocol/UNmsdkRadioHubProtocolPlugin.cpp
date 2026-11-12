#include "UNmsdkRadioHubProtocolPlugin.h"

#include "USensorLabFrameDecoder.h"

namespace RDK {

void UNmsdkRadioHubProtocolPlugin::negotiate(UArduinoPluginHost* host, int protocolVersion)
{
    IArduinoProtocolPlugin::negotiate(host, protocolVersion);
}

void UNmsdkRadioHubProtocolPlugin::onHealthCheck(UArduinoPluginHost* host)
{
    if (host)
        host->enqueueCommand(QStringLiteral("PING"));
}

void UNmsdkRadioHubProtocolPlugin::onBinaryFrame(UArduinoPluginHost* host, uint8_t type,
                                                 const QByteArray& payload)
{
    if (!host)
        return;
    host->appendFrameLog(QStringLiteral("radio hub type=0x%1 len=%2")
                             .arg(type, 2, 16, QChar('0'))
                             .arg(payload.size()));
    if (type == 0x7F) {
        host->setProtocolReady(true);
        host->publishNamedFloat(QStringLiteral("pong"), 1.f);
        return;
    }
    const auto decoded = USensorLabFrameDecoder::decodeSensors(payload);
    if (!decoded.ok && type != 0x51)
        return;
    if (type == 0x50) {
        if (decoded.paramCount >= 1)
            host->publishNamedFloat(QStringLiteral("rx_len"), decoded.values[0]);
        if (decoded.paramCount >= 2)
            host->publishNamedFloat(QStringLiteral("rssi"), decoded.values[1]);
        host->setProtocolReady(true);
    } else if (type == 0x51) {
        // uid as up to 4 floats of byte packs, or single float hash; publish first float as uid
        if (decoded.ok && decoded.paramCount >= 1)
            host->publishNamedFloat(QStringLiteral("uid"), decoded.values[0]);
        else if (!payload.isEmpty())
            host->publishNamedFloat(QStringLiteral("uid"),
                                    static_cast<float>(static_cast<uint8_t>(payload[0])));
        host->setProtocolReady(true);
    } else if (type == 0x52) {
        if (decoded.paramCount >= 1)
            host->publishNamedFloat(QStringLiteral("rssi"), decoded.values[0]);
        if (decoded.paramCount >= 2)
            host->publishNamedFloat(QStringLiteral("ip0"), decoded.values[1]);
        if (decoded.paramCount >= 3)
            host->publishNamedFloat(QStringLiteral("ip1"), decoded.values[2]);
        if (decoded.paramCount >= 4)
            host->publishNamedFloat(QStringLiteral("ip2"), decoded.values[3]);
        if (decoded.paramCount >= 5)
            host->publishNamedFloat(QStringLiteral("ip3"), decoded.values[4]);
        host->setProtocolReady(true);
    }
}

} // namespace RDK
