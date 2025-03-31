#ifndef UARDUINOSENSOR_CPP
#define UARDUINOSENSOR_CPP

#include "UArduinoSensor.h"

namespace RDK{

class  UArduinoConnect;

UArduinoSensor::UArduinoSensor(void)
:LowerSensorLimit("LowerSensorLimit", this),
 UpperSensorLimit("UpperSensorLimit", this),
 DoubleMatrixReadings("DoubleMatrixReadings", this),
 PortToConnect("PortToConnect", this, &UArduinoSensor::SetPortToConnect),
 PortChanged("PortChanged", this),
 Command("Command", this),
 SendCommandFlag("SendCommandFlag", this),
 SentCommand("SentCommand", this),
 GetDataFromBuffers("GetDataFromBuffers", this),
 MatrixCols("MatrixCols", this)
{
}

UArduinoSensor::~UArduinoSensor(void)
{
}

bool UArduinoSensor::SetPortToConnect(const string& value)
{
    UnInit();
    Ready=false;
    PortChanged = true;
    return true;
}

// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
UArduinoSensor* UArduinoSensor::New(void)
{
    return new UArduinoSensor;
}

void UArduinoSensor::AInit()
{
}

void UArduinoSensor::AUnInit(void)
{
    if(UArdConn)
    {
        delete UArdConn;      // Освобождаем память
        UArdConn = nullptr;   // Обнуляем указатель
    }
}

bool UArduinoSensor::ADefault(void)
{
    return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool UArduinoSensor::ABuild(void)
{
//    Init();
    // ResetPortChanged();
    // if (PortChanged == true) {
    //     string PortName = PortToConnect;
    //     AInit(PortName);
    // }
    DoubleMatrixReadings.Assign(4,MatrixCols,0.0);
    return true;
}

// Сброс процесса счета.
bool UArduinoSensor::AReset(void)
{
    SendCommandFlag = false;
    return true;
}

// Выполняет расчет этого объекта
bool UArduinoSensor::ACalculate(void)
{
    string PortName = PortToConnect;

    CurrentCol = 0;
    if (UArdConn == nullptr)
    {
        UArdConn = new  UArduinoConnect(PortName);
    }

    if(GetDataFromBuffers)
    {
         PutDataToMatrix();
    }

    if(SendCommandFlag)
    {
        SendCommand(Command);
        SentCommand = Command;
        SendCommandFlag = false;
    }
    return true;
}

void UArduinoSensor::ResetPortChanged() {
    PortChanged = false;
}

void UArduinoSensor::PutDataToMatrix() {
    if (!UArdConn) {
        qDebug() << "Arduino connection not found";
        return;
    }

    QVector<double> timeData = UArdConn->GetAndClearTimeBuffer();
    QVector<double> tempData = UArdConn->GetAndClearBuffer1();
    QVector<double> humData = UArdConn->GetAndClearBuffer2();
    QVector<double> mfieldData = UArdConn->GetAndClearBuffer3();

    int dataCount = qMin(
        qMin(timeData.size(), tempData.size()),
        qMin(humData.size(), mfieldData.size())
        );

    if (dataCount == 0) {
        qDebug() << "No data available in buffers";
        return;
    }

    int cols = MatrixCols; //или dataCount
    // int rows = DoubleMatrixReadings.GetRows();

    for (int i = 0; i < dataCount; i++) {
        if (CurrentCol >= cols) {
            CurrentCol = 0;
        }

        DoubleMatrixReadings(0, CurrentCol) = timeData[i];
        DoubleMatrixReadings(1, CurrentCol) = tempData[i];
        DoubleMatrixReadings(2, CurrentCol) = humData[i];
        DoubleMatrixReadings(3, CurrentCol) = mfieldData[i];

        qDebug() << "Recorded data at col" << CurrentCol
                 << "Time:" << DoubleMatrixReadings(0, CurrentCol)
                 << "Temp:" << DoubleMatrixReadings(1, CurrentCol)
                 << "Hum:" << DoubleMatrixReadings(2, CurrentCol)
                 << "MField:" << DoubleMatrixReadings(3, CurrentCol);

        CurrentCol++;
    }
}

void UArduinoSensor::SendCommand(string command) {
    // QMutexLocker locker(&arduinoMutex);
    if (UArdConn) {
        UArdConn->SetCommand(command);
        UArdConn->SendData();
    }
}
}
#endif
