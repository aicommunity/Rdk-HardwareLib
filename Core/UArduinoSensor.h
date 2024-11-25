#ifndef UARDUINOSENSOR_H
#define UARDUINOSENSOR_H

// #include <QString>
// #include <QThread>

#include "../../../Rdk/Deploy/Include/rdk.h"
#include "UArduinoChooseFirm.h"


namespace RDK {

class ArduinoUploader;

class RDK_LIB_TYPE UArduinoSensor: public UNet
{
protected:
ArduinoUploader *ardup;
QMutex ardupMutex;

public: //Входные и выходные параметры

    // ULProperty<double,UArduinoSensor<double> > LowerSensorLimit;
    // ULProperty<double,UArduinoSensor<double> > UpperSensorLimit;

    /// Вектор double - нижняя граница данных датчика
    UPropertyInputData<MDVector<float>, UArduinoSensor> LowerSensorLimit;
    /// Вектор double - верхняя граница данных датчика
    UPropertyInputData<MDVector<float>, UArduinoSensor> UpperSensorLimit;

    /// Вектор double(возможно, должен быть матрицей)
    UPropertyOutputData<MDVector<float>, UArduinoSensor, ptPubParameter | ptOutput> DoubleVectorReadings;

public:
UArduinoSensor(void);
virtual ~UArduinoSensor(void);

public:
// Выделяет память для новой чистой копии объекта этого класса
virtual UArduinoSensor* New(void);
void updateReadings(float temperature);

protected:
    // bool Initialized;

protected:
/// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool ASDefault(void);

/// Обеспечивает сборку внутренней структуры объекта
/// после настройки параметров
/// Автоматически вызывает метод Reset() и выставляет Ready в true
/// в случае успешной сборки
virtual bool ASBuild(void);

/// Сброс процесса счета.
virtual bool ASReset(void);

/// Выполняет расчет этого объекта
virtual bool ASCalculate(void);

protected:
// bool Initialize(void);
virtual void AInit(void);
virtual void AUnInit(void);
};
}

#endif // UARDUINOSENSOR_H
