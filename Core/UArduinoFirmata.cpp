#include "UArduinoFirmata.h"

#include "UFirmwareManifest.h"
#include "Transport/UArduinoPinMap.h"
#include "Transport/UArduinoSerialSession.h"
#include "UArduinoSampleBuffer.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

namespace RDK {

namespace {

QByteArray parseHexBytes(const QString& hex)
{
    QByteArray out;
    const QString cleaned = hex.simplified().remove(' ');
    for (int i = 0; i + 1 < cleaned.size(); i += 2) {
        bool ok = false;
        const int byte = cleaned.mid(i, 2).toInt(&ok, 16);
        if (ok)
            out.append(char(byte & 0xFF));
    }
    return out;
}

QString hexEncode(const QByteArray& data)
{
    QString out;
    for (char b : data) {
        if (!out.isEmpty())
            out.append(' ');
        out.append(QStringLiteral("%1").arg(static_cast<uint8_t>(b), 2, 16, QChar('0')));
    }
    return out;
}

string appendStreamLog(string log, const QString& line, int max_bytes = 8192)
{
    QString combined = QString::fromStdString(log);
    if (!combined.isEmpty())
        combined.append('\n');
    combined.append(line);
    while (combined.toUtf8().size() > max_bytes) {
        const int nl = combined.indexOf('\n');
        if (nl < 0)
            break;
        combined = combined.mid(nl + 1);
    }
    return combined.toStdString();
}

} // namespace

UArduinoFirmata::UArduinoFirmata()
    : FirmataReady("FirmataReady", this)
    , FirmataFirmwareVersion("FirmataFirmwareVersion", this)
    , SelectedPin("SelectedPin", this)
    , SelectedPinMode("SelectedPinMode", this)
    , DigitalPinValue("DigitalPinValue", this)
    , AnalogPinValue("AnalogPinValue", this)
    , SetPinModeFlag("SetPinModeFlag", this)
    , ReadAnalogFlag("ReadAnalogFlag", this)
    , WriteDigitalFlag("WriteDigitalFlag", this)
    , SetPinMode("SetPinMode", this)
    , WriteDigital("WriteDigital", this)
    , ReadAnalog("ReadAnalog", this)
    , HandshakeStage("HandshakeStage", this)
    , PinStatusJson("PinStatusJson", this)
    , CapabilityJson("CapabilityJson", this)
    , AutoRefreshPins("AutoRefreshPins", this)
    , RefreshPins("RefreshPins", this)
    , SampleBufferRows("SampleBufferRows", this)
    , ClearSamplesOnCalculate("ClearSamplesOnCalculate", this)
    , AnalogSamples("AnalogSamples", this)
    , DigitalSamples("DigitalSamples", this)
    , DigitalOutputCommands("DigitalOutputCommands", this)
    , PinConfigBatch("PinConfigBatch", this)
    , StreamLog("StreamLog", this)
    , StreamLogEnable("StreamLogEnable", this)
    , PwmPinValue("PwmPinValue", this)
    , WritePwm("WritePwm", this)
    , AnalogOutputCommands("AnalogOutputCommands", this)
    , PinConfigPreset("PinConfigPreset", this)
    , LoadPreset("LoadPreset", this)
    , QueryPin("QueryPin", this)
    , QueryPinState("QueryPinState", this)
    , ServoPin("ServoPin", this)
    , ServoAngle("ServoAngle", this)
    , ServoMinPulse("ServoMinPulse", this)
    , ServoMaxPulse("ServoMaxPulse", this)
    , ConfigureServo("ConfigureServo", this)
    , WriteServo("WriteServo", this)
    , I2cAddress("I2cAddress", this)
    , I2cWriteData("I2cWriteData", this)
    , I2cReadData("I2cReadData", this)
    , I2cWrite("I2cWrite", this)
    , I2cRead("I2cRead", this)
    , RestartFirmata("RestartFirmata", this)
    , ApplyPinConfig("ApplyPinConfig", this)
    , IsFirmataReady("IsFirmataReady", this)
    , IsLinkReady("IsLinkReady", this)
    , ReportAnalogEnable("ReportAnalogEnable", this)
{
}

UArduinoFirmata::~UArduinoFirmata() = default;

UArduinoFirmata* UArduinoFirmata::New()
{
    return new UArduinoFirmata;
}

bool UArduinoFirmata::ADefault()
{
    UArduinoBoard::ADefault();
    FirmataReady = false;
    FirmataFirmwareVersion = "";
    SelectedPin = 13;
    SelectedPinMode = 1;
    DigitalPinValue = 0;
    AnalogPinValue = 0;
    SetPinModeFlag = false;
    ReadAnalogFlag = false;
    WriteDigitalFlag = false;
    SetPinMode = false;
    WriteDigital = false;
    ReadAnalog = false;
    HandshakeStage = 0;
    PinStatusJson = "";
    CapabilityJson = "";
    AutoRefreshPins = false;
    RefreshPins = false;
    SampleBufferRows = 512;
    ClearSamplesOnCalculate = false;
    AnalogSamples.Assign(0, 4, 0.0);
    DigitalSamples.Assign(0, 3, 0.0);
    DigitalOutputCommands.Assign(0, 2, 0.0);
    PinConfigBatch.Assign(0, 2, 0.0);
    StreamLog = "";
    StreamLogEnable = false;
    PwmPinValue = 0;
    WritePwm = false;
    AnalogOutputCommands.Assign(0, 2, 0.0);
    PinConfigPreset = "";
    LoadPreset = false;
    QueryPin = 13;
    QueryPinState = false;
    ServoPin = 9;
    ServoAngle = 90;
    ServoMinPulse = 544;
    ServoMaxPulse = 2400;
    ConfigureServo = false;
    WriteServo = false;
    I2cAddress = 72;
    I2cWriteData = "";
    I2cReadData = "";
    I2cWrite = false;
    I2cRead = false;
    RestartFirmata = false;
    ApplyPinConfig = false;
    IsFirmataReady = false;
    IsLinkReady = false;
    ReportAnalogEnable = false;
    BundledFirmwareId = "standard_firmata";
    FirmwarePath = UFirmwareManifest::bundledHexRelativePath(QStringLiteral("standard_firmata"), 0)
                       .toStdString();
    HandshakeSent = false;
    FirmataClient.setBoardProfile(BoardProfile);
    SyncFirmataStates();
    return true;
}

void UArduinoFirmata::SyncFirmataStates()
{
    IsFirmataReady = FirmataReady;
    IsLinkReady = IsConnected && FirmataReady;
}

void UArduinoFirmata::StartFirmataHandshake()
{
    FirmataClient.reset();
    FirmataClient.setBoardProfile(BoardProfile);
    if (Session && Session->isOpen()) {
        FirmataClient.onReady([this]() {
            if (Session && Session->isOpen())
                FirmataClient.setSamplingInterval(session(), 19);
        });
        FirmataClient.setOnBytesWritten([this](const QByteArray& bytes) {
            if (!StreamLogEnable || !ShowDebug)
                return;
            StreamLog = appendStreamLog(string(StreamLog),
                                        QStringLiteral("TX %1").arg(QString(bytes.toHex(' '))));
        });
        FirmataClient.startHandshake(session(), BoardProfile);
    }
    HandshakeSent = true;
}

void UArduinoFirmata::ApplyLoadPreset()
{
    const QString preset = QString::fromStdString(PinConfigPreset);
    if (preset == QStringLiteral("uno_d13_blink") || preset == QStringLiteral("mega_d13_blink")) {
        SelectedPin = 13;
        SelectedPinMode = 1;
        DigitalPinValue = 1;
    } else if (preset == QStringLiteral("uno_a0_monitor") || preset == QStringLiteral("mega_a0_monitor")) {
        const int profile = BoardProfile;
        SelectedPin = UArduinoPinMap::firmataPinForLabel(QStringLiteral("A0"), profile);
        SelectedPinMode = 2;
        ReportAnalogEnable = true;
        AutoRefreshPins = true;
    }
}

void UArduinoFirmata::ProcessFirmataEdges()
{
    if (RestartFirmata) {
        HandshakeSent = false;
        FirmataReady = false;
        if (IsConnected)
            StartFirmataHandshake();
        ResetEdge(RestartFirmata);
    }

    if (LoadPreset) {
        ApplyLoadPreset();
        ResetEdge(LoadPreset);
    }

    if (ApplyPinConfig) {
        RunFirmataActions();
        ResetEdge(ApplyPinConfig);
    }
}

void UArduinoFirmata::CloseConnection()
{
    UArduinoBoard::CloseConnection();
    HandshakeSent = false;
    FirmataReady = false;
}

bool UArduinoFirmata::EnsureConnected()
{
    const bool ok = UArduinoBoard::EnsureConnected();
    if (ok && !HandshakeSent)
        StartFirmataHandshake();
    if (!ok)
        HandshakeSent = false;
    return ok;
}

void UArduinoFirmata::AppendAnalogSample(int channel, int value)
{
    const int pin = FirmataClient.ChannelToPin.value(channel, SelectedPin);
    const double row[4] = {LastActivityMs, static_cast<double>(pin), static_cast<double>(channel),
                           static_cast<double>(value)};
    UArduinoSampleBuffer::appendRow(*AnalogSamples, row, 4);
    UArduinoSampleBuffer::trimRows(*AnalogSamples, SampleBufferRows);
}

void UArduinoFirmata::AppendDigitalSample(int firmata_pin, int value)
{
    const double row[3] = {LastActivityMs, static_cast<double>(firmata_pin),
                           static_cast<double>(value)};
    UArduinoSampleBuffer::appendRow(*DigitalSamples, row, 3);
    UArduinoSampleBuffer::trimRows(*DigitalSamples, SampleBufferRows);
}

void UArduinoFirmata::ProcessFirmata()
{
    if (!Session)
        return;
    const QByteArray data = Session->takeReceivedBytes();
    if (data.isEmpty())
        return;
    TouchActivity();
    LastHealthResponseMs = LastActivityMs;

    const QMap<int, int> analog_before = FirmataClient.AnalogValues;
    const QVector<int> ports_before = FirmataClient.PortDigitalMask;

    FirmataClient.processIncoming(data);

    for (auto it = FirmataClient.AnalogValues.constBegin(); it != FirmataClient.AnalogValues.constEnd();
         ++it) {
        if (!analog_before.contains(it.key()) || analog_before.value(it.key()) != it.value())
            AppendAnalogSample(it.key(), it.value());
    }

    for (int port = 0; port < FirmataClient.PortDigitalMask.size(); ++port) {
        const int mask = FirmataClient.PortDigitalMask[port];
        const int prev = port < ports_before.size() ? ports_before[port] : -1;
        if (mask == prev)
            continue;
        for (int bit = 0; bit < 8; ++bit) {
            const int pin = port * 8 + bit;
            AppendDigitalSample(pin, (mask >> bit) & 1);
        }
    }

    FirmataReady = FirmataClient.HandshakeReady;
    FirmataFirmwareVersion = FirmataClient.FirmwareVersion.toStdString();
    HandshakeStage = FirmataClient.HandshakeStage;
    AnalogPinValue = FirmataClient.analogValueForChannel(
        FirmataClient.analogChannelForPin(SelectedPin));
    UpdateCapabilityJson();

    if (StreamLogEnable && ShowDebug) {
        StreamLog = appendStreamLog(string(StreamLog),
                                    QStringLiteral("RX %1").arg(QString(data.toHex(' '))));
    }
}

void UArduinoFirmata::UpdateCapabilityJson()
{
    QJsonArray pins;
    for (auto it = FirmataClient.PinDeviceModeByPin.constBegin();
         it != FirmataClient.PinDeviceModeByPin.constEnd(); ++it) {
        QJsonObject o;
        o.insert(QStringLiteral("pin"), it.key());
        o.insert(QStringLiteral("mode"), it.value());
        pins.append(o);
    }
    QJsonObject root;
    root.insert(QStringLiteral("pinCount"), FirmataClient.PinCount);
    root.insert(QStringLiteral("pins"), pins);
    CapabilityJson = QJsonDocument(root).toJson(QJsonDocument::Compact).toStdString();
}

void UArduinoFirmata::BuildPinStatusJson()
{
    const int profile = BoardProfile;
    const int max_pin = UArduinoPinMap::maxFirmataPin(profile);
    QJsonArray pins;
    for (int pin = 0; pin <= max_pin; ++pin) {
        QJsonObject o;
        const QString id = UArduinoPinMap::labelForFirmataPin(pin, profile);
        o.insert(QStringLiteral("id"), id);
        o.insert(QStringLiteral("firmataPin"), pin);
        const bool supported = FirmataClient.PinCount <= 0 || pin < FirmataClient.PinCount;
        o.insert(QStringLiteral("supported"), supported);
        o.insert(QStringLiteral("hostMode"), FirmataClient.PinModeByPin.value(pin, -1));
        o.insert(QStringLiteral("deviceMode"), FirmataClient.deviceModeForPin(pin));
        const int dig = FirmataClient.digitalValue(pin);
        o.insert(QStringLiteral("digital"), dig);
        const int ch = FirmataClient.analogChannelForPin(pin);
        int analog = -1;
        if (ch >= 0)
            analog = FirmataClient.analogValueForChannel(ch);
        o.insert(QStringLiteral("analog"), analog);
        o.insert(QStringLiteral("analogChannel"), ch);
        pins.append(o);
    }
    QJsonObject root;
    root.insert(QStringLiteral("version"), 1);
    root.insert(QStringLiteral("boardProfile"), profile);
    root.insert(QStringLiteral("linkReady"), static_cast<bool>(IsLinkReady));
    root.insert(QStringLiteral("pinCount"), FirmataClient.PinCount);
    root.insert(QStringLiteral("pins"), pins);
    PinStatusJson = QJsonDocument(root).toJson(QJsonDocument::Compact).toStdString();
}

void UArduinoFirmata::RunFirmataActions()
{
    const bool do_set_mode = SetPinMode || SetPinModeFlag;
    const bool do_write = WriteDigital || WriteDigitalFlag;
    const bool do_read = ReadAnalog || ReadAnalogFlag;

    if (Session && Session->isOpen() && FirmataClient.HandshakeReady) {
    if (do_set_mode) {
        FirmataClient.setPinMode(session(), SelectedPin, SelectedPinMode);
        SetPinModeFlag = false;
    }
    if (do_write) {
        FirmataClient.setDigitalPinValue(session(), SelectedPin, DigitalPinValue ? 1 : 0);
        WriteDigitalFlag = false;
    }
    if (do_read) {
        if (SelectedPinMode != 2)
            FirmataClient.setPinMode(session(), SelectedPin, 2);
        FirmataClient.reportAnalog(session(), SelectedPin, 1);
        ReadAnalogFlag = false;
    }
    if (ReportAnalogEnable)
        FirmataClient.reportAnalog(session(), SelectedPin, 1);

    if (RefreshPins || AutoRefreshPins) {
        for (int port = 0; port < UArduinoPinMap::digitalPortCount(BoardProfile); ++port)
            FirmataClient.reportDigitalPort(session(), port, 1);
        if (SelectedPinMode == 2 || ReportAnalogEnable)
            FirmataClient.reportAnalog(session(), SelectedPin, 1);
    }

    if (WritePwm)
        FirmataClient.extendedAnalogWrite(session(), SelectedPin, PwmPinValue);

    if (ConfigureServo)
        FirmataClient.configureServo(session(), ServoPin, ServoMinPulse, ServoMaxPulse);
    if (WriteServo)
        FirmataClient.servoWrite(session(), ServoPin, ServoAngle);

    if (QueryPinState) {
        const int pin = QueryPin >= 0 ? QueryPin : SelectedPin;
        FirmataClient.queryPinState(session(), pin);
    }

    if (I2cWrite) {
        FirmataClient.i2cConfig(session(), 0);
        const QByteArray payload = parseHexBytes(QString::fromStdString(I2cWriteData));
        FirmataClient.i2cWrite(session(), I2cAddress, payload);
    }
    if (I2cRead) {
        FirmataClient.i2cConfig(session(), 0);
        FirmataClient.i2cReadRequest(session(), I2cAddress, 8);
        I2cReadData = hexEncode(FirmataClient.lastI2cReadData()).toStdString();
    }

    for (int r = 0; r < PinConfigBatch->GetRows(); ++r) {
        const int pin = static_cast<int>((*PinConfigBatch)(r, 0));
        const int mode = static_cast<int>((*PinConfigBatch)(r, 1));
        FirmataClient.setPinMode(session(), pin, mode);
    }
    if (PinConfigBatch->GetRows() > 0)
        PinConfigBatch.Assign(0, 2, 0.0);

    for (int r = 0; r < DigitalOutputCommands->GetRows(); ++r) {
        const int pin = static_cast<int>((*DigitalOutputCommands)(r, 0));
        const int value = static_cast<int>((*DigitalOutputCommands)(r, 1));
        FirmataClient.setDigitalPinValue(session(), pin, value ? 1 : 0);
    }
    if (DigitalOutputCommands->GetRows() > 0)
        DigitalOutputCommands.Assign(0, 2, 0.0);

    for (int r = 0; r < AnalogOutputCommands->GetRows(); ++r) {
        const int pin = static_cast<int>((*AnalogOutputCommands)(r, 0));
        const int value = static_cast<int>((*AnalogOutputCommands)(r, 1));
        FirmataClient.extendedAnalogWrite(session(), pin, value);
    }
    if (AnalogOutputCommands->GetRows() > 0)
        AnalogOutputCommands.Assign(0, 2, 0.0);
    }

    if (RefreshPins)
        ResetEdge(RefreshPins);
    if (SetPinMode)
        ResetEdge(SetPinMode);
    if (WriteDigital)
        ResetEdge(WriteDigital);
    if (ReadAnalog)
        ResetEdge(ReadAnalog);
    if (WritePwm)
        ResetEdge(WritePwm);
    if (ConfigureServo)
        ResetEdge(ConfigureServo);
    if (WriteServo)
        ResetEdge(WriteServo);
    if (QueryPinState)
        ResetEdge(QueryPinState);
    if (I2cWrite)
        ResetEdge(I2cWrite);
    if (I2cRead)
        ResetEdge(I2cRead);
}

void UArduinoFirmata::OnBoardCalculate()
{
    ProcessFirmata();
    RunFirmataActions();
    BuildPinStatusJson();
}

bool UArduinoFirmata::ACalculate()
{
    ProcessFirmataEdges();
    if (ClearSamplesOnCalculate) {
        UArduinoSampleBuffer::clear(*AnalogSamples);
        UArduinoSampleBuffer::clear(*DigitalSamples);
    }
    const bool ok = UArduinoBoard::ACalculate();
    SyncFirmataStates();
    return ok;
}

} // namespace RDK
