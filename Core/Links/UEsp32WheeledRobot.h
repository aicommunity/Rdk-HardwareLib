#ifndef UESP32WHEELEDROBOT_H
#define UESP32WHEELEDROBOT_H

#include "UEsp32CustomLink.h"
#include "Protocol/IArduinoProtocolPlugin.h"
#include "Wheeled/UWheeledDriveLogic.h"

#include <QMap>
#include <memory>

namespace RDK {

/**
 * ESP32 open-loop wheeled robot (nmsdk_motor_hub framed protocol).
 * Single inheritance from UEsp32CustomLink; plugin host via composition (plan §0.4).
 */
class RDK_LIB_TYPE UEsp32WheeledRobot : public UEsp32CustomLink {
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

protected:
    bool ADefault() override;
    bool ACalculate() override;
    void OnBinaryFrame(uint8_t type, const QByteArray& payload) override;
    void OnHealthCheck() override;
    void NegotiateProtocol() override;
    void ProcessWheeledEdges();

private:
    class HostAdapter;
    friend class HostAdapter;
    IArduinoProtocolPlugin* resolvePlugin() const;
    UArduinoPluginHost* pluginHost();
    void onNamedFloat(const QString& key, float value);
    void hostEnqueue(const QString& line);
    void hostSetProtocolReady(bool ready);
    void hostSetLastError(const QString& error);
    int hostProtocolVersion() const;

    std::unique_ptr<HostAdapter> Host;
    QMap<QString, float> NamedValues;
};

} // namespace RDK

#endif
