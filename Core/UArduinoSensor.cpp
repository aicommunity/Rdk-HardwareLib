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
    // ardupMutex.lock(); // Блокируем мьютекс перед доступом к ardup
    delete ardup;      // Освобождаем память
    ardup = nullptr;   // Обнуляем указатель
    // ardupMutex.unlock(); // Разблокируем мьютекс
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
