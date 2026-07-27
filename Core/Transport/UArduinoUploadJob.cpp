#include "UArduinoUploadJob.h"

#include "UArduinoFlasher.h"

#include <QMutexLocker>
#include <QThread>

namespace RDK {

namespace {

void setStatus(UArduinoUploadJobState* state, int progress, const QString& message)
{
    if (!state)
        return;
    state->progress = progress;
    QMutexLocker lock(&state->messageMutex);
    state->statusMessage = message;
}

} // namespace

void UArduinoUploadJob::runSync(UArduinoUploadJobState* state,
                                UArduinoFlasher* /*flasher*/,
                                const UArduinoBoardProfile& profile,
                                const QString& port,
                                const QString& hexPath)
{
    if (!state)
        return;

    setStatus(state, 5, QStringLiteral("Preparing upload\u2026"));
    QThread::msleep(400);

    setStatus(state, 8, QStringLiteral("Resetting to bootloader\u2026"));

    UArduinoFlasher flasher;
    const QMetaObject::Connection progressConn = QObject::connect(
        &flasher,
        &UArduinoFlasher::progressChanged,
        &flasher,
        [state](int percent) {
            const int mapped = qBound(15, percent, 99);
            state->progress = mapped;
            QMutexLocker lock(&state->messageMutex);
            state->statusMessage = QStringLiteral("Flashing\u2026 %1%").arg(percent);
        },
        Qt::DirectConnection);

    if (state->cancelRequested.load()) {
        state->success = false;
        state->progress = 0;
        {
            QMutexLocker lock(&state->messageMutex);
            state->errorMessage = QStringLiteral("Upload cancelled");
            state->statusMessage = state->errorMessage;
        }
        state->running = false;
        state->finished = true;
        QObject::disconnect(progressConn);
        return;
    }

    QString err;
    const bool ok = flasher.flash(profile, port, hexPath, &err, &state->cancelRequested);
    QObject::disconnect(progressConn);

    state->success = ok;
    state->progress = ok ? 100 : 0;
    {
        QMutexLocker lock(&state->messageMutex);
        state->errorMessage = err;
        state->statusMessage = ok ? QStringLiteral("ok") : err;
    }
    state->running = false;
    state->finished = true;
}

} // namespace RDK
