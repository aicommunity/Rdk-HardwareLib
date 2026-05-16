#include "UArduinoSensorSketch.h"

#include <QDebug>
#include <cstring>

namespace RDK {

namespace {
constexpr int kA0 = 14;
constexpr int kA5 = 19;
}

UArduinoSensorSketch::UArduinoSensorSketch()
    : LowerSensorLimit("LowerSensorLimit", this)
    , UpperSensorLimit("UpperSensorLimit", this)
    , MatrixCols("MatrixCols", this)
    , GetDataFromBuffers("GetDataFromBuffers", this)
    , DoubleMatrixReadings("DoubleMatrixReadings", this)
    , GetPinsInfo("GetPinsInfo", this)
    , PinStatusJson("PinStatusJson", this)
{
}

UArduinoSensorSketch::~UArduinoSensorSketch() = default;

UArduinoSensorSketch* UArduinoSensorSketch::New()
{
    return new UArduinoSensorSketch;
}

bool UArduinoSensorSketch::ADefault()
{
    UArduinoCustomLink::ADefault();
    LowerSensorLimit = -100;
    UpperSensorLimit = 100;
    MatrixCols = 100;
    GetDataFromBuffers = false;
    GetPinsInfo = false;
    PinStatusJson = "";
    BundledFirmwareId = "sensor_lab_v1";
    return true;
}

bool UArduinoSensorSketch::ABuild()
{
    CurrentRow = 0;
    DoubleMatrixReadings.Assign(4, MatrixCols, 0.0);
    return UArduinoCustomLink::ABuild();
}

bool UArduinoSensorSketch::AReset()
{
    SendCommandFlag = false;
    GetPinsInfo = false;
    return UArduinoCustomLink::AReset();
}

QString UArduinoSensorSketch::pinToString(int pin)
{
    if (pin >= kA0 && pin <= kA5)
        return QStringLiteral("A%1").arg(pin - kA0);
    if (pin >= 2 && pin <= 13)
        return QStringLiteral("D%1").arg(pin);
    return QString::number(pin);
}

void UArduinoSensorSketch::FillSensorBuffer(double timestamp, uint8_t paramCount,
                                          float t, float h, float hall, float speed)
{
    if (DataBuffer.size() >= 512)
        DataBuffer.removeFirst();

    USensorDataPoint dp;
    dp.data.append(timestamp);
    dp.data.append(static_cast<double>(paramCount));
    dp.data.append(static_cast<double>(t));
    dp.data.append(static_cast<double>(h));
    dp.data.append(static_cast<double>(hall));
    dp.data.append(static_cast<double>(speed));
    DataBuffer.append(dp);
}

void UArduinoSensorSketch::OnBinaryFrame(uint8_t type, const QByteArray& payload)
{
    if (type == 0x01 && payload.size() >= 2) {
        const uint8_t errorFlags = static_cast<uint8_t>(payload[0]);
        const uint8_t paramCount = static_cast<uint8_t>(payload[1]);
        Q_UNUSED(errorFlags);

        if (payload.size() < 2 + paramCount * static_cast<int>(sizeof(float)))
            return;

        float values[4] = {0, 0, 0, 0};
        for (int i = 0; i < paramCount && i < 4; ++i)
            memcpy(&values[i], payload.constData() + 2 + i * sizeof(float), sizeof(float));

        FillSensorBuffer(UArduinoBinaryStreamParser::legacyTimestamp(), paramCount,
                         values[0], values[1], values[2], values[3]);
    } else if (type == 0x04 && payload.size() >= 3) {
        const uint8_t analogPinCount = static_cast<uint8_t>(payload[0]);
        const int need = 1 + analogPinCount + 2;
        if (payload.size() < need)
            return;

        QStringList analogPins;
        int idx = 1;
        for (int i = 0; i < analogPinCount; ++i)
            analogPins.append(pinToString(static_cast<uint8_t>(payload[idx++])));

        const int dhtPin = static_cast<uint8_t>(payload[idx++]);
        const int servoPin = static_cast<uint8_t>(payload[idx++]);

        PinStatusJson = QStringLiteral("{\"analog\":[%1],\"dht\":\"%2\",\"servo\":\"%3\"}")
                            .arg(analogPins.join(QStringLiteral(",")),
                                 pinToString(dhtPin),
                                 pinToString(servoPin))
                            .toStdString();
    }
}

void UArduinoSensorSketch::PutDataToMatrix()
{
    QVector<double> allData;
    for (const USensorDataPoint& dp : DataBuffer)
        allData += dp.data;
    DataBuffer.clear();

    int index = 0;
    const int MAX_PARAMS = 10;

    while (index < allData.size()) {
        if (index + 2 > allData.size())
            break;

        const double timestamp = allData[index++];
        const int paramCount = static_cast<int>(allData[index++]);

        if (paramCount >= 1000 || paramCount <= 0 || paramCount > MAX_PARAMS)
            continue;

        if (index + paramCount > allData.size())
            break;

        const int totalCols = paramCount + 1;
        if (DoubleMatrixReadings->GetCols() != totalCols)
            DoubleMatrixReadings.Assign(MatrixCols, totalCols, 0.0);

        if (CurrentRow >= MatrixCols)
            CurrentRow = 0;

        for (int col = 0; col < totalCols; ++col) {
            double value = (col == 0) ? timestamp : allData[index++];
            if (col > 0) {
                if (value < LowerSensorLimit)
                    value = LowerSensorLimit;
                if (value > UpperSensorLimit)
                    value = UpperSensorLimit;
            }
            if (CurrentRow < MatrixCols && col < DoubleMatrixReadings->GetCols())
                DoubleMatrixReadings(CurrentRow, col) = value;
        }
        CurrentRow++;
    }
}

void UArduinoSensorSketch::OnBoardCalculate()
{
    UArduinoCustomLink::OnBoardCalculate();

    if (GetPinsInfo) {
        EnqueueCommand("GET STATUS");
        FlushCommandQueue();
        GetPinsInfo = false;
    }

    if (GetDataFromBuffers)
        PutDataToMatrix();
}

bool UArduinoSensorSketch::ACalculate()
{
    return UArduinoCustomLink::ACalculate();
}

} // namespace RDK
