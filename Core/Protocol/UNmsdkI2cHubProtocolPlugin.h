#ifndef UNMSDKI2CHUBPROTOCOLPLUGIN_H
#define UNMSDKI2CHUBPROTOCOLPLUGIN_H

#include "IArduinoProtocolPlugin.h"

namespace RDK {

/** Host plugin for catalog firmware `nmsdk_i2c_hub_v1` (BME280 / Tier C). */
class UNmsdkI2cHubProtocolPlugin : public IArduinoProtocolPlugin {
public:
    QString id() const override { return QStringLiteral("nmsdk_i2c_hub_v1"); }
    QStringList protocolIds() const override
    {
        return {QStringLiteral("nmsdk_i2c_hub"), QStringLiteral("nmsdk_hub_i2c_v1")};
    }
    QStringList knownCommands() const override
    {
        return {QStringLiteral("PROTO 2"), QStringLiteral("PING"), QStringLiteral("START READING"),
                QStringLiteral("STOP READING"), QStringLiteral("SET DELAY 500")};
    }
    void onBinaryFrame(UArduinoPluginHost* host, uint8_t type, const QByteArray& payload) override;
    void negotiate(UArduinoPluginHost* host, int protocolVersion) override;
    void onHealthCheck(UArduinoPluginHost* host) override;
};

} // namespace RDK

#endif
