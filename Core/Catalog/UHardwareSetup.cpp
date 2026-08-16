#include "UHardwareSetup.h"

#include "UHardwareCatalog.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSet>

namespace RDK {

namespace {

void addIssue(QVector<UHwIssue>* issues, UHwIssueSeverity sev, const QString& code, const QString& msg)
{
    if (!issues)
        return;
    UHwIssue issue;
    issue.severity = sev;
    issue.code = code;
    issue.message = msg;
    issues->append(issue);
}

} // namespace

bool UHardwareSetup::loadFromFile(const QString& path, QString* error)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        if (error)
            *error = QStringLiteral("Cannot open setup: %1").arg(path);
        return false;
    }
    return loadFromJson(file.readAll(), error);
}

bool UHardwareSetup::loadFromJson(const QByteArray& json, QString* error)
{
    const QJsonDocument doc = QJsonDocument::fromJson(json);
    if (!doc.isObject()) {
        if (error)
            *error = QStringLiteral("Setup JSON must be an object");
        return false;
    }
    const QJsonObject obj = doc.object();
    UHwSetupDocument out;
    out.schemaVersion = obj.value(QStringLiteral("schemaVersion")).toInt(1);
    out.board = obj.value(QStringLiteral("board")).toString();
    out.firmwareId = obj.value(QStringLiteral("firmwareId")).toString();
    for (const QJsonValue& v : obj.value(QStringLiteral("stack")).toArray())
        out.stack.append(v.toString());
    for (const QJsonValue& v : obj.value(QStringLiteral("devices")).toArray()) {
        const QJsonObject d = v.toObject();
        UHwSetupDevice device;
        device.id = d.value(QStringLiteral("id")).toString();
        device.module = d.value(QStringLiteral("module")).toString();
        device.port = d.value(QStringLiteral("port")).toString();
        device.channel = d.value(QStringLiteral("channel")).toString();
        device.role = d.value(QStringLiteral("role")).toString();
        out.devices.append(device);
    }
    Doc = out;
    return true;
}

QByteArray UHardwareSetup::toJson(bool compact) const
{
    QJsonObject obj;
    obj.insert(QStringLiteral("schemaVersion"), Doc.schemaVersion);
    obj.insert(QStringLiteral("board"), Doc.board);
    obj.insert(QStringLiteral("firmwareId"), Doc.firmwareId);
    QJsonArray stack;
    for (const QString& s : Doc.stack)
        stack.append(s);
    obj.insert(QStringLiteral("stack"), stack);
    QJsonArray devices;
    for (const UHwSetupDevice& d : Doc.devices) {
        QJsonObject o;
        o.insert(QStringLiteral("id"), d.id);
        o.insert(QStringLiteral("module"), d.module);
        if (!d.port.isEmpty())
            o.insert(QStringLiteral("port"), d.port);
        if (!d.channel.isEmpty())
            o.insert(QStringLiteral("channel"), d.channel);
        if (!d.role.isEmpty())
            o.insert(QStringLiteral("role"), d.role);
        devices.append(o);
    }
    obj.insert(QStringLiteral("devices"), devices);
    return QJsonDocument(obj).toJson(compact ? QJsonDocument::Compact : QJsonDocument::Indented);
}

bool UHardwareSetup::saveToFile(const QString& path, QString* error) const
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (error)
            *error = QStringLiteral("Cannot write setup: %1").arg(path);
        return false;
    }
    file.write(toJson(false));
    return true;
}

UHwSetupDocument UHardwareSetup::fromFirmwareDefaults(const UHwFirmwareInfo& firmware, const QString& boardId)
{
    UHwSetupDocument doc;
    doc.schemaVersion = 1;
    doc.board = boardId;
    doc.firmwareId = firmware.id;
    int i = 0;
    for (auto it = firmware.defaultPins.constBegin(); it != firmware.defaultPins.constEnd(); ++it) {
        UHwSetupDevice device;
        device.id = QStringLiteral("auto_%1").arg(++i);
        device.module = it.key();
        device.port = it.value();
        device.role = QStringLiteral("sensor");
        if (it.key().contains(QStringLiteral("servo"), Qt::CaseInsensitive))
            device.role = QStringLiteral("actuator");
        doc.devices.append(device);
    }
    return doc;
}

QSet<QString> UHardwareSetup::occupiedPins(const UHardwareCatalog& catalog) const
{
    QSet<QString> pins;
    for (const QString& shieldId : Doc.stack) {
        const UHwShieldInfo* shield = catalog.shield(shieldId);
        if (!shield)
            continue;
        for (const QString& p : shield->occupiesPins)
            pins.insert(p);
    }
    for (const UHwSetupDevice& device : Doc.devices) {
        if (!device.port.isEmpty())
            pins.insert(device.port);
        if (device.channel.isEmpty())
            continue;
        for (const QString& shieldId : Doc.stack) {
            const UHwShieldInfo* shield = catalog.shield(shieldId);
            if (!shield || !shield->channels.contains(device.channel))
                continue;
            const UHwMotorChannel& ch = shield->channels.value(device.channel);
            for (const QString& p : {ch.dir, ch.dir2, ch.pwm, ch.brake, ch.sense, ch.enable}) {
                if (!p.isEmpty())
                    pins.insert(p);
            }
        }
    }
    return pins;
}

