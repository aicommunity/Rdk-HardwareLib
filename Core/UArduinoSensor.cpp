#ifndef UARDUINOSENSOR_CPP
#define UARDUINOSENSOR_CPP

#include "UArduinoSensor.h"
namespace RDK{

UArduinoSensor::UArduinoSensor(void)
:LowerSensorLimit("LowerSensorLimit", this),
 UpperSensorLimit("UpperSensorLimit", this),
 DoubleVectorReadings("DoubleVectorReadings", this)
{
}

UArduinoSensor::~UArduinoSensor(void)
{
}

// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
UArduinoSensor* UArduinoSensor::New(void)
{
    return new UArduinoSensor;
}

// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool UArduinoSensor::ASDefault(void)
{
    return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool UArduinoSensor::ASBuild(void)
{
    return true;
}

// ����� �������� �����.
bool UArduinoSensor::ASReset(void)
{
    return true;
}

// ��������� ������ ����� �������
bool UArduinoSensor::ASCalculate(void)
{
    return true;
}

// UArduinoSensor::RecieveData(double data)
// {
//     Data.push_back(data);
// }

}
#endif
