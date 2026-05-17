#include "UArduinoDcDemo.h"

#include "UArduinoSensorSketch.h"

namespace RDK {

UArduinoDcDemo::UArduinoDcDemo()
    : LinkedSketchName("LinkedSketchName", this)
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
    UArduinoCustomLink::ADefault();
    LinkedSketchName = "";
    Speed = 0;
    Acceleration = 0;
    GetSpeed = false;
    BundledFirmwareId = "sensor_lab_v1";
    CachedSpeed = 0.f;
    CachedAcceleration = 0.f;
    return true;
}

bool UArduinoDcDemo::ABuild()
{
    return UArduinoCustomLink::ABuild();
}

bool UArduinoDcDemo::AReset()
{
    GetSpeed = false;
    return UArduinoCustomLink::AReset();
}

bool UArduinoDcDemo::DelegateToLinkedSketch()
{
    if (LinkedSketchName->empty())
        return false;

    UContainer* owner = dynamic_cast<UContainer*>(GetOwner().Get());
    if (!owner)
        return false;

    UEPtr<UArduinoSensorSketch> sketch =
        owner->GetComponentL<UArduinoSensorSketch>(*LinkedSketchName, true);
    if (!sketch)
        return false;

    if (!LinkedSketchName->empty()) {
        LastError =
            "LinkedSketchName is deprecated; use a single ArduinoDcDemo node with PortName";
    }

    const bool send_requested = (SendCommand || SendCommandFlag) && !Command->empty();
    if (send_requested) {
        sketch->Command = Command;
        sketch->SendCommand = true;
        SentCommand = Command;
        SendCommandFlag = false;
        ResetEdge(SendCommand);
    }

    if (GetSpeed) {
        // Deprecated path: no nested Calculate(); read last matrix row if already filled.
        sketch->GetDataFromBuffers = true;
        if (sketch->DoubleMatrixReadings->GetRows() > 0) {
            const int row = qMax(0, sketch->DoubleMatrixReadings->GetRows() - 1);
            const int cols = sketch->DoubleMatrixReadings->GetCols();
            if (cols > 4)
                Speed = static_cast<float>(sketch->DoubleMatrixReadings(row, 4));
            if (cols > 5)
                Acceleration = static_cast<float>(sketch->DoubleMatrixReadings(row, 5));
        }
        ResetEdge(GetSpeed);
    }
    return true;
}

void UArduinoDcDemo::OnBinaryFrame(uint8_t type, const QByteArray& payload)
{
    if (type != 0x01 || payload.size() < 2)
        return;

    const uint8_t param_count = static_cast<uint8_t>(payload[1]);
    if (payload.size() < 2 + param_count * static_cast<int>(sizeof(float)))
        return;

    float values[5] = {0, 0, 0, 0, 0};
    for (int i = 0; i < param_count && i < 5; ++i)
        memcpy(&values[i], payload.constData() + 2 + i * sizeof(float), sizeof(float));

    // Legacy 0x01 layout: t, h, hall, speed[, acceleration]
    if (param_count >= 5) {
        CachedSpeed = values[3];
        CachedAcceleration = values[4];
    } else if (param_count >= 4) {
        CachedSpeed = values[3];
        CachedAcceleration = 0.f;
    } else if (param_count >= 1) {
        CachedSpeed = values[0];
        CachedAcceleration = 0.f;
    }
}

void UArduinoDcDemo::ProcessDcDemoEdges()
{
    if (GetSpeed) {
        Speed = CachedSpeed;
        Acceleration = CachedAcceleration;
        ResetEdge(GetSpeed);
    }
}

bool UArduinoDcDemo::ACalculate()
{
    if (!LinkedSketchName->empty())
        return DelegateToLinkedSketch();

    ProcessDcDemoEdges();
    return UArduinoCustomLink::ACalculate();
}

} // namespace RDK
