#include "UArduinoDcDemo.h"

#include "UArduinoSensorSketch.h"

namespace RDK {

UArduinoDcDemo::UArduinoDcDemo()
    : LinkedSketchName("LinkedSketchName", this)
    , Command("Command", this)
    , SendCommandFlag("SendCommandFlag", this)
    , SentCommand("SentCommand", this)
    , Speed("Speed", this)
    , Acceleration("Acceleration", this)
    , GetSpeed("GetSpeed", this)
{
}

UArduinoDcDemo::~UArduinoDcDemo() = default;

UArduinoDcDemo* UArduinoDcDemo::New()
{
    return new UArduinoDcDemo;
}

bool UArduinoDcDemo::ADefault()
{
    LinkedSketchName = "";
    Command = "";
    SendCommandFlag = false;
    SentCommand = "";
    Speed = 0;
    Acceleration = 0;
    GetSpeed = false;
    return true;
}

bool UArduinoDcDemo::ABuild()
{
    return true;
}

bool UArduinoDcDemo::AReset()
{
    SendCommandFlag = false;
    return true;
}

bool UArduinoDcDemo::ACalculate()
{
    if (LinkedSketchName->empty())
        return true;

    UContainer* owner = dynamic_cast<UContainer*>(GetOwner().Get());
    if (!owner)
        return true;

    UEPtr<UArduinoSensorSketch> sketch = owner->GetComponentL<UArduinoSensorSketch>(*LinkedSketchName, true);
    if (!sketch)
        return true;

    if (SendCommandFlag) {
        sketch->InputCommand = Command;
        sketch->SendCommandFlag = true;
        SentCommand = Command;
        SendCommandFlag = false;
    }

    if (GetSpeed) {
        sketch->GetDataFromBuffers = true;
        sketch->Calculate();
        if (sketch->DoubleMatrixReadings->GetRows() > 0
            && sketch->DoubleMatrixReadings->GetCols() > 4) {
            const int row = qMax(0, sketch->DoubleMatrixReadings->GetRows() - 1);
            Speed = static_cast<float>(sketch->DoubleMatrixReadings(row, 4));
        }
        GetSpeed = false;
    }
    Q_UNUSED(Acceleration);
    return true;
}

} // namespace RDK
