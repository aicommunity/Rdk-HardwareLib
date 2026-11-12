#ifndef UARDUINOADC_H
#define UARDUINOADC_H

#include "rdk.h"

namespace RDK {

class UArduinoFirmata;

class RDK_LIB_TYPE UArduinoAdc : public UNet {
public:
    UProperty<string, UArduinoAdc, ptPubParameter> LinkedFirmataName;
    UProperty<int, UArduinoAdc, ptPubParameter> AnalogPin;
    UProperty<int, UArduinoAdc, ptPubParameter> BoardProfile;
    UProperty<bool, UArduinoAdc, ptPubParameter> UseLinkedAnalogSamples;
    UProperty<int, UArduinoAdc, ptPubState> AdcValue;
    UProperty<bool, UArduinoAdc, ptPubParameter | ptInput> ReadAdcFlag;
    UProperty<bool, UArduinoAdc, ptPubState> AdcReadOk;

    UArduinoAdc();
    virtual ~UArduinoAdc();
    UArduinoAdc* New() override;

protected:
    bool ADefault() override;
    bool ACalculate() override;
};

} // namespace RDK

#endif
