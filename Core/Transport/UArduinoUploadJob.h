#ifndef UARDUINOUPLOADJOB_H
#define UARDUINOUPLOADJOB_H

#include <atomic>

#include <QMutex>
#include <QString>

#include "UArduinoBoardProfile.h"

namespace RDK {

class UArduinoFlasher;

struct UArduinoUploadJobState {
    std::atomic<bool> running{false};
    std::atomic<bool> finished{false};
    std::atomic<bool> success{false};
    std::atomic<bool> cancelRequested{false};
    std::atomic<int> progress{0};
    QMutex messageMutex;
    QString statusMessage;
    QString errorMessage;
};

class UArduinoUploadJob {
public:
    /** Owns a local UArduinoFlasher for the worker thread. Cancel via state->cancelRequested. */
    static void runSync(UArduinoUploadJobState* state,
                        const UArduinoBoardProfile& profile,
                        const QString& port,
                        const QString& hexPath);
};

} // namespace RDK

#endif
