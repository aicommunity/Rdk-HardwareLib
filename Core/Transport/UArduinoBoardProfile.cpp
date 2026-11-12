#include "UArduinoBoardProfile.h"

#include <QFileInfo>

namespace RDK {

namespace {

bool hexPathLooksMega(const QString& file_name_lower)
{
    return file_name_lower.contains(QStringLiteral("mega2560"))
           || (file_name_lower.contains(QStringLiteral("mega"))
               && !file_name_lower.contains(QStringLiteral("uno")));
}

bool hexPathLooksUno(const QString& file_name_lower)
{
    return file_name_lower.contains(QStringLiteral("uno"))
           || file_name_lower.contains(QStringLiteral("atmega328"));
}

} // namespace

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
        p.protocol = "wiring";
    } else {
        p.mcu = "atmega328p";
        p.programmer = "arduino";
        p.protocol = "arduino";
    }
    p.uploadBaud = 115200;
    return p;
}

QString UArduinoBoardProfileUtil::boardKindToManifestKey(UArduinoBoardKind kind)
{
    return kind == UArduinoBoardKind::Mega2560 ? QStringLiteral("mega2560")
                                               : QStringLiteral("uno");
}

QString UArduinoBoardProfileUtil::validateUploadTargets(int board_profile_int,
                                                        const QString& hex_absolute_path)
{
    if (hex_absolute_path.isEmpty())
        return QString();

    const QString fn = QFileInfo(hex_absolute_path).fileName().toLower();
    const bool mega_hex = hexPathLooksMega(fn);
    const bool uno_hex = hexPathLooksUno(fn);

    if (!mega_hex && !uno_hex)
        return QString();

    const bool profile_mega =
        (board_profile_int == static_cast<int>(UArduinoBoardKind::Mega2560));

    if (!profile_mega && mega_hex) {
        return QStringLiteral(
            "Board profile is Arduino Uno (0), but the HEX file appears to be for "
            "Arduino Mega 2560. Set Board to Arduino Mega 2560 (1) or pick the Uno HEX.");
    }
    if (profile_mega && uno_hex && !mega_hex) {
        return QStringLiteral(
            "Board profile is Arduino Mega 2560 (1), but the HEX file appears to be for "
            "Arduino Uno. Set Board to Arduino Uno (0) or pick the Mega HEX.");
    }
    return QString();
}

int UArduinoBoardProfileUtil::boardProfileFromUsbIds(quint16 vendor_id, quint16 product_id)
{
    if (vendor_id != 0x2341)
        return -1;
    switch (product_id) {
    case 0x0010:
    case 0x0042:
        return static_cast<int>(UArduinoBoardKind::Mega2560);
    case 0x0043:
    case 0x0001:
    case 0x006A:
        return static_cast<int>(UArduinoBoardKind::Uno);
    default:
        return -1;
    }
}

int UArduinoBoardProfileUtil::boardProfileFromDescription(const QString& text)
{
    const QString t = text.toLower();
    if (t.contains(QStringLiteral("mega")) || t.contains(QStringLiteral("2560")))
        return static_cast<int>(UArduinoBoardKind::Mega2560);
    if (t.contains(QStringLiteral("uno")) || t.contains(QStringLiteral("328p"))
        || t.contains(QStringLiteral("atmega328")))
        return static_cast<int>(UArduinoBoardKind::Uno);
    return -1;
}

} // namespace RDK
