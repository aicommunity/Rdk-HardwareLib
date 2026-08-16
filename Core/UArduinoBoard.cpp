#include "UArduinoBoard.h"

#include "Transport/UArduinoBoardProfile.h"
#include "Transport/UArduinoFlasher.h"
#include "Transport/UArduinoSerialSession.h"
#include "Transport/UArduinoUploadJob.h"
#include "UArduinoPropertyString.h"
#include "UFirmwareManifest.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QFile>
#include <QMutexLocker>
#include <QThread>

#include <memory>

namespace RDK {

namespace {

bool uploadUsesSyncPath()
{
    return qEnvironmentVariableIntValue("ARDUINO_SYNC_UPLOAD") != 0;
}

} // namespace

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
    , CancelUpload("CancelUpload", this)
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
    CancelUpload = false;
    ClearLastError = false;
    ConnectionState = ArduinoDisconnected;
    LastError = "";
    LastActivityMs = 0;
    HeartbeatEnabled = true;
    HeartbeatIntervalMs = 3000;
    HeartbeatTimeoutMs = 10000;
    MissedHeartbeats = 0;
    RequestHealthCheck = false;
    FirmwarePath = UArduinoPropertyString::toStdProperty(
        UFirmwareManifest::bundledHexRelativePath(QStringLiteral("sensor_lab_v1"), 0));
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
    CancelUpload = false;
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
    requestCancelUpload();
    if (UploadThread) {
        UploadThread->wait(30000);
        finishUploadThread();
    }
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
    const bool job_active = UploadJob && !UploadJob->finished.load();
    IsUploading = job_active || (UploadProgress > 0 && UploadProgress < 100);
    UploadComplete = (*UploadLastResult == std::string("ok"));
}

void UArduinoBoard::ProcessBoardEdges()
{
    if (ClearLastError) {
        LastError = "";
        ResetEdge(ClearLastError);
    }

    if (CancelUpload) {
        requestCancelUpload();
        ResetEdge(CancelUpload);
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
        ResetEdge(UploadFirmware);
        UploadFirmwareFlag = false;
        if (UploadJob && !UploadJob->finished.load()) {
            UploadLastResult = UArduinoPropertyString::toStdProperty(
                QStringLiteral("Upload already in progress"));
        } else if (uploadUsesSyncPath()) {
            RunUploadBlocking();
        } else {
            startUploadAsync();
        }
    }
}

void UArduinoBoard::requestCancelUpload()
{
    if (UploadJob && !UploadJob->finished.load()) {
        UploadJob->cancelRequested.store(true);
        UploadLastResult = UArduinoPropertyString::toStdProperty(QStringLiteral("cancelling"));
        SyncDerivedStates();
    }
    if (Flasher)
        Flasher->requestCancel();
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
    const QString port = UArduinoPropertyString::fromStdProperty(*PortName);
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
    LastError = err.isEmpty() ? std::string("Failed to open serial port")
                              : UArduinoPropertyString::toStdProperty(err);
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
        const QString resolved = UFirmwareManifest::resolveFromApplicationDir(
            UArduinoPropertyString::fromStdProperty(*FirmwarePath));
        if (QFile::exists(resolved))
            return resolved;
    }
    return UFirmwareManifest::resolveBundledHex(
        UArduinoPropertyString::fromStdProperty(*BundledFirmwareId), BoardProfile);
}

void UArduinoBoard::RunUploadBlocking()
{
    const QString hex = ResolveHexPath();
    if (hex.isEmpty()) {
        UploadLastResult = "No firmware path resolved";
        UploadProgress = 0;
        SyncDerivedStates();
        return;
    }

    const QString validation_err =
        UArduinoBoardProfileUtil::validateUploadTargets(BoardProfile, hex);
    if (!validation_err.isEmpty()) {
        UploadProgress = 0;
        UploadLastResult = UArduinoPropertyString::toStdProperty(validation_err);
        SyncDerivedStates();
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
    const QString port = UArduinoPropertyString::fromStdProperty(*PortName);

    const QMetaObject::Connection progressConn = QObject::connect(
        Flasher,
        &UArduinoFlasher::progressChanged,
        Flasher,
        [this](int percent) { UploadProgress = percent; });

    QString err;
    const bool ok = Flasher->flash(profile, port, hex, &err, nullptr);
    QObject::disconnect(progressConn);
    UploadProgress = ok ? 100 : 0;
    UploadLastResult =
        ok ? std::string("ok") : UArduinoPropertyString::toStdProperty(err);

    if (ConnectOnBuild && ok)
        EnsureConnected();
    SyncDerivedStates();
}

void UArduinoBoard::startUploadAsync()
{
    const QString hex = ResolveHexPath();
    if (hex.isEmpty()) {
        UploadLastResult = "No firmware path resolved";
        UploadProgress = 0;
        SyncDerivedStates();
        return;
    }

    const QString validation_err =
        UArduinoBoardProfileUtil::validateUploadTargets(BoardProfile, hex);
    if (!validation_err.isEmpty()) {
        UploadProgress = 0;
        UploadLastResult = UArduinoPropertyString::toStdProperty(validation_err);
        SyncDerivedStates();
        return;
    }

    CloseConnection();

    UploadJob = std::make_unique<UArduinoUploadJobState>();
    UploadJob->running = true;
    UploadJob->progress = 5;
    {
        QMutexLocker lock(&UploadJob->messageMutex);
        UploadJob->statusMessage = QStringLiteral("Preparing upload\u2026");
    }
    UploadProgress = 5;
    UploadLastResult = UArduinoPropertyString::toStdProperty(QStringLiteral("uploading"));
    SyncDerivedStates();

    const UArduinoBoardProfile profile =
        UArduinoBoardProfileUtil::profileForKind(BoardProfile);
    const QString port = UArduinoPropertyString::fromStdProperty(*PortName);

    UArduinoUploadJobState* job_ptr = UploadJob.get();
    UploadThread = QThread::create([job_ptr, profile, port, hex]() {
        UArduinoUploadJob::runSync(job_ptr, profile, port, hex);
    });
    QObject::connect(UploadThread, &QThread::finished, UploadThread, [this]() {
        finishUploadThread();
    });
    UploadThread->start();
}

void UArduinoBoard::PollUploadJob()
{
    if (!UploadJob)
        return;

    if (!UploadJob->finished.load()) {
        UploadProgress = UploadJob->progress.load();
        {
            QMutexLocker lock(&UploadJob->messageMutex);
            if (!UploadJob->statusMessage.isEmpty())
                UploadLastResult =
                    UArduinoPropertyString::toStdProperty(UploadJob->statusMessage);
        }
        SyncDerivedStates();
        return;
    }

    const bool ok = UploadJob->success.load();
    UploadProgress = ok ? 100 : 0;
    QString finalMsg;
    {
        QMutexLocker lock(&UploadJob->messageMutex);
        finalMsg = ok ? QStringLiteral("ok") : UploadJob->errorMessage;
    }
    UploadLastResult = UArduinoPropertyString::toStdProperty(finalMsg);
    if (ConnectOnBuild && ok)
        EnsureConnected();
    UploadJob.reset();
    SyncDerivedStates();
}

void UArduinoBoard::finishUploadThread()
{
    if (UploadThread) {
        UploadThread->deleteLater();
        UploadThread = nullptr;
    }
}

void UArduinoBoard::RunUpload()
{
    RunUploadBlocking();
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
    PollUploadJob();
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
