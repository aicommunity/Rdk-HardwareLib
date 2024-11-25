#ifndef UARDUINOSENSOR_CPP
#define UARDUINOSENSOR_CPP

#include "UArduinoSensor.h"
namespace RDK{

class ArduinoUploader;

UArduinoSensor::UArduinoSensor(void)
:LowerSensorLimit("LowerSensorLimit", this),
 UpperSensorLimit("UpperSensorLimit", this),
 DoubleVectorReadings("DoubleVectorReadings", this)
{
}

UArduinoSensor::~UArduinoSensor(void)
{
}

void UArduinoSensor::updateReadings(float temperature) {
    // ��������� �������� � DoubleVectorReadings
    // DoubleVectorReadings.push_back(temperature);

}

// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
UArduinoSensor* UArduinoSensor::New(void)
{
    return new UArduinoSensor;
}

void UArduinoSensor::AInit(void)
{
    // ardupMutex.lock();
    ardup = new ArduinoUploader();
    // ardupMutex.unlock();
    // if(!Initialized)
    // {
    //     if(!Initialize())
    //         return;
    // }
}

void UArduinoSensor::AUnInit(void)
{
    // ardupMutex.lock(); // ��������� ������� ����� �������� � ardup
    delete ardup;      // ����������� ������
    ardup = nullptr;   // �������� ���������
    // ardupMutex.unlock(); // ������������ �������
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
