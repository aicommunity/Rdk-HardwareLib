#ifndef UARDUINOCHOOSEFIRM_H
#define UARDUINOCHOOSEFIRM_H

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

#include "../../../Rdk/Deploy/Include/rdk.h"
// #include "UArduinoSensor.h"

namespace RDK{
class ArduinoUploader: public QThread
{
 Q_OBJECT

private:
    void initSerialPort();
    bool uploadArduino(const QString &fileName);

    QSerialPort *serialPort;
    QString currentPortName;
    QTimer *readTimer;
    QVector<float> dataBuffer;
    QVector<float> dataBuffer2;
    QMutex bufferMutex;

public:
 ArduinoUploader(void);
 virtual ~ArduinoUploader();
 void run() override;

signals:
 void uploadFinished(bool success);
 void dataReceived(const QString &data);
 void serialPortConnected(bool connected);

public slots:
 void onUploadClicked();
 void onSerialPortRead();
 // void updateSerialPortList();
 void setSerialPortName(const QString &portName);
 void readDHT11Data();

};
}

#endif // UARDUINOCHOOSEFIRM_H
