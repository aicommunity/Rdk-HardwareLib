#include "UArduinoFlasher.h"

#include "UArduinoSerialPortUtil.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QThread>

#include <QtSerialPort/QSerialPort>

namespace RDK {

namespace {

QString applicationWorkDirectory()
{
    if (QCoreApplication::instance())
        return QDir::fromNativeSeparators(QCoreApplication::applicationDirPath());
    return QDir::currentPath();
}

QString bundledAvrdudeCandidate()
{
    const QDir toolsBin(QDir(applicationWorkDirectory()).filePath(QStringLiteral("ArduinoTools/bin")));
    if (!toolsBin.exists())
        return QString();

#if defined(Q_OS_WIN)
    const QStringList names = {QStringLiteral("avrdude.exe"), QStringLiteral("avrdude")};
#else
    const QStringList names = {QStringLiteral("avrdude")};
#endif
    for (const QString& name : names) {
        const QString path = toolsBin.absoluteFilePath(name);
        if (QFileInfo::exists(path))
            return QFileInfo(path).absoluteFilePath();
    }
    return QString();
}

QString bundledAvrdudeConfCandidate()
{
    const QDir bundledEtc(QDir(applicationWorkDirectory()).filePath(QStringLiteral("ArduinoTools/etc")));
    const QString bundledConf = bundledEtc.absoluteFilePath(QStringLiteral("avrdude.conf"));
    if (QFileInfo::exists(bundledConf))
        return QFileInfo(bundledConf).absoluteFilePath();
    return QString();
}

QString enrichUploadError(const QString& avrdudeOutput)
{
    if (!avrdudeOutput.contains(QStringLiteral("Permission denied"), Qt::CaseInsensitive))
        return avrdudeOutput.trimmed();

    const QString hint = QStringLiteral(
        "Cannot open serial port (permission denied).\n"
        "On Linux, add your user to group dialout:\n"
        "  sudo usermod -aG dialout $USER\n"
        "Then log out and log in again, or run: newgrp dialout\n"
        "Close Arduino IDE / serial monitors that may hold the port.\n\n"
        "--- avrdude output ---\n");
    return hint + avrdudeOutput.trimmed();
}

void prepareBootloaderEntry(const QString& device_port, UArduinoBoardKind kind)
{
    if (device_port.isEmpty())
        return;

    if (kind == UArduinoBoardKind::Uno) {
        QSerialPort reset_port;
        reset_port.setPortName(device_port);
        reset_port.setBaudRate(1200);
        if (reset_port.open(QIODevice::ReadWrite))
            reset_port.close();
        QThread::msleep(2500);
        return;
    }

    QSerialPort reset_port;
    reset_port.setPortName(device_port);
    reset_port.setBaudRate(1200);
    if (reset_port.open(QIODevice::ReadWrite)) {
        reset_port.setDataTerminalReady(false);
        reset_port.setDataTerminalReady(true);
        reset_port.close();
    }
    QThread::msleep(1500);
}

int parseUploadPercent(const QString& output)
{
    static const QRegularExpression percentPattern(QStringLiteral(R"((\d{1,3})\s*%)"));
    int best = -1;
    QRegularExpressionMatchIterator it = percentPattern.globalMatch(output);
    while (it.hasNext()) {
        const QRegularExpressionMatch match = it.next();
        bool ok = false;
        const int value = match.captured(1).toInt(&ok);
        if (ok)
            best = qMax(best, qBound(0, value, 100));
    }
    return best;
}

} // namespace

UArduinoFlasher::UArduinoFlasher(QObject* parent)
    : QObject(parent)
{
}

QString UArduinoFlasher::locateAvrdudeBinary()
{
    const QByteArray envBin = qgetenv("AVRDUDE");
    if (!envBin.isEmpty() && QFileInfo::exists(QString::fromUtf8(envBin)))
        return QFileInfo(QString::fromUtf8(envBin)).absoluteFilePath();

    const QString inPath = QStandardPaths::findExecutable(QStringLiteral("avrdude"));
    if (!inPath.isEmpty())
        return inPath;

    const QString bundled = bundledAvrdudeCandidate();
    if (!bundled.isEmpty())
        return bundled;

    const QDir toolsRoot(QDir::homePath()
                         + QStringLiteral("/.arduino15/packages/arduino/tools/avrdude"));
    if (toolsRoot.exists()) {
        const QStringList versions =
            toolsRoot.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name | QDir::Reversed);
        for (const QString& ver : versions) {
            const QString candidate = toolsRoot.absoluteFilePath(ver + QStringLiteral("/bin/avrdude"));
            if (QFileInfo::exists(candidate))
                return candidate;
        }
    }
    return QString();
}