bool UHardwareSetup::validate(const UHardwareCatalog& catalog, QVector<UHwIssue>* issues) const
{
    if (issues)
        issues->clear();
    bool ok = true;

    if (Doc.schemaVersion != 1) {
        addIssue(issues, UHwIssueSeverity::Error, QStringLiteral("SchemaVersion"),
                 QStringLiteral("Unsupported schemaVersion %1").arg(Doc.schemaVersion));
        ok = false;
    }

    if (!Doc.board.isEmpty() && !catalog.board(Doc.board)) {
        addIssue(issues, UHwIssueSeverity::Error, QStringLiteral("UnknownId"),
                 QStringLiteral("Unknown board id: %1").arg(Doc.board));
        ok = false;
    }

    const UHwFirmwareInfo* fw = Doc.firmwareId.isEmpty() ? nullptr : catalog.firmware(Doc.firmwareId);
    if (!Doc.firmwareId.isEmpty() && !fw) {
        addIssue(issues, UHwIssueSeverity::Error, QStringLiteral("UnknownId"),
                 QStringLiteral("Unknown firmware id: %1").arg(Doc.firmwareId));
        ok = false;
    }
    if (fw && !fw->available) {
        addIssue(issues, UHwIssueSeverity::Error, QStringLiteral("FirmwareUnavailable"),
                 QStringLiteral("Firmware %1 is not available").arg(fw->id));
        ok = false;
    }

    for (const QString& shieldId : Doc.stack) {
        const UHwShieldInfo* shield = catalog.shield(shieldId);
        if (!shield) {
            addIssue(issues, UHwIssueSeverity::Error, QStringLiteral("UnknownId"),
                     QStringLiteral("Unknown shield id: %1").arg(shieldId));
            ok = false;
            continue;
        }
        if (!Doc.board.isEmpty() && !shield->compatibleBoards.contains(Doc.board)) {
            addIssue(issues, UHwIssueSeverity::Error, QStringLiteral("BoardMismatch"),
                     QStringLiteral("Shield %1 incompatible with board %2").arg(shieldId, Doc.board));
            ok = false;
        }
        if (fw && !fw->compatibleShields.contains(QStringLiteral("*"))
            && !fw->compatibleShields.contains(shieldId)) {
            addIssue(issues, UHwIssueSeverity::Error, QStringLiteral("FirmwareShieldIncompatible"),
                     QStringLiteral("Firmware %1 incompatible with shield %2").arg(fw->id, shieldId));
            ok = false;
        }
    }

    QSet<QString> used_ports;
    QSet<QString> device_ids;
    for (const UHwSetupDevice& device : Doc.devices) {
        if (device.id.isEmpty() || device_ids.contains(device.id)) {
            addIssue(issues, UHwIssueSeverity::Error, QStringLiteral("UnknownId"),
                     QStringLiteral("Device id missing or duplicate: %1").arg(device.id));
            ok = false;
        } else {
            device_ids.insert(device.id);
        }

        const UHwModuleInfo* mod = catalog.module(device.module);
        if (!mod) {
            addIssue(issues, UHwIssueSeverity::Error, QStringLiteral("UnknownId"),
                     QStringLiteral("Unknown module: %1").arg(device.module));
            ok = false;
            continue;
        }

        if (mod->requiresMcuTiming && fw && fw->transport == QLatin1String("firmata")) {
            addIssue(issues, UHwIssueSeverity::Warning, QStringLiteral("TimingPreferHub"),
                     QStringLiteral("Module %1 prefers MCU timing hub over Firmata").arg(mod->id));
        }

        if (fw) {
            for (const QString& cap : mod->requiredCapabilities) {
                if (!fw->capabilities.contains(cap)) {
                    addIssue(issues, UHwIssueSeverity::Error, QStringLiteral("ModuleCapabilityMissing"),
                             QStringLiteral("Firmware %1 missing capability %2 for %3")
                                 .arg(fw->id, cap, mod->id));
                    ok = false;
                }
            }
        }

        QString pin_key = device.port;
        if (mod->portKind == QLatin1String("channel")) {
            if (device.channel.isEmpty()) {
                addIssue(issues, UHwIssueSeverity::Error, QStringLiteral("UnknownId"),
                         QStringLiteral("Device %1 needs channel").arg(device.id));
                ok = false;
            }
            pin_key = QStringLiteral("channel:%1").arg(device.channel);
        } else if (!device.port.isEmpty()) {
            bool port_ok = Doc.stack.isEmpty();
            for (const QString& shieldId : Doc.stack) {
                const UHwShieldInfo* shield = catalog.shield(shieldId);
                if (shield && shield->ports.contains(device.port)) {
                    port_ok = true;
                    if (!shield->ports.value(device.port).reserved.isEmpty()) {
                        addIssue(issues, UHwIssueSeverity::Warning, QStringLiteral("ReservedPort"),
                                 QStringLiteral("Port %1 is reserved (%2)")
                                     .arg(device.port, shield->ports.value(device.port).reserved));
                    }
                }
            }
            if (!port_ok && !Doc.stack.isEmpty()) {
                addIssue(issues, UHwIssueSeverity::Error, QStringLiteral("UnknownId"),
                         QStringLiteral("Port %1 not on shield stack").arg(device.port));
                ok = false;
            }
        }

        if (!pin_key.isEmpty()) {
            if (used_ports.contains(pin_key)) {
                addIssue(issues, UHwIssueSeverity::Error, QStringLiteral("PinConflict"),
                         QStringLiteral("Pin/port conflict on %1").arg(pin_key));
                ok = false;
            }
            used_ports.insert(pin_key);
        }
    }

    return ok;
}

} // namespace RDK
