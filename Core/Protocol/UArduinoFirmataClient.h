#ifndef UARDUINOFIRMATACLIENT_H
#define UARDUINOFIRMATACLIENT_H

#include <QByteArray>
#include <QMap>
#include <QString>
#include <functional>

namespace RDK {

class UArduinoSerialSession;

class UArduinoFirmataClient {
public:
    bool HandshakeReady = false;
    QString FirmwareVersion;

    void reset();
    void processIncoming(const QByteArray& data);
    void onReady(std::function<void()> cb) { OnReadyCallback = std::move(cb); }

    bool startHandshake(UArduinoSerialSession* session);
    bool setPinMode(UArduinoSerialSession* session, int pin, int mode);
    bool digitalWrite(UArduinoSerialSession* session, int pin, int value);
    bool reportAnalog(UArduinoSerialSession* session, int pin, int enable);
    bool queryAnalogMapping(UArduinoSerialSession* session);
    int analogValue(int pin) const;
    int analogChannelForPin(int pin) const;

private:
    void handleMessage(uint8_t status, uint8_t data1, uint8_t data2);
    void handleSysex(const QByteArray& sysex);
    void writeBytes(UArduinoSerialSession* session, const QByteArray& bytes);

    QByteArray SysexBuffer;
    bool InSysex = false;
    QMap<int, int> AnalogValues;
    QMap<int, int> AnalogChannelByPin;
    std::function<void()> OnReadyCallback;
    bool GotFirmware = false;
    bool GotCapability = false;
    bool GotAnalogMapping = false;
};

} // namespace RDK

#endif
