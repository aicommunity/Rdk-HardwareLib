#ifndef UHardwareLibrary_H
#define UHardwareLibrary_H

#include "../../../Rdk/Deploy/Include/rdk.h"

#include "UArduinoChooseFirm.h"
#include "UArduinoSensor.h"
#include "UADC.h"

namespace RDK {

class RDK_LIB_TYPE UHardwareLibrary: public ULibrary
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UHardwareLibrary(void);
// --------------------------

// --------------------------
// Методы заполенения бибилиотеки
// --------------------------
// Заполняет массив ClassSamples готовыми экземплярами образцов и их именами.
// Не требуется предварительная очистка массива и уборка памяти.
virtual void CreateClassSamples(UStorage *storage);

// --------------------------
};

extern RDK_LIB_TYPE UHardwareLibrary HardwareLibrary;

}
#endif

