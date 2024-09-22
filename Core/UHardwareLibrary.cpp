#ifndef UHardwareLibrary_CPP
#define UHardwareLibrary_CPP

#include "UHardwareLibrary.h"


namespace RDK {

UHardwareLibrary HardwareLibrary;



// --------------------------
// Конструкторы и деструкторы
// --------------------------
UHardwareLibrary::UHardwareLibrary(void)
 : ULibrary("HardwareLibrary","1.0", GetGlobalVersion())
{
}
// --------------------------

// --------------------------
// Методы заполенения бибилиотеки
// --------------------------
// Заполняет массив ClassSamples готовыми экземплярами образцов и их именами.
// Не требуется предварительная очистка массива и уборка памяти.
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
