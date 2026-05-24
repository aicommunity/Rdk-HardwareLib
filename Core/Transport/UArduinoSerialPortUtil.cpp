#include "UArduinoSerialPortUtil.h"

#include "UArduinoBoardProfile.h"

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

QString displayLabelFor(const QSerialPortInfo& info, const QString& device_path)
{
    const QString desc = info.description().trimmed();
    const QString mfg = info.manufacturer().trimmed();
    QString extra;
    if (!desc.isEmpty())
        extra = desc;
    else if (!mfg.isEmpty())
        extra = mfg;
    if (extra.isEmpty())
        return device_path;
    return QStringLiteral("%1 \u2014 %2").arg(device_path, extra);
}

} // namespace

QString UArduinoSerialPortUtil::normalizeDevicePath(const QString& port_name)
{
    const QString trimmed = port_name.trimmed();
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

QString UArduinoSerialPortUtil::devicePathFromPortName(const QString& port_name)
{
    return normalizeDevicePath(port_name);
}

int UArduinoSerialPortUtil::inferBoardProfileFromPortInfo(const QSerialPortInfo& info)
{
    if (info.hasVendorIdentifier() && info.hasProductIdentifier()) {
        const int from_usb = UArduinoBoardProfileUtil::boardProfileFromUsbIds(
            info.vendorIdentifier(), info.productIdentifier());
        if (from_usb >= 0)
            return from_usb;
    }
    const QString blob = info.description() + QLatin1Char(' ') + info.manufacturer();
    return UArduinoBoardProfileUtil::boardProfileFromDescription(blob);
}

int UArduinoSerialPortUtil::inferBoardProfileForDevicePath(const QString& device_path)
{
    QSerialPortInfo info;
    if (!portInfoForPath(device_path, &info))
        return -1;
    return inferBoardProfileFromPortInfo(info);
}

bool UArduinoSerialPortUtil::portInfoForPath(const QString& port_name, QSerialPortInfo* out_info)
{
    if (!out_info)
        return false;
    for (const QSerialPortInfo& info : QSerialPortInfo::availablePorts()) {
        if (pathsEquivalent(port_name, info.systemLocation())
            || pathsEquivalent(port_name, info.portName())) {
            *out_info = info;
            return true;
        }
    }
    return false;
}

QString UArduinoSerialPortUtil::preferredOpenName(const QString& port_name)
{
    QSerialPortInfo info;
    if (portInfoForPath(port_name, &info)) {
#if defined(Q_OS_WIN)
        return info.portName();
#else
        if (!info.systemLocation().isEmpty())
            return info.systemLocation();
        if (!info.portName().isEmpty())
            return normalizeDevicePath(info.portName());
#endif
    }
    return normalizeDevicePath(port_name);
}

QString UArduinoSerialPortUtil::avrdudePortArgument(const QString& port_name)
{
    const QString openName = preferredOpenName(port_name);
#if defined(Q_OS_WIN)
    if (openName.startsWith(QStringLiteral("\\\\.\\")))
        return openName;
    if (openName.startsWith(QStringLiteral("COM"), Qt::CaseInsensitive))
        return QStringLiteral("\\\\.\\") + openName;
    return openName;
#else
    return normalizeDevicePath(openName);
#endif
}

QStringList UArduinoSerialPortUtil::listAvailableDevicePaths()
{
    QStringList paths;
    for (const UArduinoSerialPortEntry& entry : listPortsSorted())
        paths.append(entry.DevicePath);
    return paths;
}

QList<UArduinoSerialPortEntry> UArduinoSerialPortUtil::listPortsSorted()
{
    QList<UArduinoSerialPortEntry> entries;
    for (const QSerialPortInfo& info : QSerialPortInfo::availablePorts()) {
        UArduinoSerialPortEntry entry;
#if defined(Q_OS_WIN)
        entry.DevicePath = info.portName();
#else
        entry.DevicePath = !info.systemLocation().isEmpty() ? info.systemLocation()
                                                            : normalizeDevicePath(info.portName());
#endif
        if (entry.DevicePath.isEmpty())
            continue;
        entry.LikelyAttachedDevice = isLikelyUsbSerialDevice(info) && !isLikelyHostUart(info);
        entry.DisplayLabel = displayLabelFor(info, entry.DevicePath);
        entry.SuggestedBoardProfile = inferBoardProfileFromPortInfo(info);
        if (entry.SuggestedBoardProfile >= 0) {
            if (info.hasVendorIdentifier() && info.hasProductIdentifier()
                && UArduinoBoardProfileUtil::boardProfileFromUsbIds(
                       info.vendorIdentifier(), info.productIdentifier()) >= 0)
                entry.BoardDetectSource = QStringLiteral("usb");
            else
                entry.BoardDetectSource = QStringLiteral("description");
        }
        entries.append(entry);
    }

    std::sort(entries.begin(), entries.end(), [](const UArduinoSerialPortEntry& a,
                                                 const UArduinoSerialPortEntry& b) {
        if (a.LikelyAttachedDevice != b.LikelyAttachedDevice)
            return a.LikelyAttachedDevice > b.LikelyAttachedDevice;
        return a.DevicePath < b.DevicePath;
    });
    return entries;
}

} // namespace RDK
