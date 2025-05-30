#ifndef UArduinoControl_H
#define UArduinoControl_H

#include <QObject>
#include <QVector>
#include <string>
#include <QTimer>
#include <cmath>
#include <QtDebug>

#include "../../../Rdk/Deploy/Include/rdk.h"
#include "UArduinoConnect.h"


namespace RDK {

class  UArduinoConnect;

class RDK_LIB_TYPE UArduinoControl: public UNet
{
protected:
    UEPtr<UArduinoConnect>UArdConn;
    // UArduinoConnect *UArdConn;

public: //Входные и выходные параметры

    //Параметр - нижняя граница входных данных
    ULProperty<double, UArduinoControl, ptPubParameter> LowerSensorLimit;

    //Параметр - верхняя граница входных данных
    ULProperty<double, UArduinoControl, ptPubParameter> UpperSensorLimit;

    //Параметр - имя порта для подключения
    ULProperty<string, UArduinoControl, ptPubParameter> PortToConnect;

    //Параметр - команда, отправляемая на ардуино
    UPropertyInputData<string, UArduinoControl, ptPubParameter | ptInput> Command;

    //Параметр - количество отображаемых столбцов матрицы
    ULProperty<int, UArduinoControl, ptPubParameter> MatrixCols;

    // Состояние - флаг необходимости отправить команду
    ULProperty<bool, UArduinoControl, ptPubState> SendCommandFlag;

    // Состояние - флаг необходимости отправить команду
    // ULProperty<bool, UArduinoControl, ptPubState> SendInputCommandFlag;

    // Состояние - последняя команда, отправленная на ардуино
    ULProperty<string, UArduinoControl, ptPubState> SentCommand;

    // Команда, пришедшая от UDcControlDemo
    UPropertyInputData<string, UArduinoControl, ptPubState | ptInput> InputCommand;

    // Состояние - флаг необходимости забрать данные из буферов
    ULProperty<bool, UArduinoControl, ptPubState> GetDataFromBuffers;

    //Матрица полученных значений
    UPropertyOutputData<MDMatrix<double>, UArduinoControl, ptPubState> DoubleMatrixReadings;

    // Состояние - флаг необходимости забрать список пинов
    ULProperty<bool, UArduinoControl, ptPubState> GetPinsInfo;

    // Состояние - флаг необходимости отображать дебаговые сообщения
    ULProperty<bool, UArduinoControl, ptPubState> ShowDebug;

    int CurrentRow;
    bool portchanged;

    QString LastCom;

    QVector<double> SpeedValues;

    // QMutex arduinoMutex;

public:
    UArduinoControl(void);
    virtual ~UArduinoControl(void);
    void SendCommand(string command);
    void PutDataToMatrix();

protected:
    void ResetPortChanged();

public:
    // Выделяет память для новой чистой копии объекта этого класса
    virtual UArduinoControl* New(void);

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

#endif // UArduinoControl_H
