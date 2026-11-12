#ifndef UNMSDKDISPLAYHUBPROTOCOLPLUGIN_H
#define UNMSDKDISPLAYHUBPROTOCOLPLUGIN_H

#include "IArduinoProtocolPlugin.h"

namespace RDK {

class UNmsdkDisplayHubProtocolPlugin : public IArduinoProtocolPlugin {
public:
    QString id() const override { return QStringLiteral("nmsdk_display_hub_v1"); }
    QStringList protocolIds() const override
    {
        return {QStringLiteral("nmsdk_display_hub"), QStringLiteral("nmsdk_hub_display_v1")};
    }
    QStringList knownCommands() const override
    {
        return {QStringLiteral("PROTO 2"), QStringLiteral("PING"), QStringLiteral("CLEAR"),
                QStringLiteral("PRINT 0 0 hello"), QStringLiteral("OLED CLEAR"),
                QStringLiteral("OLED PRINT hello")};
    }
    void onBinaryFrame(UArduinoPluginHost* host, uint8_t type, const QByteArray& payload) override;
    void negotiate(UArduinoPluginHost* host, int protocolVersion) override;
    void onHealthCheck(UArduinoPluginHost* host) override;
};

} // namespace RDK

#endif
