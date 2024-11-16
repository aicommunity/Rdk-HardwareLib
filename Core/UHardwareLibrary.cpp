#ifndef UHardwareLibrary_CPP
#define UHardwareLibrary_CPP

#include "UHardwareLibrary.h"
#include <QDebug>

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
 UContainer *cont;
 cont=new UArduinoSensor;
 cont->SetName("Arduino");  // Тут будут разные контейнеры под разные датчики/эффекторы
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
