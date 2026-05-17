#include "UArduinoBoardProfile.h"

namespace RDK {

UArduinoBoardProfile UArduinoBoardProfileUtil::profileForKind(int board_profile_int)
{
    if (board_profile_int == static_cast<int>(UArduinoBoardKind::Mega2560))
        return profileForKind(UArduinoBoardKind::Mega2560);
    return profileForKind(UArduinoBoardKind::Uno);
}

UArduinoBoardProfile UArduinoBoardProfileUtil::profileForKind(UArduinoBoardKind kind)
{
    UArduinoBoardProfile p;
    p.kind = kind;
    if (kind == UArduinoBoardKind::Mega2560) {
        p.mcu = "atmega2560";
        p.programmer = "wiring";
        p.protocol = "stk500v2";
    } else {
        p.mcu = "atmega328p";
        p.programmer = "arduino";
        p.protocol = "stk500";
    }
    p.uploadBaud = 115200;
    return p;
}

QString UArduinoBoardProfileUtil::boardKindToManifestKey(UArduinoBoardKind kind)
{
    return kind == UArduinoBoardKind::Mega2560 ? QStringLiteral("mega2560")
                                               : QStringLiteral("uno");
}

} // namespace RDK
