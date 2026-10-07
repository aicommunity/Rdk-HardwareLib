#ifndef UESP32WHEELEDROBOT_H
#define UESP32WHEELEDROBOT_H

#include "UEsp32CustomLink.h"
#include "Protocol/IArduinoProtocolPlugin.h"
#include "Wheeled/UWheeledDriveLogic.h"

#include <QMap>

namespace RDK {

/** ESP32 open-loop wheeled robot using nmsdk_motor_hub framed protocol (not WaveRover). */
class RDK_LIB_TYPE UEsp32WheeledRobot : public UEsp32CustomLink, public UArduinoPluginHost {
public:
    UProperty<int, UEsp32WheeledRobot, ptPubParameter> LeftPwm;
    UProperty<int, UEsp32WheeledRobot, ptPubParameter> RightPwm;
    UProperty<int, UEsp32WheeledRobot, ptPubParameter> LeftDir;
    UProperty<int, UEsp32WheeledRobot, ptPubParameter> RightDir;
    UProperty<bool, UEsp32WheeledRobot, ptPubParameter | ptInput> ApplyDrive;
    UProperty<bool, UEsp32WheeledRobot, ptPubParameter | ptInput> Stop;
    UProperty<int, UEsp32WheeledRobot, ptPubParameter> WatchdogMs;
    UProperty<string, UEsp32WheeledRobot, ptPubParameter> MotorDriverId;
    UProperty<bool, UEsp32WheeledRobot, ptPubParameter | ptInput> ApplyMotorDriver;
    UProperty<int, UEsp32WheeledRobot, ptPubState> LeftPwmFb;
    UProperty<int, UEsp32WheeledRobot, ptPubState> RightPwmFb;
    UProperty<double, UEsp32WheeledRobot, ptPubState> LeftSense;
    UProperty<double, UEsp32WheeledRobot, ptPubState> RightSense;
    UProperty<string, UEsp32WheeledRobot, ptPubParameter> HostPluginId;
    UProperty<string, UEsp32WheeledRobot, ptPubState> NamedValuesJson;
    UProperty<bool, UEsp32WheeledRobot, ptPubState> PluginBound;

    UEsp32WheeledRobot();
    ~UEsp32WheeledRobot() override;
    UEsp32WheeledRobot* New() override;

    void enqueueCommand(const QString& line) override;
    int boardProfile() const override;
    int protocolVersion() const override;
    void setProtocolReady(bool ready) override;
    void setLastError(const QString& error) override;
    void publishSensorMatrixRow(const QVector<double>& row) override;
    void publishPinStatusJson(const QString& json) override;
    void publishNamedFloat(const QString& key, float value) override;
    void appendFrameLog(const QString& line) override;

protected:
    bool ADefault() override;
    bool ACalculate() override;
    void OnBinaryFrame(uint8_t type, const QByteArray& payload) override;
    void OnHealthCheck() override;
    void NegotiateProtocol() override;
    void ProcessWheeledEdges();

private:
    IArduinoProtocolPlugin* resolvePlugin() const;
    QMap<QString, float> NamedValues;
};

} // namespace RDK

#endif
