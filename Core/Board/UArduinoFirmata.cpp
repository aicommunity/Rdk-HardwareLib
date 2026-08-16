#include "UArduinoFirmata.h"

#include "UArduinoPropertyString.h"

#include "UFirmwareManifest.h"
#include "Catalog/UHardwareCatalog.h"
#include "Catalog/UHardwareSetup.h"
#include "Devices/UArduinoDevicePinResolver.h"
#include "Devices/UArduinoFirmataBatchBuilder.h"
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
    QString combined = UArduinoPropertyString::fromStdProperty(log);
    if (!combined.isEmpty())
        combined.append('\n');
    combined.append(line);
    while (combined.toUtf8().size() > max_bytes) {
        const int nl = combined.indexOf('\n');
        if (nl < 0)
            break;
        combined = combined.mid(nl + 1);
    }
    return UArduinoPropertyString::toStdProperty(combined);
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
    , I2cReadPending("I2cReadPending", this)
    , ApplyHardwareSetup("ApplyHardwareSetup", this)
    , LastSetupApplyResult("LastSetupApplyResult", this)
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
    I2cReadPending = false;
    ApplyHardwareSetup = false;
    LastSetupApplyResult = "";
    RestartFirmata = false;
    ApplyPinConfig = false;
    IsFirmataReady = false;
    IsLinkReady = false;
    ReportAnalogEnable = false;
    BundledFirmwareId = "standard_firmata";
    FirmwarePath = UArduinoPropertyString::toStdProperty(
        UFirmwareManifest::bundledHexRelativePath(QStringLiteral("standard_firmata"), 0));
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
    const QString preset = UArduinoPropertyString::fromStdProperty(PinConfigPreset);
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
    } else if (preset == QStringLiteral("sensor_shield_inputs_A0_A5")) {
        UArduinoFirmataBatchBuilder::clearPinConfig(*PinConfigBatch);
        for (int i = 0; i < 6; ++i) {
            const QString label = QStringLiteral("A%1").arg(i);
            const int pin = UArduinoPinMap::firmataPinForLabel(label, BoardProfile);
            UArduinoFirmataBatchBuilder::appendPinMode(*PinConfigBatch, pin,
                                                       UArduinoFirmataBatchBuilder::kModeAnalog);
        }
        ReportAnalogEnable = true;
        AutoRefreshPins = true;
        ApplyPinConfig = true;
    } else if (preset == QStringLiteral("motor_shield_r3_coast")
               || preset == QStringLiteral("motor_shield_seeed_enable")) {
        UHwSetupDocument doc;
        doc.board = BoardProfile == 1 ? QStringLiteral("mega2560") : QStringLiteral("uno");
        doc.firmwareId = QStringLiteral("standard_firmata");
        doc.stack.append(preset.startsWith(QStringLiteral("motor_shield_seeed"))
                             ? QStringLiteral("motor_shield_seeed_v1")
                             : QStringLiteral("motor_shield_r3"));
        UHardwareCatalog& catalog = UHardwareCatalog::instance();
        if (!catalog.isLoaded())
            catalog.load(nullptr);
        UArduinoFirmataBatchBuilder::clearPinConfig(*PinConfigBatch);
        for (const QString& ch : {QStringLiteral("A"), QStringLiteral("B")}) {
            const auto pins = UArduinoDevicePinResolver::resolve(
                catalog, &doc, QStringLiteral("dc_motor_channel"), QString(), ch, BoardProfile);
            UArduinoFirmataBatchBuilder::appendPinMode(*PinConfigBatch, pins.dirPin,
                                                       UArduinoFirmataBatchBuilder::kModeOutput);
            if (pins.dir2Pin >= 0)
                UArduinoFirmataBatchBuilder::appendPinMode(*PinConfigBatch, pins.dir2Pin,
                                                           UArduinoFirmataBatchBuilder::kModeOutput);
            UArduinoFirmataBatchBuilder::appendPinMode(*PinConfigBatch, pins.pwmPin,
                                                       UArduinoFirmataBatchBuilder::kModePwm);
            if (pins.brakePin >= 0) {
                UArduinoFirmataBatchBuilder::appendPinMode(*PinConfigBatch, pins.brakePin,
                                                           UArduinoFirmataBatchBuilder::kModeOutput);
                UArduinoFirmataBatchBuilder::appendDigital(*DigitalOutputCommands, pins.brakePin, 0);
            }
            if (pins.enablePin >= 0) {
                UArduinoFirmataBatchBuilder::appendPinMode(*PinConfigBatch, pins.enablePin,
                                                           UArduinoFirmataBatchBuilder::kModeOutput);
                UArduinoFirmataBatchBuilder::appendDigital(*DigitalOutputCommands, pins.enablePin, 1);
            }
        }
        ApplyPinConfig = true;
    }
}

