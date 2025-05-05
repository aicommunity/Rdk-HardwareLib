#ifndef UArduinoControl_CPP
#define UArduinoControl_CPP

#include "UArduinoControl.h"

namespace RDK{

class  UArduinoConnect;

UArduinoControl::UArduinoControl(void)
    :LowerSensorLimit("LowerSensorLimit", this),
    UpperSensorLimit("UpperSensorLimit", this),
    DoubleMatrixReadings("DoubleMatrixReadings", this),
    PortToConnect("PortToConnect", this, &UArduinoControl::SetPortToConnect),
    PortChanged("PortChanged", this),
    Command("Command", this),
    SendCommandFlag("SendCommandFlag", this),
    SentCommand("SentCommand", this),
    GetDataFromBuffers("GetDataFromBuffers", this),
    MatrixCols("MatrixCols", this),
    InputCommand("InputCommand", this),
    GetPinsInfo("GetPinsInfo", this)
// Pins("Pins", this)
{
}

UArduinoControl::~UArduinoControl(void)
{
}

bool UArduinoControl::SetPortToConnect(const string& value)
{
    UnInit();
    Ready=false;
    PortChanged = true;
    return true;
}

// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
UArduinoControl* UArduinoControl::New(void)
{
    return new UArduinoControl;
}

void UArduinoControl::AInit()
{
}

void UArduinoControl::AUnInit(void)
{
    if(UArdConn)
    {
        delete UArdConn;      // Освобождаем память
        UArdConn = nullptr;   // Обнуляем указатель
    }
}

bool UArduinoControl::ADefault(void)
{
    return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool UArduinoControl::ABuild(void)
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
bool UArduinoControl::AReset(void)
{
    SendCommandFlag = false;
    return true;
}

// Выполняет расчет этого объекта
bool UArduinoControl::ACalculate(void)
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

    // if(GetPinsInfo)
    // {
    //     Pins = UArdConn->GetPins();
    // }

    if(InputCommand.IsConnected() && InputCommand.IsNewData() && !InputCommand->empty())
    {
     qDebug() << "InputCommand send" << QString::fromStdString(InputCommand);
     SendCommand(InputCommand);
     SentCommand = InputCommand;
    }

    if(SendCommandFlag)
    {
     qDebug() << "Command send" << QString::fromStdString(Command);
     SendCommand(Command);
     SentCommand = Command;
     SendCommandFlag = false;
    }
    return true;
}

void UArduinoControl::PutDataToMatrix() {
    if (!UArdConn) {
        qDebug() << "Arduino connection not found";
        return;
    }

    QVector<double> allData = UArdConn->GetAndClearAllData();
    qDebug() << "Processing" << allData.size() << "data points";

    int index = 0;
    while (index < allData.size()) {
        if (index + 1 >= allData.size()) {
            qDebug() << "Incomplete data block";
            break;
        }

        // Читаем временную метку и количество параметров
        double timestamp = allData[index++];
        int paramCount = static_cast<int>(allData[index++]);
        int totalRows = paramCount + 1; // Время + параметры

        qDebug() << "Processing block:"
                 << "Time:" << timestamp
                 << "Params:" << paramCount;

        if(paramCount > 0) {
            double speed = allData[index + paramCount - 1];

            if(SpeedValues.size() >= 512) {
                SpeedValues.removeFirst();
            }
            SpeedValues.append(speed);
        }

        // Проверяем целостность данных
        if (index + paramCount > allData.size()) {
            qDebug() << "Data corruption in block! Expected"
                     << paramCount << "parameters, got"
                     << (allData.size() - index);
            break;
        }

        // Ресайз матрицы при необходимости
        if (DoubleMatrixReadings->GetRows() != totalRows) {
            qDebug() << "Resizing matrix to" << totalRows << "rows";
            DoubleMatrixReadings.Assign(totalRows, MatrixCols, 0.0);
        }

        // Заполняем данные
        for (int row = 0; row < totalRows; row++) {
            if (row == 0) {
                DoubleMatrixReadings(row, CurrentCol) = timestamp;
            } else {
                DoubleMatrixReadings(row, CurrentCol) = allData[index++];
            }
            qDebug() << "Matrix[" << row << "," << CurrentCol << "] ="
                     << DoubleMatrixReadings(row, CurrentCol);
        }

        CurrentCol = (CurrentCol + 1) % MatrixCols;
        qDebug() << "Current column index updated to:" << CurrentCol;
    }
}



void UArduinoControl::SendCommand(string command) {
    // QMutexLocker locker(&arduinoMutex);
    if (UArdConn) {
        UArdConn->SetCommand(command);
        UArdConn->SendData();
    }
}
}
#endif
