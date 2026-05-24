#include "UArduinoPropertyString.h"

namespace RDK {
namespace UArduinoPropertyString {

QString fromStdProperty(const std::string& value)
{
    if (value.empty())
        return QString();
#if defined(Q_OS_WIN)
    return QString::fromLocal8Bit(value.c_str());
#else
    return QString::fromUtf8(value.c_str());
#endif
}

std::string toStdProperty(const QString& value)
{
    if (value.isEmpty())
        return std::string();
#if defined(Q_OS_WIN)
    const QByteArray bytes = value.toLocal8Bit();
#else
    const QByteArray bytes = value.toUtf8();
#endif
    return std::string(bytes.constData(), static_cast<size_t>(bytes.size()));
}

} // namespace UArduinoPropertyString
} // namespace RDK
