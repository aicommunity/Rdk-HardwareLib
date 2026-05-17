#include "UArduinoFirmataClient.h"

#include "../Transport/UArduinoSerialSession.h"

namespace RDK {

namespace {
constexpr uint8_t kDigitalMessage = 0x90;
constexpr uint8_t kAnalogMessage = 0xE0;
constexpr uint8_t kSetPinMode = 0xF4;
constexpr uint8_t kReportAnalog = 0xC0;
constexpr uint8_t kStartSysex = 0xF0;
constexpr uint8_t kEndSysex = 0xF7;
constexpr uint8_t kFirmwareVersion = 0x79;
constexpr uint8_t kCapabilityResponse = 0x6C;
constexpr uint8_t kCapabilityQuery = 0x6B;
constexpr uint8_t kAnalogMappingQuery = 0x69;
constexpr uint8_t kAnalogMappingResponse = 0x6A;
}

void UArduinoFirmataClient::reset()
{
    HandshakeReady = false;
    FirmwareVersion.clear();
    SysexBuffer.clear();
    InSysex = false;
    AnalogValues.clear();
    AnalogChannelByPin.clear();
    GotFirmware = false;
    GotCapability = false;
    GotAnalogMapping = false;
}

void UArduinoFirmataClient::writeBytes(UArduinoSerialSession* session, const QByteArray& bytes)
{
    if (session)
        session->write(bytes);
}

bool UArduinoFirmataClient::startHandshake(UArduinoSerialSession* session)
{
    reset();
    QByteArray query;
    query.append(char(kStartSysex));
    query.append(char(kFirmwareVersion));
    query.append(char(kEndSysex));
    writeBytes(session, query);

    QByteArray cap;
    cap.append(char(kStartSysex));
    cap.append(char(kCapabilityQuery));
    cap.append(char(kEndSysex));
    writeBytes(session, cap);
    queryAnalogMapping(session);
    return true;
}

bool UArduinoFirmataClient::queryAnalogMapping(UArduinoSerialSession* session)
{
    QByteArray msg;
    msg.append(char(kStartSysex));
    msg.append(char(kAnalogMappingQuery));
    msg.append(char(kEndSysex));
    writeBytes(session, msg);
    return true;
}

int UArduinoFirmataClient::analogChannelForPin(int pin) const
{
    return AnalogChannelByPin.value(pin, pin);
}

bool UArduinoFirmataClient::setPinMode(UArduinoSerialSession* session, int pin, int mode)
{
    QByteArray msg;
    msg.append(char(kSetPinMode));
    msg.append(char(pin & 0x7F));
    msg.append(char(mode & 0x7F));
    writeBytes(session, msg);
    return true;
}

bool UArduinoFirmataClient::digitalWrite(UArduinoSerialSession* session, int pin, int value)
{
    const int port = pin / 8;
    const int bit = pin % 8;
    QByteArray msg;
    msg.append(char(kDigitalMessage | (port & 0x0F)));
    msg.append(char(value ? (1 << bit) : 0));
    msg.append(char(0));
    writeBytes(session, msg);
    return true;
}

bool UArduinoFirmataClient::reportAnalog(UArduinoSerialSession* session, int pin, int enable)
{
    QByteArray msg;
    msg.append(char(kReportAnalog | (pin & 0x0F)));
    msg.append(char(enable ? 1 : 0));
    writeBytes(session, msg);
    return true;
}

int UArduinoFirmataClient::analogValue(int pin) const
{
    return AnalogValues.value(pin, 0);
}

void UArduinoFirmataClient::handleSysex(const QByteArray& sysex)
{
    if (sysex.isEmpty())
        return;
    const uint8_t cmd = static_cast<uint8_t>(sysex[0]);
    if (cmd == kFirmwareVersion && sysex.size() >= 3) {
        FirmwareVersion = QStringLiteral("%1.%2")
                              .arg(static_cast<uint8_t>(sysex[1]))
                              .arg(static_cast<uint8_t>(sysex[2]));
        GotFirmware = true;
    } else if (cmd == kCapabilityResponse) {
        GotCapability = true;
    } else if (cmd == kAnalogMappingResponse) {
        AnalogChannelByPin.clear();
        for (int pin = 0; pin < sysex.size(); ++pin) {
            const uint8_t channel = static_cast<uint8_t>(sysex[pin]);
            if (channel != 127)
                AnalogChannelByPin[pin] = channel;
        }
        GotAnalogMapping = true;
    }
    if (GotFirmware && GotCapability && GotAnalogMapping && !HandshakeReady) {
        HandshakeReady = true;
        if (OnReadyCallback)
            OnReadyCallback();
    }
}

void UArduinoFirmataClient::handleMessage(uint8_t status, uint8_t data1, uint8_t data2)
{
    const uint8_t cmd = status & 0xF0;
    if (cmd == kAnalogMessage) {
        const int pin = status & 0x0F;
        const int value = (data1 & 0x7F) | ((data2 & 0x7F) << 7);
        AnalogValues[pin] = value;
    }
}

void UArduinoFirmataClient::processIncoming(const QByteArray& data)
{
    for (int i = 0; i < data.size(); ++i) {
        const uint8_t b = static_cast<uint8_t>(data[i]);
        if (b == kStartSysex) {
            InSysex = true;
            SysexBuffer.clear();
            continue;
        }
        if (InSysex) {
            if (b == kEndSysex) {
                InSysex = false;
                handleSysex(SysexBuffer);
            } else {
                SysexBuffer.append(char(b));
            }
            continue;
        }
        if (b & 0x80) {
            const uint8_t status = b;
            if (i + 2 < data.size()) {
                handleMessage(status, static_cast<uint8_t>(data[i + 1]),
                              static_cast<uint8_t>(data[i + 2]));
                i += 2;
            }
        }
    }
}

} // namespace RDK
