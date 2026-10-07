#ifndef UNMSDKPIXELHUBPROTOCOLPLUGIN_H
#define UNMSDKPIXELHUBPROTOCOLPLUGIN_H

#include "IArduinoProtocolPlugin.h"

namespace RDK {

class UNmsdkPixelHubProtocolPlugin : public IArduinoProtocolPlugin {
public:
    QString id() const override { return QStringLiteral("nmsdk_pixel_hub_v1"); }
    QStringList protocolIds() const override
    {
        return {QStringLiteral("nmsdk_pixel_hub"), QStringLiteral("nmsdk_hub_pixel_v1")};
    }
    QStringList knownCommands() const override
    {
        return {QStringLiteral("PROTO 2"), QStringLiteral("PING"), QStringLiteral("LED FILL"),
                QStringLiteral("LED SHOW"), QStringLiteral("MATRIX CLEAR"),
                QStringLiteral("MATRIX TEXT hi"), QStringLiteral("TFT FILL 0"),
                QStringLiteral("TFT TEXT hi"), QStringLiteral("EPD CLEAR")};
    }
    void onBinaryFrame(UArduinoPluginHost* host, uint8_t type, const QByteArray& payload) override;
    void negotiate(UArduinoPluginHost* host, int protocolVersion) override;
    void onHealthCheck(UArduinoPluginHost* host) override;
};

} // namespace RDK

#endif
