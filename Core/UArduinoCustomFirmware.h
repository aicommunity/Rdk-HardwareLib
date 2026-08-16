#ifndef UARDUINOCUSTOMFIRMWARE_H
#define UARDUINOCUSTOMFIRMWARE_H

#include "UArduinoCustomLink.h"
#include "Protocol/IArduinoProtocolPlugin.h"

#include <QMap>
#include <QVector>

namespace RDK {

class RDK_LIB_TYPE UArduinoCustomFirmware : public UArduinoCustomLink, public UArduinoPluginHost {
public:
    UProperty<string, UArduinoCustomFirmware, ptPubParameter> HostPluginId;
    UProperty<string, UArduinoCustomFirmware, ptPubState> FrameLog;
    UProperty<string, UArduinoCustomFirmware, ptPubState> NamedValuesJson;
    UProperty<MDMatrix<double>, UArduinoCustomFirmware, ptPubOutput | ptPubState> DoubleMatrixReadings;
    UProperty<bool, UArduinoCustomFirmware, ptPubParameter | ptInput> ClearFrameLog;
    UProperty<bool, UArduinoCustomFirmware, ptPubState> PluginBound;

    UArduinoCustomFirmware();
    virtual ~UArduinoCustomFirmware();
    UArduinoCustomFirmware* New() override;

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

private:
    IArduinoProtocolPlugin* resolvePlugin() const;
    QMap<QString, float> NamedValues;
};

} // namespace RDK

#endif
