#ifndef UHardwareLibrary_H
#define UHardwareLibrary_H

#include "../../../Rdk/Deploy/Include/rdk.h"

#include "UArduinoBoard.h"
#include "UArduinoSensorSketch.h"
#include "UArduinoFirmata.h"
#include "UArduinoAdc.h"
#include "UArduinoDcDemo.h"
#include "Devices/UArduinoDeviceIO.h"

namespace RDK {

class RDK_LIB_TYPE UHardwareLibrary: public ULibrary
{
public:
    UHardwareLibrary(void);
    virtual void CreateClassSamples(UStorage *storage);
};

extern RDK_LIB_TYPE UHardwareLibrary HardwareLibrary;

}
#endif
