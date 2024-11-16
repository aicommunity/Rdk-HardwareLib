#ifndef UARDUINOSENSOR_H
#define UARDUINOSENSOR_H

#include <QObject>
#include <QString>

#include "../../../Rdk/Deploy/Include/rdk.h"
#include "UArduinoChooseFirm.h"

namespace RDK {

class RDK_LIB_TYPE UArduinoSensor: public UNet
{

// protected:
// QVector<QVector<double>> Data;

public:
UArduinoSensor(void);
virtual ~UArduinoSensor(void);

public:
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual UArduinoSensor* New(void);

public: //������� � �������� ���������

// ULProperty<double,UArduinoSensor<double> > LowerSensorLimit;
// ULProperty<double,UArduinoSensor<double> > UpperSensorLimit;

/// ������ double - ������ ������� ������ �������
UPropertyInputData<MDVector<double>, UArduinoSensor> LowerSensorLimit;
/// ������ double - ������� ������� ������ �������
UPropertyInputData<MDVector<double>, UArduinoSensor> UpperSensorLimit;

/// ������ double(��������, ������ ���� ��������)
UPropertyOutputData<MDVector<double>, UArduinoSensor, ptPubParameter | ptOutput> DoubleVectorReadings;

protected:
// void RecieveData(double data);
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
};
}

#endif // UARDUINOSENSOR_H
