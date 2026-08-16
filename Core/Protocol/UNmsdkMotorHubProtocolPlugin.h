#ifndef UNMSDKMOTORHUBPROTOCOLPLUGIN_H
#define UNMSDKMOTORHUBPROTOCOLPLUGIN_H

#include "IArduinoProtocolPlugin.h"

namespace RDK {

/** Host plugin for catalog firmware `nmsdk_motor_hub_v1` (framed v2 motor status). */
class UNmsdkMotorHubProtocolPlugin : public IArduinoProtocolPlugin {
public:
    QString id() const override { return QStringLiteral("nmsdk_motor_hub_v1"); }
    QStringList protocolIds() const override
    {
        return {QStringLiteral("nmsdk_motor_hub"), QStringLiteral("nmsdk_hub_motor_v1")};
    }
    QStringList knownCommands() const override
    {
        return {QStringLiteral("PROTO 2"), QStringLiteral("PING"), QStringLiteral("MOTOR STOP"),
                QStringLiteral("GET STATUS")};
    }
    void onBinaryFrame(UArduinoPluginHost* host, uint8_t type, const QByteArray& payload) override;
    void negotiate(UArduinoPluginHost* host, int protocolVersion) override;
    void onHealthCheck(UArduinoPluginHost* host) override;
};

} // namespace RDK

#endif
