#ifndef UArduinoControl_CPP
#define UArduinoControl_CPP

#include "UArduinoControl.h"

namespace RDK{

class  UArduinoConnect;

UArduinoControl::UArduinoControl(void)
    :LowerSensorLimit("LowerSensorLimit", this),
    UpperSensorLimit("UpperSensorLimit", this),
    PortToConnect("PortToConnect", this, &UArduinoControl::SetPortToConnect),
    Command("Command", this),
    SendCommandFlag("SendCommandFlag", this),
    SentCommand("SentCommand", this),
    MatrixCols("MatrixCols", this),
    InputCommand("InputCommand", this),
    GetDataFromBuffers("GetDataFromBuffers", this),
    DoubleMatrixReadings("DoubleMatrixReadings", this),
    GetPinsInfo("GetPinsInfo", this),
    ShowDebug("ShowDebug", this),
    portchanged(false)
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

    CurrentRow = 0;
    if (UArdConn == nullptr)
    {
        UArdConn = new  UArduinoConnect(PortName);
    }

    UArdConn->deb = ShowDebug;

    if(GetDataFromBuffers)
    {
        PutDataToMatrix();
    }

    if(InputCommand.IsConnected() && InputCommand.IsNewData() && !InputCommand->empty())
    {
        if (ShowDebug)
            qDebug() << "InputCommand send" << QString::fromStdString(InputCommand);
        SendCommand(InputCommand);
        SentCommand = InputCommand;
    }

    if(SendCommandFlag)
    {
        if (ShowDebug)
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
    if (ShowDebug)
        qDebug() << "Processing" << allData.size() << "data points (MatrixCols:" << MatrixCols << ")";

    int index = 0;
    const int MAX_PARAMS = 10;

    while (index < allData.size()) {
        if (index + 2 > allData.size()) {
            if (ShowDebug)
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

        int totalCols = paramCount + 1;
        if (DoubleMatrixReadings->GetCols() != totalCols) {
            DoubleMatrixReadings.Assign(MatrixCols, totalCols, 0.0);
            if (ShowDebug)
                qDebug() << "Resized matrix to" << totalCols << "columns";
        }

        if (CurrentRow >= MatrixCols) {
            CurrentRow = 0;
        }

        if (ShowDebug)
            qDebug() << "Writing to row" << CurrentRow;
        for (int col = 0; col < totalCols; col++) {
            double value = (col == 0) ? timestamp : allData[index++];

            if (CurrentRow < MatrixCols && col < DoubleMatrixReadings->GetCols()) {
                DoubleMatrixReadings(CurrentRow, col) = value;
                if (ShowDebug)
                    qDebug() << "  [" << CurrentRow << "," << col << "] =" << value;
            }
        }

        CurrentRow++;

        if (CurrentRow >= MatrixCols) {
            CurrentRow = 0;
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
