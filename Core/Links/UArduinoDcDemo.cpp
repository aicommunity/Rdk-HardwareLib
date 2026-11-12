#include "UArduinoDcDemo.h"

#include "UArduinoPropertyString.h"
#include "UFirmwareManifest.h"

#include <cstring>

namespace RDK {

UArduinoDcDemo::UArduinoDcDemo()
    : Speed("Speed", this)
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
    Speed = 0;
    Acceleration = 0;
    GetSpeed = false;
    BundledFirmwareId = "sensor_lab_v1";
    FirmwarePath = UArduinoPropertyString::toStdProperty(
        UFirmwareManifest::bundledHexRelativePath(QStringLiteral("sensor_lab_v1"), 0));
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
    ProcessDcDemoEdges();
    return UArduinoCustomLink::ACalculate();
}

} // namespace RDK
