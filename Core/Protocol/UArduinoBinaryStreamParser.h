#ifndef UARDUINOBINARYSTREAMPARSER_H
#define UARDUINOBINARYSTREAMPARSER_H

#include <QByteArray>
#include <functional>

namespace RDK {

class UArduinoBinaryStreamParser {
public:
    using FrameCallback = std::function<void(uint8_t type, const QByteArray& payload)>;

    void setProtocolVersion(int version) { ProtocolVersionValue = version; }
    int protocolVersion() const { return ProtocolVersionValue; }
    void setDebug(bool debug) { DebugEnabled = debug; }

    void feed(const QByteArray& data, const FrameCallback& onFrame);

    static double legacyTimestamp();
    static uint8_t crc8Maxim(const QByteArray& data, int len);

private:
    void feedLegacyV1(const QByteArray& data, const FrameCallback& onFrame);
    void feedFramedV2(const QByteArray& data, const FrameCallback& onFrame);

    QByteArray V2Buffer;
    int ProtocolVersionValue = 1;
    bool DebugEnabled = false;
};

} // namespace RDK

#endif
