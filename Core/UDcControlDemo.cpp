#ifndef UDCCONTROLDEMO_CPP
#define UDCCONTROLDEMO_CPP

#include "UDcControlDemo.h"

namespace RDK {

UDcControlDemo::UDcControlDemo(void)
 : Command("Command", this),
 SendCommandFlag("SendCommandFlag", this),
 SentCommand("SentCommand", this),
 Speed("Speed", this),
 Acceleration("Acceleartion", this),
 GetSpeed("GetSpeed", this)
{
}

UDcControlDemo::~UDcControlDemo(void)
{
}

// UDcControlDemo::SetSpeed(in speed)
// {

// }

// Выделяет память для новой чистой копии объекта этого класса
UDcControlDemo* UDcControlDemo::New(void)
{
    return new UDcControlDemo;
}

void UDcControlDemo::AInit()
{
}

void UDcControlDemo::AUnInit(void)
{
}

bool UDcControlDemo::ADefault(void)
{
    return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool UDcControlDemo::ABuild(void)
{

    return true;
}

// Сброс процесса счета.
bool UDcControlDemo::AReset(void)
{
    return true;
}

// Выполняет расчет этого объекта
bool UDcControlDemo::ACalculate(void)
{
    if(UArdContr == nullptr)
    {
        UArdContr = new UArduinoControl();
    }

    // GetEnvironment()->GetTime();

    if(SendCommandFlag)
    {
        UArdContr->InputCommand = Command;
        SentCommand = Command;
        SendCommandFlag = false;
    }

    if (GetSpeed) {
        if (!UArdContr->SpeedValues.isEmpty()) {
            Speed = static_cast<float>(UArdContr->SpeedValues.last());
        }
        GetSpeed = false;
    }
    return true;
}

}
#endif // UDCCONTROLDEMO_CPP
