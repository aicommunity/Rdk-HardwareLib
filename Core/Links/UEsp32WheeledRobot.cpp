#include "UEsp32WheeledRobot.h"

#include "Protocol/UArduinoProtocolPluginRegistry.h"
#include "UArduinoPropertyString.h"

#include <QJsonDocument>
#include <QJsonObject>

namespace RDK {

class UEsp32WheeledRobot::HostAdapter : public UArduinoPluginHost {
public:
    explicit HostAdapter(UEsp32WheeledRobot* owner)
        : Owner(owner)
    {
    }

    void enqueueCommand(const QString& line) override
    {
        if (Owner)
            Owner->hostEnqueue(line);
    }
    int boardProfile() const override { return -1; }
    int protocolVersion() const override
    {
        return Owner ? Owner->hostProtocolVersion() : 2;
    }
    void setProtocolReady(bool ready) override
    {
        if (Owner)
            Owner->hostSetProtocolReady(ready);
    }
    void setLastError(const QString& error) override
    {
        if (Owner)
            Owner->hostSetLastError(error);
    }
    void publishSensorMatrixRow(const QVector<double>& row) override { Q_UNUSED(row); }
    void publishPinStatusJson(const QString& json) override { Q_UNUSED(json); }
    void publishNamedFloat(const QString& key, float value) override
    {
        if (Owner)
            Owner->onNamedFloat(key, value);
    }
    void appendFrameLog(const QString& line) override { Q_UNUSED(line); }

    UEsp32WheeledRobot* Owner = nullptr;
};

UEsp32WheeledRobot::UEsp32WheeledRobot()
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
    , Host(std::make_unique<HostAdapter>(this))
{
}

UEsp32WheeledRobot::~UEsp32WheeledRobot() = default;

UEsp32WheeledRobot* UEsp32WheeledRobot::New()
{
    return new UEsp32WheeledRobot;
}

UArduinoPluginHost* UEsp32WheeledRobot::pluginHost()
{
    return Host.get();
}

void UEsp32WheeledRobot::hostEnqueue(const QString& line)
{
    EnqueueCommand(UArduinoPropertyString::toStdProperty(line));
}

void UEsp32WheeledRobot::hostSetProtocolReady(bool ready)
{
    ProtocolNegotiated = ready;
}

void UEsp32WheeledRobot::hostSetLastError(const QString& error)
{
    LastError = UArduinoPropertyString::toStdProperty(error);
}

int UEsp32WheeledRobot::hostProtocolVersion() const
{
    return ProtocolVersion;
}

bool UEsp32WheeledRobot::ADefault()
{
    UEsp32CustomLink::ADefault();
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
    BaudRate = 115200;
    BundledFirmwareId = "nmsdk_motor_hub_esp32_v1";
    FirmwarePath = "";
    ProtocolVersion = 2;
    NamedValues.clear();
    registerBuiltinArduinoProtocolPlugins();
    return true;
}

IArduinoProtocolPlugin* UEsp32WheeledRobot::resolvePlugin() const
{
    registerBuiltinArduinoProtocolPlugins();
    return findArduinoProtocolPlugin(QStringLiteral("nmsdk_motor_hub_v1"));
}

void UEsp32WheeledRobot::onNamedFloat(const QString& key, float value)
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

void UEsp32WheeledRobot::ProcessWheeledEdges()
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
            for (const QString& line : UWheeledDriveLogic::buildMotorHubCommands(cmd))
                EnqueueCommand(UArduinoPropertyString::toStdProperty(line));
        }
    }
    if (applyCleared)
        ResetEdge(ApplyDrive);
    if (stopCleared)
        ResetEdge(Stop);
}

void UEsp32WheeledRobot::NegotiateProtocol()
{
    if (ProtocolNegotiated || ConnectionState != ArduinoConnected)
        return;
    IArduinoProtocolPlugin* plugin = resolvePlugin();
    PluginBound = (plugin != nullptr);
    if (plugin)
        plugin->negotiate(pluginHost(), ProtocolVersion);
    else
        UEsp32CustomLink::NegotiateProtocol();
}

void UEsp32WheeledRobot::OnHealthCheck()
{
    IArduinoProtocolPlugin* plugin = resolvePlugin();
    if (plugin)
        plugin->onHealthCheck(pluginHost());
    else
        UEsp32CustomLink::OnHealthCheck();
}

void UEsp32WheeledRobot::OnBinaryFrame(uint8_t type, const QByteArray& payload)
{
    IArduinoProtocolPlugin* plugin = resolvePlugin();
    if (plugin)
        plugin->onBinaryFrame(pluginHost(), type, payload);
}

bool UEsp32WheeledRobot::ACalculate()
{
    ProcessWheeledEdges();
    return UEsp32CustomLink::ACalculate();
}

} // namespace RDK
