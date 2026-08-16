#ifndef UARDUINODEVICEPINRESOLVER_H
#define UARDUINODEVICEPINRESOLVER_H

#include "Catalog/UHardwareCatalogTypes.h"

#include <QString>
#include <QStringList>

namespace RDK {

class UHardwareCatalog;

struct UResolvedDevicePins {
    int signalPin = -1;
    int pwmPin = -1;
    int dirPin = -1;
    int dir2Pin = -1;
    int brakePin = -1;
    int sensePin = -1;
    int enablePin = -1;
    QStringList occupiedLabels;
    QString error;
};

class UArduinoDevicePinResolver {
public:
    static UResolvedDevicePins resolve(const UHardwareCatalog& catalog,
                                       const UHwSetupDocument* setup_or_null,
                                       const QString& module_id,
                                       const QString& port,
                                       const QString& channel,
                                       int board_profile);
};

} // namespace RDK

#endif
