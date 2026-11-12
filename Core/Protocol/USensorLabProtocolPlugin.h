#ifndef USENSORLABPROTOCOLPLUGIN_H
#define USENSORLABPROTOCOLPLUGIN_H

#include "IArduinoProtocolPlugin.h"

namespace RDK {

class USensorLabProtocolPlugin : public IArduinoProtocolPlugin {
public:
    QString id() const override { return QStringLiteral("sensor_lab_v1"); }
    QStringList protocolIds() const override
    {
        return {QStringLiteral("sensor_lab"), QStringLiteral("sensor_lab_v1")};
    }
    QStringList knownCommands() const override
    {
        return {QStringLiteral("START READING"), QStringLiteral("STOP READING"),
                QStringLiteral("ROTATE"),        QStringLiteral("STOP ROTATE"),
                QStringLiteral("GET STATUS"),    QStringLiteral("PROTO 2")};
    }
    void onBinaryFrame(UArduinoPluginHost* host, uint8_t type, const QByteArray& payload) override;
    void negotiate(UArduinoPluginHost* host, int protocolVersion) override;
};

} // namespace RDK

#endif
