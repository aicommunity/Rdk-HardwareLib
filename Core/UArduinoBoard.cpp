#include "UArduinoBoard.h"

#include "Transport/UArduinoBoardProfile.h"
#include "Transport/UArduinoFlasher.h"
#include "Transport/UArduinoSerialSession.h"
#include "UFirmwareManifest.h"

#include <QDateTime>
#include <QDebug>

namespace RDK {

UArduinoBoard::UArduinoBoard()
    : PortName("PortName", this, &UArduinoBoard::SetPortName)
    , BaudRate("BaudRate", this)
    , BoardProfile("BoardProfile", this)
    , AutoReconnect("AutoReconnect", this)
    , ConnectOnBuild("ConnectOnBuild", this)
    , ConnectionState("ConnectionState", this)
    , LastError("LastError", this)
    , LastActivityMs("LastActivityMs", this)
    , HeartbeatEnabled("HeartbeatEnabled", this)
    , HeartbeatIntervalMs("HeartbeatIntervalMs", this)
    , HeartbeatTimeoutMs("HeartbeatTimeoutMs", this)
    , MissedHeartbeats("MissedHeartbeats", this)
    , RequestHealthCheck("RequestHealthCheck", this)
    , FirmwarePath("FirmwarePath", this)
    , BundledFirmwareId("BundledFirmwareId", this)
    , UploadFirmwareFlag("UploadFirmwareFlag", this)
    , UploadProgress("UploadProgress", this)
    , UploadLastResult("UploadLastResult", this)
    , ShowDebug("ShowDebug", this)
{
}

UArduinoBoard::~UArduinoBoard()
{
    AUnInit();
}

UArduinoBoard* UArduinoBoard::New()
{
    return new UArduinoBoard;
}

bool UArduinoBoard::SetPortName(const string& value)
{
    Q_UNUSED(value);
    Disconnect();
    Ready = false;
    PortChanged = true;
    return true;
}

bool UArduinoBoard::ADefault()
{
    PortName = "";
    BaudRate = 57600;
    BoardProfile = 0;
    AutoReconnect = false;
    ConnectOnBuild = true;
    ConnectionState = ArduinoDisconnected;
    LastError = "";
    LastActivityMs = 0;
    HeartbeatEnabled = true;
    HeartbeatIntervalMs = 3000;
    HeartbeatTimeoutMs = 10000;
    MissedHeartbeats = 0;
    RequestHealthCheck = false;
    FirmwarePath = "";
    BundledFirmwareId = "sensor_lab_v1";
    UploadFirmwareFlag = false;
    UploadProgress = 0;
    UploadLastResult = "";
    ShowDebug = false;
    return true;
}

bool UArduinoBoard::ABuild()
{
    PortChanged = false;
    if (ConnectOnBuild && !PortName->empty())
        EnsureConnected();
    return true;
}

bool UArduinoBoard::AReset()
{
    RequestHealthCheck = false;
    UploadFirmwareFlag = false;
    return true;
}

void UArduinoBoard::AInit()
{
}

void UArduinoBoard::AUnInit()
{
    Disconnect();
    delete Session;
    Session = nullptr;
    delete Flasher;
    Flasher = nullptr;
}

UArduinoSerialSession* UArduinoBoard::session()
{
    if (!Session)
        Session = new UArduinoSerialSession();
    return Session;
}

void UArduinoBoard::TouchActivity()
{
    LastActivityMs = QDateTime::currentMSecsSinceEpoch();
}

bool UArduinoBoard::EnsureConnected()
{
    if (PortName->empty()) {
        ConnectionState = ArduinoError;
        LastError = "PortName is empty";
        return false;
    }

    ConnectionState = ArduinoOpening;
    UArduinoSerialSession* s = session();
    s->showDebug = ShowDebug;
    const QString port = QString::fromStdString(*PortName);
    if (s->open(port, BaudRate)) {
        ConnectionState = ArduinoConnected;
        LastError = "";
        TouchActivity();
        LastHealthResponseMs = LastActivityMs;
        ReconnectBackoffMs = 1000;
        return true;
    }

    ConnectionState = ArduinoError;
    LastError = "Failed to open serial port";
    return false;
}

void UArduinoBoard::Disconnect()
{
    if (Session)
        Session->close();
    ConnectionState = ArduinoDisconnected;
}

QString UArduinoBoard::resolveHexPath() const
{
    if (!FirmwarePath->empty())
        return QString::fromStdString(*FirmwarePath);
    return UFirmwareManifest::resolveBundledHex(QString::fromStdString(*BundledFirmwareId), BoardProfile);
}

void UArduinoBoard::RunUpload()
{
    const QString hex = resolveHexPath();
    if (hex.isEmpty()) {
        UploadLastResult = "No firmware path resolved";
        UploadProgress = 0;
        return;
    }

    if (!Flasher)
        Flasher = new UArduinoFlasher();

    Disconnect();
    UploadProgress = 10;

    const UArduinoBoardProfile profile =
        UArduinoBoardProfileUtil::profileForKind(BoardProfile);
    const QString port = QString::fromStdString(*PortName);

    QString err;
    const bool ok = Flasher->flash(profile, port, hex, &err);
    UploadProgress = ok ? 100 : 0;
    UploadLastResult = ok ? "ok" : err.toStdString();

    if (ConnectOnBuild && ok)
        EnsureConnected();
}

void UArduinoBoard::HeartbeatTick()
{
    if (!HeartbeatEnabled || ConnectionState != ArduinoConnected)
        return;

    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (now - LastHeartbeatSentMs < HeartbeatIntervalMs)
        return;

    LastHeartbeatSentMs = now;
    OnHealthCheck();

    if (now - LastHealthResponseMs > HeartbeatTimeoutMs) {
        MissedHeartbeats = MissedHeartbeats + 1;
        if (AutoReconnect) {
            Disconnect();
            EnsureConnected();
        }
    }
}

void UArduinoBoard::OnHealthCheck()
{
    if (Session && Session->isOpen())
        TouchActivity();
}

void UArduinoBoard::OnBoardCalculate()
{
}

bool UArduinoBoard::ACalculate()
{
    if (PortChanged) {
        Disconnect();
        if (!PortName->empty())
            EnsureConnected();
        PortChanged = false;
    }

    if (UploadFirmwareFlag) {
        RunUpload();
        UploadFirmwareFlag = false;
    }

    if (ConnectionState != ArduinoConnected && AutoReconnect && !PortName->empty()) {
        const qint64 now = QDateTime::currentMSecsSinceEpoch();
        if (now - LastReconnectAttemptMs > ReconnectBackoffMs) {
            EnsureConnected();
            LastReconnectAttemptMs = now;
            ReconnectBackoffMs = qMin(ReconnectBackoffMs * 2, 30000);
        }
    }

    HeartbeatTick();

    if (RequestHealthCheck) {
        OnHealthCheck();
        RequestHealthCheck = false;
    }

    OnBoardCalculate();
    return true;
}

} // namespace RDK
