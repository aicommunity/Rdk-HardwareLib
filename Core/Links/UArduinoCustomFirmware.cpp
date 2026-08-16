#include "UArduinoCustomFirmware.h"

#include "UArduinoPropertyString.h"
#include "UFirmwareManifest.h"
#include "Protocol/UArduinoProtocolPluginRegistry.h"

#include <QJsonDocument>
#include <QJsonObject>

namespace RDK {

UArduinoCustomFirmware::UArduinoCustomFirmware()
    : HostPluginId("HostPluginId", this)
    , FrameLog("FrameLog", this)
    , NamedValuesJson("NamedValuesJson", this)
    , DoubleMatrixReadings("DoubleMatrixReadings", this)
    , ClearFrameLog("ClearFrameLog", this)
    , PluginBound("PluginBound", this)
{
}

UArduinoCustomFirmware::~UArduinoCustomFirmware() = default;

UArduinoCustomFirmware* UArduinoCustomFirmware::New()
{
    return new UArduinoCustomFirmware;
}

bool UArduinoCustomFirmware::ADefault()
{
    UArduinoCustomLink::ADefault();
    HostPluginId = "";
    FrameLog = "";
    NamedValuesJson = "{}";
    DoubleMatrixReadings.Assign(0, 8, 0.0);
    ClearFrameLog = false;
    PluginBound = false;
    BundledFirmwareId = "sensor_lab_v1";
    FirmwarePath = UArduinoPropertyString::toStdProperty(
        UFirmwareManifest::bundledHexRelativePath(QStringLiteral("sensor_lab_v1"), 0));
    NamedValues.clear();
    registerBuiltinArduinoProtocolPlugins();
    return true;
}

IArduinoProtocolPlugin* UArduinoCustomFirmware::resolvePlugin() const
{
    registerBuiltinArduinoProtocolPlugins();
    QString id = UArduinoPropertyString::fromStdProperty(*HostPluginId);
    if (id.isEmpty())
        id = UArduinoPropertyString::fromStdProperty(*BundledFirmwareId);
    return findArduinoProtocolPlugin(id);
}

void UArduinoCustomFirmware::enqueueCommand(const QString& line)
{
    EnqueueCommand(UArduinoPropertyString::toStdProperty(line));
}

int UArduinoCustomFirmware::boardProfile() const
{
    return BoardProfile;
}

int UArduinoCustomFirmware::protocolVersion() const
{
    return ProtocolVersion;
}

void UArduinoCustomFirmware::setProtocolReady(bool ready)
{
    ProtocolNegotiated = ready;
}

void UArduinoCustomFirmware::setLastError(const QString& error)
{
    LastError = UArduinoPropertyString::toStdProperty(error);
}

void UArduinoCustomFirmware::publishSensorMatrixRow(const QVector<double>& row)
{
    if (row.isEmpty())
        return;
    const int r = DoubleMatrixReadings->GetRows();
    const int cols = qMax(row.size(), DoubleMatrixReadings->GetCols());
    DoubleMatrixReadings->Resize(r + 1, cols, 0.0);
    for (int c = 0; c < row.size(); ++c)
        (*DoubleMatrixReadings)(r, c) = row[c];
}

void UArduinoCustomFirmware::publishPinStatusJson(const QString& json)
{
    Q_UNUSED(json);
}

void UArduinoCustomFirmware::publishNamedFloat(const QString& key, float value)
{
    NamedValues.insert(key, value);
    QJsonObject obj;
    for (auto it = NamedValues.constBegin(); it != NamedValues.constEnd(); ++it)
        obj.insert(it.key(), it.value());
    NamedValuesJson =
        UArduinoPropertyString::toStdProperty(QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
}

void UArduinoCustomFirmware::appendFrameLog(const QString& line)
{
    QString log = UArduinoPropertyString::fromStdProperty(*FrameLog);
    if (!log.isEmpty())
        log.append('\n');
    log.append(line);
    while (log.size() > 4096) {
        const int nl = log.indexOf('\n');
        if (nl < 0)
            break;
        log = log.mid(nl + 1);
    }
    FrameLog = UArduinoPropertyString::toStdProperty(log);
}

void UArduinoCustomFirmware::NegotiateProtocol()
{
    IArduinoProtocolPlugin* plugin = resolvePlugin();
    PluginBound = plugin != nullptr;
    if (!plugin) {
        ProtocolNegotiated = false;
        return;
    }
    plugin->negotiate(this, ProtocolVersion);
}

void UArduinoCustomFirmware::OnHealthCheck()
{
    IArduinoProtocolPlugin* plugin = resolvePlugin();
    if (plugin)
        plugin->onHealthCheck(this);
    else
        UArduinoCustomLink::OnHealthCheck();
}

void UArduinoCustomFirmware::OnBinaryFrame(uint8_t type, const QByteArray& payload)
{
    IArduinoProtocolPlugin* plugin = resolvePlugin();
    PluginBound = plugin != nullptr;
    if (plugin) {
        plugin->onBinaryFrame(this, type, payload);
        return;
    }
    appendFrameLog(QStringLiteral("RX type=0x%1 hex=%2")
                       .arg(type, 2, 16, QChar('0'))
                       .arg(QString(payload.toHex(' '))));
}

bool UArduinoCustomFirmware::ACalculate()
{
    if (ClearFrameLog) {
        FrameLog = "";
        ClearFrameLog = false;
    }
    PluginBound = resolvePlugin() != nullptr;
    return UArduinoCustomLink::ACalculate();
}

} // namespace RDK
