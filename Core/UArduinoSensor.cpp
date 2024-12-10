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
    // Добавляем значения в DoubleVectorReadings
    // DoubleVectorReadings.push_back(temperature);

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
        UArdConn = new  UArduinoConnect(PortName);
    }
}

void UArduinoSensor::AUnInit(void)
{
    delete UArdConn;      // Освобождаем память
    UArdConn = nullptr;   // Обнуляем указатель
}

bool UArduinoSensor::ADefault(void)
{
    return ASDefault();
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool UArduinoSensor::ABuild(void)
{
    // ResetPortChanged();
    // if (PortChanged == true) {
    //     string PortName = PortToConnect;
    //     AInit(PortName);
    // }
    return ASBuild();
}

// Сброс процесса счета.
bool UArduinoSensor::AReset(void)
{
    return ASReset();
}

// Выполняет расчет этого объекта
bool UArduinoSensor::ACalculate(void)
{
    return ASCalculate();
}

// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool UArduinoSensor::ASDefault(void)
{
    return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool UArduinoSensor::ASBuild(void)
{
    return true;
}

// Сброс процесса счета.
bool UArduinoSensor::ASReset(void)
{
    return true;
}

// Выполняет расчет этого объекта
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
