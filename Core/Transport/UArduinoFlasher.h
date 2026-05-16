#ifndef UARDUINOFLASHER_H
#define UARDUINOFLASHER_H

#include <QObject>
#include <QString>

#include "UArduinoBoardProfile.h"

namespace RDK {

class UArduinoFlasher : public QObject {
    Q_OBJECT
public:
    explicit UArduinoFlasher(QObject* parent = nullptr);

    static QString locateAvrdudeConf();
    static QString buildCommand(const UArduinoBoardProfile& profile,
                                const QString& port,
                                const QString& hexPath,
                                const QString& confPath);

    bool flash(const UArduinoBoardProfile& profile,
               const QString& port,
               const QString& hexPath,
               QString* errorOut = nullptr);

signals:
    void progressChanged(int percent);
    void finished(bool success, const QString& message);

private:
    static QString locateAvrdudeBinary();
};

} // namespace RDK

#endif
