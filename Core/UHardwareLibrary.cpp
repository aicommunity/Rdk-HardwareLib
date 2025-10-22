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
 std::shared_ptr<UContainer> cont;
 cont=std::make_shared<UArduinoControl>();
 cont->SetName("Arduino");  //       /
 cont->Default();
 UploadClass("Arduino",cont);

 cont=std::make_shared<UADC>();
 cont->SetName("ADC");
 cont->Default();
 UploadClass("ADC",cont);

 cont=std::make_shared<UDcControlDemo>();
 cont->SetName("DC");
 cont->Default();
 UploadClass("DC",cont);
}
// --------------------------

}

#endif
