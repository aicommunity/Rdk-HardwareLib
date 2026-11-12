#include "UMcuSerialBoard.h"

#include "Catalog/UHardwareCatalog.h"
#include "Catalog/UHardwareSetup.h"
#include "Transport/UArduinoSerialSession.h"
#include "Transport/UArduinoUploadJob.h"
#include "UArduinoPropertyString.h"
#include "UFirmwareManifest.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutexLocker>
#include <QThread>

namespace RDK {

namespace {

bool uploadUsesSyncPath()
{
    return qEnvironmentVariableIntValue("ARDUINO_SYNC_UPLOAD") != 0;
}

} // namespace

UMcuSerialBoard::UMcuSerialBoard()
    : PortName("PortName", this, &UMcuSerialBoard::SetPortName)
    , BaudRate("BaudRate", this)
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
    , HardwareSetupPath("HardwareSetupPath", this)
    , HardwareSetupJson("HardwareSetupJson", this)
    , HardwareSetupValid("HardwareSetupValid", this)
    , HardwareSetupIssues("HardwareSetupIssues", this)
    , ShowDebug("ShowDebug", this)
{
}

UMcuSerialBoard::~UMcuSerialBoard()
{
    AUnInit();
}

void UMcuSerialBoard::ResetEdge(bool& flag)
{
    if (flag)
        flag = false;
}

bool UMcuSerialBoard::SetPortName(const string& value)
{
    Q_UNUSED(value);
    CloseConnection();
    Ready = false;
    PortChanged = true;
    return true;
}

int UMcuSerialBoard::DefaultBaudRate() const
{
    return 57600;
}

void UMcuSerialBoard::ConfigureSerialSession(UArduinoSerialSession& session)
{
    Q_UNUSED(session);
}

QString UMcuSerialBoard::ResolveFirmwarePath() const
{
    if (!FirmwarePath->empty()) {
        const QString resolved = UFirmwareManifest::resolveFromApplicationDir(
            UArduinoPropertyString::fromStdProperty(*FirmwarePath));
        if (QFile::exists(resolved))
            return resolved;
    }
    return QString();
}

bool UMcuSerialBoard::ADefault()
{
    PortName = "";
    BaudRate = DefaultBaudRate();
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
    FirmwarePath = "";
    BundledFirmwareId = "";
    UploadFirmwareFlag = false;
    UploadProgress = 0;
    UploadLastResult = "";
    HardwareSetupPath = "";
    HardwareSetupJson = "";
    HardwareSetupValid = true;
    HardwareSetupIssues = "";
    ShowDebug = false;
    SyncDerivedStates();
    return true;
}

bool UMcuSerialBoard::ABuild()
{
    PortChanged = false;
    if (ConnectOnBuild && !PortName->empty())
        EnsureConnected();
    SyncDerivedStates();
    return true;
}

bool UMcuSerialBoard::AReset()
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

void UMcuSerialBoard::AInit()
{
}

void UMcuSerialBoard::AUnInit()
{
    requestCancelUpload();
    if (UploadThread) {
        UploadThread->wait(30000);
        finishUploadThread();
    }
    CloseConnection();
    delete Session;
    Session = nullptr;
}

UArduinoSerialSession* UMcuSerialBoard::session()
{
    if (!Session) {
        QObject* parent = QCoreApplication::instance();
        Session = new UArduinoSerialSession(parent);
    }
    return Session;
}

void UMcuSerialBoard::TouchActivity()
{
    LastActivityMs = QDateTime::currentMSecsSinceEpoch();
}

void UMcuSerialBoard::SyncDerivedStates()
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

void UMcuSerialBoard::ProcessBoardEdges()
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

void UMcuSerialBoard::requestCancelUpload()
{
    if (UploadJob && !UploadJob->finished.load()) {
        UploadJob->cancelRequested.store(true);
        UploadLastResult = UArduinoPropertyString::toStdProperty(QStringLiteral("cancelling"));
        SyncDerivedStates();
    }
}

bool UMcuSerialBoard::EnsureConnected()
{
    if (PortName->empty()) {
        ConnectionState = ArduinoError;
        LastError = "PortName is empty";
        return false;
    }

    ConnectionState = ArduinoOpening;
    UArduinoSerialSession* s = session();
    s->ShowDebug = ShowDebug;
    ConfigureSerialSession(*s);
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

void UMcuSerialBoard::CloseConnection()
{
    if (Session)
        Session->close();
    ConnectionState = ArduinoDisconnected;
}

void UMcuSerialBoard::RunUploadBlocking()
{
    UploadProgress = 0;
    UploadLastResult = UArduinoPropertyString::toStdProperty(
        QStringLiteral("Upload not supported on this board class"));
    LastError = *UploadLastResult;
    SyncDerivedStates();
}

void UMcuSerialBoard::startUploadAsync()
{
    RunUploadBlocking();
}

void UMcuSerialBoard::PollUploadJob()
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

void UMcuSerialBoard::finishUploadThread()
{
    if (UploadThread) {
        UploadThread->deleteLater();
        UploadThread = nullptr;
    }
}

void UMcuSerialBoard::RunUpload()
{
    RunUploadBlocking();
}

void UMcuSerialBoard::HeartbeatTick()
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

void UMcuSerialBoard::OnHealthCheck()
{
    if (Session && Session->isOpen())
        TouchActivity();
}

void UMcuSerialBoard::OnBoardCalculate()
{
}

void UMcuSerialBoard::RefreshHardwareSetup()
{
    const QString path = UArduinoPropertyString::fromStdProperty(*HardwareSetupPath);
    const QString inline_json = UArduinoPropertyString::fromStdProperty(*HardwareSetupJson);
    if (path.isEmpty() && inline_json.isEmpty()) {
        HardwareSetupValid = true;
        HardwareSetupIssues = "";
        return;
    }

    QString catalog_error;
    if (!UHardwareCatalog::instance().isLoaded()
        && !UHardwareCatalog::instance().load(&catalog_error)) {
        HardwareSetupValid = false;
        HardwareSetupIssues = UArduinoPropertyString::toStdProperty(catalog_error);
        return;
    }

    UHardwareSetup setup;
    QString load_error;
    const bool loaded = !inline_json.isEmpty()
                            ? setup.loadFromJson(inline_json.toUtf8(), &load_error)
                            : setup.loadFromFile(path, &load_error);
    if (!loaded) {
        HardwareSetupValid = false;
        HardwareSetupIssues = UArduinoPropertyString::toStdProperty(load_error);
        return;
    }

    QVector<UHwIssue> issues;
    const bool valid = setup.validate(UHardwareCatalog::instance(), &issues);
    HardwareSetupValid = valid;
    QJsonArray arr;
    for (const UHwIssue& issue : issues) {
        QJsonObject o;
        o.insert(QStringLiteral("severity"),
                 issue.severity == UHwIssueSeverity::Error ? QStringLiteral("error")
                                                            : QStringLiteral("warning"));
        o.insert(QStringLiteral("code"), issue.code);
        o.insert(QStringLiteral("message"), issue.message);
        arr.append(o);
    }
    HardwareSetupIssues =
        UArduinoPropertyString::toStdProperty(QString::fromUtf8(QJsonDocument(arr).toJson(QJsonDocument::Compact)));

    if (!setup.document().firmwareId.isEmpty()
        && setup.document().firmwareId
            != UArduinoPropertyString::fromStdProperty(*BundledFirmwareId)) {
        // Drift is informational only (no auto-change of BundledFirmwareId).
    }
}

bool UMcuSerialBoard::ACalculate()
{
    PollUploadJob();
    SyncDerivedStates();
    ProcessBoardEdges();
    RefreshHardwareSetup();

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
