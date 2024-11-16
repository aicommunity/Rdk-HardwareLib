#ifndef UARDUINOCHOOSEFIRM_CPP
#define UARDUINOCHOOSEFIRM_CPP

#include "UArduinoChooseFirm.h"
// #include <QFileDialog>
// #include <QMessageBox>
#include <QDebug>

namespace RDK {

void ArduinoUploader::initSerialPort()
{
 QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

 if (!ports.isEmpty()) {
  // currentPortName = ports.first().portName();
  currentPortName = "COM7";
  serialPort = new QSerialPort(currentPortName);
  serialPort->setBaudRate(QSerialPort::Baud9600);

  if (serialPort->open(QIODevice::ReadWrite)) {
   qDebug() << "���� Arduino ���������.";
   readTimer->start();
   } else {
    qDebug() << "������ ����������� � ����� Arduino.";
   }
 } else {
   qDebug() << "���� Arduino �� ������.";
    }
}

ArduinoUploader::ArduinoUploader(void)
{
 initSerialPort();

 //������ �� ������� �� ����� � ���������� � 1 �������
 readTimer = new QTimer(this);
 readTimer->setInterval(1000);
 connect(readTimer, &QTimer::timeout, this, &ArduinoUploader::onSerialPortRead);
}

ArduinoUploader::~ArduinoUploader()
{
 if (serialPort) {
  serialPort->close();
  delete serialPort;
 }

 if (readTimer) {
  delete readTimer;
 }
}

void ArduinoUploader::onUploadClicked()
{
 QString fileName = "test_temp.ino";

 if (!serialPort->isOpen()) {
  emit dataReceived("���� Arduino �� ���������.");
  return;
 }

 emit dataReceived("�������� �����...");
 bool success = uploadArduino(fileName);

 if (success) {
  emit dataReceived("���� ������� ��������.");
 } else {
  emit dataReceived("������ �������� �����.");
 }

 emit uploadFinished(success);
}



bool ArduinoUploader::uploadArduino(const QString &fileName)
{
 // ������� avrdude
 //���� � avrdude(��������� ���� �� ��������������� �������)
 // C:\Users\van15\AppData\Local\Arduino15\packages\arduino\tools\avrdude\6.3.0-arduino17\bin\avrdude
 // C:\Users\van15\AppData\Local\Arduino15\packages\arduino\tools\avrdude\6.3.0-arduino17\etc\avrdude.conf
 //���� � avr-gcc(����������� ���(���� ���� �����))
 //C:\Users\van15\AppData\Local\Arduino15\packages\arduino\tools\avr-gcc\7.3.0-atmel3.6.1-arduino7\bin\avr-gcc

 QString avrdudeCommand = QString("avrdude -C avrdude.conf -v -patmega328p -carduino -P %1 -b 9600 -D -U flash:w:%2:i").arg(currentPortName).arg(fileName);

 // ��������� avrdude ����� QProcess
 QProcess process;
 process.start(avrdudeCommand);
 process.waitForFinished();

 return process.exitCode() == 0;
}

void ArduinoUploader::onSerialPortRead()
{
 if (serialPort->canReadLine()) {
  QString data = serialPort->readLine();
  qDebug() << "���������� ������: " << data;
  //����� ����� ���������� ���������� ������ � ������ ������ UArduinoSensor
 }
}

void ArduinoUploader::readDHT11Data()
{
 serialPort->write("readDHT\n");
 readTimer->start(); // ��������� ������, ����� ������� ������ Arduino
}

void ArduinoUploader::setSerialPortName(const QString &portName)
{
 currentPortName = portName;
 if (serialPort) {
  delete serialPort;
  serialPort = nullptr;
 }
 initSerialPort();
}

void ArduinoUploader::run() {
    // initSerialPort(); // �������������� ���������������� ����
    ArduinoUploader();
        onSerialPortRead();
    //     // readDHT11Data();
}
}

#endif
