#include "UArduinoBoard.h"

#include "Transport/UArduinoBoardProfile.h"
#include "Transport/UArduinoFlasher.h"
#include "Transport/UArduinoSerialSession.h"
#include "Transport/UArduinoUploadJob.h"
#include "UArduinoPropertyString.h"
#include "UFirmwareManifest.h"

#include <QCoreApplication>
#include <QFile>
#include <QMutexLocker>
#include <QThread>

namespace RDK {

UArduinoBoard::UArduinoBoard()
    : BoardProfile("BoardProfile", this)
{
}

UArduinoBoard::~UArduinoBoard()
{
}

void UArduinoBoard::AUnInit()
{
    UMcuSerialBoard::AUnInit();
    delete Flasher;
    Flasher = nullptr;
}

UArduinoBoard* UArduinoBoard::New()
{
    return new UArduinoBoard;
}

int UArduinoBoard::DefaultBaudRate() const
{
    return 57600;
}

void UArduinoBoard::ConfigureSerialSession(UArduinoSerialSession& session)
{
    session.ToggleDtrOnOpen = true;
    session.DtrOnOpen = true;
    session.RtsOnOpen = false;
}

bool UArduinoBoard::ADefault()
{
    UMcuSerialBoard::ADefault();
    BoardProfile = 0;
    FirmwarePath = UArduinoPropertyString::toStdProperty(
        UFirmwareManifest::bundledHexRelativePath(QStringLiteral("sensor_lab_v1"), 0));
    BundledFirmwareId = "sensor_lab_v1";
    SyncDerivedStates();
    return true;
}

QString UArduinoBoard::ResolveHexPath() const
{
    return ResolveFirmwarePath();
}

QString UArduinoBoard::ResolveFirmwarePath() const
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

void UArduinoBoard::requestCancelUpload()
{
    UMcuSerialBoard::requestCancelUpload();
    if (Flasher)
        Flasher->requestCancel();
}

void UArduinoBoard::RunUploadBlocking()
{
    const QString hex = ResolveFirmwarePath();
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
    const QString hex = ResolveFirmwarePath();
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

} // namespace RDK
