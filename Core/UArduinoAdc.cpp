#include "UArduinoAdc.h"

#include "UArduinoFirmata.h"

namespace RDK {

UArduinoAdc::UArduinoAdc()
    : LinkedFirmataName("LinkedFirmataName", this)
    , AnalogPin("AnalogPin", this)
    , AdcValue("AdcValue", this)
    , ReadAdcFlag("ReadAdcFlag", this)
    , AdcReadOk("AdcReadOk", this)
{
}

UArduinoAdc::~UArduinoAdc() = default;

UArduinoAdc* UArduinoAdc::New()
{
    return new UArduinoAdc;
}

bool UArduinoAdc::ADefault()
{
    LinkedFirmataName = "";
    AnalogPin = 0;
    AdcValue = 0;
    ReadAdcFlag = false;
    AdcReadOk = false;
    return true;
}

bool UArduinoAdc::ACalculate()
{
    AdcReadOk = false;
    if (!ReadAdcFlag || LinkedFirmataName->empty()) {
        ReadAdcFlag = false;
        return true;
    }

    UContainer* owner = dynamic_cast<UContainer*>(GetOwner().Get());
    if (!owner) {
        ReadAdcFlag = false;
        return true;
    }

    UEPtr<UArduinoFirmata> firmata =
        owner->GetComponentL<UArduinoFirmata>(*LinkedFirmataName, true);
    if (!firmata) {
        ReadAdcFlag = false;
        return true;
    }

    // Engine thread only (same as UArduinoAdc::ACalculate).
    firmata->SelectedPin = AnalogPin;
    firmata->ReadAnalogFlag = true;
    firmata->Calculate();
    AdcValue = firmata->AnalogPinValue;
    AdcReadOk = firmata->IsLinkReady;
    ReadAdcFlag = false;
    return true;
}

} // namespace RDK
