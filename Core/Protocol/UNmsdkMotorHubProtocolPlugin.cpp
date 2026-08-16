#include "UNmsdkMotorHubProtocolPlugin.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <cstring>

namespace RDK {

void UNmsdkMotorHubProtocolPlugin::negotiate(UArduinoPluginHost* host, int protocolVersion)
{
    IArduinoProtocolPlugin::negotiate(host, protocolVersion);
    if (host)
        host->enqueueCommand(QStringLiteral("PROTO 2"));
}

void UNmsdkMotorHubProtocolPlugin::onHealthCheck(UArduinoPluginHost* host)
{
    if (host)
        host->enqueueCommand(QStringLiteral("PING"));
}

void UNmsdkMotorHubProtocolPlugin::onBinaryFrame(UArduinoPluginHost* host, uint8_t type,
                                                 const QByteArray& payload)
{
    if (!host)
        return;
    host->appendFrameLog(QStringLiteral("motor hub type=0x%1 len=%2")
                             .arg(type, 2, 16, QChar('0'))
                             .arg(payload.size()));
    // 0x20: channel(u8) pwm(u8) dir(u8) sense(float LE)
    if (type == 0x20 && payload.size() >= 7) {
        const uint8_t channel = static_cast<uint8_t>(payload[0]);
        const uint8_t pwm = static_cast<uint8_t>(payload[1]);
        const uint8_t dir = static_cast<uint8_t>(payload[2]);
        float sense = 0.f;
        std::memcpy(&sense, payload.constData() + 3, sizeof(float));
        host->publishNamedFloat(QStringLiteral("ch"), static_cast<float>(channel));
        host->publishNamedFloat(QStringLiteral("pwm"), static_cast<float>(pwm));
        host->publishNamedFloat(QStringLiteral("dir"), static_cast<float>(dir));
        host->publishNamedFloat(QStringLiteral("sense"), sense);
        QVector<double> row;
        row << 4.0 << channel << pwm << dir << sense;
        host->publishSensorMatrixRow(row);
        host->setProtocolReady(true);
    } else if (type == 0x7F) {
        host->setProtocolReady(true);
        host->publishNamedFloat(QStringLiteral("pong"), 1.f);
    }
}

} // namespace RDK
