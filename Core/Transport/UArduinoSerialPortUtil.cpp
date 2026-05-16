#include "UArduinoSerialPortUtil.h"

#include <algorithm>

#include <QFileInfo>
#include <QtSerialPort/QSerialPortInfo>

namespace RDK {

namespace {

bool pathsEquivalent(const QString& a, const QString& b)
{
    if (a == b)
        return true;
    const QString na = UArduinoSerialPortUtil::normalizeDevicePath(a);
    const QString nb = UArduinoSerialPortUtil::normalizeDevicePath(b);
    if (na == nb)
        return true;
    return QFileInfo(na).fileName() == QFileInfo(nb).fileName();
}

bool isLikelyHostUart(const QSerialPortInfo& info)
{
    const QString name = info.portName();
#if defined(Q_OS_WIN)
    Q_UNUSED(name);
    return false;
#else
    return name.startsWith(QStringLiteral("ttyS"));
#endif
}

bool isLikelyUsbSerialDevice(const QSerialPortInfo& info)
{
    if (info.hasVendorIdentifier() && info.hasProductIdentifier())
        return true;

    const QString loc = info.systemLocation();
    if (loc.contains(QStringLiteral("ttyACM"), Qt::CaseInsensitive)
        || loc.contains(QStringLiteral("ttyUSB"), Qt::CaseInsensitive)
        || loc.contains(QStringLiteral("cu.usb"), Qt::CaseInsensitive)
        || loc.contains(QStringLiteral("cu.wchusbserial"), Qt::CaseInsensitive))
        return true;

    const QString desc = info.description().toLower();
    if (desc.contains(QStringLiteral("arduino")) || desc.contains(QStringLiteral("usb"))
        || desc.contains(QStringLiteral("serial")) || desc.contains(QStringLiteral("ch340"))
        || desc.contains(QStringLiteral("ftdi")) || desc.contains(QStringLiteral("cp210")))
        return true;

    return false;
}

QString displayLabelFor(const QSerialPortInfo& info, const QString& devicePath)
{
    const QString desc = info.description().trimmed();
    const QString mfg = info.manufacturer().trimmed();
    QString extra;
    if (!desc.isEmpty())
        extra = desc;
    else if (!mfg.isEmpty())
        extra = mfg;
    if (extra.isEmpty())
        return devicePath;
    return QStringLiteral("%1 — %2").arg(devicePath, extra);
}

} // namespace

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

bool UArduinoSerialPortUtil::portInfoForPath(const QString& portName, QSerialPortInfo* outInfo)
{
    if (!outInfo)
        return false;
    for (const QSerialPortInfo& info : QSerialPortInfo::availablePorts()) {
        if (pathsEquivalent(portName, info.systemLocation())
            || pathsEquivalent(portName, info.portName())) {
            *outInfo = info;
            return true;
        }
    }
    return false;
}

QString UArduinoSerialPortUtil::preferredOpenName(const QString& portName)
{
    QSerialPortInfo info;
    if (portInfoForPath(portName, &info)) {
#if defined(Q_OS_WIN)
        return info.portName();
#else
        if (!info.systemLocation().isEmpty())
            return info.systemLocation();
        if (!info.portName().isEmpty())
            return normalizeDevicePath(info.portName());
#endif
    }
    return normalizeDevicePath(portName);
}

QStringList UArduinoSerialPortUtil::listAvailableDevicePaths()
{
    QStringList paths;
    for (const UArduinoSerialPortEntry& entry : listPortsSorted())
        paths.append(entry.devicePath);
    return paths;
}

QList<UArduinoSerialPortEntry> UArduinoSerialPortUtil::listPortsSorted()
{
    QList<UArduinoSerialPortEntry> entries;
    for (const QSerialPortInfo& info : QSerialPortInfo::availablePorts()) {
        UArduinoSerialPortEntry entry;
#if defined(Q_OS_WIN)
        entry.devicePath = info.portName();
#else
        entry.devicePath = !info.systemLocation().isEmpty() ? info.systemLocation()
                                                            : normalizeDevicePath(info.portName());
#endif
        if (entry.devicePath.isEmpty())
            continue;
        entry.likelyAttachedDevice = isLikelyUsbSerialDevice(info) && !isLikelyHostUart(info);
        entry.displayLabel = displayLabelFor(info, entry.devicePath);
        entries.append(entry);
    }

    std::sort(entries.begin(), entries.end(), [](const UArduinoSerialPortEntry& a,
                                                 const UArduinoSerialPortEntry& b) {
        if (a.likelyAttachedDevice != b.likelyAttachedDevice)
            return a.likelyAttachedDevice > b.likelyAttachedDevice;
        return a.devicePath < b.devicePath;
    });
    return entries;
}

} // namespace RDK
