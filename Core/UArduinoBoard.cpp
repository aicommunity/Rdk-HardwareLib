#include "UArduinoBoard.h"

#include "Transport/UArduinoBoardProfile.h"
#include "Transport/UArduinoFlasher.h"
#include "Transport/UArduinoSerialSession.h"
#include "UFirmwareManifest.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QThread>

namespace RDK {

UArduinoBoard::UArduinoBoard()
    : PortName("PortName", this, &UArduinoBoard::SetPortName)
    , BaudRate("BaudRate", this)
    , BoardProfile("BoardProfile", this)
    , AutoReconnect("AutoReconnect", this)
    , ConnectOnBuild("ConnectOnBuild", this)
    , Connect("Connect", this)
    , Disconnect("Disconnect", this)
    , Reconnect("Reconnect", this)
    , UploadFirmware("UploadFirmware", this)
    , ClearLastError("ClearLastError", this)
    , ConnectionState("ConnectionState", this)
    , LastError("LastError", this)
    , LastActivityMs("LastActivityMs", this)
    , IsConnected("IsConnected", this)
    , IsOpening("IsOpening", this)
    , HasError("HasError", this)
    , IsDisconnected("IsDisconnected", this)
    , IsUploading("IsUploading", this)
    , UploadComplete("UploadComplete", this)
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

void UArduinoBoard::ResetEdge(bool& flag)
{
    if (flag)
        flag = false;
}

bool UArduinoBoard::SetPortName(const string& value)
{
    Q_UNUSED(value);
    CloseConnection();
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
    Connect = false;
    Disconnect = false;
    Reconnect = false;
    UploadFirmware = false;
    ClearLastError = false;
    ConnectionState = ArduinoDisconnected;
    LastError = "";
    LastActivityMs = 0;
    HeartbeatEnabled = true;
    HeartbeatIntervalMs = 3000;
    HeartbeatTimeoutMs = 10000;
    MissedHeartbeats = 0;
    RequestHealthCheck = false;
    FirmwarePath = UFirmwareManifest::bundledHexRelativePath(QStringLiteral("sensor_lab_v1"), 0)
                       .toStdString();
    BundledFirmwareId = "sensor_lab_v1";
    UploadFirmwareFlag = false;
    UploadProgress = 0;
    UploadLastResult = "";
    ShowDebug = false;
    SyncDerivedStates();
    return true;
}

bool UArduinoBoard::ABuild()
{
    PortChanged = false;
    if (ConnectOnBuild && !PortName->empty())
        EnsureConnected();
    SyncDerivedStates();
    return true;
}

bool UArduinoBoard::AReset()
{
    RequestHealthCheck = false;
    UploadFirmwareFlag = false;
    UploadFirmware = false;
    Connect = false;
    Disconnect = false;
    Reconnect = false;
    ClearLastError = false;
    return true;
}

void UArduinoBoard::AInit()
{
}

void UArduinoBoard::AUnInit()
{
    CloseConnection();
    delete Session;
    Session = nullptr;
    delete Flasher;
    Flasher = nullptr;
}

UArduinoSerialSession* UArduinoBoard::session()
{
    if (!Session) {
        QObject* parent = QCoreApplication::instance();
        Session = new UArduinoSerialSession(parent);
        // RX is pulled in ACalculate (engine thread). Do not connect bytesReceived to UNet.
    }
    return Session;
}

void UArduinoBoard::TouchActivity()
{
    LastActivityMs = QDateTime::currentMSecsSinceEpoch();
}

void UArduinoBoard::SyncDerivedStates()
{
    const int state = ConnectionState;
    IsConnected = (state == ArduinoConnected);
    IsOpening = (state == ArduinoOpening);
    HasError = (state == ArduinoError);
    IsDisconnected = (state == ArduinoDisconnected);
    IsUploading = (UploadProgress > 0 && UploadProgress < 100);
    UploadComplete = (*UploadLastResult == std::string("ok"));
}

void UArduinoBoard::ProcessBoardEdges()
{
    if (ClearLastError) {
        LastError = "";
        ResetEdge(ClearLastError);
    }

    if (Disconnect) {
        CloseConnection();
        ResetEdge(Disconnect);
    }

    if (Reconnect) {
        CloseConnection();
        if (!PortName->empty())
            EnsureConnected();
        ResetEdge(Reconnect);
    }

    if (Connect) {
        if (!PortName->empty())
            EnsureConnected();
        ResetEdge(Connect);
    }

    const bool uploadRequested = UploadFirmware || UploadFirmwareFlag;
    if (uploadRequested) {
        RunUpload();
        ResetEdge(UploadFirmware);
        UploadFirmwareFlag = false;
    }
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
    s->ShowDebug = ShowDebug;
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
    const QString err = s->lastError();
    LastError = err.isEmpty() ? "Failed to open serial port" : err.toStdString();
    return false;
}

void UArduinoBoard::CloseConnection()
{
    if (Session)
        Session->close();
    ConnectionState = ArduinoDisconnected;
}

QString UArduinoBoard::ResolveHexPath() const
{
    if (!FirmwarePath->empty()) {
        const QString resolved =
            UFirmwareManifest::resolveFromApplicationDir(QString::fromStdString(*FirmwarePath));
        if (QFile::exists(resolved))
            return resolved;
    }
    return UFirmwareManifest::resolveBundledHex(QString::fromStdString(*BundledFirmwareId), BoardProfile);
}

void UArduinoBoard::RunUpload()
{
    const QString hex = ResolveHexPath();
    if (hex.isEmpty()) {
        UploadLastResult = "No firmware path resolved";
        UploadProgress = 0;
        return;
    }

    if (!Flasher) {
        QObject* parent = QCoreApplication::instance();
        Flasher = new UArduinoFlasher(parent);
    }

    CloseConnection();
    QThread::msleep(400);

    UploadProgress = 10;

    const UArduinoBoardProfile profile =
        UArduinoBoardProfileUtil::profileForKind(BoardProfile);
    const QString port = QString::fromStdString(*PortName);

    const QMetaObject::Connection progressConn = QObject::connect(
        Flasher,
        &UArduinoFlasher::progressChanged,
        Flasher,
        [this](int percent) { UploadProgress = percent; });

    QString err;
    const bool ok = Flasher->flash(profile, port, hex, &err);
    QObject::disconnect(progressConn);
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
            CloseConnection();
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
    SyncDerivedStates();
    ProcessBoardEdges();

    if (PortChanged) {
        CloseConnection();
        if (!PortName->empty())
            EnsureConnected();
        PortChanged = false;
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
    SyncDerivedStates();
    return true;
}

} // namespace RDK
