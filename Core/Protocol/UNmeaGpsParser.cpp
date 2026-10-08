#include "UNmeaGpsParser.h"

#include <QtGlobal>
#include <QStringList>

#include <cmath>

namespace RDK {

namespace {

bool parseCoordinate(const QString& field, const QString& hemisphere, int degreeDigits,
                     double maxDegrees, double* result)
{
    if (!result || field.isEmpty() || field.size() <= degreeDigits || hemisphere.size() != 1)
        return false;

    bool ok = false;
    const double packed = field.toDouble(&ok);
    if (!ok || !std::isfinite(packed) || packed < 0.0)
        return false;

    const double degrees = std::floor(packed / 100.0);
    const double minutes = packed - degrees * 100.0;
    if (degrees > maxDegrees || minutes < 0.0 || minutes >= 60.0
        || (degrees == maxDegrees && minutes != 0.0))
        return false;

    const QString hemi = hemisphere.toUpper();
    const bool negative = hemi == QLatin1String("S") || hemi == QLatin1String("W");
    if (!negative && hemi != QLatin1String("N") && hemi != QLatin1String("E"))
        return false;

    const double decimal = degrees + minutes / 60.0;
    *result = negative ? -decimal : decimal;
    return true;
}

bool checksumMatches(const QString& sentence, int star)
{
    if (star < 0)
        return true;
    if (star + 3 != sentence.size())
        return false;

    bool ok = false;
    const int expected = sentence.mid(star + 1, 2).toInt(&ok, 16);
    if (!ok || expected < 0 || expected > 255)
        return false;

    quint8 checksum = 0;
    for (int i = 1; i < star; ++i) {
        const ushort ch = sentence.at(i).unicode();
        if (ch > 0x7f)
            return false;
        checksum ^= static_cast<quint8>(ch);
    }
    return checksum == static_cast<quint8>(expected);
}

} // namespace

bool UNmeaGpsParser::parseGga(const QString& input, double* latitude, double* longitude)
{
    if (!latitude || !longitude)
        return false;

    const QString sentence = input.trimmed();
    if (!sentence.startsWith(QLatin1Char('$')))
        return false;
    const int star = sentence.indexOf(QLatin1Char('*'));
    if (star >= 0 && !checksumMatches(sentence, star))
        return false;

    const QString body = sentence.mid(1, (star >= 0 ? star : sentence.size()) - 1);
    const QStringList fields = body.split(QLatin1Char(','));
    if (fields.size() < 7 || fields[0].size() != 5 || !fields[0].endsWith(QLatin1String("GGA")))
        return false;

    bool fixOk = false;
    const int fixQuality = fields[6].toInt(&fixOk);
    if (!fixOk || fixQuality <= 0)
        return false;

    double lat = 0.0;
    double lon = 0.0;
    if (!parseCoordinate(fields[2], fields[3], 2, 90.0, &lat)
        || !parseCoordinate(fields[4], fields[5], 3, 180.0, &lon))
        return false;

    *latitude = lat;
    *longitude = lon;
    return true;
}

} // namespace RDK
