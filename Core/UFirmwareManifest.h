#ifndef UFIRMWAREMANIFEST_H
#define UFIRMWAREMANIFEST_H

#include <QString>

namespace RDK {

class UFirmwareManifest {
public:
    static QString firmwareRoot();
    static QString resolveBundledHex(const QString& bundledId, int boardProfileInt);
    static QString resolveBundledHex(const QString& bundledId, const QString& boardKey);
};

} // namespace RDK

#endif
