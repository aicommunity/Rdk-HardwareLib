#include "UArduinoFlasher.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>

namespace RDK {

UArduinoFlasher::UArduinoFlasher(QObject* parent)
    : QObject(parent)
{
}

QString UArduinoFlasher::locateAvrdudeBinary()
{
    return QStandardPaths::findExecutable(QStringLiteral("avrdude"));
}

QString UArduinoFlasher::locateAvrdudeConf()
{
    const QByteArray env = qgetenv("AVRUDUDE_CONF");
    if (!env.isEmpty() && QFileInfo::exists(QString::fromUtf8(env)))
        return QString::fromUtf8(env);

    const QStringList candidates = {
        QStringLiteral("/etc/avrdude.conf"),
        QDir::homePath() + QStringLiteral("/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino17/etc/avrdude.conf"),
        QDir::homePath() + QStringLiteral("/.arduino15/packages/arduino/tools/avrdude/7.2.0-arduino.1/etc/avrdude.conf"),
    };
    for (const QString& path : candidates) {
        if (QFileInfo::exists(path))
            return path;
    }

    const QString avrdude = locateAvrdudeBinary();
    if (!avrdude.isEmpty()) {
        const QDir dir = QFileInfo(avrdude).dir();
        const QString sibling = dir.filePath(QStringLiteral("../etc/avrdude.conf"));
        if (QFileInfo::exists(sibling))
            return QFileInfo(sibling).absoluteFilePath();
    }
    return QString();
}

QString UArduinoFlasher::buildCommand(const UArduinoBoardProfile& profile,
                                      const QString& port,
                                      const QString& hexPath,
                                      const QString& confPath)
{
    return QStringLiteral("-C\"%1\" -v -p%2 -c%3 -P%4 -b%5 -D -Uflash:w:\"%6\":i")
        .arg(confPath, profile.mcu, profile.protocol, port)
        .arg(profile.uploadBaud)
        .arg(hexPath);
}

bool UArduinoFlasher::flash(const UArduinoBoardProfile& profile,
                            const QString& port,
                            const QString& hexPath,
                            QString* errorOut)
{
    const QString avrdude = locateAvrdudeBinary();
    const QString conf = locateAvrdudeConf();

    if (avrdude.isEmpty()) {
        const QString msg = QStringLiteral("avrdude not found in PATH");
        if (errorOut)
            *errorOut = msg;
        emit finished(false, msg);
        return false;
    }
    if (conf.isEmpty()) {
        const QString msg = QStringLiteral("avrdude.conf not found; set AVRUDUDE_CONF");
        if (errorOut)
            *errorOut = msg;
        emit finished(false, msg);
        return false;
    }
    if (!QFileInfo::exists(hexPath)) {
        const QString msg = QStringLiteral("HEX file not found: %1").arg(hexPath);
        if (errorOut)
            *errorOut = msg;
        emit finished(false, msg);
        return false;
    }

    const QString args = buildCommand(profile, port, hexPath, conf);
    QProcess process;
    process.setProgram(avrdude);
    process.setArguments(QProcess::splitCommand(args));
    process.setProcessChannelMode(QProcess::MergedChannels);

    emit progressChanged(10);
    process.start();
    if (!process.waitForStarted(5000)) {
        const QString msg = QStringLiteral("Failed to start avrdude");
        if (errorOut)
            *errorOut = msg;
        emit finished(false, msg);
        return false;
    }

    emit progressChanged(50);
    process.waitForFinished(-1);
    emit progressChanged(100);

    const bool ok = process.exitCode() == 0;
    const QString output = QString::fromUtf8(process.readAllStandardOutput());
    const QString msg = ok ? QStringLiteral("ok") : output.trimmed();
    if (errorOut)
        *errorOut = msg;
    emit finished(ok, msg);
    return ok;
}

} // namespace RDK
