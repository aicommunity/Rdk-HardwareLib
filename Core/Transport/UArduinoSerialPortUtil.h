#ifndef UARDUINOSERIALPORTUTIL_H
#define UARDUINOSERIALPORTUTIL_H

#include <QString>
#include <QStringList>

namespace RDK {

class UArduinoSerialPortUtil {
public:
    /** Device path for QSerialPort / avrdude (-P). Prefers /dev/... on Linux. */
    static QString normalizeDevicePath(const QString& portName);
    static QString devicePathFromPortName(const QString& portName);
    static QStringList listAvailableDevicePaths();
};

} // namespace RDK

#endif
