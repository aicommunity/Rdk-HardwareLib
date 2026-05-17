#ifndef UFIRMWAREMANIFEST_H
#define UFIRMWAREMANIFEST_H

#include <QString>

namespace RDK {

class UFirmwareManifest {
public:
    static QString firmwareRoot();
    static QString resolveBundledHex(const QString& bundled_id, int board_profile_int);
    static QString resolveBundledHex(const QString& bundled_id, const QString& board_key);
};

} // namespace RDK

#endif
