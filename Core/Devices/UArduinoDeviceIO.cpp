#include "UArduinoDeviceIO.h"

#include "UArduinoDevicePinResolver.h"
#include "UArduinoFirmataBatchBuilder.h"
#include "UArduinoFirmata.h"
#include "UArduinoPropertyString.h"
#include "Catalog/UHardwareCatalog.h"
#include "Catalog/UHardwareSetup.h"
#include "Transport/UArduinoPinMap.h"

namespace RDK {

UArduinoDeviceIO::UArduinoDeviceIO()
    : LinkedFirmataName("LinkedFirmataName", this)
    , ModuleId("ModuleId", this)
    , DeviceId("DeviceId", this)
    , Port("Port", this)
    , Channel("Channel", this)
    , Role("Role", this)
    , BoardProfile("BoardProfile", this)
    , HardwareSetupPath("HardwareSetupPath", this)
    , Value("Value", this)
    , ValueRaw("ValueRaw", this)
    , ValueIn("ValueIn", this)
    , InputMode("InputMode", this)
    , IsOnline("IsOnline", this)
    , LastError("LastError", this)
    , ApplyConfig("ApplyConfig", this)
    , WriteOutput("WriteOutput", this)
    , ReadInput("ReadInput", this)
    , Continuous("Continuous", this)
{
}

UArduinoDeviceIO::~UArduinoDeviceIO() = default;

UArduinoDeviceIO* UArduinoDeviceIO::New()
{
    return new UArduinoDeviceIO;
}

bool UArduinoDeviceIO::ADefault()
{
    LinkedFirmataName = "";
    ModuleId = "potentiometer";
    DeviceId = "";
    Port = "A0";
    Channel = "";
    Role = 2;
    BoardProfile = 0;
    HardwareSetupPath = "";
    Value = 0.0;
    ValueRaw = 0;
    ValueIn = 0.0;
    InputMode = 0;
    IsOnline = false;
    LastError = "";
    ApplyConfig = false;
    WriteOutput = false;
    ReadInput = false;
    Continuous = true;
    LastValueIn = 0.0;
    HadValueIn = false;
    return true;
}

int UArduinoDeviceIO::resolveRole(const QString& signal_type) const
{
    if (Role == 0 || Role == 1)
        return Role;
    if (signal_type.contains(QStringLiteral("out"), Qt::CaseInsensitive)
        || signal_type.contains(QStringLiteral("servo"), Qt::CaseInsensitive)
        || signal_type.contains(QStringLiteral("motor"), Qt::CaseInsensitive)
        || signal_type.contains(QStringLiteral("pwm"), Qt::CaseInsensitive))
        return 1;
    return 0;
}

bool UArduinoDeviceIO::ACalculate()
{
    IsOnline = false;
    if (LinkedFirmataName->empty()) {
        LastError = "LinkedFirmataName empty";
        return true;
    }

    UContainer* owner = dynamic_cast<UContainer*>(GetOwner().Get());
    if (!owner) {
        LastError = "No owner container";
        return true;
    }

    UEPtr<UArduinoFirmata> firmata =
        owner->GetComponentL<UArduinoFirmata>(*LinkedFirmataName, true);
    if (!firmata) {
        LastError = "Firmata not found";
        ApplyConfig = false;
        WriteOutput = false;
        ReadInput = false;
        return true;
    }

    UHardwareCatalog& catalog = UHardwareCatalog::instance();
    if (!catalog.isLoaded())
        catalog.load(nullptr);

    const QString module_id = UArduinoPropertyString::fromStdProperty(*ModuleId);
    const UHwModuleInfo* mod = catalog.module(module_id);
    if (!mod) {
        LastError = UArduinoPropertyString::toStdProperty(
            QStringLiteral("Unknown ModuleId: %1").arg(module_id));
        return true;
    }

    if (module_id == QLatin1String("dht11") || module_id == QLatin1String("hc_sr04")) {
        LastError = UArduinoPropertyString::toStdProperty(
            QStringLiteral("Module %1 unsupported on Standard Firmata; use hub firmware")
                .arg(module_id));
        IsOnline = false;
        ApplyConfig = false;
        WriteOutput = false;
        ReadInput = false;
        return true;
    }

    UHwSetupDocument setup_doc;
    const UHwSetupDocument* setup_ptr = nullptr;
    UHardwareSetup setup;
    const QString setup_path = UArduinoPropertyString::fromStdProperty(*HardwareSetupPath);
    if (!setup_path.isEmpty() && setup.loadFromFile(setup_path, nullptr)) {
        setup_doc = setup.document();
        setup_ptr = &setup_doc;
    } else {
        const QString firmata_path =
            UArduinoPropertyString::fromStdProperty(*firmata->HardwareSetupPath);
        if (!firmata_path.isEmpty() && setup.loadFromFile(firmata_path, nullptr)) {
            setup_doc = setup.document();
            setup_ptr = &setup_doc;
        }
    }

    const int profile = BoardProfile;
    const UResolvedDevicePins pins = UArduinoDevicePinResolver::resolve(
        catalog, setup_ptr, module_id,
        UArduinoPropertyString::fromStdProperty(*Port),
        UArduinoPropertyString::fromStdProperty(*Channel), profile);

    if (!pins.error.isEmpty()) {
        LastError = UArduinoPropertyString::toStdProperty(pins.error);
        return true;
    }

    LastError = "";
    IsOnline = firmata->IsLinkReady;
    const int role = resolveRole(mod->signalType);

    if (ApplyConfig) {
        UArduinoFirmataBatchBuilder::clearPinConfig(*firmata->PinConfigBatch);
        if (mod->signalType.contains(QStringLiteral("analog"), Qt::CaseInsensitive)) {
            UArduinoFirmataBatchBuilder::appendPinMode(
                *firmata->PinConfigBatch, pins.signalPin, UArduinoFirmataBatchBuilder::kModeAnalog);
            firmata->ReportAnalogEnable = true;
        } else if (mod->signalType.contains(QStringLiteral("servo"), Qt::CaseInsensitive)) {
            UArduinoFirmataBatchBuilder::appendPinMode(
                *firmata->PinConfigBatch, pins.signalPin, UArduinoFirmataBatchBuilder::kModeServo);
            firmata->ServoPin = pins.signalPin;
            firmata->ConfigureServo = true;
        } else if (mod->signalType.contains(QStringLiteral("motor"), Qt::CaseInsensitive)) {
            UArduinoFirmataBatchBuilder::appendPinMode(
                *firmata->PinConfigBatch, pins.dirPin, UArduinoFirmataBatchBuilder::kModeOutput);
            if (pins.dir2Pin >= 0)
                UArduinoFirmataBatchBuilder::appendPinMode(
                    *firmata->PinConfigBatch, pins.dir2Pin, UArduinoFirmataBatchBuilder::kModeOutput);
            UArduinoFirmataBatchBuilder::appendPinMode(
                *firmata->PinConfigBatch, pins.pwmPin, UArduinoFirmataBatchBuilder::kModePwm);
            if (pins.brakePin >= 0)
                UArduinoFirmataBatchBuilder::appendPinMode(
                    *firmata->PinConfigBatch, pins.brakePin, UArduinoFirmataBatchBuilder::kModeOutput);
            if (pins.enablePin >= 0)
                UArduinoFirmataBatchBuilder::appendPinMode(
                    *firmata->PinConfigBatch, pins.enablePin, UArduinoFirmataBatchBuilder::kModeOutput);
            if (pins.sensePin >= 0)
                UArduinoFirmataBatchBuilder::appendPinMode(
                    *firmata->PinConfigBatch, pins.sensePin, UArduinoFirmataBatchBuilder::kModeAnalog);
        } else if (role == 1) {
            UArduinoFirmataBatchBuilder::appendPinMode(
                *firmata->PinConfigBatch, pins.signalPin, UArduinoFirmataBatchBuilder::kModeOutput);
        } else {
            UArduinoFirmataBatchBuilder::appendPinMode(
                *firmata->PinConfigBatch, pins.signalPin, UArduinoFirmataBatchBuilder::kModeInput);
        }
        firmata->ApplyPinConfig = true;
        ApplyConfig = false;
    }

    if (role == 0) {
        if (Continuous || ReadInput) {
            bool found = false;
            if (UArduinoPinMap::isAnalogLabel(UArduinoPropertyString::fromStdProperty(*Port))
                || mod->signalType.contains(QStringLiteral("analog"), Qt::CaseInsensitive)) {
                for (int r = firmata->AnalogSamples->GetRows() - 1; r >= 0; --r) {
                    const int pin = static_cast<int>((*firmata->AnalogSamples)(r, 1));
                    if (pin == pins.signalPin) {
                        ValueRaw = static_cast<int>((*firmata->AnalogSamples)(r, 3));
                        Value = ValueRaw / 1023.0;
                        found = true;
                        break;
                    }
                }
                if (!found && ReadInput) {
                    firmata->SelectedPin = pins.signalPin;
                    firmata->ReadAnalog = true;
                }
            } else {
                for (int r = firmata->DigitalSamples->GetRows() - 1; r >= 0; --r) {
                    const int pin = static_cast<int>((*firmata->DigitalSamples)(r, 1));
                    if (pin == pins.signalPin) {
                        ValueRaw = static_cast<int>((*firmata->DigitalSamples)(r, 2));
                        Value = ValueRaw ? 1.0 : 0.0;
                        found = true;
                        break;
                    }
                }
            }
            ReadInput = false;
        }
        return true;
    }

    const bool value_changed = HadValueIn && qAbs(ValueIn - LastValueIn) > 1e-9;
    const bool do_write = WriteOutput || value_changed || (!HadValueIn && WriteOutput);
    HadValueIn = true;
    LastValueIn = ValueIn;

    if (do_write) {
        double vin = ValueIn;
        int raw = 0;
        if (InputMode == 0) {
            if (mod->signalType.contains(QStringLiteral("servo"), Qt::CaseInsensitive))
                raw = qBound(0, static_cast<int>(vin * 180.0), 180);
            else if (mod->signalType.contains(QStringLiteral("pwm"), Qt::CaseInsensitive)
                     || mod->signalType.contains(QStringLiteral("motor"), Qt::CaseInsensitive))
                raw = qBound(0, static_cast<int>(qAbs(vin) * 255.0), 255);
            else
                raw = vin >= 0.5 ? 1 : 0;
        } else {
            raw = static_cast<int>(vin);
        }
        ValueRaw = raw;
        Value = vin;

        if (mod->signalType.contains(QStringLiteral("servo"), Qt::CaseInsensitive)) {
            firmata->ServoPin = pins.signalPin;
            firmata->ServoAngle = raw;
            firmata->WriteServo = true;
        } else if (mod->signalType.contains(QStringLiteral("motor"), Qt::CaseInsensitive)) {
            const bool forward = vin >= 0;
            UArduinoFirmataBatchBuilder::appendDigital(*firmata->DigitalOutputCommands, pins.dirPin,
                                                       forward ? 1 : 0);
            if (pins.dir2Pin >= 0)
                UArduinoFirmataBatchBuilder::appendDigital(*firmata->DigitalOutputCommands,
                                                           pins.dir2Pin, forward ? 0 : 1);
            if (pins.brakePin >= 0)
                UArduinoFirmataBatchBuilder::appendDigital(
                    *firmata->DigitalOutputCommands, pins.brakePin, qAbs(vin) < 1e-3 ? 1 : 0);
            if (pins.enablePin >= 0)
                UArduinoFirmataBatchBuilder::appendDigital(*firmata->DigitalOutputCommands,
                                                           pins.enablePin, 1);
            UArduinoFirmataBatchBuilder::appendAnalog(*firmata->AnalogOutputCommands, pins.pwmPin,
                                                      raw);
        } else if (mod->signalType.contains(QStringLiteral("pwm"), Qt::CaseInsensitive)) {
            firmata->SelectedPin = pins.signalPin;
            firmata->PwmPinValue = raw;
            firmata->WritePwm = true;
        } else {
            UArduinoFirmataBatchBuilder::appendDigital(*firmata->DigitalOutputCommands,
                                                       pins.signalPin, raw);
        }
        WriteOutput = false;
    }

    return true;
}

} // namespace RDK
