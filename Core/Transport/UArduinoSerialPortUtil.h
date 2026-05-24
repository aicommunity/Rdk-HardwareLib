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
    int SuggestedBoardProfile = -1;
    QString BoardDetectSource;
};

class UArduinoSerialPortUtil {
public:
    /** Device path for QSerialPort / avrdude (-P). Prefers /dev/... on Linux. */
    static QString normalizeDevicePath(const QString& port_name);
    static QString devicePathFromPortName(const QString& port_name);
    /** Best name to pass to QSerialPort::open for this port. */
    static QString preferredOpenName(const QString& port_name);
    /** Port string for avrdude `-P` (e.g. `\\.\COM3` on Windows). */
    static QString avrdudePortArgument(const QString& port_name);
    static QStringList listAvailableDevicePaths();
    static QList<UArduinoSerialPortEntry> listPortsSorted();
    static bool portInfoForPath(const QString& port_name, QSerialPortInfo* out_info);
    static int inferBoardProfileFromPortInfo(const QSerialPortInfo& info);
    static int inferBoardProfileForDevicePath(const QString& device_path);
};

} // namespace RDK

#endif
