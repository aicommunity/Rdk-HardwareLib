#ifndef UARDUINOCONNECT_H
#define UARDUINOCONNECT_H

#include <QObject>
#include <QThread>
#include <QtSerialPort/qserialport>
#include <QtSerialPort/qserialportinfo>
#include <QProcess>
#include <QTimer>
#include <QString>
#include <QDebug>
#include <QMutex>
#include <QDateTime>
#include <QTimeZone>
#include <thread>
#include <mutex>
#include <string>

#include "../../../Rdk/Deploy/Include/rdk.h"
#include "UArduinoSensor.h"

namespace RDK{

class UArduinoSensor;

class UArduinoConnect: public QThread
{
 Q_OBJECT

private:
    void InitSerialPort(string &PortName);
    bool UploadArduino(const QString &FileName);

    QSerialPort *SerialPort;
    QString CurrentPortName;
    QTimer *ReadTimer;
    QVector<double> DataBuffer;
    QVector<double> DataBuffer2;
    QVector<double> DataBuffer3;
    QVector<double> TimeBuffer;
    UArduinoSensor* Sensor;

public:
 UArduinoConnect(string &PortName, UArduinoSensor* sensor);
 virtual ~ UArduinoConnect();
 // void run() override;

signals:
 void UploadFinished(bool success);
 void DataReceived(const QString &data);
 void SerialPortConnected(bool connected);

public:
 void OnSerialPortRead();
double DateTime();
};
}

#endif // UARDUINOCONNECT_H
