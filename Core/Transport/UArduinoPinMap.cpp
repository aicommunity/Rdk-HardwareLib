#include "UArduinoPinMap.h"

namespace RDK {

int UArduinoPinMap::analogBase(int board_profile)
{
    return board_profile == 1 ? 54 : 14;
}

int UArduinoPinMap::maxFirmataPin(int board_profile)
{
    return board_profile == 1 ? 69 : 19;
}

int UArduinoPinMap::digitalPortCount(int board_profile)
{
    return board_profile == 1 ? 7 : 2;
}

int UArduinoPinMap::bitIndexInPort(int firmata_pin)
{
    return firmata_pin % 8;
}

int UArduinoPinMap::portForPin(int firmata_pin)
{
    return firmata_pin / 8;
}

bool UArduinoPinMap::isAnalogLabel(const QString& label)
{
    return label.startsWith(QLatin1String("A"), Qt::CaseInsensitive);
}

int UArduinoPinMap::firmataPinForLabel(const QString& label, int board_profile)
{
    if (label.startsWith(QLatin1String("D"), Qt::CaseInsensitive)) {
        bool ok = false;
        const int n = label.mid(1).toInt(&ok);
        return ok ? n : -1;
    }
    if (label.startsWith(QLatin1String("A"), Qt::CaseInsensitive)) {
        bool ok = false;
        const int n = label.mid(1).toInt(&ok);
        return ok ? (analogBase(board_profile) + n) : -1;
    }
    return -1;
}

QString UArduinoPinMap::labelForFirmataPin(int firmata_pin, int board_profile)
{
    const int base = analogBase(board_profile);
    if (firmata_pin >= base)
        return QStringLiteral("A%1").arg(firmata_pin - base);
    return QStringLiteral("D%1").arg(firmata_pin);
}

} // namespace RDK
