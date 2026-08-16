#include "UFirmwareManifest.h"

#include "Catalog/UHardwareCatalog.h"
#include "Transport/UArduinoBoardProfile.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace RDK {

namespace {

QString normalizeSlashes(const QString& path)
{
    QString normalized = path;
    normalized.replace(QLatin1Char('\\'), QLatin1Char('/'));
    return normalized;
}

QString applicationWorkDirectory()
{
    if (QCoreApplication::instance())
        return QDir::fromNativeSeparators(QCoreApplication::applicationDirPath());
    return QDir::currentPath();
}

QString resolveRelativeFromWorkDir(const QString& relative_path)
{
    const QString trimmed = normalizeSlashes(relative_path.trimmed());
    if (trimmed.isEmpty())
        return trimmed;
    if (QDir::isAbsolutePath(trimmed))
        return QDir(trimmed).absolutePath();

    const QString work_dir = applicationWorkDirectory();
    const QString resolved = QDir(work_dir).absoluteFilePath(trimmed);
    if (QFileInfo::exists(resolved))
        return QDir(resolved).absolutePath();
    return resolved;
}

QString devFirmwareRootCandidates()
{
    return QDir(QString::fromLocal8Bit(qgetenv("NMSDK_SOURCE_DIR")))
        .absoluteFilePath(QStringLiteral("Libraries/Rdk-HardwareLib/Firmware"));
}

QString manifestPath()
{
    const QByteArray env = qgetenv("RDK_HARDWARE_FIRMWARE_DIR");
    if (!env.isEmpty()) {
        const QString path = QString::fromUtf8(env);
        if (QFile::exists(path + QStringLiteral("/manifest.json")))
            return QDir(path).filePath(QStringLiteral("manifest.json"));
    }

    const QString from_work =
        resolveRelativeFromWorkDir(UFirmwareManifest::bundledFirmwareRelativeRoot());
    if (QFile::exists(QDir(from_work).filePath(QStringLiteral("manifest.json"))))
        return QDir(from_work).filePath(QStringLiteral("manifest.json"));

    const QStringList candidates = {
        QDir(applicationWorkDirectory()).absoluteFilePath(
            QStringLiteral("../../../Bin/ArduinoFirmware")),
        devFirmwareRootCandidates(),
        QDir::currentPath() + QStringLiteral("/Bin/ArduinoFirmware"),
        QDir::currentPath() + QStringLiteral("/Libraries/Rdk-HardwareLib/Firmware"),
    };
    for (const QString& c : candidates) {
        const QString manifest = QDir(c).filePath(QStringLiteral("manifest.json"));
        if (QFile::exists(manifest))
            return manifest;
    }
    return QString();
}

QString hexRelativeFromManifest(const QString& bundled_id, const QString& board_key)
{
    if (bundled_id.isEmpty())
        return QString();

    const QString manifest_path = manifestPath();
    if (manifest_path.isEmpty())
        return QString();

    QFile file(manifest_path);
    if (!file.open(QIODevice::ReadOnly))
        return QString();

    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    const QJsonArray bundled = doc.object().value(QStringLiteral("bundled")).toArray();
    for (const QJsonValue& entryVal : bundled) {
        const QJsonObject entry = entryVal.toObject();
        if (entry.value(QStringLiteral("id")).toString() != bundled_id)
            continue;
        const QJsonObject boards = entry.value(QStringLiteral("boards")).toObject();
        const QString hex_rel = boards.value(board_key).toObject().value(QStringLiteral("hex")).toString();
        if (hex_rel.isEmpty())
            return QString();
        return UFirmwareManifest::bundledFirmwareRelativeRoot() + QLatin1Char('/') + hex_rel;
    }
    return QString();
}

QJsonObject bundledEntryObject(const QString& bundled_id)
{
    if (bundled_id.isEmpty())
        return {};

    const QString manifest_path = manifestPath();
    if (manifest_path.isEmpty())
        return {};

    QFile file(manifest_path);
    if (!file.open(QIODevice::ReadOnly))
        return {};

    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    const QJsonArray bundled = doc.object().value(QStringLiteral("bundled")).toArray();
    for (const QJsonValue& entryVal : bundled) {
        const QJsonObject entry = entryVal.toObject();
        if (entry.value(QStringLiteral("id")).toString() == bundled_id)
            return entry;
    }
    return {};
}

} // namespace

QString UFirmwareManifest::bundledFirmwareRelativeRoot()
{
    return QStringLiteral("../../ArduinoFirmware");
}

QString UFirmwareManifest::bundledHexRelativePath(const QString& bundled_id, int board_profile_int)
{
    return bundledHexRelativePath(
        bundled_id,
        UArduinoBoardProfileUtil::boardKindToManifestKey(
            UArduinoBoardProfileUtil::profileForKind(board_profile_int).kind));
}

QString UFirmwareManifest::bundledHexRelativePath(const QString& bundled_id,
                                                  const QString& board_key)
{
    UHardwareCatalog& catalog = UHardwareCatalog::instance();
    if (!catalog.isLoaded())
        catalog.load(nullptr);
    if (const UHwFirmwareInfo* fw = catalog.firmware(bundled_id)) {
        const QString hex_rel = fw->hex.value(board_key);
        if (!hex_rel.isEmpty())
            return bundledFirmwareRelativeRoot() + QLatin1Char('/') + hex_rel;
    }
    return hexRelativeFromManifest(bundled_id, board_key);
}

QString UFirmwareManifest::resolveFromApplicationDir(const QString& relative_path)
{
    return resolveRelativeFromWorkDir(relative_path);
}

