#ifndef UARDUINOSENSOR_H
#define UARDUINOSENSOR_H

// #include <QString>
// #include <QThread>
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
    ULProperty<bool, UArduinoSensor, ptPubParameter> PortChanged;
    /// ������ double(��������, ������ ���� ��������)
    UPropertyOutputData<MDVector<double>, UArduinoSensor, ptPubParameter | ptOutput> DoubleVectorReadings;

public:
UArduinoSensor(void);
virtual ~UArduinoSensor(void);
void UpdateReadings(float temperature);

protected:
void ResetPortChanged();

public:
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual UArduinoSensor* New(void);

    // --------------------------
    // ������� ������ ���������� ������
    // --------------------------
protected:
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
/// �������������� �������� �� ��������� � ����� �������� �����
virtual bool ASDefault(void);

/// ������������ ������ ���������� ��������� �������
/// ����� ��������� ����������
/// ������������� �������� ����� Reset() � ���������� Ready � true
/// � ������ �������� ������
virtual bool ASBuild(void);

/// ����� �������� �����.
virtual bool ASReset(void);

/// ��������� ������ ����� �������
virtual bool ASCalculate(void);

protected:
virtual void AInit(void);
virtual void AUnInit(void);
};
}

#endif // UARDUINOSENSOR_H
