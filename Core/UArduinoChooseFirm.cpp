#ifndef UARDUINOCHOOSEFIRM_CPP
#define UARDUINOCHOOSEFIRM_CPP

#include "UArduinoChooseFirm.h"

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
   qDebug() << "Arduino port is connected";
   } else {
    qDebug() << "Failed to connect to Arduino port";
   }
 } else {
   qDebug() << "Arduino port not found";
    }
}

void ArduinoUploader::onSerialPortRead()
{
    qDebug() << "Start to read data";

    while (serialPort->bytesAvailable() >= sizeof(float)) {
        QByteArray data = serialPort->readAll(); // Чтение всех доступных данных с порта

        // Обработка полученных данных
        if (data.size() >= sizeof(float)) {
            float temperature;
            float humidity;

            memcpy(&temperature, data.constData(), sizeof(temperature));
            memcpy(&humidity, data.constData() + sizeof(temperature), sizeof(humidity));
            dataBuffer.append(temperature);
            dataBuffer2.append(humidity);
            if (dataBuffer.size() > 512) { // Если сохраненных значений больше 512, удаляем самое старое
                dataBuffer.removeFirst(); // Удаляем первое (самое старое) значение
            }
            if (dataBuffer2.size() > 512) { // Если сохраненных значений больше 512, удаляем самое старое
                dataBuffer2.removeFirst(); // Удаляем первое (самое старое) значение
            }
            qDebug() << "Received temperature:" << QString::number(temperature, 'f', 2);
            qDebug() << "Received humidity:" << QString::number(humidity, 'f', 2);
        } else {
            qDebug() << "Failed to get data";
        }

    }
}

ArduinoUploader::ArduinoUploader(void)
{
 initSerialPort();
 //Таймер на чтениие из порта с интервалом в 2 секунды
 readTimer = new QTimer(this);
 readTimer->setInterval(2000);
 connect(readTimer, &QTimer::timeout, this, &ArduinoUploader::onSerialPortRead);
 readTimer->start();

 onSerialPortRead();
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



bool ArduinoUploader::uploadArduino(const QString &fileName)
{
 // Команда avrdude
 //Пути к avrdude(загружает файл на микроконтроллер ардуины)
 // C:\Users\van15\AppData\Local\Arduino15\packages\arduino\tools\avrdude\6.3.0-arduino17\bin\avrdude
 // C:\Users\van15\AppData\Local\Arduino15\packages\arduino\tools\avrdude\6.3.0-arduino17\etc\avrdude.conf
 //Пути к avr-gcc(компилирует код(если надо будет))
 //C:\Users\van15\AppData\Local\Arduino15\packages\arduino\tools\avr-gcc\7.3.0-atmel3.6.1-arduino7\bin\avr-gcc

 QString avrdudeCommand = QString("avrdude -C avrdude.conf -v -patmega328p -carduino -P %1 -b 9600 -D -U flash:w:%2:i").arg(currentPortName).arg(fileName);

 // // Запускаем avrdude через QProcess
 QProcess process;
 process.start(avrdudeCommand);
 process.waitForFinished();

 return process.exitCode() == 0;
}



void ArduinoUploader::readDHT11Data()
{
 // serialPort->write("readDHT\n");
 // readTimer->start(); // Запускаем таймер, чтобы ожидать ответа Arduino
}

void ArduinoUploader::setSerialPortName(const QString &portName)
{
 // currentPortName = portName;
 // if (serialPort) {
 //  delete serialPort;
 //  serialPort = nullptr;
 // }
 // initSerialPort();
}

void ArduinoUploader::run() {
    // initSerialPort(); // Инициализируем последовательный порт
    //     onSerialPortRead();
}
}

#endif
