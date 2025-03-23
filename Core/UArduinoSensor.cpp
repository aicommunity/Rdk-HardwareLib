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
    DoubleMatrixReadings.Assign(4,4,0.0);
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
    // UpdateReadings(temperature, humidity, time, mfield);
    // if(GetDataFromBuffers)
    // {
        // UArdConn->PopDataPortion
        PutDataToMatrix();
    // }
}

void UArduinoSensor::PutDataToMatrix() {
    int cols = DoubleMatrixReadings.GetCols();
    int rows = DoubleMatrixReadings.GetRows();

    // Определяем минимальный размер буферов
    int minSize = qMin(UArdConn->TimeBuffer.size(),
                       qMin(UArdConn->DataBuffer.size(),
                            qMin(UArdConn->DataBuffer2.size(),
                                 UArdConn->DataBuffer3.size())));

    // Записываем столько данных, сколько есть во всех буферах
    for (int i = 0; i < minSize && CurrentCol < cols; i++) {
        DoubleMatrixReadings(0, CurrentCol) = UArdConn->TimeBuffer.takeFirst();
        DoubleMatrixReadings(1, CurrentCol) = UArdConn->DataBuffer.takeFirst();
        DoubleMatrixReadings(2, CurrentCol) = UArdConn->DataBuffer2.takeFirst();
        DoubleMatrixReadings(3, CurrentCol) = UArdConn->DataBuffer3.takeFirst();

        qDebug() << "Recorded data at col" << CurrentCol
                 << "Time:" << DoubleMatrixReadings(0, CurrentCol)
                 << "Temp:" << DoubleMatrixReadings(1, CurrentCol)
                 << "Hum:" << DoubleMatrixReadings(2, CurrentCol)
                 << "MField:" << DoubleMatrixReadings(3, CurrentCol);

        CurrentCol++;
    }

    // Если матрица заполнена - сбросить
    if (CurrentCol >= cols) {
        DoubleMatrixReadings.Assign(rows, cols, 0.0);
        CurrentCol = 0;
        qDebug() << "Matrix reset";
    }
}

void UArduinoSensor::SendCommand(string command){
    if(UArdConn){   
        UArdConn->com = command;
        UArdConn->CheckWrite();
        UArdConn->SendData();
    }
}
}
#endif
