#include "UArduinoSensorSketch.h"

#include "UArduinoPropertyString.h"
#include "UFirmwareManifest.h"
#include "Protocol/UArduinoProtocolPluginRegistry.h"
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
    registerBuiltinArduinoProtocolPlugins();
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

IArduinoProtocolPlugin* UArduinoSensorSketch::resolvePlugin() const
{
    registerBuiltinArduinoProtocolPlugins();
    QString id = UArduinoPropertyString::fromStdProperty(*BundledFirmwareId);
    if (id.isEmpty())
        id = QStringLiteral("sensor_lab_v1");
    return findArduinoProtocolPlugin(id);
}

void UArduinoSensorSketch::enqueueCommand(const QString& line)
{
    EnqueueCommand(UArduinoPropertyString::toStdProperty(line));
}

int UArduinoSensorSketch::boardProfile() const
{
    return BoardProfile;
}

int UArduinoSensorSketch::protocolVersion() const
{
    return ProtocolVersion;
}

void UArduinoSensorSketch::setProtocolReady(bool ready)
{
    ProtocolNegotiated = ready;
}

void UArduinoSensorSketch::setLastError(const QString& error)
{
    LastError = UArduinoPropertyString::toStdProperty(error);
}

void UArduinoSensorSketch::publishSensorMatrixRow(const QVector<double>& row)
{
    if (row.size() < 1)
        return;
    const int param_count = static_cast<int>(row[0]);
    const float t = row.size() > 1 ? static_cast<float>(row[1]) : 0.f;
    const float h = row.size() > 2 ? static_cast<float>(row[2]) : 0.f;
    const float hall = row.size() > 3 ? static_cast<float>(row[3]) : 0.f;
    const float speed = row.size() > 4 ? static_cast<float>(row[4]) : 0.f;
    FillSensorBuffer(UArduinoBinaryStreamParser::legacyTimestamp(),
                     static_cast<uint8_t>(qBound(1, param_count, 8)), t, h, hall, speed);
}

void UArduinoSensorSketch::publishPinStatusJson(const QString& json)
{
    PinStatusJson = UArduinoPropertyString::toStdProperty(json);
}

void UArduinoSensorSketch::appendFrameLog(const QString& line)
{
    Q_UNUSED(line);
}

void UArduinoSensorSketch::NegotiateProtocol()
{
    IArduinoProtocolPlugin* plugin = resolvePlugin();
    if (!plugin) {
        ProtocolNegotiated = false;
        return;
    }
    plugin->negotiate(this, ProtocolVersion);
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
    IArduinoProtocolPlugin* plugin = resolvePlugin();
    if (plugin) {
        plugin->onBinaryFrame(this, type, payload);
        return;
    }
    setLastError(QStringLiteral("No protocol plugin for BundledFirmwareId"));
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
