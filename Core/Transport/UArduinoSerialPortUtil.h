#ifndef UARDUINOSERIALPORTUTIL_H
#define UARDUINOSERIALPORTUTIL_H

#include <QString>
#include <QStringList>

class QSerialPortInfo;

namespace RDK {

struct UArduinoSerialPortEntry {
    QString devicePath;
    QString displayLabel;
    bool likelyAttachedDevice = false;
};

class UArduinoSerialPortUtil {
public:
    /** Device path for QSerialPort / avrdude (-P). Prefers /dev/... on Linux. */
    static QString normalizeDevicePath(const QString& portName);
    static QString devicePathFromPortName(const QString& portName);
    /** Best name to pass to QSerialPort::open for this port. */
    static QString preferredOpenName(const QString& portName);
    static QStringList listAvailableDevicePaths();
    static QList<UArduinoSerialPortEntry> listPortsSorted();
    static bool portInfoForPath(const QString& portName, QSerialPortInfo* outInfo);
};

} // namespace RDK

#endif
