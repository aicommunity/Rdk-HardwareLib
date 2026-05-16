#include "UArduinoSerialPortUtil.h"

#include <QtSerialPort/QSerialPortInfo>

namespace RDK {

QString UArduinoSerialPortUtil::normalizeDevicePath(const QString& portName)
{
    const QString trimmed = portName.trimmed();
    if (trimmed.isEmpty())
        return trimmed;

#if defined(Q_OS_WIN)
    return trimmed;
#else
    if (trimmed.startsWith(QLatin1String("/dev/")) || trimmed.startsWith(QLatin1String("/tmp/")))
        return trimmed;

    for (const QSerialPortInfo& info : QSerialPortInfo::availablePorts()) {
        if (info.portName() == trimmed || info.systemLocation() == trimmed) {
            if (!info.systemLocation().isEmpty())
                return info.systemLocation();
            break;
        }
    }

    if (trimmed.startsWith(QLatin1String("tty")) || trimmed.startsWith(QLatin1String("cu")))
        return QStringLiteral("/dev/") + trimmed;
    return trimmed;
#endif
}

QString UArduinoSerialPortUtil::devicePathFromPortName(const QString& portName)
{
    return normalizeDevicePath(portName);
}

QStringList UArduinoSerialPortUtil::listAvailableDevicePaths()
{
    QStringList paths;
    for (const QSerialPortInfo& info : QSerialPortInfo::availablePorts()) {
#if defined(Q_OS_WIN)
        paths.append(info.portName());
#else
        if (!info.systemLocation().isEmpty())
            paths.append(info.systemLocation());
        else if (!info.portName().isEmpty())
            paths.append(normalizeDevicePath(info.portName()));
#endif
    }
    paths.removeDuplicates();
    return paths;
}

} // namespace RDK
