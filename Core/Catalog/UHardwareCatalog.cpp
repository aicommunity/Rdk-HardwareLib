#include "UHardwareCatalog.h"

#include "UHardwareCatalogPaths.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>

namespace RDK {

namespace {

QStringList toStringList(const QJsonArray& arr)
{
    QStringList out;
    for (const QJsonValue& v : arr)
        out.append(v.toString());
    return out;
}

bool readJsonFile(const QString& path, QJsonObject* obj, QString* error)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        if (error)
            *error = QStringLiteral("Cannot open %1").arg(path);
        return false;
    }
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isObject()) {
        if (error)
            *error = QStringLiteral("Invalid JSON object: %1").arg(path);
        return false;
    }
    *obj = doc.object();
    return true;
}

} // namespace

UHardwareCatalog& UHardwareCatalog::instance()
{
    static UHardwareCatalog catalog;
    return catalog;
}

void UHardwareCatalog::unload()
{
    Loaded = false;
    RootPath.clear();
    Boards.clear();
    Shields.clear();
    Modules.clear();
    Firmwares.clear();
}

bool UHardwareCatalog::reload(QString* error)
{
    unload();
    return load(error);
}

bool UHardwareCatalog::load(QString* error)
{
    if (Loaded)
        return true;
    const QString root = UHardwareCatalogPaths::catalogRoot();
    if (root.isEmpty()) {
        if (error)
            *error = QStringLiteral("Hardware catalog root not found (set RDK_HARDWARE_CATALOG_DIR or NMSDK_SOURCE_DIR)");
        return false;
    }
    return loadFromRoot(root, error);
}

bool UHardwareCatalog::loadFromRoot(const QString& root, QString* error)
{
    QJsonObject index;
    const QString index_path = QDir(root).filePath(QStringLiteral("catalog.json"));
    if (!readJsonFile(index_path, &index, error))
        return false;

    auto loadListed = [&](const char* key, auto&& loader) -> bool {
        const QJsonArray list = index.value(QLatin1String(key)).toArray();
        for (const QJsonValue& rel : list) {
            const QString path = QDir(root).filePath(rel.toString());
            QJsonObject obj;
            if (!readJsonFile(path, &obj, error))
                return false;
            if (!loader(obj, path))
                return false;
        }
        return true;
    };

    if (!loadListed("boards", [&](const QJsonObject& obj, const QString& path) {
            UHwBoardInfo info;
            info.id = obj.value(QStringLiteral("id")).toString();
            if (info.id.isEmpty()) {
                if (error)
                    *error = QStringLiteral("Board missing id: %1").arg(path);
                return false;
            }
            info.title = obj.value(QStringLiteral("title")).toString(info.id);
            info.boardProfileInt = obj.value(QStringLiteral("boardProfileInt")).toInt(0);
            info.mcu = obj.value(QStringLiteral("mcu")).toString();
            info.protocol = obj.value(QStringLiteral("protocol")).toString();
            info.uploadBaud = obj.value(QStringLiteral("uploadBaud")).toInt(115200);
            info.runtimeBaudDefault = obj.value(QStringLiteral("runtimeBaudDefault")).toInt(57600);
            info.firmataPinCount = obj.value(QStringLiteral("firmataPinCount")).toInt(20);
            info.analogBase = obj.value(QStringLiteral("analogBase")).toInt(14);
            info.pinoutSvg = obj.value(QStringLiteral("pinoutSvg")).toString();
            info.pinsJson = obj.value(QStringLiteral("pinsJson")).toString();
            Boards.insert(info.id, info);
            return true;
        }))
        return false;

    if (!loadListed("shields", [&](const QJsonObject& obj, const QString& path) {
            UHwShieldInfo info;
            info.id = obj.value(QStringLiteral("id")).toString();
            if (info.id.isEmpty()) {
                if (error)
                    *error = QStringLiteral("Shield missing id: %1").arg(path);
                return false;
            }
            info.title = obj.value(QStringLiteral("title")).toString(info.id);
            info.compatibleBoards = toStringList(obj.value(QStringLiteral("compatibleBoards")).toArray());
            info.defaultFirmware = obj.value(QStringLiteral("defaultFirmware")).toString();
            info.layoutAsset = obj.value(QStringLiteral("layoutAsset")).toString();
            info.occupiesPins = toStringList(obj.value(QStringLiteral("occupiesPins")).toArray());
            info.compatibleModules = toStringList(obj.value(QStringLiteral("compatibleModules")).toArray());
            const QJsonObject power = obj.value(QStringLiteral("power")).toObject();
            info.warnings = toStringList(power.value(QStringLiteral("warnings")).toArray());
            const QJsonObject ports = obj.value(QStringLiteral("ports")).toObject();
            for (auto it = ports.begin(); it != ports.end(); ++it) {
                const QJsonObject p = it.value().toObject();
                UHwShieldPort port;
                port.signalPin = p.value(QStringLiteral("signalPin")).toString(it.key());
                port.header = p.value(QStringLiteral("header")).toString();
                port.powerRail = p.value(QStringLiteral("powerRail")).toString();
                port.reserved = p.value(QStringLiteral("reserved")).toString();
                info.ports.insert(it.key(), port);
            }
            const QJsonObject channels = obj.value(QStringLiteral("channels")).toObject();
            for (auto it = channels.begin(); it != channels.end(); ++it) {
                const QJsonObject c = it.value().toObject();
                UHwMotorChannel ch;
                ch.dir = c.value(QStringLiteral("dir")).toString();
                ch.dir2 = c.value(QStringLiteral("dir2")).toString();
                ch.pwm = c.value(QStringLiteral("pwm")).toString();
                ch.brake = c.value(QStringLiteral("brake")).toString();
                ch.sense = c.value(QStringLiteral("sense")).toString();
                ch.enable = c.value(QStringLiteral("enable")).toString();
                info.channels.insert(it.key(), ch);
            }
            Shields.insert(info.id, info);
            return true;
        }))
        return false;

    if (!loadListed("modules", [&](const QJsonObject& obj, const QString& path) {
            UHwModuleInfo info;
            info.id = obj.value(QStringLiteral("id")).toString();
            if (info.id.isEmpty()) {
                if (error)
                    *error = QStringLiteral("Module missing id: %1").arg(path);
                return false;
            }
            info.title = obj.value(QStringLiteral("title")).toString(info.id);
            info.signalType = obj.value(QStringLiteral("signalType")).toString();
            info.wires = toStringList(obj.value(QStringLiteral("wires")).toArray());
            info.roles = toStringList(obj.value(QStringLiteral("roles")).toArray());
            info.portKind = obj.value(QStringLiteral("portKind")).toString();
            info.channelIds = toStringList(obj.value(QStringLiteral("channelIds")).toArray());
            info.requiresMcuTiming = obj.value(QStringLiteral("requiresMcuTiming")).toBool(false);
            info.requiredCapabilities = toStringList(obj.value(QStringLiteral("requiredCapabilities")).toArray());
            info.preferredFirmware = toStringList(obj.value(QStringLiteral("preferredFirmware")).toArray());
            info.asset = obj.value(QStringLiteral("asset")).toString();
            Modules.insert(info.id, info);
            return true;
        }))
        return false;

    if (!loadListed("firmwares", [&](const QJsonObject& obj, const QString& path) {
            UHwFirmwareInfo info;
            info.id = obj.value(QStringLiteral("id")).toString();
            if (info.id.isEmpty()) {
                if (error)
                    *error = QStringLiteral("Firmware missing id: %1").arg(path);
                return false;
            }
            info.title = obj.value(QStringLiteral("title")).toString(info.id);
            info.tier = obj.value(QStringLiteral("tier")).toString(QStringLiteral("A"));
            info.transport = obj.value(QStringLiteral("transport")).toString();
            info.available = obj.value(QStringLiteral("available")).toBool(true);
            info.runtimeBaud = obj.value(QStringLiteral("runtimeBaud")).toInt(57600);
            info.boards = toStringList(obj.value(QStringLiteral("boards")).toArray());
            info.compatibleShields = toStringList(obj.value(QStringLiteral("compatibleShields")).toArray());
            info.capabilities = toStringList(obj.value(QStringLiteral("capabilities")).toArray());
            info.hostPlugin = obj.value(QStringLiteral("hostPlugin")).toString();
            info.protocolId = obj.value(QStringLiteral("protocolId")).toString();
            info.legacyManifestId = obj.value(QStringLiteral("legacyManifestId")).toString(info.id);
            const QJsonObject hex = obj.value(QStringLiteral("hex")).toObject();
            for (auto it = hex.begin(); it != hex.end(); ++it)
                info.hex.insert(it.key(), it.value().toString());
            const QJsonObject pins = obj.value(QStringLiteral("defaultPins")).toObject();
            for (auto it = pins.begin(); it != pins.end(); ++it)
                info.defaultPins.insert(it.key(), it.value().toString());
            Firmwares.insert(info.id, info);
            return true;
        }))
        return false;

    RootPath = root;
    Loaded = true;
    return true;
}

