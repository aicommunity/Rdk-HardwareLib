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
#include "UEPtr.h"
#include "ModernSmartPointers.h"
#include <memory>
#include <mutex>
#include <chrono>
#include <thread>
#include <atomic>


namespace RDK {

class  UArduinoConnect;

class RDK_LIB_TYPE UArduinoControl: public UNet
{
protected:
    std::shared_ptr<UArduinoConnect>UArdConn;
    // UArduinoConnect *UArdConn;

public: //������� � �������� ���������

    //�������� - ������ ������� ������� ������
    ULProperty<double, UArduinoControl, ptPubParameter> LowerSensorLimit;

    //�������� - ������� ������� ������� ������
    ULProperty<double, UArduinoControl, ptPubParameter> UpperSensorLimit;

    //�������� - ��� ����� ��� �����������
    ULProperty<string, UArduinoControl, ptPubParameter> PortToConnect;

    //�������� - �������, ������������ �� �������
    UPropertyInputData<string, UArduinoControl, ptPubParameter | ptInput> Command;

    //�������� - ���������� ������������ �������� �������
    ULProperty<int, UArduinoControl, ptPubParameter> MatrixCols;

    // ��������� - ���� ������������� ��������� �������
    ULProperty<bool, UArduinoControl, ptPubState> SendCommandFlag;

    // ��������� - ���� ������������� ��������� �������
    // ULProperty<bool, UArduinoControl, ptPubState> SendInputCommandFlag;

    // ��������� - ��������� �������, ������������ �� �������
    ULProperty<string, UArduinoControl, ptPubState> SentCommand;

    // �������, ��������� �� UDcControlDemo
    UPropertyInputData<string, UArduinoControl, ptPubState | ptInput> InputCommand;

    // ��������� - ���� ������������� ������� ������ �� �������
    ULProperty<bool, UArduinoControl, ptPubState> GetDataFromBuffers;

    //������� ���������� ��������
    UPropertyOutputData<MDMatrix<double>, UArduinoControl, ptPubState> DoubleMatrixReadings;

    // ��������� - ���� ������������� ������� ������ �����
    ULProperty<bool, UArduinoControl, ptPubState> GetPinsInfo;

    // ��������� - ���� ������������� ���������� ��������� ���������
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
    // �������� ������ ��� ����� ������ ����� ������� ����� ������
    virtual UArduinoControl* New(void);

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

// Modern C++20 thread-safe Arduino control
// Thread-safe Arduino operations
void SendCommandSafe(const std::string& command);
std::string GetResponseSafe(void) const;
void SetSensorLimitsSafe(double lower, double upper);

// Modern move semantics
UArduinoControl(UArduinoControl&& other) noexcept;
UArduinoControl& operator=(UArduinoControl&& other) noexcept;

// Modern smart pointer factory
static std::shared_ptr<UArduinoControl> Create(void);

// Modern hardware communication
bool ConnectToArduinoSafe(const std::string& port);
void DisconnectFromArduinoSafe(void);
bool IsConnectedSafe(void) const;

// --------------------------

protected:
    virtual void AInit(void);
    virtual void AUnInit(void);

};
}

#endif // UArduinoControl_H
