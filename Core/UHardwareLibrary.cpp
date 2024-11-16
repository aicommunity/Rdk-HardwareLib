#ifndef UHardwareLibrary_CPP
#define UHardwareLibrary_CPP

#include "UHardwareLibrary.h"
#include <QDebug>

namespace RDK {

UHardwareLibrary HardwareLibrary;



// --------------------------
// ������������ � �����������
// --------------------------
UHardwareLibrary::UHardwareLibrary(void)
 : ULibrary("HardwareLibrary","1.0", GetGlobalVersion())
{
}
// --------------------------

// --------------------------
// ������ ����������� �����������
// --------------------------
// ��������� ������ ClassSamples �������� ������������ �������� � �� �������.
// �� ��������� ��������������� ������� ������� � ������ ������.
void UHardwareLibrary::CreateClassSamples(UStorage *storage)
{
 UContainer *cont;
 cont=new UArduinoSensor;
 cont->SetName("Arduino");  // ��� ����� ������ ���������� ��� ������ �������/���������
 cont->Default();
 UploadClass("Arduino",cont);

 cont=new UADC;
 cont->SetName("ADC");
 cont->Default();
 UploadClass("ADC",cont);
}
// --------------------------

}

#endif
