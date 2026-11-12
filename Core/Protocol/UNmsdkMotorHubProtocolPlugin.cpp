#include "UNmsdkMotorHubProtocolPlugin.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <cstring>

namespace RDK {

void UNmsdkMotorHubProtocolPlugin::negotiate(UArduinoPluginHost* host, int protocolVersion)
{
    IArduinoProtocolPlugin::negotiate(host, protocolVersion);
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
    } else if (type == 0x21 && payload.size() >= 8) {
        // V1 frames contain four pins per channel. V2 adds dir2 and enable and
        // uses 255 for an unassigned pin.
        const bool extended = payload.size() >= 12;
        const int stride = extended ? 6 : 4;
        const int dirIndex = 0;
        const int dir2Index = extended ? 1 : -1;
        const int pwmIndex = extended ? 2 : 1;
        const int enableIndex = extended ? 3 : -1;
        const int brakeIndex = extended ? 4 : 2;
        const int senseIndex = extended ? 5 : 3;
        const auto publishPin = [&](const QString& name, int index) {
            if (index < 0)
                return;
            const int raw = static_cast<uint8_t>(payload[index]);
            host->publishNamedFloat(name, raw == 255 ? -1.f : static_cast<float>(raw));
        };
        const auto publishChannel = [&](const QString& suffix, int base) {
            publishPin(QStringLiteral("pin_dir%1").arg(suffix), base + dirIndex);
            publishPin(QStringLiteral("pin_dir2%1").arg(suffix), dir2Index < 0 ? -1 : base + dir2Index);
            publishPin(QStringLiteral("pin_pwm%1").arg(suffix), base + pwmIndex);
            publishPin(QStringLiteral("pin_enable%1").arg(suffix), enableIndex < 0 ? -1 : base + enableIndex);
            publishPin(QStringLiteral("pin_brake%1").arg(suffix), base + brakeIndex);
            publishPin(QStringLiteral("pin_sense%1").arg(suffix), base + senseIndex);
        };
        publishChannel(QString(), 0);
        publishChannel(QStringLiteral("_b"), stride);
        host->publishNamedFloat(QStringLiteral("left_pin_dir"),
                                static_cast<float>(static_cast<uint8_t>(payload[0])));
        host->publishNamedFloat(QStringLiteral("left_pin_pwm"),
                                static_cast<float>(static_cast<uint8_t>(payload[pwmIndex])));
        host->publishNamedFloat(QStringLiteral("right_pin_dir"),
                                static_cast<float>(static_cast<uint8_t>(payload[stride])));
        host->publishNamedFloat(QStringLiteral("right_pin_pwm"),
                                static_cast<float>(static_cast<uint8_t>(payload[stride + pwmIndex])));
        host->setProtocolReady(true);
    } else if (type == 0x7F) {
        host->setProtocolReady(true);
        host->publishNamedFloat(QStringLiteral("pong"), 1.f);
    }
}

} // namespace RDK
