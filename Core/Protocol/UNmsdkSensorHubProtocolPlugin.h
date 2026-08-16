#ifndef UNMSDKSENSORHUBPROTOCOLPLUGIN_H
#define UNMSDKSENSORHUBPROTOCOLPLUGIN_H

#include "IArduinoProtocolPlugin.h"

namespace RDK {

/** Host plugin for catalog firmware `nmsdk_sensor_hub_v1` (framed v2, sensor frames). */
class UNmsdkSensorHubProtocolPlugin : public IArduinoProtocolPlugin {
public:
    QString id() const override { return QStringLiteral("nmsdk_sensor_hub_v1"); }
    QStringList protocolIds() const override
    {
        return {QStringLiteral("nmsdk_hub_v1"), QStringLiteral("nmsdk_sensor_hub")};
    }
    QStringList knownCommands() const override
    {
        return {QStringLiteral("START READING"), QStringLiteral("STOP READING"),
                QStringLiteral("GET STATUS"),    QStringLiteral("PROTO 2"),
                QStringLiteral("PING")};
    }
    void onBinaryFrame(UArduinoPluginHost* host, uint8_t type, const QByteArray& payload) override;
    void negotiate(UArduinoPluginHost* host, int protocolVersion) override;
    void onHealthCheck(UArduinoPluginHost* host) override;
};

} // namespace RDK

#endif