QString UArduinoFlasher::locateAvrdudeConf()
{
    const QByteArray env = qgetenv("AVRUDUDE_CONF");
    if (!env.isEmpty() && QFileInfo::exists(QString::fromUtf8(env)))
        return QString::fromUtf8(env);

    const QString bundledConf = bundledAvrdudeConfCandidate();
    if (!bundledConf.isEmpty())
        return bundledConf;

    const QStringList candidates = {QStringLiteral("/etc/avrdude.conf")};
    for (const QString& path : candidates) {
        if (QFileInfo::exists(path))
            return path;
    }

    const QString avrdude = locateAvrdudeBinary();
    if (!avrdude.isEmpty()) {
        const QDir binDir = QFileInfo(avrdude).absoluteDir();
        const QString sibling = binDir.absoluteFilePath(QStringLiteral("../etc/avrdude.conf"));
        if (QFileInfo::exists(sibling))
            return QFileInfo(sibling).absoluteFilePath();
    }

    const QDir toolsRoot(QDir::homePath()
                         + QStringLiteral("/.arduino15/packages/arduino/tools/avrdude"));
    if (toolsRoot.exists()) {
        const QStringList versions =
            toolsRoot.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name | QDir::Reversed);
        for (const QString& ver : versions) {
            const QString candidate =
                toolsRoot.absoluteFilePath(ver + QStringLiteral("/etc/avrdude.conf"));
            if (QFileInfo::exists(candidate))
                return candidate;
        }
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
        const QString msg = QStringLiteral(
            "avrdude not found. Windows: run Bin\\Platform\\Win\\SetupArduinoTools.bat. "
            "Linux: install avrdude package or arduino-cli core install arduino:avr.");
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

    const QString devicePort = UArduinoSerialPortUtil::normalizeDevicePath(port);
    prepareBootloaderEntry(devicePort, profile.kind);
    const QString args = buildCommand(profile, devicePort, hexPath, conf);
    QProcess process;
    process.setProgram(avrdude);
    process.setArguments(QProcess::splitCommand(args));
    process.setProcessChannelMode(QProcess::MergedChannels);

    QString accumulated;
    auto reportProgress = [this, &accumulated](const QByteArray& chunk) {
        if (chunk.isEmpty())
            return;
        accumulated += QString::fromUtf8(chunk);
        const int percent = parseUploadPercent(accumulated);
        if (percent >= 0)
            emit progressChanged(qBound(10, percent, 99));
    };

    QObject::connect(&process, &QProcess::readyReadStandardOutput, [&process, reportProgress]() {
        reportProgress(process.readAllStandardOutput());
    });

    emit progressChanged(10);
    process.start();
    if (!process.waitForStarted(5000)) {
        const QString msg = QStringLiteral("Failed to start avrdude");
        if (errorOut)
            *errorOut = msg;
        emit finished(false, msg);
        return false;
    }

    while (process.state() != QProcess::NotRunning) {
        process.waitForReadyRead(200);
        reportProgress(process.readAllStandardOutput());
    }
    reportProgress(process.readAllStandardOutput());

    const bool ok = process.exitCode() == 0;
    emit progressChanged(ok ? 100 : 0);
    const QString msg = ok ? QStringLiteral("ok") : enrichUploadError(accumulated);
    if (errorOut)
        *errorOut = msg;
    emit finished(ok, msg);
    return ok;
}

} // namespace RDK
