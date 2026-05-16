#ifndef UARDUINOBINARYSTREAMPARSER_H
#define UARDUINOBINARYSTREAMPARSER_H

#include <QByteArray>
#include <functional>

namespace RDK {

class UArduinoBinaryStreamParser {
public:
    using FrameCallback = std::function<void(uint8_t type, const QByteArray& payload)>;

    void setProtocolVersion(int version) { m_protocolVersion = version; }
    int protocolVersion() const { return m_protocolVersion; }
    void setDebug(bool debug) { m_debug = debug; }

    void feed(const QByteArray& data, const FrameCallback& onFrame);

    static double legacyTimestamp();
    static uint8_t crc8Maxim(const QByteArray& data, int len);

private:
    void feedLegacyV1(const QByteArray& data, const FrameCallback& onFrame);
    void feedFramedV2(const QByteArray& data, const FrameCallback& onFrame);

    QByteArray m_v2Buffer;
    int m_protocolVersion = 1;
    bool m_debug = false;
};

} // namespace RDK

#endif
