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
   qDebug() << "Порт Arduino подключен.";
   readTimer->start();
   } else {
    qDebug() << "Ошибка подключения к порту Arduino.";
   }
 } else {
   qDebug() << "Порт Arduino не найден.";
    }
}

ArduinoUploader::ArduinoUploader(void)
{
 initSerialPort();

 //Таймер на чтениие из порта с интервалом в 1 секунду
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
  emit dataReceived("Порт Arduino не подключен.");
  return;
 }

 emit dataReceived("Загрузка файла...");
 bool success = uploadArduino(fileName);

 if (success) {
  emit dataReceived("Файл успешно загружен.");
 } else {
  emit dataReceived("Ошибка загрузки файла.");
 }

 emit uploadFinished(success);
}



bool ArduinoUploader::uploadArduino(const QString &fileName)
{
 // Команда avrdude
 //Пути к avrdude(загружает файл на микроконтроллер ардуины)
 // C:\Users\van15\AppData\Local\Arduino15\packages\arduino\tools\avrdude\6.3.0-arduino17\bin\avrdude
 // C:\Users\van15\AppData\Local\Arduino15\packages\arduino\tools\avrdude\6.3.0-arduino17\etc\avrdude.conf
 //Пути к avr-gcc(компилирует код(если надо будет))
 //C:\Users\van15\AppData\Local\Arduino15\packages\arduino\tools\avr-gcc\7.3.0-atmel3.6.1-arduino7\bin\avr-gcc

 QString avrdudeCommand = QString("avrdude -C avrdude.conf -v -patmega328p -carduino -P %1 -b 9600 -D -U flash:w:%2:i").arg(currentPortName).arg(fileName);

 // Запускаем avrdude через QProcess
 QProcess process;
 process.start(avrdudeCommand);
 process.waitForFinished();

 return process.exitCode() == 0;
}

void ArduinoUploader::onSerialPortRead()
{
 if (serialPort->canReadLine()) {
  QString data = serialPort->readLine();
  qDebug() << "Полученные данные: " << data;
  //Здесь пойдёт сохранение полученных данных в вектор класса UArduinoSensor
 }
}

void ArduinoUploader::readDHT11Data()
{
 serialPort->write("readDHT\n");
 readTimer->start(); // Запускаем таймер, чтобы ожидать ответа Arduino
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
    // initSerialPort(); // Инициализируем последовательный порт
    ArduinoUploader();
        onSerialPortRead();
    //     // readDHT11Data();
}
}

#endif
