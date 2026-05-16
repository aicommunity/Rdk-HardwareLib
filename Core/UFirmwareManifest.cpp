#include "UFirmwareManifest.h"

#include "Transport/UArduinoBoardProfile.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace RDK {

QString UFirmwareManifest::firmwareRoot()
{
    const QByteArray env = qgetenv("RDK_HARDWARE_FIRMWARE_DIR");
    if (!env.isEmpty()) {
        const QString path = QString::fromUtf8(env);
        if (QDir(path).exists())
            return QDir(path).absolutePath();
    }

    const QStringList candidates = {
        QCoreApplication::applicationDirPath() + QStringLiteral("/../Libraries/Rdk-HardwareLib/Firmware"),
        QDir::currentPath() + QStringLiteral("/Libraries/Rdk-HardwareLib/Firmware"),
        QStringLiteral("%1/Libraries/Rdk-HardwareLib/Firmware").arg(
            qEnvironmentVariable("NMSDK_SOURCE_DIR")),
    };
    for (const QString& c : candidates) {
        if (QDir(c).exists())
            return QDir(c).absolutePath();
    }
    return QDir(QStringLiteral("%1/Firmware").arg(
                    QFileInfo(QStringLiteral(__FILE__)).absolutePath() + QStringLiteral("/..")))
        .absolutePath();
}

QString UFirmwareManifest::resolveBundledHex(const QString& bundledId, int boardProfileInt)
{
    return resolveBundledHex(bundledId,
                           UArduinoBoardProfileUtil::boardKindToManifestKey(
                               UArduinoBoardProfileUtil::profileForKind(boardProfileInt).kind));
}

QString UFirmwareManifest::resolveBundledHex(const QString& bundledId, const QString& boardKey)
{
    if (bundledId.isEmpty())
        return QString();

    const QString manifestPath = QDir(firmwareRoot()).filePath(QStringLiteral("manifest.json"));
    QFile file(manifestPath);
    if (!file.open(QIODevice::ReadOnly))
        return QString();

    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    const QJsonArray bundled = doc.object().value(QStringLiteral("bundled")).toArray();
    for (const QJsonValue& entryVal : bundled) {
        const QJsonObject entry = entryVal.toObject();
        if (entry.value(QStringLiteral("id")).toString() != bundledId)
            continue;
        const QJsonObject boards = entry.value(QStringLiteral("boards")).toObject();
        const QJsonObject board = boards.value(boardKey).toObject();
        const QString hexRel = board.value(QStringLiteral("hex")).toString();
        if (hexRel.isEmpty())
            return QString();
        return QDir(firmwareRoot()).filePath(hexRel);
    }
    return QString();
}

} // namespace RDK
