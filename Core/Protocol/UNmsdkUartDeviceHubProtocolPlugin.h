#ifndef UNMSDKUARTDEVICEHUBPROTOCOLPLUGIN_H
#define UNMSDKUARTDEVICEHUBPROTOCOLPLUGIN_H

#include "IArduinoProtocolPlugin.h"

namespace RDK {

class UNmsdkUartDeviceHubProtocolPlugin : public IArduinoProtocolPlugin {
public:
    QString id() const override { return QStringLiteral("nmsdk_uart_device_hub_v1"); }
    QStringList protocolIds() const override
    {
        return {QStringLiteral("nmsdk_uart_device_hub"), QStringLiteral("nmsdk_hub_uart_device_v1")};
    }
    QStringList knownCommands() const override
    {
        return {QStringLiteral("PROTO 2"), QStringLiteral("PING"), QStringLiteral("AT"),
                QStringLiteral("HMI TX hello"), QStringLiteral("BRIDGE ON")};
    }
    void onBinaryFrame(UArduinoPluginHost* host, uint8_t type, const QByteArray& payload) override;
    void negotiate(UArduinoPluginHost* host, int protocolVersion) override;
    void onHealthCheck(UArduinoPluginHost* host) override;
};

} // namespace RDK

#endif
