#ifndef UARDUINOFLASHER_H
#define UARDUINOFLASHER_H

#include <atomic>

#include <QObject>
#include <QString>

#include "UArduinoBoardProfile.h"

namespace RDK {

class UArduinoFlasher : public QObject {
    Q_OBJECT
public:
    explicit UArduinoFlasher(QObject* parent = nullptr);

    static QString locateAvrdudeBinary();
    static QString locateAvrdudeConf();
    static QString buildCommand(const UArduinoBoardProfile& profile,
                                const QString& port,
                                const QString& hexPath,
                                const QString& confPath);

    /// Optional cancelFlag (AU-11): when set true, avrdude is killed and flash returns false.
    bool flash(const UArduinoBoardProfile& profile,
               const QString& port,
               const QString& hexPath,
               QString* errorOut = nullptr,
               std::atomic<bool>* cancelFlag = nullptr);

    void requestCancel();

signals:
    void progressChanged(int percent);
    void finished(bool success, const QString& message);

private:
    std::atomic<bool> m_localCancel{false};
};

} // namespace RDK

#endif
