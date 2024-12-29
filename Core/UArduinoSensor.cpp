#ifndef UARDUINOSENSOR_CPP
#define UARDUINOSENSOR_CPP

#include "UArduinoSensor.h"
namespace RDK{

class  UArduinoConnect;

UArduinoSensor::UArduinoSensor(void)
:LowerSensorLimit("LowerSensorLimit", this),
 UpperSensorLimit("UpperSensorLimit", this),
 DoubleMatrixReadings("DoubleMatrixReadings", this),
 PortToConnect("PortToConnect", this, &UArduinoSensor::SetPortToConnect),
 PortChanged("PortChanged", this)
{
}

UArduinoSensor::~UArduinoSensor(void)
{
}

void UArduinoSensor::UpdateReadings(float temperature, float humidity) {

    DoubleMatrixReadings(1,0) = temperature;
    DoubleMatrixReadings(2,0) = humidity;

    qDebug() << "Received temperature on Sensor:" << QString::number(temperature, 'lf', 2);
    qDebug() << "Received humidity on Sensor:" << QString::number(humidity, 'lf', 2);

}

bool UArduinoSensor::SetPortToConnect(const string& value)
{
    UnInit();
    Ready=false;
    PortChanged = true;
    return true;
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
        UArdConn = new  UArduinoConnect(PortName, this);
        // QObject::connect(UArdConn, &UArduinoConnect::newDataReceived, this, &UArduinoSensor::onDataReceived);
    }
}

void UArduinoSensor::AUnInit(void)
{
    if(UArdConn)
    {
        // QObject::disconnect(UArdConn, &UArduinoConnect::newDataReceived, this, &UArduinoSensor::onDataReceived);
        delete UArdConn;      // ����������� ������
        UArdConn = nullptr;   // �������� ���������
    }
}

bool UArduinoSensor::ADefault(void)
{
    return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool UArduinoSensor::ABuild(void)
{
    Init();
    // ResetPortChanged();
    // if (PortChanged == true) {
    //     string PortName = PortToConnect;
    //     AInit(PortName);
    // }
    return true;
}

// ����� �������� �����.
bool UArduinoSensor::AReset(void)
{
    return true;
}

// ��������� ������ ����� �������
bool UArduinoSensor::ACalculate(void)
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

void UArduinoSensor::onDataReceived(float temperature, float humidity)
{
    UpdateReadings(temperature, humidity);
}

}
#endif
