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
 GetDataFromBuffers("GetDataFromBuffers", this)
{
}

UArduinoSensor::~UArduinoSensor(void)
{
}

void UArduinoSensor::UpdateReadings(float temperature, float humidity, double time, float mfield) {

    int cols = DoubleMatrixReadings.GetCols();
    int rows = DoubleMatrixReadings.GetRows();

    qDebug() << "Cols:" << cols;

    if (CurrentCol >= cols) {
        qDebug() << "Maximum number of columns reached. Cannot add more data.";
        DoubleMatrixReadings.Assign(rows, cols, 0.0);
        CurrentCol = 0;
    }

    DoubleMatrixReadings(0, CurrentCol) = time;
    DoubleMatrixReadings(1, CurrentCol) = temperature;
    DoubleMatrixReadings(2, CurrentCol) = humidity;
    DoubleMatrixReadings(3, CurrentCol) = mfield;

    CurrentCol++; // Переход к следующему столбцу

    qDebug() << "Received temperature on Sensor:" << QString::number(temperature, 'lf', 2);
    qDebug() << "Received humidity on Sensor:" << QString::number(humidity, 'lf', 2);
    qDebug() << "Time:" << QString::number(time, 'lf', 2);
    qDebug() << "Magnetic Field" << QString::number(mfield, 'lf', 2);
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
    DoubleMatrixReadings.Assign(4,4,0.0);
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
        UArdConn = new  UArduinoConnect(PortName, std::bind(&UArduinoSensor::DataReceived, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }

   // if(GetDataFromBuffers)
   // {
   //     // UArdConn->PopDataPortion
   //     PutDataToMatrix();
   // }

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

void UArduinoSensor::DataReceived(float temperature, float humidity, double time, float mfield)
{
    if(GetDataFromBuffers)
    {
        PutDataToMatrix();
    }
}

void UArduinoSensor::PutDataToMatrix() {

    QMutexLocker locker(&arduinoMutex);
    if (!UArdConn) return;

    QVector<double> timeData = UArdConn->GetTimeBuffer();
    QVector<double> tempData = UArdConn->GetBuffer1();
    QVector<double> humData = UArdConn->GetBuffer2();
    QVector<double> mfieldData = UArdConn->GetBuffer3();

    const int minSize = qMin(
        qMin(timeData.size(), tempData.size()),
        qMin(humData.size(), mfieldData.size())
        );

    int cols = DoubleMatrixReadings.GetCols();
    int rows = DoubleMatrixReadings.GetRows();

    for (int i = 0; i < minSize && CurrentCol < cols; i++) {
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

    if (CurrentCol >= cols) {
        DoubleMatrixReadings.Assign(rows, cols, 0.0);
        CurrentCol = 0;
        qDebug() << "Matrix reset";
    }

    UArdConn->ClearBuffer1();
    UArdConn->ClearBuffer2();
    UArdConn->ClearBuffer3();
    UArdConn->ClearTimeBuffer();
}

void UArduinoSensor::SendCommand(string command) {
    QMutexLocker locker(&arduinoMutex);
    if (UArdConn) {
        UArdConn->SetCommand(command);
        UArdConn->SendData();
    }
}
}
#endif
