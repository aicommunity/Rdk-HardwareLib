#ifndef UARDUINODCDEMO_H
#define UARDUINODCDEMO_H

#include "../../../Rdk/Deploy/Include/rdk.h"

namespace RDK {

class UArduinoSensorSketch;

class RDK_LIB_TYPE UArduinoDcDemo : public UNet {
public:
    UProperty<string, UArduinoDcDemo, ptPubParameter> LinkedSketchName;
    UProperty<string, UArduinoDcDemo, ptPubParameter | ptOutput> Command;
    UProperty<bool, UArduinoDcDemo, ptPubState> SendCommandFlag;
    UProperty<string, UArduinoDcDemo, ptPubState> SentCommand;
    UProperty<float, UArduinoDcDemo, ptPubState> Speed;
    UProperty<float, UArduinoDcDemo, ptPubState> Acceleration;
    UProperty<bool, UArduinoDcDemo, ptPubState> GetSpeed;

    UArduinoDcDemo();
    virtual ~UArduinoDcDemo();
    UArduinoDcDemo* New() override;

protected:
    bool ADefault() override;
    bool ABuild() override;
    bool AReset() override;
    bool ACalculate() override;
};

} // namespace RDK

#endif
