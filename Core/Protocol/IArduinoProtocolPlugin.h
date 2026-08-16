#ifndef IARDUINOPROTOCOLPLUGIN_H
#define IARDUINOPROTOCOLPLUGIN_H

#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QVector>

namespace RDK {

class UArduinoPluginHost {
public:
    virtual ~UArduinoPluginHost() = default;
    virtual void enqueueCommand(const QString& line) = 0;
    virtual int boardProfile() const = 0;
    virtual int protocolVersion() const = 0;
    virtual void setProtocolReady(bool ready) = 0;
    virtual void setLastError(const QString& error) = 0;
    virtual void publishSensorMatrixRow(const QVector<double>& row) {}
    virtual void publishPinStatusJson(const QString& json) {}
    virtual void publishNamedFloat(const QString& key, float value) {}
    virtual void appendFrameLog(const QString& line) {}
};

class IArduinoProtocolPlugin {
public:
    virtual ~IArduinoProtocolPlugin() = default;
    virtual QString id() const = 0;
    virtual QStringList protocolIds() const = 0;

    virtual void onConnected(UArduinoPluginHost* host) { Q_UNUSED(host); }
    virtual void onDisconnected(UArduinoPluginHost* host) { Q_UNUSED(host); }
    virtual void onHealthCheck(UArduinoPluginHost* host)
    {
        if (host)
            host->enqueueCommand(QStringLiteral("GET STATUS"));
    }

    virtual void onBinaryFrame(UArduinoPluginHost* host, uint8_t type, const QByteArray& payload) = 0;
    virtual void onRawRx(UArduinoPluginHost* host, const QByteArray& chunk) { Q_UNUSED(host); Q_UNUSED(chunk); }

    virtual QStringList knownCommands() const { return {}; }
    virtual void negotiate(UArduinoPluginHost* host, int protocolVersion)
    {
        if (!host)
            return;
        if (protocolVersion >= 2)
            host->enqueueCommand(QStringLiteral("PROTO 2"));
        else
            host->setProtocolReady(true);
    }
};

} // namespace RDK

#endif
