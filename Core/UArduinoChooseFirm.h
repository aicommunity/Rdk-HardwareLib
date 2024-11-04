#ifndef UARDUINOCHOOSEFIRM_H
#define UARDUINOCHOOSEFIRM_H

#include <QObject>
#include <QThread>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QProcess>
#include <QTimer>
#include <QString>
#include <QMutex>
#include <thread>
#include <mutex>

#include "../../../Rdk/Deploy/Include/rdk.h"
#include "UArduinoSensor.h"

namespace RDK{
class ArduinoUploader: public QThread
{
 Q_OBJECT
public:
 ArduinoUploader(void);
 virtual ~ArduinoUploader();

signals:
 void uploadFinished(bool success);
 void dataReceived(const QString &data);
 void serialPortConnected(bool connected);

public slots:
 void onUploadClicked();
 void onSerialPortRead();
 void updateSerialPortList();
 void setSerialPortName(const QString &portName);
 // void readDHT11Data();

private:
 void initSerialPort();
 bool uploadArduino(const QString &fileName);

 QSerialPort *serialPort;
 QString currentPortName;
 QTimer *readTimer;
 // UArduinoSensor *sensor;
};
}

#endif // UARDUINOCHOOSEFIRM_H
