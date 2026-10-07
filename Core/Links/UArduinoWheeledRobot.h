#ifndef UARDUINOWHEELEDROBOT_H
#define UARDUINOWHEELEDROBOT_H

#include "UArduinoCustomLink.h"
#include "Protocol/IArduinoProtocolPlugin.h"
#include "Wheeled/UWheeledDriveLogic.h"

#include <QMap>

namespace RDK {

class RDK_LIB_TYPE UArduinoWheeledRobot : public UArduinoCustomLink, public UArduinoPluginHost {
public:
    UProperty<int, UArduinoWheeledRobot, ptPubParameter> LeftPwm;
    UProperty<int, UArduinoWheeledRobot, ptPubParameter> RightPwm;
    UProperty<int, UArduinoWheeledRobot, ptPubParameter> LeftDir;
    UProperty<int, UArduinoWheeledRobot, ptPubParameter> RightDir;
    UProperty<bool, UArduinoWheeledRobot, ptPubParameter | ptInput> ApplyDrive;
    UProperty<bool, UArduinoWheeledRobot, ptPubParameter | ptInput> Stop;
    UProperty<int, UArduinoWheeledRobot, ptPubParameter> WatchdogMs;
    UProperty<string, UArduinoWheeledRobot, ptPubParameter> MotorDriverId;
    UProperty<bool, UArduinoWheeledRobot, ptPubParameter | ptInput> ApplyMotorDriver;

    UProperty<int, UArduinoWheeledRobot, ptPubState> LeftPwmFb;
    UProperty<int, UArduinoWheeledRobot, ptPubState> RightPwmFb;
    UProperty<double, UArduinoWheeledRobot, ptPubState> LeftSense;
    UProperty<double, UArduinoWheeledRobot, ptPubState> RightSense;

    UProperty<string, UArduinoWheeledRobot, ptPubParameter> HostPluginId;
    UProperty<string, UArduinoWheeledRobot, ptPubState> NamedValuesJson;
    UProperty<bool, UArduinoWheeledRobot, ptPubState> PluginBound;

    UArduinoWheeledRobot();
    virtual ~UArduinoWheeledRobot();
    UArduinoWheeledRobot* New() override;

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
    void emitDrive(const UWheeledDriveCommand& cmd);
    QMap<QString, float> NamedValues;
};

} // namespace RDK

#endif