QString UFirmwareManifest::firmwareRoot()
{
    const QByteArray env = qgetenv("RDK_HARDWARE_FIRMWARE_DIR");
    if (!env.isEmpty()) {
        const QString path = QString::fromUtf8(env);
        if (QDir(path).exists())
            return QDir(path).absolutePath();
    }

    const QString from_work = resolveRelativeFromWorkDir(bundledFirmwareRelativeRoot());
    if (QDir(from_work).exists())
        return from_work;

    const QStringList candidates = {
        QDir(applicationWorkDirectory()).absoluteFilePath(
            QStringLiteral("../../../Bin/ArduinoFirmware")),
        devFirmwareRootCandidates(),
        QDir::currentPath() + QStringLiteral("/Bin/ArduinoFirmware"),
        QDir::currentPath() + QStringLiteral("/Libraries/Rdk-HardwareLib/Firmware"),
    };
    for (const QString& c : candidates) {
        if (QDir(c).exists())
            return QDir(c).absolutePath();
    }
    return QString();
}

QString UFirmwareManifest::resolveBundledHex(const QString& bundled_id, int board_profile_int)
{
    return resolveBundledHex(bundled_id,
                           UArduinoBoardProfileUtil::boardKindToManifestKey(
                               UArduinoBoardProfileUtil::profileForKind(board_profile_int).kind));
}

QString UFirmwareManifest::resolveBundledHex(const QString& bundled_id, const QString& board_key)
{
    UHardwareCatalog& catalog = UHardwareCatalog::instance();
    if (!catalog.isLoaded())
        catalog.load(nullptr);
    if (const UHwFirmwareInfo* fw = catalog.firmware(bundled_id)) {
        const QString hex_rel = fw->hex.value(board_key);
        if (!hex_rel.isEmpty()) {
            const QString via_fw_root = QDir(firmwareRoot()).filePath(hex_rel);
            if (QFile::exists(via_fw_root))
                return via_fw_root;
            const QString via_app = resolveFromApplicationDir(bundledFirmwareRelativeRoot() + QLatin1Char('/')
                                                              + hex_rel);
            if (QFile::exists(via_app))
                return via_app;
        }
    }

    const QString relative = hexRelativeFromManifest(bundled_id, board_key);
    if (!relative.isEmpty()) {
        const QString resolved = resolveFromApplicationDir(relative);
        if (QFile::exists(resolved))
            return resolved;
    }

    const QString manifest_path = manifestPath();
    if (manifest_path.isEmpty())
        return relative.isEmpty() ? QString() : resolveFromApplicationDir(relative);

    QFile file(manifest_path);
    if (!file.open(QIODevice::ReadOnly))
        return relative.isEmpty() ? QString() : resolveFromApplicationDir(relative);

    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    const QJsonArray bundled = doc.object().value(QStringLiteral("bundled")).toArray();
    for (const QJsonValue& entryVal : bundled) {
        const QJsonObject entry = entryVal.toObject();
        if (entry.value(QStringLiteral("id")).toString() != bundled_id)
            continue;
        const QJsonObject boards = entry.value(QStringLiteral("boards")).toObject();
        const QString hex_rel = boards.value(board_key).toObject().value(QStringLiteral("hex")).toString();
        if (hex_rel.isEmpty())
            return QString();
        return QDir(QFileInfo(manifest_path).absolutePath()).filePath(hex_rel);
    }
    return relative.isEmpty() ? QString() : resolveFromApplicationDir(relative);
}

QStringList UFirmwareManifest::bundledDefaultPinLabels(const QString& bundled_id)
{
    UHardwareCatalog& catalog = UHardwareCatalog::instance();
    if (!catalog.isLoaded())
        catalog.load(nullptr);
    if (const UHwFirmwareInfo* fw = catalog.firmware(bundled_id)) {
        QStringList labels;
        for (auto it = fw->defaultPins.constBegin(); it != fw->defaultPins.constEnd(); ++it) {
            const QString label = it.value().trimmed();
            if (!label.isEmpty() && !labels.contains(label))
                labels.append(label);
        }
        if (!labels.isEmpty())
            return labels;
    }
    const QJsonObject pins = bundledEntryObject(bundled_id).value(QStringLiteral("defaultPins")).toObject();
    QStringList labels;
    for (auto it = pins.begin(); it != pins.end(); ++it) {
        const QString label = it.value().toString().trimmed();
        if (!label.isEmpty() && !labels.contains(label))
            labels.append(label);
    }
    return labels;
}

QMap<QString, QString> UFirmwareManifest::bundledDefaultPinRoles(const QString& bundled_id)
{
    UHardwareCatalog& catalog = UHardwareCatalog::instance();
    if (!catalog.isLoaded())
        catalog.load(nullptr);
    if (const UHwFirmwareInfo* fw = catalog.firmware(bundled_id)) {
        QMap<QString, QString> roles;
        for (auto it = fw->defaultPins.constBegin(); it != fw->defaultPins.constEnd(); ++it) {
            const QString label = it.value().trimmed();
            if (label.isEmpty())
                continue;
            roles.insert(label, it.key());
        }
        if (!roles.isEmpty())
            return roles;
    }
    const QJsonObject pins = bundledEntryObject(bundled_id).value(QStringLiteral("defaultPins")).toObject();
    QMap<QString, QString> roles;
    for (auto it = pins.begin(); it != pins.end(); ++it) {
        const QString label = it.value().toString().trimmed();
        if (label.isEmpty())
            continue;
        roles.insert(label, it.key());
    }
    return roles;
}

} // namespace RDK
