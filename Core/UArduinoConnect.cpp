#ifndef UARDUINOCONNECT_CPP
#define UARDUINOCONNECT_CPP

#include "UArduinoConnect.h"

namespace RDK {

void  UArduinoConnect::InitSerialPort(string &PortName)
{
 QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

 if (!ports.isEmpty()) {
  CurrentPortName = QString::fromStdString(PortName);
  SerialPort = new QSerialPort(CurrentPortName);
  SerialPort->setBaudRate(QSerialPort::Baud9600);

  if (SerialPort->open(QIODevice::ReadWrite)) {
   qDebug() << "Arduino port is connected";
   } else {
    qDebug() << "Failed to connect to Arduino port";
   }
 } else {
   qDebug() << "Arduino port not found";
    }
}

void  UArduinoConnect::OnSerialPortRead()
{
    qDebug() << "Start to read data";

    while (SerialPort->bytesAvailable() >= sizeof(float)) {
        QByteArray data = SerialPort->readAll(); // ������ ���� ��������� ������ � �����

        // ��������� ���������� ������
        if (data.size() >= sizeof(float)) {
            float temperature;
            float humidity;

            memcpy(&temperature, data.constData(), sizeof(temperature));
            memcpy(&humidity, data.constData() + sizeof(temperature), sizeof(humidity));
            DataBuffer.append(temperature);
            DataBuffer2.append(humidity);
            if (DataBuffer.size() > 512) { // ���� ����������� �������� ������ 512, ������� ����� ������
                DataBuffer.removeFirst(); // ������� ������ (����� ������) ��������
            }
            if (DataBuffer2.size() > 512) { // ���� ����������� �������� ������ 512, ������� ����� ������
                DataBuffer2.removeFirst(); // ������� ������ (����� ������) ��������
            }
            qDebug() << "Received temperature:" << QString::number(temperature, 'f', 2);
            qDebug() << "Received humidity:" << QString::number(humidity, 'f', 2);
        } else {
            qDebug() << "Failed to get data";
        }

    }
}

 UArduinoConnect:: UArduinoConnect(string &PortName)
{
 InitSerialPort(PortName);
 //������ �� ������� �� ����� � ���������� � 2 �������
 ReadTimer = new QTimer(this);
 ReadTimer->setInterval(2000);
 connect(ReadTimer, &QTimer::timeout, this, & UArduinoConnect::OnSerialPortRead);
 ReadTimer->start();

 OnSerialPortRead();
}

 UArduinoConnect::~ UArduinoConnect()
{
 if (SerialPort) {
  SerialPort->close();
  delete SerialPort;
 }

 if (ReadTimer) {
  delete ReadTimer;
 }
}

void  UArduinoConnect::OnUploadClicked()
{
 // QString fileName = "test_temp.ino";

 // if (!serialPort->isOpen()) {
 //  emit dataReceived("Failed to connect to Arduino port");
 //  return;
 // }

 // emit dataReceived("Loading file...");
 // bool success = uploadArduino(fileName);

 // if (success) {
 //  emit dataReceived("File is uploaded succesfully");
 // } else {
 //  emit dataReceived("File loading error");
 // }

 // emit uploadFinished(success);
}



bool  UArduinoConnect::UploadArduino(const QString &fileName)
{
 // ������� avrdude
 //���� � avrdude(��������� ���� �� ��������������� �������)
 // C:\Users\van15\AppData\Local\Arduino15\packages\arduino\tools\avrdude\6.3.0-arduino17\bin\avrdude
 // C:\Users\van15\AppData\Local\Arduino15\packages\arduino\tools\avrdude\6.3.0-arduino17\etc\avrdude.conf
 //���� � avr-gcc(����������� ���(���� ���� �����))
 //C:\Users\van15\AppData\Local\Arduino15\packages\arduino\tools\avr-gcc\7.3.0-atmel3.6.1-arduino7\bin\avr-gcc

 QString avrdudeCommand = QString("avrdude -C avrdude.conf -v -patmega328p -carduino -P %1 -b 9600 -D -U flash:w:%2:i").arg(CurrentPortName).arg(fileName);

 // // ��������� avrdude ����� QProcess
 QProcess process;
 process.start(avrdudeCommand);
 process.waitForFinished();

 return process.exitCode() == 0;
}



void  UArduinoConnect::ReadDHT11Data()
{
 // serialPort->write("readDHT\n");
 // readTimer->start(); // ��������� ������, ����� ������� ������ Arduino
}

void  UArduinoConnect::SetSerialPortName(const QString &portName)
{
 // currentPortName = portName;
 // if (serialPort) {
 //  delete serialPort;
 //  serialPort = nullptr;
 // }
 // initSerialPort();
}

void  UArduinoConnect::run() {
    // initSerialPort(); // �������������� ���������������� ����
    //     onSerialPortRead();
}
}

#endif
