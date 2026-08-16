#include "UHardwareLibrary.h"

#include "Protocol/UArduinoProtocolPluginRegistry.h"

namespace RDK {

UHardwareLibrary HardwareLibrary;

UHardwareLibrary::UHardwareLibrary(void)
    : ULibrary("HardwareLibrary", "2.0", GetGlobalVersion())
{
}

void UHardwareLibrary::CreateClassSamples(UStorage *storage)
{
    Q_UNUSED(storage);
    registerBuiltinArduinoProtocolPlugins();
    UContainer *cont;

    cont = new UArduinoBoard;
    cont->SetName("ArduinoBoard");
    cont->Default();
    UploadClass("ArduinoBoard", cont);

    cont = new UArduinoSensorSketch;
    cont->SetName("ArduinoSensorSketch");
    cont->Default();
    UploadClass("ArduinoSensorSketch", cont);

    cont = new UArduinoFirmata;
    cont->SetName("ArduinoFirmata");
    cont->Default();
    UploadClass("ArduinoFirmata", cont);

    cont = new UArduinoAdc;
    cont->SetName("ArduinoAdc");
    cont->Default();
    UploadClass("ArduinoAdc", cont);

    cont = new UArduinoDcDemo;
    cont->SetName("ArduinoDcDemo");
    cont->Default();
    UploadClass("ArduinoDcDemo", cont);

    cont = new UArduinoDeviceIO;
    cont->SetName("ArduinoDeviceIO");
    cont->Default();
    UploadClass("ArduinoDeviceIO", cont);

    cont = new UArduinoCustomFirmware;
    cont->SetName("ArduinoCustomFirmware");
    cont->Default();
    UploadClass("ArduinoCustomFirmware", cont);
}

} // namespace RDK
