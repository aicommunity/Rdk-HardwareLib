#ifndef UARDUINOSERIALPORTUTIL_H
#define UARDUINOSERIALPORTUTIL_H

#include <QString>
#include <QStringList>

class QSerialPortInfo;

namespace RDK {

struct UArduinoSerialPortEntry {
    QString DevicePath;
    QString DisplayLabel;
    bool LikelyAttachedDevice = false;
};

class UArduinoSerialPortUtil {
public:
    /** Device path for QSerialPort / avrdude (-P). Prefers /dev/... on Linux. */
    static QString normalizeDevicePath(const QString& port_name);
    static QString devicePathFromPortName(const QString& port_name);
    /** Best name to pass to QSerialPort::open for this port. */
    static QString preferredOpenName(const QString& port_name);
    static QStringList listAvailableDevicePaths();
    static QList<UArduinoSerialPortEntry> listPortsSorted();
    static bool portInfoForPath(const QString& port_name, QSerialPortInfo* out_info);
};

} // namespace RDK

#endif
