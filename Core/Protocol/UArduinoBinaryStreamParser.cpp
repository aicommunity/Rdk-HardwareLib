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
    if (ProtocolVersionValue >= 2)
        feedFramedV2(data, onFrame);
    else
        feedLegacyV1(data, onFrame);
}

void UArduinoBinaryStreamParser::feedLegacyV1(const QByteArray& data, const FrameCallback& onFrame)
{
    const char* ptr = data.constData();
    int index = 0;
    const int data_size = data.size();

    while (index < data_size) {
        const uint8_t packet_id = static_cast<uint8_t>(ptr[index++]);

        if (packet_id == 0x01) {
            if (index + 2 > data_size)
                break;
            const uint8_t error_flags = static_cast<uint8_t>(ptr[index++]);
            const uint8_t param_count = static_cast<uint8_t>(ptr[index++]);
            const int required_bytes = param_count * static_cast<int>(sizeof(float));
            if (index + required_bytes > data_size)
                break;

            QByteArray payload;
            payload.append(char(error_flags));
            payload.append(char(param_count));
            payload.append(ptr + index, required_bytes);
            index += required_bytes;
            onFrame(packet_id, payload);
        } else if (packet_id == 0x02) {
            if (index + 1 > data_size)
                break;
            const uint8_t pin_count = static_cast<uint8_t>(ptr[index++]);
            if (index + pin_count > data_size)
                break;
            QByteArray payload;
            payload.append(char(pin_count));
            payload.append(ptr + index, pin_count);
            index += pin_count;
            onFrame(packet_id, payload);
        } else if (packet_id == 0x03) {
            if (index + 1 > data_size)
                break;
            QByteArray payload;
            payload.append(ptr[index++]);
            onFrame(packet_id, payload);
        } else if (packet_id == 0x04) {
            if (index + 1 > data_size)
                break;
            const uint8_t analog_pin_count = static_cast<uint8_t>(ptr[index++]);
            const int need = analog_pin_count + 2;
            if (index + need > data_size)
                break;
            QByteArray payload;
            payload.append(char(analog_pin_count));
            payload.append(ptr + index, need);
            index += need;
            onFrame(packet_id, payload);
        } else {
            if (DebugEnabled)
                qDebug() << "UArduinoBinaryStreamParser: unknown packet" << packet_id;
        }
    }
}

void UArduinoBinaryStreamParser::feedFramedV2(const QByteArray& data, const FrameCallback& onFrame)
{
    V2Buffer.append(data);
    const int kHeader = 4;
    const int kCrc = 1;

    while (V2Buffer.size() >= kHeader + kCrc) {
        int start = V2Buffer.indexOf(char(0xAA));
        if (start < 0) {
            V2Buffer.clear();
            return;
        }
        if (start > 0)
            V2Buffer.remove(0, start);

        if (V2Buffer.size() < kHeader + kCrc)
            return;

        const uint8_t type = static_cast<uint8_t>(V2Buffer[1]);
        const uint16_t len = qFromLittleEndian<uint16_t>(
            reinterpret_cast<const uchar*>(V2Buffer.constData() + 2));
        const int frame_len = kHeader + len + kCrc;
        if (V2Buffer.size() < frame_len)
            return;

        const QByteArray frame = V2Buffer.left(frame_len);
        const uint8_t expected_crc = crc8Maxim(frame, kHeader + len);
        const uint8_t actual_crc = static_cast<uint8_t>(frame[kHeader + len]);
        if (expected_crc != actual_crc) {
            V2Buffer.remove(0, 1);
            continue;
        }

        onFrame(type, frame.mid(kHeader, len));
        V2Buffer.remove(0, frame_len);
    }
}

} // namespace RDK
