#ifndef UARDUINOSENSORSKETCH_H
#define UARDUINOSENSORSKETCH_H

#include "UArduinoCustomLink.h"
#include "Protocol/IArduinoProtocolPlugin.h"

#include <QVector>

namespace RDK {

struct USensorDataPoint {
    QVector<double> Data;
};

class RDK_LIB_TYPE UArduinoSensorSketch : public UArduinoCustomLink, public UArduinoPluginHost {
public:
    UProperty<double, UArduinoSensorSketch, ptPubParameter> LowerSensorLimit;
    UProperty<double, UArduinoSensorSketch, ptPubParameter> UpperSensorLimit;
    UProperty<int, UArduinoSensorSketch, ptPubParameter> MatrixCols;
    UProperty<bool, UArduinoSensorSketch, ptPubParameter | ptInput> GetDataFromBuffers;
    UProperty<MDMatrix<double>, UArduinoSensorSketch, ptPubState> DoubleMatrixReadings;
    UProperty<bool, UArduinoSensorSketch, ptPubParameter | ptInput> GetPinsInfo;
    UProperty<string, UArduinoSensorSketch, ptPubState> PinStatusJson;

    UProperty<bool, UArduinoSensorSketch, ptPubParameter | ptInput> StartReading;
    UProperty<bool, UArduinoSensorSketch, ptPubParameter | ptInput> StopReading;
    UProperty<bool, UArduinoSensorSketch, ptPubParameter | ptInput> Rotate;
    UProperty<bool, UArduinoSensorSketch, ptPubParameter | ptInput> StopRotate;

    UArduinoSensorSketch();
    virtual ~UArduinoSensorSketch();

    UArduinoSensorSketch* New() override;

    void enqueueCommand(const QString& line) override;
    int boardProfile() const override;
    int protocolVersion() const override;
    void setProtocolReady(bool ready) override;
    void setLastError(const QString& error) override;
    void publishSensorMatrixRow(const QVector<double>& row) override;
    void publishPinStatusJson(const QString& json) override;
    void appendFrameLog(const QString& line) override;

protected:
    bool ADefault() override;
    bool ABuild() override;
    bool AReset() override;
    bool ACalculate() override;

    void OnBinaryFrame(uint8_t type, const QByteArray& payload) override;
    void OnBoardCalculate() override;
    void NegotiateProtocol() override;

    void ProcessSketchEdges();
    void PutDataToMatrix();
    void FillSensorBuffer(double timestamp,
                          uint8_t param_count,
                          float temperature,
                          float humidity,
                          float hall_value,
                          float speed_value);
    void RunPresetCommand(const char* text);
    QString pinToString(int pin);
    IArduinoProtocolPlugin* resolvePlugin() const;

    QVector<USensorDataPoint> DataBuffer;
    int CurrentRow = 0;
};

} // namespace RDK

#endif
