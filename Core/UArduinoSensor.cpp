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

void UArduinoSensor::UpdateReadings(float temperature, float humidity, double time, float mfield) {

    int cols = DoubleMatrixReadings.GetCols();
    int rows = DoubleMatrixReadings.GetRows();

    qDebug() << "Cols:" << cols;

    if (CurrentCol >= cols) {
        qDebug() << "Maximum number of columns reached. Cannot add more data.";
        DoubleMatrixReadings.Assign(rows, cols, 0.0);
        CurrentCol = 0;
    }

    DoubleMatrixReadings(0, CurrentCol) = time;
    DoubleMatrixReadings(1, CurrentCol) = temperature;
    DoubleMatrixReadings(2, CurrentCol) = humidity;
    DoubleMatrixReadings(3, CurrentCol) = mfield;

    CurrentCol++; // ������� � ���������� �������

    qDebug() << "Received temperature on Sensor:" << QString::number(temperature, 'lf', 2);
    qDebug() << "Received humidity on Sensor:" << QString::number(humidity, 'lf', 2);
    qDebug() << "Time:" << QString::number(time, 'lf', 2);
    qDebug() << "Magnetic Field" << QString::number(mfield, 'lf', 2);
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
    DoubleMatrixReadings.Assign(4,4,0.0);
    CurrentCol = 0;
    if (UArdConn == NULL) {
        UArdConn = new  UArduinoConnect(PortName, this);
    }
}

void UArduinoSensor::AUnInit(void)
{
    if(UArdConn)
    {
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

void UArduinoSensor::ResetPortChanged() {
    PortChanged = false;
}

void UArduinoSensor::DataReceived(float temperature, float humidity, double time, float mfield)
{
    UpdateReadings(temperature, humidity, time, mfield);
}

}
#endif
