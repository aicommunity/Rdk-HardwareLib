#ifndef UNMEAGPSPARSER_H
#define UNMEAGPSPARSER_H

#include <QString>

namespace RDK {

/** Parse an NMEA GGA sentence into signed decimal-degree coordinates. */
class UNmeaGpsParser {
public:
    static bool parseGga(const QString& sentence, double* latitude, double* longitude);
};

} // namespace RDK

#endif
