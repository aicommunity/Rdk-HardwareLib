#include "UArduinoPropertyString.h"

namespace RDK {
namespace UArduinoPropertyString {

QString fromStdProperty(const std::string& value)
{
    if (value.empty())
        return QString();
#if defined(RDK_UNICODE_RUN) || !defined(Q_OS_WIN)
    return QString::fromUtf8(value.c_str());
#else
    return QString::fromLocal8Bit(value.c_str());
#endif
}

std::string toStdProperty(const QString& value)
{
    if (value.isEmpty())
        return std::string();
#if defined(RDK_UNICODE_RUN) || !defined(Q_OS_WIN)
    const QByteArray bytes = value.toUtf8();
#else
    const QByteArray bytes = value.toLocal8Bit();
#endif
    return std::string(bytes.constData(), static_cast<size_t>(bytes.size()));
}

} // namespace UArduinoPropertyString
} // namespace RDK
