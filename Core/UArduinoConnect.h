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
// #include "UArduinoSensor.h"

namespace RDK{

// class UArduinoSensor;

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
    QVector<double> DataBuffer;
    QVector<double> DataBuffer2;
    QVector<double> DataBuffer3;
    QVector<double> TimeBuffer;
    QMutex bufferMutex;
    QMutex writeMutex;
    QMutex commandMutex;
    QByteArray WriteBuffer;

     std::function<void(float, float, float, double)> onDataReceived;

 UArduinoConnect(string &PortName,  std::function<void(float, float, float, double)> onDataReceived);
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

 QVector<double> GetBuffer1();
 QVector<double> GetBuffer2();
 QVector<double> GetBuffer3();
 QVector<double> GetTimeBuffer();

 void ClearBuffer1();
 void ClearBuffer2();
 void ClearBuffer3();
 void ClearTimeBuffer();

 void CheckWrite();
 void SetCommand(string& cmd);
 void SendData();
 double DateTime();
};
}

#endif // UARDUINOCONNECT_H
