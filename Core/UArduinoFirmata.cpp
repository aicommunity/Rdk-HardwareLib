#include "UArduinoFirmata.h"

#include "Transport/UArduinoSerialSession.h"

namespace RDK {

UArduinoFirmata::UArduinoFirmata()
    : FirmataReady("FirmataReady", this)
    , FirmataFirmwareVersion("FirmataFirmwareVersion", this)
    , SelectedPin("SelectedPin", this)
    , SelectedPinMode("SelectedPinMode", this)
    , DigitalPinValue("DigitalPinValue", this)
    , AnalogPinValue("AnalogPinValue", this)
    , SetPinModeFlag("SetPinModeFlag", this)
    , ReadAnalogFlag("ReadAnalogFlag", this)
    , WriteDigitalFlag("WriteDigitalFlag", this)
{
}

UArduinoFirmata::~UArduinoFirmata() = default;

UArduinoFirmata* UArduinoFirmata::New()
{
    return new UArduinoFirmata;
}

bool UArduinoFirmata::ADefault()
{
    UArduinoBoard::ADefault();
    FirmataReady = false;
    FirmataFirmwareVersion = "";
    SelectedPin = 13;
    SelectedPinMode = 1;
    DigitalPinValue = 0;
    AnalogPinValue = 0;
    SetPinModeFlag = false;
    ReadAnalogFlag = false;
    WriteDigitalFlag = false;
    BundledFirmwareId = "standard_firmata";
    HandshakeSent = false;
    return true;
}

bool UArduinoFirmata::EnsureConnected()
{
    const bool ok = UArduinoBoard::EnsureConnected();
    if (ok && !HandshakeSent) {
        FirmataClient.reset();
        FirmataClient.startHandshake(session());
        HandshakeSent = true;
    }
    if (!ok)
        HandshakeSent = false;
    return ok;
}

void UArduinoFirmata::ProcessFirmata()
{
    if (!Session)
        return;
    const QByteArray data = Session->takeReceivedBytes();
    if (data.isEmpty())
        return;
    TouchActivity();
    LastHealthResponseMs = LastActivityMs;
    FirmataClient.processIncoming(data);
    FirmataReady = FirmataClient.ready;
    FirmataFirmwareVersion = FirmataClient.firmwareVersion.toStdString();
}

void UArduinoFirmata::RunFirmataActions()
{
    if (!Session || !Session->isOpen() || !FirmataClient.ready)
        return;

    if (SetPinModeFlag) {
        FirmataClient.setPinMode(session(), SelectedPin, SelectedPinMode);
        SetPinModeFlag = false;
    }
    if (WriteDigitalFlag) {
        FirmataClient.setPinMode(session(), SelectedPin, 1);
        FirmataClient.digitalWrite(session(), SelectedPin, DigitalPinValue ? 1 : 0);
        WriteDigitalFlag = false;
    }
    if (ReadAnalogFlag) {
        FirmataClient.reportAnalog(session(), SelectedPin, 1);
        AnalogPinValue = FirmataClient.analogValue(SelectedPin);
        ReadAnalogFlag = false;
    }
}

void UArduinoFirmata::OnBoardCalculate()
{
    ProcessFirmata();
    RunFirmataActions();
}

bool UArduinoFirmata::ACalculate()
{
    return UArduinoBoard::ACalculate();
}

} // namespace RDK
