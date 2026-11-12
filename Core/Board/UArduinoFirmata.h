#ifndef UARDUINOFIRMATA_H
#define UARDUINOFIRMATA_H

#include "UArduinoBoard.h"
#include "Protocol/UArduinoFirmataClient.h"

namespace RDK {

class RDK_LIB_TYPE UArduinoFirmata : public UArduinoBoard {
public:
    UProperty<bool, UArduinoFirmata, ptPubState> FirmataReady;
    UProperty<string, UArduinoFirmata, ptPubState> FirmataFirmwareVersion;
    UProperty<int, UArduinoFirmata, ptPubParameter> SelectedPin;
    UProperty<int, UArduinoFirmata, ptPubParameter> SelectedPinMode;
    UProperty<int, UArduinoFirmata, ptPubParameter> DigitalPinValue;
    UProperty<int, UArduinoFirmata, ptPubState> AnalogPinValue;
    UProperty<bool, UArduinoFirmata, ptPubState> SetPinModeFlag;
    UProperty<bool, UArduinoFirmata, ptPubState> ReadAnalogFlag;
    UProperty<bool, UArduinoFirmata, ptPubState> WriteDigitalFlag;

    UProperty<bool, UArduinoFirmata, ptPubParameter | ptInput> SetPinMode;
    UProperty<bool, UArduinoFirmata, ptPubParameter | ptInput> WriteDigital;
    UProperty<bool, UArduinoFirmata, ptPubParameter | ptInput> ReadAnalog;
    UProperty<int, UArduinoFirmata, ptPubState> HandshakeStage;

    UProperty<string, UArduinoFirmata, ptPubState> PinStatusJson;
    UProperty<string, UArduinoFirmata, ptPubState> CapabilityJson;
    UProperty<bool, UArduinoFirmata, ptPubParameter> AutoRefreshPins;
    UProperty<bool, UArduinoFirmata, ptPubParameter | ptInput> RefreshPins;

    UProperty<int, UArduinoFirmata, ptPubParameter> SampleBufferRows;
    UProperty<bool, UArduinoFirmata, ptPubParameter> ClearSamplesOnCalculate;
    UProperty<MDMatrix<double>, UArduinoFirmata, ptPubOutput | ptPubState> AnalogSamples;
    UProperty<MDMatrix<double>, UArduinoFirmata, ptPubOutput | ptPubState> DigitalSamples;
    UProperty<MDMatrix<double>, UArduinoFirmata, ptPubParameter | ptPubInput> DigitalOutputCommands;
    UProperty<MDMatrix<double>, UArduinoFirmata, ptPubParameter | ptPubInput> PinConfigBatch;

    UProperty<string, UArduinoFirmata, ptPubState> StreamLog;
    UProperty<bool, UArduinoFirmata, ptPubParameter> StreamLogEnable;

    UProperty<int, UArduinoFirmata, ptPubParameter> PwmPinValue;
    UProperty<bool, UArduinoFirmata, ptPubParameter | ptInput> WritePwm;
    UProperty<MDMatrix<double>, UArduinoFirmata, ptPubParameter | ptPubInput> AnalogOutputCommands;

    UProperty<string, UArduinoFirmata, ptPubParameter> PinConfigPreset;
    UProperty<bool, UArduinoFirmata, ptPubParameter | ptInput> LoadPreset;

    UProperty<int, UArduinoFirmata, ptPubParameter> QueryPin;
    UProperty<bool, UArduinoFirmata, ptPubParameter | ptInput> QueryPinState;

    UProperty<int, UArduinoFirmata, ptPubParameter> ServoPin;
    UProperty<int, UArduinoFirmata, ptPubParameter> ServoAngle;
    UProperty<int, UArduinoFirmata, ptPubParameter> ServoMinPulse;
    UProperty<int, UArduinoFirmata, ptPubParameter> ServoMaxPulse;
    UProperty<bool, UArduinoFirmata, ptPubParameter | ptInput> ConfigureServo;
    UProperty<bool, UArduinoFirmata, ptPubParameter | ptInput> WriteServo;

    UProperty<int, UArduinoFirmata, ptPubParameter> I2cAddress;
    UProperty<string, UArduinoFirmata, ptPubParameter> I2cWriteData;
    UProperty<string, UArduinoFirmata, ptPubState> I2cReadData;
    UProperty<bool, UArduinoFirmata, ptPubParameter | ptInput> I2cWrite;
    UProperty<bool, UArduinoFirmata, ptPubParameter | ptInput> I2cRead;

    UProperty<bool, UArduinoFirmata, ptPubState> I2cReadPending;
    UProperty<bool, UArduinoFirmata, ptPubParameter | ptInput> ApplyHardwareSetup;
    UProperty<string, UArduinoFirmata, ptPubState> LastSetupApplyResult;

    UProperty<bool, UArduinoFirmata, ptPubParameter | ptInput> RestartFirmata;
    UProperty<bool, UArduinoFirmata, ptPubParameter | ptInput> ApplyPinConfig;
    UProperty<bool, UArduinoFirmata, ptPubState> IsFirmataReady;
    UProperty<bool, UArduinoFirmata, ptPubState> IsLinkReady;
    UProperty<bool, UArduinoFirmata, ptPubParameter> ReportAnalogEnable;

    UArduinoFirmata();
    virtual ~UArduinoFirmata();

    UArduinoFirmata* New() override;

protected:
    bool ADefault() override;
    bool ACalculate() override;
    void OnBoardCalculate() override;
    bool EnsureConnected() override;
    void CloseConnection() override;

    void ProcessFirmataEdges();
    void SyncFirmataStates();
    void ProcessFirmata();
    void RunFirmataActions();
    void StartFirmataHandshake();
    void BuildPinStatusJson();
    void UpdateCapabilityJson();
    void AppendAnalogSample(int channel, int value);
    void AppendDigitalSample(int firmata_pin, int value);
    void ApplyLoadPreset();
    void ApplyHardwareSetupFromCatalog();
    void PollI2cReadPending();

    UArduinoFirmataClient FirmataClient;
    bool HandshakeSent = false;
    QByteArray LastSeenI2cPayload;
};

} // namespace RDK

#endif
