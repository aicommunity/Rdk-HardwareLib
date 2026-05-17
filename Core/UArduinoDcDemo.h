#ifndef UARDUINODCDEMO_H
#define UARDUINODCDEMO_H

#include "UArduinoCustomLink.h"

namespace RDK {

class UArduinoSensorSketch;

class RDK_LIB_TYPE UArduinoDcDemo : public UArduinoCustomLink {
public:
    UProperty<string, UArduinoDcDemo, ptPubParameter> LinkedSketchName;
    UProperty<float, UArduinoDcDemo, ptPubState> Speed;
    UProperty<float, UArduinoDcDemo, ptPubState> Acceleration;
    UProperty<bool, UArduinoDcDemo, ptPubParameter | ptInput> GetSpeed;

    UArduinoDcDemo();
    virtual ~UArduinoDcDemo();
    UArduinoDcDemo* New() override;

protected:
    bool ADefault() override;
    bool ABuild() override;
    bool AReset() override;
    bool ACalculate() override;

    void OnBinaryFrame(uint8_t type, const QByteArray& payload) override;
    void ProcessDcDemoEdges();
    bool delegateToLinkedSketch();

    float CachedSpeed = 0.f;
    float CachedAcceleration = 0.f;
};

} // namespace RDK

#endif
