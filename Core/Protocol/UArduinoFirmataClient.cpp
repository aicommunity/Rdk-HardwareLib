#include "UArduinoFirmataClient.h"

#include "../Transport/UArduinoPinMap.h"
#include "../Transport/UArduinoSerialSession.h"

namespace RDK {

namespace {
constexpr uint8_t kDigitalMessage = 0x90;
constexpr uint8_t kAnalogMessage = 0xE0;
constexpr uint8_t kSetPinMode = 0xF4;
constexpr uint8_t kSetDigitalPinValue = 0xF5;
constexpr uint8_t kReportAnalog = 0xC0;
constexpr uint8_t kReportDigital = 0xD0;
constexpr uint8_t kStartSysex = 0xF0;
constexpr uint8_t kEndSysex = 0xF7;
constexpr uint8_t kFirmwareVersion = 0x79;
constexpr uint8_t kCapabilityResponse = 0x6C;
constexpr uint8_t kCapabilityQuery = 0x6B;
constexpr uint8_t kAnalogMappingQuery = 0x69;
constexpr uint8_t kAnalogMappingResponse = 0x6A;
constexpr uint8_t kPinStateQuery = 0x6D;
constexpr uint8_t kPinStateResponse = 0x6E;
constexpr uint8_t kSamplingInterval = 0x7A;
constexpr uint8_t kExtendedAnalog = 0x6F;
constexpr uint8_t kServoConfig = 0x70;
constexpr uint8_t kI2cConfig = 0x78;
constexpr uint8_t kI2cWrite = 0x76;
constexpr uint8_t kI2cRequest = 0x74;
constexpr uint8_t kI2cReply = 0x77;
constexpr uint8_t kPinModeIgnore = 0x7F;
}

void UArduinoFirmataClient::reset()
{
    HandshakeReady = false;
    HandshakeStage = 0;
    PinCount = 0;
    FirmwareVersion.clear();
    SysexBuffer.clear();
    LastI2cReadData.clear();
    InSysex = false;
    AnalogValues.clear();
    AnalogChannelByPin.clear();
    ChannelToPin.clear();
    PortDigitalMask.clear();
    PinModeByPin.clear();
    PinDeviceModeByPin.clear();
    LastWrittenBytes.clear();
    GotFirmware = false;
    GotCapability = false;
    GotAnalogMapping = false;
    HandshakeSession = nullptr;
}

void UArduinoFirmataClient::updateHandshakeStage()
{
    if (HandshakeReady) {
        HandshakeStage = 4;
        return;
    }
    if (GotAnalogMapping)
        HandshakeStage = 3;
    else if (GotCapability)
        HandshakeStage = 2;
    else if (GotFirmware)
        HandshakeStage = 1;
    else
        HandshakeStage = 0;
}

void UArduinoFirmataClient::ensurePortMaskSize(int port)
{
    while (PortDigitalMask.size() <= port)
        PortDigitalMask.append(0);
}

void UArduinoFirmataClient::writeBytes(UArduinoSerialSession* session, const QByteArray& bytes)
{
    if (bytes.isEmpty())
        return;
    LastWrittenBytes = bytes;
    if (session)
        session->write(bytes);
    if (OnBytesWrittenCallback)
        OnBytesWrittenCallback(bytes);
}

void UArduinoFirmataClient::writeSysex(UArduinoSerialSession* session, const QByteArray& payload)
{
    QByteArray msg;
    msg.append(char(kStartSysex));
    msg.append(payload);
    msg.append(char(kEndSysex));
    writeBytes(session, msg);
}

void UArduinoFirmataClient::advanceHandshake()
{
    if (!HandshakeSession)
        return;
    if (!GotFirmware) {
        writeSysex(HandshakeSession, QByteArray(1, char(kFirmwareVersion)));
        return;
    }
    if (!GotCapability) {
        writeSysex(HandshakeSession, QByteArray(1, char(kCapabilityQuery)));
        return;
    }
    if (!GotAnalogMapping)
        queryAnalogMapping(HandshakeSession);
}

bool UArduinoFirmataClient::startHandshake(UArduinoSerialSession* session, int board_profile)
{
    reset();
    BoardProfileValue = board_profile;
    HandshakeSession = session;
    PortDigitalMask.resize(UArduinoPinMap::digitalPortCount(board_profile));
    PortDigitalMask.fill(0);

    advanceHandshake();
    return true;
}

bool UArduinoFirmataClient::queryAnalogMapping(UArduinoSerialSession* session)
{
    writeSysex(session, QByteArray(1, char(kAnalogMappingQuery)));
    return true;
}

int UArduinoFirmataClient::analogChannelForPin(int firmata_pin) const
{
    if (AnalogChannelByPin.contains(firmata_pin))
        return AnalogChannelByPin.value(firmata_pin);
    const int base = UArduinoPinMap::analogBase(BoardProfileValue);
    if (firmata_pin >= base)
        return firmata_pin - base;
    return -1;
}

int UArduinoFirmataClient::analogValueForChannel(int channel) const
{
    return AnalogValues.value(channel, 0);
}

int UArduinoFirmataClient::analogValue(int firmata_pin) const
{
    const int channel = analogChannelForPin(firmata_pin);
    if (channel >= 0)
        return analogValueForChannel(channel);
    return AnalogValues.value(firmata_pin, 0);
}

int UArduinoFirmataClient::digitalValue(int firmata_pin) const
{
    const int port = UArduinoPinMap::portForPin(firmata_pin);
    const int bit = UArduinoPinMap::bitIndexInPort(firmata_pin);
    if (port < 0 || port >= PortDigitalMask.size())
        return 0;
    return (PortDigitalMask[port] >> bit) & 1;
}

int UArduinoFirmataClient::deviceModeForPin(int firmata_pin) const
{
    return PinDeviceModeByPin.value(firmata_pin, -1);
}

bool UArduinoFirmataClient::setPinMode(UArduinoSerialSession* session, int pin, int mode)
{
    QByteArray msg;
    msg.append(char(kSetPinMode));
    msg.append(char(pin & 0x7F));
    msg.append(char(mode & 0x7F));
    PinModeByPin[pin] = mode;
    writeBytes(session, msg);
    return true;
}

bool UArduinoFirmataClient::setDigitalPinValue(UArduinoSerialSession* session, int pin, int value)
{
    const int port = UArduinoPinMap::portForPin(pin);
    const int bit = UArduinoPinMap::bitIndexInPort(pin);
    ensurePortMaskSize(port);
    if (value)
        PortDigitalMask[port] |= (1 << bit);
    else
        PortDigitalMask[port] &= ~(1 << bit);

    QByteArray msg;
    msg.append(char(kSetDigitalPinValue));
    msg.append(char(pin & 0x7F));
    msg.append(char(value ? 1 : 0));
    writeBytes(session, msg);
    return true;
}

bool UArduinoFirmataClient::digitalWrite(UArduinoSerialSession* session, int pin, int value)
{
    return setDigitalPinValue(session, pin, value);
}

bool UArduinoFirmataClient::reportAnalog(UArduinoSerialSession* session, int firmata_pin, int enable)
{
    int channel = analogChannelForPin(firmata_pin);
    if (channel < 0)
        channel = firmata_pin;
    QByteArray msg;
    msg.append(char(kReportAnalog | (channel & 0x0F)));
    msg.append(char(enable ? 1 : 0));
    writeBytes(session, msg);
    return true;
}

bool UArduinoFirmataClient::reportDigitalPort(UArduinoSerialSession* session, int port, int enable)
{
    QByteArray msg;
    msg.append(char(kReportDigital | (port & 0x0F)));
    msg.append(char(enable ? 1 : 0));
    writeBytes(session, msg);
    return true;
}

bool UArduinoFirmataClient::setSamplingInterval(UArduinoSerialSession* session, int interval_ms)
{
    const int ms = qBound(1, interval_ms, 32767);
    QByteArray payload;
    payload.append(char(kSamplingInterval));
    payload.append(char(ms & 0x7F));
    payload.append(char((ms >> 7) & 0x7F));
    writeSysex(session, payload);
    return true;
}

bool UArduinoFirmataClient::queryPinState(UArduinoSerialSession* session, int pin)
{
    QByteArray payload;
    payload.append(char(kPinStateQuery));
    payload.append(char(pin & 0x7F));
    writeSysex(session, payload);
    return true;
}

bool UArduinoFirmataClient::extendedAnalogWrite(UArduinoSerialSession* session, int pin, int value)
{
    const int v = qBound(0, value, 65535);
    QByteArray payload;
    payload.append(char(kExtendedAnalog));
    payload.append(char(pin & 0x7F));
    payload.append(char(v & 0x7F));
    payload.append(char((v >> 7) & 0x7F));
    writeSysex(session, payload);
    return true;
}

bool UArduinoFirmataClient::configureServo(UArduinoSerialSession* session, int pin, int min_pulse,
                                           int max_pulse)
{
    QByteArray payload;
    payload.append(char(kServoConfig));
    payload.append(char(pin & 0x7F));
    payload.append(char((min_pulse >> 7) & 0x7F));
    payload.append(char(min_pulse & 0x7F));
    payload.append(char((max_pulse >> 7) & 0x7F));
    payload.append(char(max_pulse & 0x7F));
    writeSysex(session, payload);
    return true;
}

bool UArduinoFirmataClient::servoWrite(UArduinoSerialSession* session, int pin, int angle)
{
    return extendedAnalogWrite(session, pin, qBound(0, angle, 180));
}

bool UArduinoFirmataClient::i2cConfig(UArduinoSerialSession* session, int delay)
{
    QByteArray payload;
    payload.append(char(kI2cConfig));
    payload.append(char(delay & 0x7F));
    payload.append(char((delay >> 7) & 0x7F));
    writeSysex(session, payload);
    return true;
}

bool UArduinoFirmataClient::i2cWrite(UArduinoSerialSession* session, int address, const QByteArray& data)
{
    QByteArray payload;
    payload.append(char(kI2cWrite));
    payload.append(char(address & 0x7F));
    for (char b : data)
        payload.append(b);
    writeSysex(session, payload);
    return true;
}

bool UArduinoFirmataClient::i2cReadRequest(UArduinoSerialSession* session, int address, int bytes)
{
    QByteArray payload;
    payload.append(char(kI2cRequest));
    payload.append(char(address & 0x7F));
    payload.append(char(bytes & 0x7F));
    payload.append(char((bytes >> 7) & 0x7F));
    writeSysex(session, payload);
    return true;
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
        int pin = 0;
        PinCount = 0;
        int first_mode = -1;
        for (int i = 1; i < sysex.size();) {
            const uint8_t b = static_cast<uint8_t>(sysex[i]);
            if (b == kPinModeIgnore) {
                if (first_mode >= 0) {
                    PinDeviceModeByPin[pin] = first_mode;
                    if (!PinModeByPin.contains(pin))
                        PinModeByPin[pin] = first_mode;
                }
                ++pin;
                PinCount = qMax(PinCount, pin);
                first_mode = -1;
                ++i;
                continue;
            }
            if (first_mode < 0)
                first_mode = b;
            ++i;
        }
        if (PinCount == 0)
            PinCount = UArduinoPinMap::maxFirmataPin(BoardProfileValue) + 1;
        GotCapability = true;
    } else if (cmd == kAnalogMappingResponse) {
        AnalogChannelByPin.clear();
        ChannelToPin.clear();
        for (int pin = 0; pin < sysex.size() - 1; ++pin) {
            const uint8_t channel = static_cast<uint8_t>(sysex[pin + 1]);
            if (channel != 127) {
                AnalogChannelByPin[pin] = channel;
                ChannelToPin[channel] = pin;
            }
        }
        GotAnalogMapping = true;
    } else if (cmd == kPinStateResponse && sysex.size() >= 3) {
        const int pin = static_cast<uint8_t>(sysex[1]);
        const int mode = static_cast<uint8_t>(sysex[2]);
        PinModeByPin[pin] = mode;
        PinDeviceModeByPin[pin] = mode;
        int value = 0;
        if (sysex.size() >= 4)
            value = static_cast<uint8_t>(sysex[3]) & 0x7F;
        if (sysex.size() >= 5)
            value |= (static_cast<uint8_t>(sysex[4]) & 0x7F) << 7;
        if (mode == 2 || mode == 3) {
            const int channel = analogChannelForPin(pin);
            if (channel >= 0)
                AnalogValues[channel] = value;
        } else {
            const int port = UArduinoPinMap::portForPin(pin);
            const int bit = UArduinoPinMap::bitIndexInPort(pin);
            ensurePortMaskSize(port);
            if (value)
                PortDigitalMask[port] |= (1 << bit);
            else
                PortDigitalMask[port] &= ~(1 << bit);
        }
    } else if (cmd == kI2cReply) {
        LastI2cReadData = sysex.mid(1);
    }

    advanceHandshake();

    if (GotFirmware && GotCapability && GotAnalogMapping && !HandshakeReady) {
        HandshakeReady = true;
        HandshakeSession = nullptr;
        if (OnReadyCallback)
            OnReadyCallback();
    }
    updateHandshakeStage();
}

void UArduinoFirmataClient::handleMessage(uint8_t status, uint8_t data1, uint8_t data2)
{
    const uint8_t cmd = status & 0xF0;
    if (cmd == kAnalogMessage) {
        const int channel = status & 0x0F;
        const int value = (data1 & 0x7F) | ((data2 & 0x7F) << 7);
        AnalogValues[channel] = value;
    } else if (cmd == kDigitalMessage) {
        const int port = status & 0x0F;
        const int mask = (data1 & 0x7F) | ((data2 & 0x7F) << 7);
        ensurePortMaskSize(port);
        PortDigitalMask[port] = mask;
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
