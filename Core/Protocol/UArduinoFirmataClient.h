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
    bool ready = false;
    QString firmwareVersion;

    void reset();
    void processIncoming(const QByteArray& data);
    void onReady(std::function<void()> cb) { m_onReady = std::move(cb); }

    bool startHandshake(UArduinoSerialSession* session);
    bool setPinMode(UArduinoSerialSession* session, int pin, int mode);
    bool digitalWrite(UArduinoSerialSession* session, int pin, int value);
    bool reportAnalog(UArduinoSerialSession* session, int pin, int enable);
    int analogValue(int pin) const;

private:
    void handleMessage(uint8_t status, uint8_t data1, uint8_t data2);
    void handleSysex(const QByteArray& sysex);
    void writeBytes(UArduinoSerialSession* session, const QByteArray& bytes);

    QByteArray m_sysexBuffer;
    bool m_inSysex = false;
    QMap<int, int> m_analogValues;
    std::function<void()> m_onReady;
    bool m_gotFirmware = false;
    bool m_gotCapability = false;
};

} // namespace RDK

#endif
