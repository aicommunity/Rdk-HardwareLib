#ifndef UWAVESHAREUGVJSONPROTOCOL_H
#define UWAVESHAREUGVJSONPROTOCOL_H

#include <QString>

namespace RDK {

/** Encode/decode Waveshare UGV UART JSON (WaveRover). No host UNet coupling. */
namespace UWaveshareUgvJsonProtocol {

QString encodeSpeedCtrlT1(double leftNorm, double rightNorm);
QString encodePwmInputT11(int leftSignedPwm, int rightSignedPwm);
QString encodeHeartbeatT136(int timeoutMs);
QString encodeBaseFeedbackFlowT130(int on);

/** Returns T code, or -1 if not a JSON object with T. */
int parseType(const QString& line, QString* error = nullptr);
bool parseFeedbackT1001(const QString& line, double* outL, double* outR, QString* error = nullptr);

} // namespace UWaveshareUgvJsonProtocol

} // namespace RDK

#endif
