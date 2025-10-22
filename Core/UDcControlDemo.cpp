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

// �������� ������ ��� ����� ������ ����� ������� ����� ������
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

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool UDcControlDemo::ABuild(void)
{

    return true;
}

// ����� �������� �����.
bool UDcControlDemo::AReset(void)
{
    return true;
}

// ��������� ������ ����� �������
bool UDcControlDemo::ACalculate(void)
{
    if(UArdContr == nullptr)
    {
        UArdContr = std::make_shared<UArduinoControl>();
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
