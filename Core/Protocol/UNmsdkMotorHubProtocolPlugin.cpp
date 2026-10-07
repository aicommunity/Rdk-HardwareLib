#include "UNmsdkMotorHubProtocolPlugin.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <cstring>

namespace RDK {

void UNmsdkMotorHubProtocolPlugin::negotiate(UArduinoPluginHost* host, int protocolVersion)
{
    IArduinoProtocolPlugin::negotiate(host, protocolVersion);
    if (host) {
        host->enqueueCommand(QStringLiteral("PROTO 2"));
        host->enqueueCommand(QStringLiteral("WATCHDOG 2000"));
    }
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
    // 0x20: channel(u8) pwm(u8) dir(u8) sense(float LE) — channel 0=A/left, 1=B/right
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
        if (channel == 0) {
            host->publishNamedFloat(QStringLiteral("left_pwm"), static_cast<float>(pwm));
            host->publishNamedFloat(QStringLiteral("left_dir"), static_cast<float>(dir));
            host->publishNamedFloat(QStringLiteral("left_sense"), sense);
        } else if (channel == 1) {
            host->publishNamedFloat(QStringLiteral("right_pwm"), static_cast<float>(pwm));
            host->publishNamedFloat(QStringLiteral("right_dir"), static_cast<float>(dir));
            host->publishNamedFloat(QStringLiteral("right_sense"), sense);
        }
        QVector<double> row;
        row << 4.0 << channel << pwm << dir << sense;
        host->publishSensorMatrixRow(row);
        host->setProtocolReady(true);
    } else if (type == 0x21 && payload.size() >= 4) {
        host->publishNamedFloat(QStringLiteral("pin_dir"), static_cast<float>((uint8_t)payload[0]));
        host->publishNamedFloat(QStringLiteral("pin_pwm"), static_cast<float>((uint8_t)payload[1]));
        host->publishNamedFloat(QStringLiteral("pin_brake"), static_cast<float>((uint8_t)payload[2]));
        host->publishNamedFloat(QStringLiteral("pin_sense"), static_cast<float>((uint8_t)payload[3]));
        if (payload.size() >= 8) {
            host->publishNamedFloat(QStringLiteral("pin_dir_b"),
                                    static_cast<float>((uint8_t)payload[4]));
            host->publishNamedFloat(QStringLiteral("pin_pwm_b"),
                                    static_cast<float>((uint8_t)payload[5]));
            host->publishNamedFloat(QStringLiteral("pin_brake_b"),
                                    static_cast<float>((uint8_t)payload[6]));
            host->publishNamedFloat(QStringLiteral("pin_sense_b"),
                                    static_cast<float>((uint8_t)payload[7]));
            host->publishNamedFloat(QStringLiteral("left_pin_dir"),
                                    static_cast<float>((uint8_t)payload[0]));
            host->publishNamedFloat(QStringLiteral("left_pin_pwm"),
                                    static_cast<float>((uint8_t)payload[1]));
            host->publishNamedFloat(QStringLiteral("right_pin_dir"),
                                    static_cast<float>((uint8_t)payload[4]));
            host->publishNamedFloat(QStringLiteral("right_pin_pwm"),
                                    static_cast<float>((uint8_t)payload[5]));
        }
        host->setProtocolReady(true);
    } else if (type == 0x7F) {
        host->setProtocolReady(true);
        host->publishNamedFloat(QStringLiteral("pong"), 1.f);
    }
}

} // namespace RDK
