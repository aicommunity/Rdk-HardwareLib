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

// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
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
        delete UArdConn;      // Освобождаем память
        UArdConn = nullptr;   // Обнуляем указатель
    }
}

bool UArduinoSensor::ADefault(void)
{
    return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
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

// Сброс процесса счета.
bool UArduinoSensor::AReset(void)
{
    return true;
}

// Выполняет расчет этого объекта
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
