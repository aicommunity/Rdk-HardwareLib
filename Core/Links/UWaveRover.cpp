#include "UWaveRover.h"

#include "Protocol/UWaveshareUgvJsonProtocol.h"
#include "UArduinoPropertyString.h"

#include <QDateTime>

namespace RDK {

UWaveRover::UWaveRover()
    : LeftPwm("LeftPwm", this)
    , RightPwm("RightPwm", this)
    , LeftDir("LeftDir", this)
    , RightDir("RightDir", this)
    , ApplyDrive("ApplyDrive", this)
    , Stop("Stop", this)
    , WatchdogMs("WatchdogMs", this)
    , RetransmitIntervalMs("RetransmitIntervalMs", this)
    , EnableFeedbackFlow("EnableFeedbackFlow", this)
    , FeedbackLeft("FeedbackLeft", this)
    , FeedbackRight("FeedbackRight", this)
{
}

UWaveRover::~UWaveRover() = default;

UWaveRover* UWaveRover::New()
{
    return new UWaveRover;
}

bool UWaveRover::ADefault()
{
    UEsp32JsonLink::ADefault();
    LeftPwm = 0;
    RightPwm = 0;
    LeftDir = 1;
    RightDir = 1;
    ApplyDrive = false;
    Stop = false;
    WatchdogMs = 3000;
    RetransmitIntervalMs = 200;
    EnableFeedbackFlow = false;
    FeedbackLeft = 0;
    FeedbackRight = 0;
    BaudRate = 115200;
    BundledFirmwareId = "";
    HaveLastCmd = false;
    return true;
}

void UWaveRover::ProcessWheeledEdges()
{
    if (EnableFeedbackFlow) {
        EnqueueJson(UWaveshareUgvJsonProtocol::encodeBaseFeedbackFlowT130(1));
        EnableFeedbackFlow = false;
    }

    UWheeledDriveCommand cmd;
    cmd.leftPwm = LeftPwm;
    cmd.rightPwm = RightPwm;
    cmd.leftDir = LeftDir;
    cmd.rightDir = RightDir;
    bool applyCleared = false;
    bool stopCleared = false;
    if (UWheeledDriveLogic::processEdges(ApplyDrive, Stop, &cmd, &applyCleared, &stopCleared)) {
        if (stopCleared) {
            cmd.leftPwm = 0;
            cmd.rightPwm = 0;
            LeftPwm = 0;
            RightPwm = 0;
            EnqueueJson(UWheeledDriveLogic::buildWaveshareT11Json(cmd));
            HaveLastCmd = false;
        } else if (applyCleared) {
            EnqueueJson(UWheeledDriveLogic::buildWaveshareT11Json(cmd));
            LastCmd = cmd;
            HaveLastCmd = true;
            LastDriveTxMs = QDateTime::currentMSecsSinceEpoch();
            if (WatchdogMs > 0)
                EnqueueJson(UWaveshareUgvJsonProtocol::encodeHeartbeatT136(WatchdogMs));
        }
    }
    if (applyCleared)
        ResetEdge(ApplyDrive);
    if (stopCleared)
        ResetEdge(Stop);
}

void UWaveRover::OnJsonLine(const QString& line)
{
    UEsp32JsonLink::OnJsonLine(line);
    double l = 0, r = 0;
    if (UWaveshareUgvJsonProtocol::parseFeedbackT1001(line, &l, &r, nullptr)) {
        FeedbackLeft = l;
        FeedbackRight = r;
    }
}

bool UWaveRover::ACalculate()
{
    ProcessWheeledEdges();
    if (HaveLastCmd && ConnectionState == ArduinoConnected) {
        const qint64 now = QDateTime::currentMSecsSinceEpoch();
        if (UWheeledDriveLogic::shouldRetransmit(now, LastDriveTxMs, RetransmitIntervalMs)) {
            EnqueueJson(UWheeledDriveLogic::buildWaveshareT11Json(LastCmd));
            LastDriveTxMs = now;
        }
    }
    return UEsp32JsonLink::ACalculate();
}

} // namespace RDK
