#include "UArduinoSensorSketch.h"

#include "UFirmwareManifest.h"

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
    , StartReading("StartReading", this)
    , StopReading("StopReading", this)
    , Rotate("Rotate", this)
    , StopRotate("StopRotate", this)
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
    StartReading = false;
    StopReading = false;
    Rotate = false;
    StopRotate = false;
    BundledFirmwareId = "sensor_lab_v1";
    FirmwarePath = UFirmwareManifest::bundledHexRelativePath(QStringLiteral("sensor_lab_v1"), 0)
                       .toStdString();
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
    GetDataFromBuffers = false;
    StartReading = false;
    StopReading = false;
    Rotate = false;
    StopRotate = false;
    return UArduinoCustomLink::AReset();
}

void UArduinoSensorSketch::RunPresetCommand(const char* text)
{
    Command = text;
    EnqueueCommand(text);
    SentCommand = text;
}

void UArduinoSensorSketch::ProcessSketchEdges()
{
    if (StartReading) {
        RunPresetCommand("START READING");
        ResetEdge(StartReading);
    }
    if (StopReading) {
        RunPresetCommand("STOP READING");
        ResetEdge(StopReading);
    }
    if (Rotate) {
        RunPresetCommand("ROTATE");
        ResetEdge(Rotate);
    }
    if (StopRotate) {
        RunPresetCommand("STOP ROTATE");
        ResetEdge(StopRotate);
    }

    if (GetPinsInfo) {
        EnqueueCommand("GET STATUS");
        FlushCommandQueue();
        ResetEdge(GetPinsInfo);
    }

    if (GetDataFromBuffers) {
        PutDataToMatrix();
        ResetEdge(GetDataFromBuffers);
    }
}

QString UArduinoSensorSketch::pinToString(int pin)
{
    if (pin >= kA0 && pin <= kA5)
        return QStringLiteral("A%1").arg(pin - kA0);
    if (pin >= 2 && pin <= 13)
        return QStringLiteral("D%1").arg(pin);
    return QString::number(pin);
}

void UArduinoSensorSketch::FillSensorBuffer(double timestamp,
                                          uint8_t param_count,
                                          float temperature,
                                          float humidity,
                                          float hall_value,
                                          float speed_value)
{
    if (DataBuffer.size() >= 512)
        DataBuffer.removeFirst();

    USensorDataPoint dp;
    dp.Data.append(timestamp);
    dp.Data.append(static_cast<double>(param_count));
    dp.Data.append(static_cast<double>(temperature));
    dp.Data.append(static_cast<double>(humidity));
    dp.Data.append(static_cast<double>(hall_value));
    dp.Data.append(static_cast<double>(speed_value));
    DataBuffer.append(dp);
}

void UArduinoSensorSketch::OnBinaryFrame(uint8_t type, const QByteArray& payload)
{
    if (type == 0x01 && payload.size() >= 2) {
        const uint8_t error_flags = static_cast<uint8_t>(payload[0]);
        const uint8_t param_count = static_cast<uint8_t>(payload[1]);
        Q_UNUSED(error_flags);

        if (payload.size() < 2 + param_count * static_cast<int>(sizeof(float)))
            return;

        float values[4] = {0, 0, 0, 0};
        for (int i = 0; i < param_count && i < 4; ++i)
            memcpy(&values[i], payload.constData() + 2 + i * sizeof(float), sizeof(float));

        FillSensorBuffer(UArduinoBinaryStreamParser::legacyTimestamp(),
                         param_count,
                         values[0],
                         values[1],
                         values[2],
                         values[3]);
    } else if (type == 0x04 && payload.size() >= 3) {
        const uint8_t analog_pin_count = static_cast<uint8_t>(payload[0]);
        const int need = 1 + analog_pin_count + 2;
        if (payload.size() < need)
            return;

        QStringList analog_pins;
        int idx = 1;
        for (int i = 0; i < analog_pin_count; ++i)
            analog_pins.append(pinToString(static_cast<uint8_t>(payload[idx++])));

        const int dht_pin = static_cast<uint8_t>(payload[idx++]);
        const int servo_pin = static_cast<uint8_t>(payload[idx++]);

        PinStatusJson = QStringLiteral("{\"analog\":[%1],\"dht\":\"%2\",\"servo\":\"%3\"}")
                            .arg(analog_pins.join(QStringLiteral(",")),
                                 pinToString(dht_pin),
                                 pinToString(servo_pin))
                            .toStdString();
    }
}

void UArduinoSensorSketch::PutDataToMatrix()
{
    QVector<double> allData;
    for (const USensorDataPoint& dp : DataBuffer)
        allData += dp.Data;
    DataBuffer.clear();

    int index = 0;
    const int max_params = 10;

    while (index < allData.size()) {
        if (index + 2 > allData.size())
            break;

        const double timestamp = allData[index++];
        const int param_count = static_cast<int>(allData[index++]);

        if (param_count >= 1000 || param_count <= 0 || param_count > max_params)
            continue;

        if (index + param_count > allData.size())
            break;

        const int total_cols = param_count + 1;
        if (DoubleMatrixReadings->GetCols() != total_cols)
            DoubleMatrixReadings.Assign(MatrixCols, total_cols, 0.0);

        if (CurrentRow >= MatrixCols)
            CurrentRow = 0;

        for (int col = 0; col < total_cols; ++col) {
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
    ProcessSketchEdges();
    UArduinoCustomLink::OnBoardCalculate();
}

bool UArduinoSensorSketch::ACalculate()
{
    return UArduinoCustomLink::ACalculate();
}

} // namespace RDK
