#include "UNmsdkI2cHubProtocolPlugin.h"

#include "USensorLabFrameDecoder.h"

namespace RDK {

namespace {

void publishMatrix(UArduinoPluginHost* host, uint8_t type,
                   const USensorLabDecodedSensors& decoded)
{
    QVector<double> row;
    row << static_cast<double>(type) << decoded.paramCount;
    for (int i = 0; i < decoded.paramCount && i < 8; ++i)
        row << decoded.values[i];
    host->publishSensorMatrixRow(row);
}

bool publishDecoded(UArduinoPluginHost* host, uint8_t type, const QByteArray& payload)
{
    const auto decoded = USensorLabFrameDecoder::decodeSensors(payload);
    if (!decoded.ok)
        return false;
    publishMatrix(host, type, decoded);

    switch (type) {
    case 0x01: // BME280
    case 0x34: // BMP280 / BME680
        if (decoded.paramCount >= 1)
            host->publishNamedFloat(QStringLiteral("t"), decoded.values[0]);
        if (decoded.paramCount >= 2)
            host->publishNamedFloat(QStringLiteral("h"), decoded.values[1]);
        if (decoded.paramCount >= 3)
            host->publishNamedFloat(QStringLiteral("pressure_hpa"), decoded.values[2]);
        break;
    case 0x30: // VL53L0X
    case 0x35: // VL53L1X
        if (decoded.paramCount >= 1)
            host->publishNamedFloat(QStringLiteral("distance_mm"), decoded.values[0]);
        break;
    case 0x31: // MPU6050
    case 0x36: // ICM-20948
        if (decoded.paramCount >= 1)
            host->publishNamedFloat(QStringLiteral("ax"), decoded.values[0]);
        if (decoded.paramCount >= 2)
            host->publishNamedFloat(QStringLiteral("ay"), decoded.values[1]);
        if (decoded.paramCount >= 3)
            host->publishNamedFloat(QStringLiteral("az"), decoded.values[2]);
        if (decoded.paramCount >= 4)
            host->publishNamedFloat(QStringLiteral("gx"), decoded.values[3]);
        if (decoded.paramCount >= 5)
            host->publishNamedFloat(QStringLiteral("gy"), decoded.values[4]);
        if (decoded.paramCount >= 6)
            host->publishNamedFloat(QStringLiteral("gz"), decoded.values[5]);
        if (type == 0x36 && decoded.paramCount >= 7)
            host->publishNamedFloat(QStringLiteral("mx"), decoded.values[6]);
        if (type == 0x36 && decoded.paramCount >= 8)
            host->publishNamedFloat(QStringLiteral("my"), decoded.values[7]);
        break;
    case 0x32:
        if (decoded.paramCount >= 4) {
            host->publishNamedFloat(QStringLiteral("bus_v"), decoded.values[0]);
            host->publishNamedFloat(QStringLiteral("shunt_v"), decoded.values[1]);
            host->publishNamedFloat(QStringLiteral("current_ma"), decoded.values[2]);
            host->publishNamedFloat(QStringLiteral("power_mw"), decoded.values[3]);
        } else {
            // legacy 3-float: bus_v, current_ma, power_mw
            if (decoded.paramCount >= 1)
                host->publishNamedFloat(QStringLiteral("bus_v"), decoded.values[0]);
            if (decoded.paramCount >= 2)
                host->publishNamedFloat(QStringLiteral("current_ma"), decoded.values[1]);
            if (decoded.paramCount >= 3)
                host->publishNamedFloat(QStringLiteral("power_mw"), decoded.values[2]);
        }
        break;
    case 0x33:
        if (decoded.paramCount >= 1)
            host->publishNamedFloat(QStringLiteral("pca_ch"), decoded.values[0]);
        if (decoded.paramCount >= 2)
            host->publishNamedFloat(QStringLiteral("pca_duty"), decoded.values[1]);
        break;
    case 0x37: // AHT20 / SHT31
        if (decoded.paramCount >= 1)
            host->publishNamedFloat(QStringLiteral("t"), decoded.values[0]);
        if (decoded.paramCount >= 2)
            host->publishNamedFloat(QStringLiteral("h"), decoded.values[1]);
        break;
    case 0x38: // BH1750
        if (decoded.paramCount >= 1)
            host->publishNamedFloat(QStringLiteral("lux"), decoded.values[0]);
        break;
    case 0x39: // MLX90614
        if (decoded.paramCount >= 1)
            host->publishNamedFloat(QStringLiteral("object_c"), decoded.values[0]);
        if (decoded.paramCount >= 2)
            host->publishNamedFloat(QStringLiteral("ambient_c"), decoded.values[1]);
        break;
    case 0x3A: // SGP30
        if (decoded.paramCount >= 1)
            host->publishNamedFloat(QStringLiteral("eco2"), decoded.values[0]);
        if (decoded.paramCount >= 2)
            host->publishNamedFloat(QStringLiteral("tvoc"), decoded.values[1]);
        break;
    case 0x3B: // TCS34725
        if (decoded.paramCount >= 1)
            host->publishNamedFloat(QStringLiteral("r"), decoded.values[0]);
        if (decoded.paramCount >= 2)
            host->publishNamedFloat(QStringLiteral("g"), decoded.values[1]);
        if (decoded.paramCount >= 3)
            host->publishNamedFloat(QStringLiteral("b"), decoded.values[2]);
        if (decoded.paramCount >= 4)
            host->publishNamedFloat(QStringLiteral("c"), decoded.values[3]);
        break;
    case 0x3C: // ADXL345
        if (decoded.paramCount >= 1)
            host->publishNamedFloat(QStringLiteral("ax"), decoded.values[0]);
        if (decoded.paramCount >= 2)
            host->publishNamedFloat(QStringLiteral("ay"), decoded.values[1]);
        if (decoded.paramCount >= 3)
            host->publishNamedFloat(QStringLiteral("az"), decoded.values[2]);
        break;
    case 0x3D: // APDS-9960
        if (decoded.paramCount >= 1)
            host->publishNamedFloat(QStringLiteral("gesture"), decoded.values[0]);
        if (decoded.paramCount >= 2)
            host->publishNamedFloat(QStringLiteral("proximity"), decoded.values[1]);
        if (decoded.paramCount >= 3)
            host->publishNamedFloat(QStringLiteral("r"), decoded.values[2]);
        if (decoded.paramCount >= 4)
            host->publishNamedFloat(QStringLiteral("g"), decoded.values[3]);
        if (decoded.paramCount >= 5)
            host->publishNamedFloat(QStringLiteral("b"), decoded.values[4]);
        break;
    case 0x3E: // ADS1115
        if (decoded.paramCount >= 1)
            host->publishNamedFloat(QStringLiteral("ch0"), decoded.values[0]);
        if (decoded.paramCount >= 2)
            host->publishNamedFloat(QStringLiteral("ch1"), decoded.values[1]);
        if (decoded.paramCount >= 3)
            host->publishNamedFloat(QStringLiteral("ch2"), decoded.values[2]);
        if (decoded.paramCount >= 4)
            host->publishNamedFloat(QStringLiteral("ch3"), decoded.values[3]);
        break;
    default:
        return false;
    }
    host->setProtocolReady(true);
    return true;
}

} // namespace

void UNmsdkI2cHubProtocolPlugin::negotiate(UArduinoPluginHost* host, int protocolVersion)
{
    IArduinoProtocolPlugin::negotiate(host, protocolVersion);
}

void UNmsdkI2cHubProtocolPlugin::onHealthCheck(UArduinoPluginHost* host)
{
    if (host)
        host->enqueueCommand(QStringLiteral("PING"));
}

void UNmsdkI2cHubProtocolPlugin::onBinaryFrame(UArduinoPluginHost* host, uint8_t type,
                                               const QByteArray& payload)
{
    if (!host)
        return;
    host->appendFrameLog(QStringLiteral("i2c hub type=0x%1 len=%2")
                             .arg(type, 2, 16, QChar('0'))
                             .arg(payload.size()));
    if (type == 0x7F) {
        host->setProtocolReady(true);
        host->publishNamedFloat(QStringLiteral("pong"), 1.f);
        return;
    }
    if ((type >= 0x30 && type <= 0x3E) || type == 0x01)
        publishDecoded(host, type, payload);
}

} // namespace RDK
