#ifndef UARDUINOFIRMATABATCHBUILDER_H
#define UARDUINOFIRMATABATCHBUILDER_H

#include "../../../Rdk/Deploy/Include/rdk.h"

namespace RDK {

struct UArduinoFirmataBatchBuilder {
    static constexpr int kModeInput = 0;
    static constexpr int kModeOutput = 1;
    static constexpr int kModeAnalog = 2;
    static constexpr int kModePwm = 3;
    static constexpr int kModeServo = 4;

    static void clearPinConfig(MDMatrix<double>& batch);
    static void appendPinMode(MDMatrix<double>& batch, int pin, int mode);
    static void appendDigital(MDMatrix<double>& batch, int pin, int value);
    static void appendAnalog(MDMatrix<double>& batch, int pin, int value);
};

} // namespace RDK

#endif
