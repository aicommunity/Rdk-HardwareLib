#include "UArduinoAdc.h"

#include "UArduinoFirmata.h"
#include "Transport/UArduinoPinMap.h"

namespace RDK {

UArduinoAdc::UArduinoAdc()
    : LinkedFirmataName("LinkedFirmataName", this)
    , AnalogPin("AnalogPin", this)
    , BoardProfile("BoardProfile", this)
    , UseLinkedAnalogSamples("UseLinkedAnalogSamples", this)
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
    AnalogPin = UArduinoPinMap::firmataPinForLabel(QStringLiteral("A0"), 0);
    BoardProfile = 0;
    UseLinkedAnalogSamples = true;
    AdcValue = 0;
    ReadAdcFlag = false;
    AdcReadOk = false;
    return true;
}

bool UArduinoAdc::ACalculate()
{
    AdcReadOk = false;
    if (LinkedFirmataName->empty())
        return true;

    UContainer* owner = dynamic_cast<UContainer*>(GetOwner().Get());
    if (!owner)
        return true;

    UEPtr<UArduinoFirmata> firmata =
        owner->GetComponentL<UArduinoFirmata>(*LinkedFirmataName, true);
    if (!firmata) {
        ReadAdcFlag = false;
        return true;
    }

    if (UseLinkedAnalogSamples && firmata->AnalogSamples->GetRows() > 0) {
        const int target_pin = AnalogPin;
        for (int r = firmata->AnalogSamples->GetRows() - 1; r >= 0; --r) {
            const int pin = static_cast<int>((*firmata->AnalogSamples)(r, 1));
            if (pin == target_pin) {
                AdcValue = static_cast<int>((*firmata->AnalogSamples)(r, 3));
                AdcReadOk = firmata->IsLinkReady;
                ReadAdcFlag = false;
                return true;
            }
        }
    }

    if (ReadAdcFlag) {
        firmata->SelectedPin = AnalogPin;
        firmata->ReadAnalog = true;
        firmata->Calculate();
        AdcValue = firmata->AnalogPinValue;
        AdcReadOk = firmata->IsLinkReady;
        ReadAdcFlag = false;
    }
    return true;
}

} // namespace RDK
