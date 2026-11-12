#include "UHardwareCatalogPaths.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>

namespace RDK {

namespace {

QString normalizeSlashes(QString path)
{
    path.replace(QLatin1Char('\\'), QLatin1Char('/'));
    return path;
}

QString applicationWorkDirectory()
{
    if (QCoreApplication::instance())
        return QDir::fromNativeSeparators(QCoreApplication::applicationDirPath());
    return QDir::currentPath();
}

} // namespace

QString UHardwareCatalogPaths::bundledCatalogRelativeRoot()
{
    return QStringLiteral("../../HardwareCatalog");
}

QString UHardwareCatalogPaths::catalogRoot()
{
    const QByteArray env = qgetenv("RDK_HARDWARE_CATALOG_DIR");
    if (!env.isEmpty()) {
        const QString path = QString::fromUtf8(env);
        if (QFile::exists(QDir(path).filePath(QStringLiteral("catalog.json"))))
            return QDir(path).absolutePath();
    }

    const QString from_work =
        QDir(applicationWorkDirectory()).absoluteFilePath(bundledCatalogRelativeRoot());
    if (QFile::exists(QDir(from_work).filePath(QStringLiteral("catalog.json"))))
        return QDir(from_work).absolutePath();

    const QStringList candidates = {
        QDir(applicationWorkDirectory())
            .absoluteFilePath(QStringLiteral("../../../Bin/HardwareCatalog")),
        QDir(QString::fromLocal8Bit(qgetenv("NMSDK_SOURCE_DIR")))
            .absoluteFilePath(QStringLiteral("Libraries/Rdk-HardwareLib/Catalog")),
        QDir::currentPath() + QStringLiteral("/Bin/HardwareCatalog"),
        QDir::currentPath() + QStringLiteral("/Libraries/Rdk-HardwareLib/Catalog"),
    };
    for (const QString& c : candidates) {
        if (QFile::exists(QDir(c).filePath(QStringLiteral("catalog.json"))))
            return QDir(normalizeSlashes(c)).absolutePath();
    }
    return QString();
}

QString UHardwareCatalogPaths::catalogIndexPath()
{
    const QString root = catalogRoot();
    if (root.isEmpty())
        return QString();
    return QDir(root).filePath(QStringLiteral("catalog.json"));
}

} // namespace RDK