const UHwBoardInfo* UHardwareCatalog::board(const QString& id) const
{
    auto it = Boards.constFind(id);
    return it == Boards.constEnd() ? nullptr : &it.value();
}

const UHwShieldInfo* UHardwareCatalog::shield(const QString& id) const
{
    auto it = Shields.constFind(id);
    return it == Shields.constEnd() ? nullptr : &it.value();
}

const UHwModuleInfo* UHardwareCatalog::module(const QString& id) const
{
    auto it = Modules.constFind(id);
    return it == Modules.constEnd() ? nullptr : &it.value();
}

const UHwFirmwareInfo* UHardwareCatalog::firmware(const QString& id) const
{
    auto it = Firmwares.constFind(id);
    return it == Firmwares.constEnd() ? nullptr : &it.value();
}

QStringList UHardwareCatalog::boardIds() const
{
    return Boards.keys();
}

QStringList UHardwareCatalog::shieldIds() const
{
    return Shields.keys();
}

QStringList UHardwareCatalog::moduleIds() const
{
    return Modules.keys();
}

QStringList UHardwareCatalog::firmwareIds(bool onlyAvailable) const
{
    QStringList ids;
    for (auto it = Firmwares.constBegin(); it != Firmwares.constEnd(); ++it) {
        if (!onlyAvailable || it.value().available)
            ids.append(it.key());
    }
    return ids;
}

QStringList UHardwareCatalog::firmwaresForShield(const QString& shieldId, bool onlyAvailable) const
{
    QStringList ids;
    for (auto it = Firmwares.constBegin(); it != Firmwares.constEnd(); ++it) {
        const UHwFirmwareInfo& fw = it.value();
        if (onlyAvailable && !fw.available)
            continue;
        if (fw.compatibleShields.contains(QStringLiteral("*")) || fw.compatibleShields.contains(shieldId)
            || (shieldId.isEmpty() && fw.compatibleShields.isEmpty()))
            ids.append(it.key());
        else if (fw.compatibleShields.contains(shieldId))
            ids.append(it.key());
    }
    // Also include firmwares that list the shield
    ids.removeDuplicates();
    return ids;
}

} // namespace RDK
