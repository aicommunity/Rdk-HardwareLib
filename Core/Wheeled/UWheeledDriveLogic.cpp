#include "UWheeledDriveLogic.h"

#include "Catalog/UHardwareCatalog.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QtGlobal>

namespace RDK {

int UWheeledDriveLogic::clampPwm(int v)
{
    return qBound(0, v, 255);
}

int UWheeledDriveLogic::signedPwm(int pwm, int dir)
{
    const int p = clampPwm(pwm);
    return dir ? p : -p;
}

bool UWheeledDriveLogic::processEdges(bool applyDrive, bool stop, UWheeledDriveCommand* cmd,
                                      bool* applyCleared, bool* stopCleared)
{
    if (applyCleared)
        *applyCleared = false;
    if (stopCleared)
        *stopCleared = false;
    if (!cmd)
        return false;
    if (stop) {
        cmd->leftPwm = 0;
        cmd->rightPwm = 0;
        if (stopCleared)
            *stopCleared = true;
        return true;
    }
    if (applyDrive) {
        cmd->leftPwm = clampPwm(cmd->leftPwm);
        cmd->rightPwm = clampPwm(cmd->rightPwm);
        cmd->leftDir = cmd->leftDir ? 1 : 0;
        cmd->rightDir = cmd->rightDir ? 1 : 0;
        if (applyCleared)
            *applyCleared = true;
        return true;
    }
    return false;
}

bool UWheeledDriveLogic::shouldRetransmit(qint64 nowMs, qint64 lastTxMs, int intervalMs)
{
    if (intervalMs <= 0)
        return false;
    return (nowMs - lastTxMs) >= intervalMs;
}

QStringList UWheeledDriveLogic::buildMotorHubCommands(const UWheeledDriveCommand& cmd)
{
    QStringList out;
    out << QStringLiteral("MOTOR A DIR %1").arg(cmd.leftDir ? 1 : 0);
    out << QStringLiteral("MOTOR A %1").arg(clampPwm(cmd.leftPwm));
    out << QStringLiteral("MOTOR B DIR %1").arg(cmd.rightDir ? 1 : 0);
    out << QStringLiteral("MOTOR B %1").arg(clampPwm(cmd.rightPwm));
    return out;
}

QStringList UWheeledDriveLogic::buildMotorStopCommands()
{
    return {QStringLiteral("MOTOR STOP")};
}

QString UWheeledDriveLogic::buildWaveshareT11Json(const UWheeledDriveCommand& cmd)
{
    QJsonObject o;
    o.insert(QStringLiteral("T"), 11);
    o.insert(QStringLiteral("L"), signedPwm(cmd.leftPwm, cmd.leftDir));
    o.insert(QStringLiteral("R"), signedPwm(cmd.rightPwm, cmd.rightDir));
    return QString::fromUtf8(QJsonDocument(o).toJson(QJsonDocument::Compact));
}

QString UWheeledDriveLogic::buildWaveshareT1Json(const UWheeledDriveCommand& cmd)
{
    QJsonObject o;
    o.insert(QStringLiteral("T"), 1);
    o.insert(QStringLiteral("L"), signedPwm(cmd.leftPwm, cmd.leftDir) / 255.0);
    o.insert(QStringLiteral("R"), signedPwm(cmd.rightPwm, cmd.rightDir) / 255.0);
    return QString::fromUtf8(QJsonDocument(o).toJson(QJsonDocument::Compact));
}

QStringList UWheeledDriveLogic::buildSetPinCommands(const QString& motorDriverId)
{
    QStringList out;
    if (motorDriverId.isEmpty())
        return out;
    QString err;
    if (!UHardwareCatalog::instance().isLoaded())
        UHardwareCatalog::instance().load(&err);
    const UHwShieldInfo* shield = UHardwareCatalog::instance().shield(motorDriverId);
    if (!shield)
        return out;
    if (!shield->controlModel.isEmpty()
        && shield->controlModel != QLatin1String("dir_pwm")
        && shield->controlModel != QLatin1String("dir2_pwm"))
        return out;
    auto emitCh = [&](const QString& label, const UHwMotorChannel& ch) {
        const auto setPin = [&](const QString& role, const QString& pin) {
            out << QStringLiteral("SET PIN %1 %2 %3")
                       .arg(label, role, pin.isEmpty() ? QStringLiteral("NONE") : pin);
        };
        setPin(QStringLiteral("dir"), ch.dir);
        setPin(QStringLiteral("dir2"), ch.dir2);
        setPin(QStringLiteral("pwm"), ch.pwm);
        setPin(QStringLiteral("enable"), ch.enable);
        setPin(QStringLiteral("brake"), ch.brake);
        setPin(QStringLiteral("sense"), ch.sense);
    };
    if (shield->channels.contains(QStringLiteral("A")))
        emitCh(QStringLiteral("A"), shield->channels.value(QStringLiteral("A")));
    if (shield->channels.contains(QStringLiteral("B")))
        emitCh(QStringLiteral("B"), shield->channels.value(QStringLiteral("B")));
    return out;
}

} // namespace RDK
