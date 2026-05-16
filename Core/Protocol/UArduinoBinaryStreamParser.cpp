#include "UArduinoBinaryStreamParser.h"

#include <QDateTime>
#include <QDebug>
#include <QtEndian>
#include <cstring>

namespace RDK {

double UArduinoBinaryStreamParser::legacyTimestamp()
{
    const QTime time = QDateTime::currentDateTime().time();
    return time.hour() * 10000.0 + time.minute() * 100.0 + time.second();
}

uint8_t UArduinoBinaryStreamParser::crc8Maxim(const QByteArray& data, int len)
{
    uint8_t crc = 0;
    for (int i = 0; i < len && i < data.size(); ++i) {
        crc ^= static_cast<uint8_t>(data[i]);
        for (int b = 0; b < 8; ++b)
            crc = (crc & 1) ? static_cast<uint8_t>((crc >> 1) ^ 0x8C) : static_cast<uint8_t>(crc >> 1);
    }
    return crc;
}

void UArduinoBinaryStreamParser::feed(const QByteArray& data, const FrameCallback& onFrame)
{
    if (data.isEmpty() || !onFrame)
        return;
    if (m_protocolVersion >= 2)
        feedFramedV2(data, onFrame);
    else
        feedLegacyV1(data, onFrame);
}

void UArduinoBinaryStreamParser::feedLegacyV1(const QByteArray& data, const FrameCallback& onFrame)
{
    const char* ptr = data.constData();
    int index = 0;
    const int dataSize = data.size();

    while (index < dataSize) {
        const uint8_t packetId = static_cast<uint8_t>(ptr[index++]);

        if (packetId == 0x01) {
            if (index + 2 > dataSize)
                break;
            const uint8_t errorFlags = static_cast<uint8_t>(ptr[index++]);
            const uint8_t paramCount = static_cast<uint8_t>(ptr[index++]);
            const int requiredBytes = paramCount * static_cast<int>(sizeof(float));
            if (index + requiredBytes > dataSize)
                break;

            QByteArray payload;
            payload.append(char(errorFlags));
            payload.append(char(paramCount));
            payload.append(ptr + index, requiredBytes);
            index += requiredBytes;
            onFrame(packetId, payload);
        } else if (packetId == 0x02) {
            if (index + 1 > dataSize)
                break;
            const uint8_t pinCount = static_cast<uint8_t>(ptr[index++]);
            if (index + pinCount > dataSize)
                break;
            QByteArray payload;
            payload.append(char(pinCount));
            payload.append(ptr + index, pinCount);
            index += pinCount;
            onFrame(packetId, payload);
        } else if (packetId == 0x03) {
            if (index + 1 > dataSize)
                break;
            QByteArray payload;
            payload.append(ptr[index++]);
            onFrame(packetId, payload);
        } else if (packetId == 0x04) {
            if (index + 1 > dataSize)
                break;
            const uint8_t analogPinCount = static_cast<uint8_t>(ptr[index++]);
            const int need = analogPinCount + 2;
            if (index + need > dataSize)
                break;
            QByteArray payload;
            payload.append(char(analogPinCount));
            payload.append(ptr + index, need);
            index += need;
            onFrame(packetId, payload);
        } else {
            if (m_debug)
                qDebug() << "UArduinoBinaryStreamParser: unknown packet" << packetId;
        }
    }
}

void UArduinoBinaryStreamParser::feedFramedV2(const QByteArray& data, const FrameCallback& onFrame)
{
    m_v2Buffer.append(data);
    const int kHeader = 4;
    const int kCrc = 1;

    while (m_v2Buffer.size() >= kHeader + kCrc) {
        int start = m_v2Buffer.indexOf(char(0xAA));
        if (start < 0) {
            m_v2Buffer.clear();
            return;
        }
        if (start > 0)
            m_v2Buffer.remove(0, start);

        if (m_v2Buffer.size() < kHeader + kCrc)
            return;

        const uint8_t type = static_cast<uint8_t>(m_v2Buffer[1]);
        const uint16_t len = qFromLittleEndian<uint16_t>(
            reinterpret_cast<const uchar*>(m_v2Buffer.constData() + 2));
        const int frameLen = kHeader + len + kCrc;
        if (m_v2Buffer.size() < frameLen)
            return;

        const QByteArray frame = m_v2Buffer.left(frameLen);
        const uint8_t expectedCrc = crc8Maxim(frame, kHeader + len);
        const uint8_t actualCrc = static_cast<uint8_t>(frame[kHeader + len]);
        if (expectedCrc != actualCrc) {
            m_v2Buffer.remove(0, 1);
            continue;
        }

        onFrame(type, frame.mid(kHeader, len));
        m_v2Buffer.remove(0, frameLen);
    }
}

} // namespace RDK
