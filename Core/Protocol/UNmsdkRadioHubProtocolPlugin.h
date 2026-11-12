#ifndef UNMSDKRADIOHUBPROTOCOLPLUGIN_H
#define UNMSDKRADIOHUBPROTOCOLPLUGIN_H

#include "IArduinoProtocolPlugin.h"

namespace RDK {

class UNmsdkRadioHubProtocolPlugin : public IArduinoProtocolPlugin {
public:
    QString id() const override { return QStringLiteral("nmsdk_radio_hub_v1"); }
    QStringList protocolIds() const override
    {
        return {QStringLiteral("nmsdk_radio_hub"), QStringLiteral("nmsdk_hub_radio_v1")};
    }
    QStringList knownCommands() const override
    {
        return {QStringLiteral("PROTO 2"), QStringLiteral("PING"),
                QStringLiteral("RADIO SEND 010203")};
    }
    void onBinaryFrame(UArduinoPluginHost* host, uint8_t type, const QByteArray& payload) override;
    void negotiate(UArduinoPluginHost* host, int protocolVersion) override;
    void onHealthCheck(UArduinoPluginHost* host) override;
};

} // namespace RDK

#endif
