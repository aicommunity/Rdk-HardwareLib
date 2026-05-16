#include "UArduinoAdc.h"

#include "UArduinoFirmata.h"

namespace RDK {

UArduinoAdc::UArduinoAdc()
    : LinkedFirmataName("LinkedFirmataName", this)
    , AnalogPin("AnalogPin", this)
    , AdcValue("AdcValue", this)
    , ReadAdcFlag("ReadAdcFlag", this)
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
    return true;
}

bool UArduinoAdc::ACalculate()
{
    if (!ReadAdcFlag || LinkedFirmataName->empty())
        return true;

    UContainer* owner = dynamic_cast<UContainer*>(GetOwner().Get());
    if (!owner)
        return true;

    UEPtr<UArduinoFirmata> firmata = owner->GetComponentL<UArduinoFirmata>(*LinkedFirmataName, true);
    if (firmata) {
        firmata->SelectedPin = AnalogPin;
        firmata->ReadAnalogFlag = true;
        firmata->Calculate();
        AdcValue = firmata->AnalogPinValue;
    }
    ReadAdcFlag = false;
    return true;
}

} // namespace RDK
