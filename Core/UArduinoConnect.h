#ifndef UARDUINOCONNECT_H
#define UARDUINOCONNECT_H

#include <QObject>
#include <QThread>
#include <QtSerialPort/qserialport>
#include <QtSerialPort/qserialportinfo>
#include <QByteArray>
#include <QProcess>
#include <QTimer>
#include <QString>
#include <QDebug>
#include <QMutex>
#include <QDateTime>
#include <QTimeZone>
#include <QQueue>
#include <thread>
#include <mutex>
#include <string>

#include "../../../Rdk/Deploy/Include/rdk.h"
// #include "UArduinoControl.h"

namespace RDK{

// class UArduinoControl;

struct DataPoint {
    QVector<double> data; // Формат: [timestamp, temperature, humidity, mfield]
};

class UArduinoConnect: public QThread
{
    Q_OBJECT

public:
    void InitSerialPort(string &PortName);
    bool UploadArduino(const QString &FileName);

    QSerialPort *SerialPort;
    QString CurrentPortName;
    QTimer *WriteTimer;
    QTimer *SendTimer;
    QVector<double> DataBuffer1;
    QVector<double> DataBuffer2;
    QVector<double> DataBuffer3;
    QVector<double> TimeBuffer;
    QMutex bufferMutex;
    QMutex writeMutex;
    QMutex commandMutex;
    QByteArray WriteBuffer;

    // std::function<void(float, float, float, double)> onDataReceived;

    UArduinoConnect(string &PortName);
    virtual ~ UArduinoConnect();
    string com;

signals:
    void UploadFinished(bool success);
    void DataReceived(float temperature, float humidity, double time, float mfield);
    void SerialPortConnected(bool connected);

public:
    void OnSerialPortRead();

    void FillBuffer1(float temperature);
    void FillBuffer2(float humidity);
    void FillBuffer3(float mfield);
    void FillTimeBuffer(float time);

    QVector<double> GetAndClearBuffer1();
    QVector<double> GetAndClearBuffer2();
    QVector<double> GetAndClearBuffer3();
    QVector<double> GetAndClearTimeBuffer();

    void CheckWrite();
    void SetCommand(string& cmd);
    void SendData();
    double DateTime();

    QVector<DataPoint> DataBuffer;
    void FillData(double timestamp, uint8_t paramCount, float temperature, float humidity, float mfield, float servo_speed);
    QVector<double> GetAndClearAllData();

    QVector<int> allPins;
};
}

#endif // UARDUINOCONNECT_H
