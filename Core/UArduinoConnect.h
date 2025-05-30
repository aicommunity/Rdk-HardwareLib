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
    // QTimer *WriteTimer;
    QMutex bufferMutex;
    QMutex writeMutex;
    QMutex commandMutex;
    QByteArray WriteBuffer;

    UArduinoConnect(string &PortName);
    virtual ~ UArduinoConnect();
    string com;
    bool deb;

signals:
    void UploadFinished(bool success);
    void DataReceived(float temperature, float humidity, double time, float mfield);
    void SerialPortConnected(bool connected);

public:
    void OnSerialPortRead();
    QString pinToString(int pin);
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
