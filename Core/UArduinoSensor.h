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

public: //������� � �������� ���������
    //�������� - ������ ������� ������� ������
    ULProperty<double, UArduinoSensor, ptPubParameter> LowerSensorLimit;
    //�������� - ������� ������� ������� ������
    ULProperty<double, UArduinoSensor, ptPubParameter> UpperSensorLimit;
    //�������� - ��� ����� ��� �����������
    ULProperty<string, UArduinoSensor, ptPubParameter> PortToConnect;
    //�������� - ��� ����� ��� �����������
    ULProperty<bool, UArduinoSensor, ptPubState> PortChanged;
    /// ������ double(��������, ������ ���� ��������)
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
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual UArduinoSensor* New(void);

    // --------------------------
    // ������� ������ ���������� ������
    // --------------------------
protected:
bool SetPortToConnect(const string& value);


    /// �������������� �������� �� ��������� � ����� �������� �����
    virtual bool ADefault(void);

    /// ������������ ������ ���������� ��������� �������
    /// ����� ��������� ����������
    /// ������������� �������� ����� Reset() � ���������� Ready � true
    /// � ������ �������� ������
    virtual bool ABuild(void);

    /// ����� �������� �����.
    virtual bool AReset(void);

    /// ��������� ������ ����� �������
    virtual bool ACalculate(void);
    // --------------------------

protected:
virtual void AInit(void);
virtual void AUnInit(void);

};
}

#endif // UARDUINOSENSOR_H
