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

// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
UArduinoSensor* UArduinoSensor::New(void)
{
    return new UArduinoSensor;
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

}
#endif