void UArduinoFirmata::ApplyHardwareSetupFromCatalog()
{
    UHardwareCatalog& catalog = UHardwareCatalog::instance();
    if (!catalog.isLoaded()) {
        QString err;
        if (!catalog.load(&err)) {
            LastSetupApplyResult = UArduinoPropertyString::toStdProperty(err);
            return;
        }
    }
    UHardwareSetup setup;
    QString load_error;
    const QString path = UArduinoPropertyString::fromStdProperty(*HardwareSetupPath);
    const QString inline_json = UArduinoPropertyString::fromStdProperty(*HardwareSetupJson);
    bool loaded = false;
    if (!path.isEmpty())
        loaded = setup.loadFromFile(path, &load_error);
    else if (!inline_json.isEmpty())
        loaded = setup.loadFromJson(inline_json.toUtf8(), &load_error);
    if (!loaded) {
        LastSetupApplyResult = UArduinoPropertyString::toStdProperty(
            load_error.isEmpty() ? QStringLiteral("No HardwareSetup") : load_error);
        return;
    }
    QVector<UHwIssue> issues;
    setup.validate(catalog, &issues);
    UArduinoFirmataBatchBuilder::clearPinConfig(*PinConfigBatch);
    QStringList unsupported;
    int configured = 0;
    for (const UHwSetupDevice& device : setup.document().devices) {
        const UHwModuleInfo* mod = catalog.module(device.module);
        if (!mod) {
            unsupported.append(device.module);
            continue;
        }
        if (device.module == QLatin1String("dht11") || device.module == QLatin1String("hc_sr04")) {
            unsupported.append(device.module);
            continue;
        }
        const auto pins = UArduinoDevicePinResolver::resolve(
            catalog, &setup.document(), device.module, device.port, device.channel, BoardProfile);
        if (!pins.error.isEmpty()) {
            unsupported.append(device.id + QStringLiteral(":") + pins.error);
            continue;
        }
        if (mod->signalType.contains(QStringLiteral("analog"), Qt::CaseInsensitive)) {
            UArduinoFirmataBatchBuilder::appendPinMode(*PinConfigBatch, pins.signalPin,
                                                       UArduinoFirmataBatchBuilder::kModeAnalog);
            ReportAnalogEnable = true;
        } else if (mod->signalType.contains(QStringLiteral("servo"), Qt::CaseInsensitive)) {
            UArduinoFirmataBatchBuilder::appendPinMode(*PinConfigBatch, pins.signalPin,
                                                       UArduinoFirmataBatchBuilder::kModeServo);
        } else if (mod->signalType.contains(QStringLiteral("motor"), Qt::CaseInsensitive)) {
            UArduinoFirmataBatchBuilder::appendPinMode(*PinConfigBatch, pins.dirPin,
                                                       UArduinoFirmataBatchBuilder::kModeOutput);
            if (pins.dir2Pin >= 0)
                UArduinoFirmataBatchBuilder::appendPinMode(*PinConfigBatch, pins.dir2Pin,
                                                           UArduinoFirmataBatchBuilder::kModeOutput);
            UArduinoFirmataBatchBuilder::appendPinMode(*PinConfigBatch, pins.pwmPin,
                                                       UArduinoFirmataBatchBuilder::kModePwm);
            if (pins.brakePin >= 0)
                UArduinoFirmataBatchBuilder::appendPinMode(*PinConfigBatch, pins.brakePin,
                                                           UArduinoFirmataBatchBuilder::kModeOutput);
            if (pins.enablePin >= 0)
                UArduinoFirmataBatchBuilder::appendPinMode(*PinConfigBatch, pins.enablePin,
                                                           UArduinoFirmataBatchBuilder::kModeOutput);
        } else if (device.role == QLatin1String("actuator")
                   || mod->roles.contains(QStringLiteral("actuator"))) {
            UArduinoFirmataBatchBuilder::appendPinMode(*PinConfigBatch, pins.signalPin,
                                                       UArduinoFirmataBatchBuilder::kModeOutput);
        } else {
            UArduinoFirmataBatchBuilder::appendPinMode(*PinConfigBatch, pins.signalPin,
                                                       UArduinoFirmataBatchBuilder::kModeInput);
        }
        ++configured;
    }
    ApplyPinConfig = true;
    QString result = QStringLiteral("configured=%1").arg(configured);
    if (!unsupported.isEmpty())
        result += QStringLiteral("; unsupported=") + unsupported.join(QLatin1Char(','));
    for (const UHwIssue& issue : issues) {
        if (issue.severity == UHwIssueSeverity::Error)
            result += QStringLiteral("; ") + issue.code;
    }
    LastSetupApplyResult = UArduinoPropertyString::toStdProperty(result);
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

    if (ApplyHardwareSetup) {
        ApplyHardwareSetupFromCatalog();
        ResetEdge(ApplyHardwareSetup);
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

void UArduinoFirmata::PollI2cReadPending()
{
    if (!I2cReadPending)
        return;
    const QByteArray payload = FirmataClient.lastI2cReadData();
    if (payload == LastSeenI2cPayload)
        return;
    LastSeenI2cPayload = payload;
    I2cReadData = UArduinoPropertyString::toStdProperty(hexEncode(payload));
    I2cReadPending = false;
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

    PollI2cReadPending();

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
    FirmataFirmwareVersion = UArduinoPropertyString::toStdProperty(FirmataClient.FirmwareVersion);
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
    CapabilityJson = UArduinoPropertyString::toStdProperty(
        QString::fromUtf8(QJsonDocument(root).toJson(QJsonDocument::Compact)));
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
    PinStatusJson = UArduinoPropertyString::toStdProperty(
        QString::fromUtf8(QJsonDocument(root).toJson(QJsonDocument::Compact)));
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
        const QByteArray payload =
            parseHexBytes(UArduinoPropertyString::fromStdProperty(I2cWriteData));
        FirmataClient.i2cWrite(session(), I2cAddress, payload);
    }
    if (I2cRead) {
        FirmataClient.i2cConfig(session(), 0);
        FirmataClient.i2cReadRequest(session(), I2cAddress, 8);
        LastSeenI2cPayload = FirmataClient.lastI2cReadData();
        I2cReadPending = true;
        I2cReadData = "";
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
