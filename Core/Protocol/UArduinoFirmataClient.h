#ifndef UARDUINOFIRMATACLIENT_H
#define UARDUINOFIRMATACLIENT_H

#include <QByteArray>
#include <QMap>
#include <QString>
#include <QVector>
#include <functional>

namespace RDK {

class UArduinoSerialSession;

class UArduinoFirmataClient {
public:
    bool HandshakeReady = false;
    QString FirmwareVersion;
    int HandshakeStage = 0;
    int PinCount = 0;
    QVector<int> PortDigitalMask;
    QMap<int, int> PinModeByPin;
    QMap<int, int> PinDeviceModeByPin;
    QMap<int, int> AnalogValues;
    QMap<int, int> ChannelToPin;
    QByteArray LastWrittenBytes;

    void reset();
    void processIncoming(const QByteArray& data);
    void onReady(std::function<void()> cb) { OnReadyCallback = std::move(cb); }

    bool startHandshake(UArduinoSerialSession* session, int board_profile);
    bool setPinMode(UArduinoSerialSession* session, int pin, int mode);
    bool setDigitalPinValue(UArduinoSerialSession* session, int pin, int value);
    bool digitalWrite(UArduinoSerialSession* session, int pin, int value);
    bool reportAnalog(UArduinoSerialSession* session, int firmata_pin, int enable);
    bool reportDigitalPort(UArduinoSerialSession* session, int port, int enable);
    bool setSamplingInterval(UArduinoSerialSession* session, int interval_ms);
    bool queryAnalogMapping(UArduinoSerialSession* session);
    bool queryPinState(UArduinoSerialSession* session, int pin);
    bool extendedAnalogWrite(UArduinoSerialSession* session, int pin, int value);
    bool configureServo(UArduinoSerialSession* session, int pin, int min_pulse, int max_pulse);
    bool servoWrite(UArduinoSerialSession* session, int pin, int angle);
    bool i2cConfig(UArduinoSerialSession* session, int delay);
    bool i2cWrite(UArduinoSerialSession* session, int address, const QByteArray& data);
    bool i2cReadRequest(UArduinoSerialSession* session, int address, int bytes);
    QByteArray lastI2cReadData() const { return LastI2cReadData; }

    int analogChannelForPin(int firmata_pin) const;
    int analogValueForChannel(int channel) const;
    int analogValue(int firmata_pin) const;
    int digitalValue(int firmata_pin) const;
    int deviceModeForPin(int firmata_pin) const;

    void setBoardProfile(int board_profile) { BoardProfileValue = board_profile; }
    void setOnBytesWritten(std::function<void(const QByteArray&)> cb)
    {
        OnBytesWrittenCallback = std::move(cb);
    }

private:
    void updateHandshakeStage();
    void advanceHandshake();
    void handleMessage(uint8_t status, uint8_t data1, uint8_t data2);
    void handleSysex(const QByteArray& sysex);
    void writeBytes(UArduinoSerialSession* session, const QByteArray& bytes);
    void writeSysex(UArduinoSerialSession* session, const QByteArray& payload);
    void ensurePortMaskSize(int port);

    QByteArray SysexBuffer;
    QByteArray LastI2cReadData;
    bool InSysex = false;
    QMap<int, int> AnalogChannelByPin;
    std::function<void()> OnReadyCallback;
    std::function<void(const QByteArray&)> OnBytesWrittenCallback;
    bool GotFirmware = false;
    bool GotCapability = false;
    bool GotAnalogMapping = false;
    int BoardProfileValue = 0;
    UArduinoSerialSession* HandshakeSession = nullptr;
};

} // namespace RDK

#endif
