#ifndef UARDUINOBOARDPROFILE_H
#define UARDUINOBOARDPROFILE_H

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
};

} // namespace RDK

#endif
