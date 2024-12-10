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
#include <thread>
#include <mutex>
#include <string>

#include "../../../Rdk/Deploy/Include/rdk.h"
// #include "UArduinoSensor.h"

namespace RDK{
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

public:
 UArduinoConnect(string &PortName);
 virtual ~ UArduinoConnect();
 void run() override;

signals:
 void UploadFinished(bool success);
 void DataReceived(const QString &data);
 void SerialPortConnected(bool connected);

public slots:
 void OnUploadClicked();
 void OnSerialPortRead();
 // void UpdateSerialPortList();
 void SetSerialPortName(const QString &portName);
 void ReadDHT11Data();

};
}

#endif // UARDUINOCONNECT_H
