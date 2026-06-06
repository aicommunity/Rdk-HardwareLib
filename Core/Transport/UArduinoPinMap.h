#ifndef UARDUINOPINMAP_H
#define UARDUINOPINMAP_H

#include <QString>

namespace RDK {

struct UArduinoPinMap {
    static int analogBase(int board_profile);
    static int firmataPinForLabel(const QString& label, int board_profile);
    static QString labelForFirmataPin(int firmata_pin, int board_profile);
    static int maxFirmataPin(int board_profile);
    static int digitalPortCount(int board_profile);
    static int bitIndexInPort(int firmata_pin);
    static int portForPin(int firmata_pin);
    static bool isAnalogLabel(const QString& label);
};

} // namespace RDK

#endif
