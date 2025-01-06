#ifndef UARDUINOSENSOR_H
#define UARDUINOSENSOR_H

// #include <QString>
// #include <QThread>
#include <QObject>
#include <QVector>
#include <string>

#include "../../../Rdk/Deploy/Include/rdk.h"
#include "UArduinoConnect.h"


namespace RDK {

class  UArduinoConnect;

class RDK_LIB_TYPE UArduinoSensor: public UNet
{
protected:
    UEPtr<UArduinoConnect>UArdConn;
 // UArduinoConnect *UArdConn;

public: //Входные и выходные параметры
    //Параметр - нижняя граница входных данных
    ULProperty<double, UArduinoSensor, ptPubParameter> LowerSensorLimit;
    //Параметр - верхняя граница входных данных
    ULProperty<double, UArduinoSensor, ptPubParameter> UpperSensorLimit;
    //Параметр - имя порта для подключения
    ULProperty<string, UArduinoSensor, ptPubParameter> PortToConnect;
    //Параметр - имя порта для подключения
    ULProperty<bool, UArduinoSensor, ptPubState> PortChanged;
    /// Вектор double(возможно, должен быть матрицей)
    UPropertyOutputData<MDMatrix<double>, UArduinoSensor, ptPubParameter | ptOutput> DoubleMatrixReadings;
    int CurrentCol;

public:
UArduinoSensor(void);
virtual ~UArduinoSensor(void);
void UpdateReadings(float temperature, float humidity, double time, float mfield);
void DataReceived(float temperature, float humidity, double time, float mfield);

protected:
void ResetPortChanged();

public:
// Выделяет память для новой чистой копии объекта этого класса
virtual UArduinoSensor* New(void);

    // --------------------------
    // Скрытые методы управления счетом
    // --------------------------
protected:
bool SetPortToConnect(const string& value);


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

protected:
virtual void AInit(void);
virtual void AUnInit(void);

};
}

#endif // UARDUINOSENSOR_H
