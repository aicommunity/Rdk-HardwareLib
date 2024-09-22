#ifndef UHardwareLibrary_CPP
#define UHardwareLibrary_CPP

#include "UHardwareLibrary.h"


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
/*
 UContainer *cont;
 cont=new UFileIO;
 cont->SetName("FileIO");
 cont->Default();
 UploadClass("UFileIO",cont);

 cont=new UIOTextConverter;
 cont->SetName("IOTextConverter");
 cont->Default();
 UploadClass("UIOTextConverter",cont);
 */
}
// --------------------------

}

#endif
