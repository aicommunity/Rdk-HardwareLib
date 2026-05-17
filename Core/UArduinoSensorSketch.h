#ifndef UARDUINOSENSORSKETCH_H
#define UARDUINOSENSORSKETCH_H

#include "UArduinoCustomLink.h"

#include <QVector>

namespace RDK {

struct USensorDataPoint {
    QVector<double> data;
};

class RDK_LIB_TYPE UArduinoSensorSketch : public UArduinoCustomLink {
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

protected:
    bool ADefault() override;
    bool ABuild() override;
    bool AReset() override;
    bool ACalculate() override;

    void OnBinaryFrame(uint8_t type, const QByteArray& payload) override;
    void OnBoardCalculate() override;

    void ProcessSketchEdges();
    void PutDataToMatrix();
    void FillSensorBuffer(double timestamp, uint8_t paramCount,
                          float t, float h, float hall, float speed);
    void runPresetCommand(const char* text);
    static QString pinToString(int pin);

    QVector<USensorDataPoint> DataBuffer;
    int CurrentRow = 0;
};

} // namespace RDK

#endif
