#ifndef UFIRMWAREMANIFEST_H
#define UFIRMWAREMANIFEST_H

#include <QString>

namespace RDK {

class UFirmwareManifest {
public:
    /** Relative to application work dir (`Bin/Platform/<OS>/`): `../../ArduinoFirmware`. */
    static QString bundledFirmwareRelativeRoot();
    /** e.g. `../../ArduinoFirmware/firmata/standard_firmata_uno.hex` */
    static QString bundledHexRelativePath(const QString& bundled_id, int board_profile_int);
    static QString bundledHexRelativePath(const QString& bundled_id, const QString& board_key);

    static QString resolveFromApplicationDir(const QString& relative_path);
    static QString firmwareRoot();
    static QString resolveBundledHex(const QString& bundled_id, int board_profile_int);
    static QString resolveBundledHex(const QString& bundled_id, const QString& board_key);
};

} // namespace RDK

#endif
