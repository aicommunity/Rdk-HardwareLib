#ifndef UARDUINOFIRMATA_H
#define UARDUINOFIRMATA_H

#include "UArduinoBoard.h"
#include "Protocol/UArduinoFirmataClient.h"

namespace RDK {

class RDK_LIB_TYPE UArduinoFirmata : public UArduinoBoard {
public:
    UProperty<bool, UArduinoFirmata, ptPubState> FirmataReady;
    UProperty<string, UArduinoFirmata, ptPubState> FirmataFirmwareVersion;
    UProperty<int, UArduinoFirmata, ptPubParameter> SelectedPin;
    UProperty<int, UArduinoFirmata, ptPubParameter> SelectedPinMode;
    UProperty<int, UArduinoFirmata, ptPubParameter> DigitalPinValue;
    UProperty<int, UArduinoFirmata, ptPubState> AnalogPinValue;
    UProperty<bool, UArduinoFirmata, ptPubState> SetPinModeFlag;
    UProperty<bool, UArduinoFirmata, ptPubState> ReadAnalogFlag;
    UProperty<bool, UArduinoFirmata, ptPubState> WriteDigitalFlag;

    UArduinoFirmata();
    virtual ~UArduinoFirmata();

    UArduinoFirmata* New() override;

protected:
    bool ADefault() override;
    bool ACalculate() override;
    void OnBoardCalculate() override;
    bool EnsureConnected() override;

    void ProcessFirmata();
    void RunFirmataActions();

    UArduinoFirmataClient FirmataClient;
    bool HandshakeSent = false;
};

} // namespace RDK

#endif
