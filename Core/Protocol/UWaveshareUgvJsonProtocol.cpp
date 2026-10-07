#include "UWaveshareUgvJsonProtocol.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QtGlobal>

namespace RDK {
namespace UWaveshareUgvJsonProtocol {

namespace {

QString compact(const QJsonObject& o)
{
    return QString::fromUtf8(QJsonDocument(o).toJson(QJsonDocument::Compact));
}

} // namespace

QString encodeSpeedCtrlT1(double leftNorm, double rightNorm)
{
    QJsonObject o;
    o.insert(QStringLiteral("T"), 1);
    o.insert(QStringLiteral("L"), leftNorm);
    o.insert(QStringLiteral("R"), rightNorm);
    return compact(o);
}

QString encodePwmInputT11(int leftSignedPwm, int rightSignedPwm)
{
    QJsonObject o;
    o.insert(QStringLiteral("T"), 11);
    o.insert(QStringLiteral("L"), qBound(-255, leftSignedPwm, 255));
    o.insert(QStringLiteral("R"), qBound(-255, rightSignedPwm, 255));
    return compact(o);
}

QString encodeHeartbeatT136(int timeoutMs)
{
    QJsonObject o;
    o.insert(QStringLiteral("T"), 136);
    o.insert(QStringLiteral("cmd"), timeoutMs);
    return compact(o);
}

QString encodeBaseFeedbackFlowT130(int on)
{
    QJsonObject o;
    o.insert(QStringLiteral("T"), 130);
    o.insert(QStringLiteral("cmd"), on ? 1 : 0);
    return compact(o);
}

int parseType(const QString& line, QString* error)
{
    QJsonParseError pe{};
    const QJsonDocument doc = QJsonDocument::fromJson(line.toUtf8(), &pe);
    if (!doc.isObject()) {
        if (error)
            *error = pe.errorString();
        return -1;
    }
    return doc.object().value(QStringLiteral("T")).toInt(-1);
}

bool parseFeedbackT1001(const QString& line, double* outL, double* outR, QString* error)
{
    QJsonParseError pe{};
    const QJsonDocument doc = QJsonDocument::fromJson(line.toUtf8(), &pe);
    if (!doc.isObject()) {
        if (error)
            *error = pe.errorString();
        return false;
    }
    const QJsonObject o = doc.object();
    if (o.value(QStringLiteral("T")).toInt() != 1001) {
        if (error)
            *error = QStringLiteral("not T:1001");
        return false;
    }
    if (outL)
        *outL = o.value(QStringLiteral("L")).toDouble();
    if (outR)
        *outR = o.value(QStringLiteral("R")).toDouble();
    return true;
}

} // namespace UWaveshareUgvJsonProtocol
} // namespace RDK
