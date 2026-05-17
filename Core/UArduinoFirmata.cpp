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
    , RestartFirmata("RestartFirmata", this)
    , ApplyPinConfig("ApplyPinConfig", this)
    , IsFirmataReady("IsFirmataReady", this)
    , IsLinkReady("IsLinkReady", this)
    , ReportAnalogEnable("ReportAnalogEnable", this)
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
    RestartFirmata = false;
    ApplyPinConfig = false;
    IsFirmataReady = false;
    IsLinkReady = false;
    ReportAnalogEnable = false;
    BundledFirmwareId = "standard_firmata";
    HandshakeSent = false;
    SyncFirmataStates();
    return true;
}

void UArduinoFirmata::SyncFirmataStates()
{
    IsFirmataReady = FirmataReady;
    IsLinkReady = IsConnected && FirmataReady;
}

void UArduinoFirmata::StartFirmataHandshake()
{
    FirmataClient.reset();
    if (Session && Session->isOpen())
        FirmataClient.startHandshake(session());
    HandshakeSent = true;
}

void UArduinoFirmata::ProcessFirmataEdges()
{
    if (RestartFirmata) {
        HandshakeSent = false;
        FirmataReady = false;
        if (IsConnected)
            StartFirmataHandshake();
        ResetEdge(RestartFirmata);
    }

    if (ApplyPinConfig) {
        RunFirmataActions();
        ResetEdge(ApplyPinConfig);
    }
}

bool UArduinoFirmata::EnsureConnected()
{
    const bool ok = UArduinoBoard::EnsureConnected();
    if (ok && !HandshakeSent)
        StartFirmataHandshake();
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
    FirmataReady = FirmataClient.HandshakeReady;
    FirmataFirmwareVersion = FirmataClient.FirmwareVersion.toStdString();
}

void UArduinoFirmata::RunFirmataActions()
{
    if (!Session || !Session->isOpen() || !FirmataClient.HandshakeReady)
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
    if (ReadAnalogFlag || ReportAnalogEnable) {
        FirmataClient.reportAnalog(session(), SelectedPin, 1);
        AnalogPinValue = FirmataClient.analogValue(SelectedPin);
        if (ReadAnalogFlag)
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
    ProcessFirmataEdges();
    const bool ok = UArduinoBoard::ACalculate();
    SyncFirmataStates();
    return ok;
}

} // namespace RDK
