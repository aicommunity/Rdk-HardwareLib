#ifndef UARDUINOSENSOR_CPP
#define UARDUINOSENSOR_CPP

#include "UArduinoSensor.h"
namespace RDK{

class  UArduinoConnect;

UArduinoSensor::UArduinoSensor(void)
:LowerSensorLimit("LowerSensorLimit", this),
 UpperSensorLimit("UpperSensorLimit", this),
 DoubleVectorReadings("DoubleVectorReadings", this),
 PortToConnect("PortToConnect", this),
 PortChanged("PortChanged", this)
{
}

UArduinoSensor::~UArduinoSensor(void)
{
}

void UArduinoSensor::UpdateReadings(float temperature) {
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

void UArduinoSensor::AInit()
{
    string PortName = PortToConnect;
    if (UArdConn == NULL) {
        UArdConn = new  UArduinoConnect(PortName);
    }
}

void UArduinoSensor::AUnInit(void)
{
    delete UArdConn;      // ����������� ������
    UArdConn = nullptr;   // �������� ���������
}

bool UArduinoSensor::ADefault(void)
{
    return ASDefault();
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool UArduinoSensor::ABuild(void)
{
    // ResetPortChanged();
    // if (PortChanged == true) {
    //     string PortName = PortToConnect;
    //     AInit(PortName);
    // }
    return ASBuild();
}

// ����� �������� �����.
bool UArduinoSensor::AReset(void)
{
    return ASReset();
}

// ��������� ������ ����� �������
bool UArduinoSensor::ACalculate(void)
{
    return ASCalculate();
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

void UArduinoSensor::ResetPortChanged() {
    PortChanged = false;
}

}
#endif
