#include "UFirmwareManifest.h"

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
    const QString relative = bundledHexRelativePath(bundled_id, board_key);
    if (relative.isEmpty())
        return QString();

    const QString resolved = resolveFromApplicationDir(relative);
    if (QFile::exists(resolved))
        return resolved;

    const QString manifest_path = manifestPath();
    if (manifest_path.isEmpty())
        return resolved;

    QFile file(manifest_path);
    if (!file.open(QIODevice::ReadOnly))
        return resolved;

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
    return QString();
}

} // namespace RDK
