#ifndef UARDUINODEVICEIO_H
#define UARDUINODEVICEIO_H

#include "rdk.h"

#include <QString>

namespace RDK {

class UArduinoFirmata;

class RDK_LIB_TYPE UArduinoDeviceIO : public UNet {
public:
    UProperty<string, UArduinoDeviceIO, ptPubParameter> LinkedFirmataName;
    UProperty<string, UArduinoDeviceIO, ptPubParameter> ModuleId;
    UProperty<string, UArduinoDeviceIO, ptPubParameter> DeviceId;
    UProperty<string, UArduinoDeviceIO, ptPubParameter> Port;
    UProperty<string, UArduinoDeviceIO, ptPubParameter> Channel;
    UProperty<int, UArduinoDeviceIO, ptPubParameter> Role;
    UProperty<int, UArduinoDeviceIO, ptPubParameter> BoardProfile;
    UProperty<string, UArduinoDeviceIO, ptPubParameter> HardwareSetupPath;
    UProperty<double, UArduinoDeviceIO, ptPubOutput | ptPubState> Value;
    UProperty<int, UArduinoDeviceIO, ptPubState> ValueRaw;
    UProperty<double, UArduinoDeviceIO, ptPubParameter | ptPubInput> ValueIn;
    UProperty<int, UArduinoDeviceIO, ptPubParameter> InputMode;
    UProperty<bool, UArduinoDeviceIO, ptPubState> IsOnline;
    UProperty<string, UArduinoDeviceIO, ptPubState> LastError;
    UProperty<bool, UArduinoDeviceIO, ptPubParameter | ptInput> ApplyConfig;
    UProperty<bool, UArduinoDeviceIO, ptPubParameter | ptInput> WriteOutput;
    UProperty<bool, UArduinoDeviceIO, ptPubParameter | ptInput> ReadInput;
    UProperty<bool, UArduinoDeviceIO, ptPubParameter> Continuous;

    UArduinoDeviceIO();
    virtual ~UArduinoDeviceIO();
    UArduinoDeviceIO* New() override;

protected:
    bool ADefault() override;
    bool ACalculate() override;

private:
    int resolveRole(const QString& signal_type) const;
    double LastValueIn = 0.0;
    bool HadValueIn = false;
};

} // namespace RDK

#endif
