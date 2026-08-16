#include "UArduinoSensorSketch.h"

#include "UArduinoPropertyString.h"
#include "UFirmwareManifest.h"
#include "Protocol/USensorLabFrameDecoder.h"

#include <QDebug>
#include <cstring>

namespace RDK {

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
    FirmwarePath = UArduinoPropertyString::toStdProperty(
        UFirmwareManifest::bundledHexRelativePath(QStringLiteral("sensor_lab_v1"), 0));
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
    return USensorLabFrameDecoder::pinToString(pin, BoardProfile);
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
    if (type == 0x01) {
        const auto decoded = USensorLabFrameDecoder::decodeSensors(payload);
        if (!decoded.ok)
            return;
        FillSensorBuffer(UArduinoBinaryStreamParser::legacyTimestamp(), decoded.paramCount,
                         decoded.values[0], decoded.values[1], decoded.values[2],
                         decoded.values[3]);
    } else if (type == 0x04) {
        const auto decoded = USensorLabFrameDecoder::decodePinStatus(payload, BoardProfile);
        if (!decoded.ok)
            return;
        PinStatusJson = UArduinoPropertyString::toStdProperty(
            QStringLiteral("{\"analog\":[%1],\"dht\":\"%2\",\"servo\":\"%3\"}")
                .arg(decoded.analogPins.join(QStringLiteral(",")), decoded.dhtPin,
                     decoded.servoPin));
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
