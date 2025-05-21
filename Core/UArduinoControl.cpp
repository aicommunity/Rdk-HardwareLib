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
    Command("Command", this),
    SendCommandFlag("SendCommandFlag", this),
    SentCommand("SentCommand", this),
    GetDataFromBuffers("GetDataFromBuffers", this),
    MatrixCols("MatrixCols", this),
    InputCommand("InputCommand", this),
    GetPinsInfo("GetPinsInfo", this),
    portchanged(false)
// Pins("Pins", this)
{
}

UArduinoControl::~UArduinoControl(void)
{
}

bool UArduinoControl::SetPortToConnect(const string& value)
{
    UnInit();
    Ready = false;
    portchanged = true; // Устанавливаем флаг при изменении порта
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
    portchanged = false;
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
    qDebug() << "Processing" << allData.size() << "data points (MatrixCols:" << MatrixCols << ")";

    int index = 0;
    const int MAX_PARAMS = 10;

    while (index < allData.size()) {
        if (index + 2 > allData.size()) {
            qDebug() << "Truncated block header at index" << index;
            break;
        }

        double timestamp = allData[index++];
        int paramCount = static_cast<int>(allData[index++]);

        if (paramCount >= 1000) {
            qDebug() << "Skipping error block, code:" << paramCount;
            continue;
        }

        if (paramCount <= 0 || paramCount > MAX_PARAMS) {
            qDebug() << "Invalid param count:" << paramCount << "at index" << index-2;
            index += paramCount;
            continue;
        }

        if (index + paramCount > allData.size()) {
            qDebug() << "Corrupted data block at index" << index;
            break;
        }

        int totalRows = paramCount + 1;
        if (DoubleMatrixReadings->GetRows() != totalRows) {
            DoubleMatrixReadings.Assign(totalRows, MatrixCols, 0.0);
            qDebug() << "Resized matrix to" << totalRows << "rows";
        }

        if (CurrentCol >= MatrixCols) {
            DoubleMatrixReadings.Assign(totalRows, MatrixCols, 0.0);
            CurrentCol = 0;
            qDebug() << "---- Matrix cleared ----";
        }

        qDebug() << "Writing to column" << CurrentCol;
        for (int row = 0; row < totalRows; row++) {
            double value = (row == 0) ? timestamp : allData[index++];

            if (row < DoubleMatrixReadings->GetRows() && CurrentCol < MatrixCols) {
                DoubleMatrixReadings(row, CurrentCol) = value;
                qDebug() << "  [" << row << "," << CurrentCol << "] =" << value;
            }
        }

        CurrentCol++;

        if (CurrentCol >= MatrixCols) {
            DoubleMatrixReadings.Assign(totalRows, MatrixCols, 0.0);
            CurrentCol = 0;
            qDebug() << "---- Matrix reset ----";
        }
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
