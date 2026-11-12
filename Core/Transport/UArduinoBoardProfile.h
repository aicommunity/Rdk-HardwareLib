#ifndef UARDUINOBOARDPROFILE_H
#define UARDUINOBOARDPROFILE_H

#include <QtGlobal>

#include <QString>

namespace RDK {

enum class UArduinoBoardKind { Uno = 0, Mega2560 = 1 };

struct UArduinoBoardProfile {
    UArduinoBoardKind kind = UArduinoBoardKind::Uno;
    const char* mcu = "atmega328p";
    const char* programmer = "arduino";
    const char* protocol = "stk500";
    int uploadBaud = 115200;
};

class UArduinoBoardProfileUtil {
public:
    static UArduinoBoardProfile profileForKind(int board_profile_int);
    static UArduinoBoardProfile profileForKind(UArduinoBoardKind kind);
    static QString boardKindToManifestKey(UArduinoBoardKind kind);

    /** Empty QString = OK. Non-empty = user-facing error (do not call avrdude). */
    static QString validateUploadTargets(int board_profile_int, const QString& hex_absolute_path);

    /** Return 0 Uno, 1 Mega, -1 unknown. */
    static int boardProfileFromUsbIds(quint16 vendor_id, quint16 product_id);
    static int boardProfileFromDescription(const QString& text);
};

} // namespace RDK

#endif
