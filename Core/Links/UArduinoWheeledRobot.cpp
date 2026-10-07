#include "UArduinoWheeledRobot.h"

#include "Protocol/UArduinoProtocolPluginRegistry.h"
#include "UArduinoPropertyString.h"
#include "UFirmwareManifest.h"

#include <QJsonDocument>
#include <QJsonObject>

namespace RDK {

UArduinoWheeledRobot::UArduinoWheeledRobot()
    : LeftPwm("LeftPwm", this)
    , RightPwm("RightPwm", this)
    , LeftDir("LeftDir", this)
    , RightDir("RightDir", this)
    , ApplyDrive("ApplyDrive", this)
    , Stop("Stop", this)
    , WatchdogMs("WatchdogMs", this)
    , MotorDriverId("MotorDriverId", this)
    , ApplyMotorDriver("ApplyMotorDriver", this)
    , LeftPwmFb("LeftPwmFb", this)
    , RightPwmFb("RightPwmFb", this)
    , LeftSense("LeftSense", this)
    , RightSense("RightSense", this)
    , HostPluginId("HostPluginId", this)
    , NamedValuesJson("NamedValuesJson", this)
    , PluginBound("PluginBound", this)
{
}

UArduinoWheeledRobot::~UArduinoWheeledRobot() = default;

UArduinoWheeledRobot* UArduinoWheeledRobot::New()
{
    return new UArduinoWheeledRobot;
}

bool UArduinoWheeledRobot::ADefault()
{
    UArduinoCustomLink::ADefault();
    LeftPwm = 0;
    RightPwm = 0;
    LeftDir = 1;
    RightDir = 1;
    ApplyDrive = false;
    Stop = false;
    WatchdogMs = 2000;
    MotorDriverId = "motor_shield_r3";
    ApplyMotorDriver = false;
    LeftPwmFb = 0;
    RightPwmFb = 0;
    LeftSense = 0;
    RightSense = 0;
    HostPluginId = "nmsdk_motor_hub_v1";
    NamedValuesJson = "{}";
    PluginBound = false;
    BaudRate = 57600;
    BundledFirmwareId = "nmsdk_motor_hub_v1";
    FirmwarePath = UArduinoPropertyString::toStdProperty(
        UFirmwareManifest::bundledHexRelativePath(QStringLiteral("nmsdk_motor_hub_v1"), 0));
    ProtocolVersion = 2;
    NamedValues.clear();
    registerBuiltinArduinoProtocolPlugins();
    return true;
}

IArduinoProtocolPlugin* UArduinoWheeledRobot::resolvePlugin() const
{
    registerBuiltinArduinoProtocolPlugins();
    QString id = UArduinoPropertyString::fromStdProperty(*HostPluginId);
    if (id.isEmpty())
        id = QStringLiteral("nmsdk_motor_hub_v1");
    return findArduinoProtocolPlugin(id);
}

void UArduinoWheeledRobot::enqueueCommand(const QString& line)
{
    EnqueueCommand(UArduinoPropertyString::toStdProperty(line));
}

int UArduinoWheeledRobot::boardProfile() const
{
    return BoardProfile;
}

int UArduinoWheeledRobot::protocolVersion() const
{
    return ProtocolVersion;
}

void UArduinoWheeledRobot::setProtocolReady(bool ready)
{
    ProtocolNegotiated = ready;
}

void UArduinoWheeledRobot::setLastError(const QString& error)
{
    LastError = UArduinoPropertyString::toStdProperty(error);
}

void UArduinoWheeledRobot::publishSensorMatrixRow(const QVector<double>& row)
{
    Q_UNUSED(row);
}

void UArduinoWheeledRobot::publishPinStatusJson(const QString& json)
{
    Q_UNUSED(json);
}

void UArduinoWheeledRobot::publishNamedFloat(const QString& key, float value)
{
    NamedValues.insert(key, value);
    if (key == QLatin1String("left_pwm"))
        LeftPwmFb = static_cast<int>(value);
    else if (key == QLatin1String("right_pwm"))
        RightPwmFb = static_cast<int>(value);
    else if (key == QLatin1String("left_sense"))
        LeftSense = value;
    else if (key == QLatin1String("right_sense"))
        RightSense = value;
    QJsonObject obj;
    for (auto it = NamedValues.constBegin(); it != NamedValues.constEnd(); ++it)
        obj.insert(it.key(), it.value());
    NamedValuesJson =
        UArduinoPropertyString::toStdProperty(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
}

void UArduinoWheeledRobot::appendFrameLog(const QString& line)
{
    Q_UNUSED(line);
}

void UArduinoWheeledRobot::emitDrive(const UWheeledDriveCommand& cmd)
{
    for (const QString& line : UWheeledDriveLogic::buildMotorHubCommands(cmd))
        EnqueueCommand(UArduinoPropertyString::toStdProperty(line));
}

void UArduinoWheeledRobot::ProcessWheeledEdges()
{
    if (ApplyMotorDriver) {
        for (const QString& line :
             UWheeledDriveLogic::buildSetPinCommands(UArduinoPropertyString::fromStdProperty(*MotorDriverId)))
            EnqueueCommand(UArduinoPropertyString::toStdProperty(line));
        EnqueueCommand(UArduinoPropertyString::toStdProperty(
            QStringLiteral("WATCHDOG %1").arg(static_cast<int>(WatchdogMs))));
        ResetEdge(ApplyMotorDriver);
    }

    UWheeledDriveCommand cmd;
    cmd.leftPwm = LeftPwm;
    cmd.rightPwm = RightPwm;
    cmd.leftDir = LeftDir;
    cmd.rightDir = RightDir;
    bool applyCleared = false;
    bool stopCleared = false;
    if (UWheeledDriveLogic::processEdges(ApplyDrive, Stop, &cmd, &applyCleared, &stopCleared)) {
        if (stopCleared) {
            for (const QString& line : UWheeledDriveLogic::buildMotorStopCommands())
                EnqueueCommand(UArduinoPropertyString::toStdProperty(line));
            LeftPwm = 0;
            RightPwm = 0;
        } else if (applyCleared) {
            emitDrive(cmd);
        }
    }
    if (applyCleared)
        ResetEdge(ApplyDrive);
    if (stopCleared)
        ResetEdge(Stop);
}

void UArduinoWheeledRobot::NegotiateProtocol()
{
    if (ProtocolNegotiated || ConnectionState != ArduinoConnected)
        return;
    IArduinoProtocolPlugin* plugin = resolvePlugin();
    PluginBound = (plugin != nullptr);
    if (plugin)
        plugin->negotiate(this, ProtocolVersion);
    else
        UArduinoCustomLink::NegotiateProtocol();
}

void UArduinoWheeledRobot::OnHealthCheck()
{
    IArduinoProtocolPlugin* plugin = resolvePlugin();
    if (plugin)
        plugin->onHealthCheck(this);
    else
        UArduinoCustomLink::OnHealthCheck();
}

void UArduinoWheeledRobot::OnBinaryFrame(uint8_t type, const QByteArray& payload)
{
    IArduinoProtocolPlugin* plugin = resolvePlugin();
    if (plugin)
        plugin->onBinaryFrame(this, type, payload);
}

bool UArduinoWheeledRobot::ACalculate()
{
    ProcessWheeledEdges();
    return UArduinoCustomLink::ACalculate();
}

} // namespace RDK
