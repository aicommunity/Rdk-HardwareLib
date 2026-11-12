#ifndef UWHEELEDDRIVELOGIC_H
#define UWHEELEDDRIVELOGIC_H

#include <QString>
#include <QStringList>
#include <QtGlobal>

namespace RDK {

/** Pure helper for open-loop left/right PWM+dir (not a UNet). */
struct UWheeledDriveCommand {
    int leftPwm = 0;
    int rightPwm = 0;
    int leftDir = 1;
    int rightDir = 1;
};

class UWheeledDriveLogic {
public:
    static int clampPwm(int v);
    static int signedPwm(int pwm, int dir); // dir 0/1 → signed -255..255

    /** ApplyDrive / Stop edge processing; returns whether TX is needed. */
    static bool processEdges(bool applyDrive, bool stop, UWheeledDriveCommand* cmd,
                             bool* applyCleared, bool* stopCleared);

    static bool shouldRetransmit(qint64 nowMs, qint64 lastTxMs, int intervalMs);

    /** MOTOR A/B lines for nmsdk_motor_hub. */
    static QStringList buildMotorHubCommands(const UWheeledDriveCommand& cmd);
    static QStringList buildMotorStopCommands();

    /** Waveshare UGV T:11 JSON — L/R signed PWM ±255. */
    static QString buildWaveshareT11Json(const UWheeledDriveCommand& cmd);
    /** Waveshare UGV T:1 JSON — L/R normalized ±1 floats. */
    static QString buildWaveshareT1Json(const UWheeledDriveCommand& cmd);

    /** SET PIN lines from catalog shield channels A/B when MotorDriverId known. */
    static QStringList buildSetPinCommands(const QString& motorDriverId);
};

} // namespace RDK

#endif
