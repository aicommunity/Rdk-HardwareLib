#ifndef UDCCONTROLDEMO_H
#define UDCCONTROLDEMO_H

#include "../../../Rdk/Deploy/Include/rdk.h"
#include "UArduinoControl.h"


namespace RDK {

class RDK_LIB_TYPE UDcControlDemo: public UNet
{

protected:
    std::shared_ptr<UArduinoControl>UArdContr;

public:

    //Параметр - команда, отправляемая на ардуино для управления скоростью и разгоном сервопривода
    UPropertyOutputData<string, UDcControlDemo, ptPubParameter| ptOutput> Command;

    // Состояние - флаг необходимости отправить команду
    ULProperty<bool, UDcControlDemo, ptPubState> SendCommandFlag;

    // Состояние - последняя команда, отправленная на ардуино
    ULProperty<string, UDcControlDemo, ptPubState> SentCommand;

    // Состояние - скорость
    ULProperty<float, UDcControlDemo, ptPubState> Speed;

    // Состояние - ускорение
    ULProperty<float, UDcControlDemo, ptPubState> Acceleration;

    // Состояние - флаг необходимости узнать скорость
    ULProperty<bool, UDcControlDemo, ptPubState> GetSpeed;

public:
 UDcControlDemo(void);
 virtual ~UDcControlDemo(void);
 virtual UDcControlDemo* New(void);
 // void SetSpeed(int speed);

  protected:
 /// Восстановление настроек по умолчанию и сброс процесса счета
 virtual bool ADefault(void);

 /// Обеспечивает сборку внутренней структуры объекта
 /// после настройки параметров
 /// Автоматически вызывает метод Reset() и выставляет Ready в true
 /// в случае успешной сборки
 virtual bool ABuild(void);

 /// Сброс процесса счета.
 virtual bool AReset(void);

 /// Выполняет расчет этого объекта
 virtual bool ACalculate(void);
 // --------------------------

 virtual void AInit(void);
 virtual void AUnInit(void);
};
}

#endif // UDCCONTROLDEMO_H
