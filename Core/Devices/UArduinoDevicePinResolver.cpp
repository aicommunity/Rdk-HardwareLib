#include "UArduinoDevicePinResolver.h"

#include "Catalog/UHardwareCatalog.h"
#include "Transport/UArduinoPinMap.h"

namespace RDK {

namespace {

int labelToPin(const QString& label, int board_profile, QStringList* occupied)
{
    if (label.isEmpty())
        return -1;
    if (occupied)
        occupied->append(label);
    return UArduinoPinMap::firmataPinForLabel(label, board_profile);
}

const UHwShieldInfo* pickMotorShield(const UHardwareCatalog& catalog, const UHwSetupDocument* setup)
{
    if (!setup)
        return nullptr;
    const UHwShieldInfo* preferred = nullptr;
    for (const QString& id : setup->stack) {
        const UHwShieldInfo* shield = catalog.shield(id);
        if (!shield || shield->channels.isEmpty())
            continue;
        if (id == QLatin1String("motor_shield_r3"))
            return shield;
        if (!preferred)
            preferred = shield;
    }
    return preferred;
}

} // namespace

UResolvedDevicePins UArduinoDevicePinResolver::resolve(const UHardwareCatalog& catalog,
                                                       const UHwSetupDocument* setup_or_null,
                                                       const QString& module_id,
                                                       const QString& port,
                                                       const QString& channel,
                                                       int board_profile)
{
    UResolvedDevicePins out;
    const UHwModuleInfo* mod = catalog.module(module_id);
    if (!mod) {
        out.error = QStringLiteral("Unknown module: %1").arg(module_id);
        return out;
    }

    if (mod->portKind == QLatin1String("channel")) {
        const UHwShieldInfo* shield = pickMotorShield(catalog, setup_or_null);
        if (!shield) {
            out.error = QStringLiteral("No motor shield in setup stack for channel module");
            return out;
        }
        const QString ch = channel.isEmpty() ? QStringLiteral("A") : channel;
        if (!shield->channels.contains(ch)) {
            out.error = QStringLiteral("Unknown motor channel: %1").arg(ch);
            return out;
        }
        const UHwMotorChannel& mc = shield->channels.value(ch);
        out.dirPin = labelToPin(mc.dir, board_profile, &out.occupiedLabels);
        out.dir2Pin = labelToPin(mc.dir2, board_profile, &out.occupiedLabels);
        out.pwmPin = labelToPin(mc.pwm, board_profile, &out.occupiedLabels);
        out.brakePin = labelToPin(mc.brake, board_profile, &out.occupiedLabels);
        out.sensePin = labelToPin(mc.sense, board_profile, &out.occupiedLabels);
        out.enablePin = labelToPin(mc.enable, board_profile, &out.occupiedLabels);
        out.signalPin = out.pwmPin;
        return out;
    }

    if (port.isEmpty()) {
        out.error = QStringLiteral("Port required for module %1").arg(module_id);
        return out;
    }
    out.signalPin = labelToPin(port, board_profile, &out.occupiedLabels);
    if (out.signalPin < 0)
        out.error = QStringLiteral("Cannot map port label: %1").arg(port);
    return out;
}

} // namespace RDK
