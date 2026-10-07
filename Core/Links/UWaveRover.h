#ifndef UWAVEROVER_H
#define UWAVEROVER_H

#include "UEsp32JsonLink.h"
#include "Wheeled/UWheeledDriveLogic.h"

namespace RDK {

/** Waveshare WaveRover / UGV JSON link (T:11 drive). Docs may cite WaveRover sources. */
class RDK_LIB_TYPE UWaveRover : public UEsp32JsonLink {
public:
    UProperty<int, UWaveRover, ptPubParameter> LeftPwm;
    UProperty<int, UWaveRover, ptPubParameter> RightPwm;
    UProperty<int, UWaveRover, ptPubParameter> LeftDir;
    UProperty<int, UWaveRover, ptPubParameter> RightDir;
    UProperty<bool, UWaveRover, ptPubParameter | ptInput> ApplyDrive;
    UProperty<bool, UWaveRover, ptPubParameter | ptInput> Stop;
    UProperty<int, UWaveRover, ptPubParameter> WatchdogMs;
    UProperty<int, UWaveRover, ptPubParameter> RetransmitIntervalMs;
    UProperty<bool, UWaveRover, ptPubParameter | ptInput> EnableFeedbackFlow;
    UProperty<double, UWaveRover, ptPubState> FeedbackLeft;
    UProperty<double, UWaveRover, ptPubState> FeedbackRight;

    UWaveRover();
    ~UWaveRover() override;
    UWaveRover* New() override;

protected:
    bool ADefault() override;
    bool ACalculate() override;
    void OnJsonLine(const QString& line) override;
    void ProcessWheeledEdges();

    qint64 LastDriveTxMs = 0;
    UWheeledDriveCommand LastCmd;
    bool HaveLastCmd = false;
};

} // namespace RDK

#endif
