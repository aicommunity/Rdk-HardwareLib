#ifndef UARDUINOSENSOR_H
#define UARDUINOSENSOR_H

// #include <QString>
// #include <QThread>

#include "../../../Rdk/Deploy/Include/rdk.h"
#include "UArduinoChooseFirm.h"


namespace RDK {

class ArduinoUploader;

class RDK_LIB_TYPE UArduinoSensor: public UNet
{
protected:
ArduinoUploader *ardup;
QMutex ardupMutex;

public: //������� � �������� ���������

    // ULProperty<double,UArduinoSensor<double> > LowerSensorLimit;
    // ULProperty<double,UArduinoSensor<double> > UpperSensorLimit;

    /// ������ double - ������ ������� ������ �������
    UPropertyInputData<MDVector<float>, UArduinoSensor> LowerSensorLimit;
    /// ������ double - ������� ������� ������ �������
    UPropertyInputData<MDVector<float>, UArduinoSensor> UpperSensorLimit;

    /// ������ double(��������, ������ ���� ��������)
    UPropertyOutputData<MDVector<float>, UArduinoSensor, ptPubParameter | ptOutput> DoubleVectorReadings;

public:
UArduinoSensor(void);
virtual ~UArduinoSensor(void);

public:
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual UArduinoSensor* New(void);
void updateReadings(float temperature);

protected:
    // bool Initialized;

protected:
/// �������������� �������� �� ��������� � ����� �������� �����
virtual bool ASDefault(void);

/// ������������ ������ ���������� ��������� �������
/// ����� ��������� ����������
/// ������������� �������� ����� Reset() � ���������� Ready � true
/// � ������ �������� ������
virtual bool ASBuild(void);

/// ����� �������� �����.
virtual bool ASReset(void);

/// ��������� ������ ����� �������
virtual bool ASCalculate(void);

protected:
// bool Initialize(void);
virtual void AInit(void);
virtual void AUnInit(void);
};
}

#endif // UARDUINOSENSOR_H
