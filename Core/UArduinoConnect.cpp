#ifndef UARDUINOCONNECT_CPP
#define UARDUINOCONNECT_CPP

#include "UArduinoConnect.h"

namespace RDK {

double UArduinoConnect::DateTime() {
    QDateTime now = QDateTime::currentDateTime();
    QDate date = now.date();
    QTime time = now.time();

    // int day = date.day();
    // int month = date.month();
    // int year = date.year() % 100; // Последние две цифры года
    int hour = time.hour();
    int minute = time.minute();
    int second = time.second();

    // Формируем число в формате 291224143251
    double customDateTime = /*(day * 10000000000LL) +
                            (month * 100000000LL) +
                            (year * 1000000LL) +*/
                            (hour * 10000LL) +
                            (minute * 100LL) +
                            second;

    return customDateTime;
}

void  UArduinoConnect::InitSerialPort(string &PortName)
{
 QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

 if (!ports.isEmpty()) {
  CurrentPortName = QString::fromStdString(PortName);
  SerialPort = new QSerialPort(CurrentPortName);
  SerialPort->setBaudRate(QSerialPort::Baud9600);

  if (SerialPort->open(QIODevice::ReadWrite)) {
   qDebug() << "Arduino port is connected";
      connect(SerialPort, &QSerialPort::readyRead, this, &UArduinoConnect::OnSerialPortRead);
   } else {
    qDebug() << "Failed to connect to Arduino port";
   }
 } else {
   qDebug() << "Arduino port not found";
    }
 //Таймер на проверку готовности порта к записи данных
 WriteTimer = new QTimer(this);
 connect(WriteTimer, &QTimer::timeout, this, &UArduinoConnect::CheckWrite);
 WriteTimer->start(100);
 // Таймер на отправку данных
 SendTimer = new QTimer(this);
 connect(SendTimer, &QTimer::timeout, this, &UArduinoConnect::SendData);
 SendTimer->start(1000);
}

void UArduinoConnect::FillBuffer1(float temperature) {
    QMutexLocker locker(&bufferMutex);
    if (DataBuffer.size() < 512) {
        DataBuffer.removeFirst();
    }
    DataBuffer.append(temperature);
}

void UArduinoConnect::FillBuffer2(float humidity) {
    QMutexLocker locker(&bufferMutex);
    if (DataBuffer2.size() < 512) {
        DataBuffer2.removeFirst();
    }
    DataBuffer2.append(humidity);
}

void UArduinoConnect::FillBuffer3(float mfield) {
    QMutexLocker locker(&bufferMutex);
    if (DataBuffer3.size() < 512) {
        DataBuffer3.removeFirst();
    }
    DataBuffer3.append(mfield);
}
void UArduinoConnect::FillTimeBuffer(float time)
{
    QMutexLocker locker(&bufferMutex);
    if (TimeBuffer.size() < 512) {
        TimeBuffer.removeFirst();
    }
    TimeBuffer.append(time);
}

QVector<double> UArduinoConnect::GetBuffer1() {
    QMutexLocker locker(&bufferMutex);
    return DataBuffer;
}

QVector<double> UArduinoConnect::GetBuffer2() {
    QMutexLocker locker(&bufferMutex);
    return DataBuffer2;
}

QVector<double> UArduinoConnect::GetBuffer3() {
    QMutexLocker locker(&bufferMutex);
    return DataBuffer3;
}

QVector<double> UArduinoConnect::GetTimeBuffer() {
    QMutexLocker locker(&bufferMutex);
    return TimeBuffer;
}

void UArduinoConnect::ClearBuffer1() {
    QMutexLocker locker(&bufferMutex);
    DataBuffer.clear();
}

void UArduinoConnect::ClearBuffer2() {
    QMutexLocker locker(&bufferMutex);
    DataBuffer2.clear();
}

void UArduinoConnect::ClearBuffer3() {
    QMutexLocker locker(&bufferMutex);
    DataBuffer3.clear();
}

void UArduinoConnect::ClearTimeBuffer() {
    QMutexLocker locker(&bufferMutex);
    TimeBuffer.clear();
}

void  UArduinoConnect::OnSerialPortRead()
{
    if (!SerialPort->isOpen()) {
        qDebug() << "Serial port is not open.";
        return;
    }
    qDebug() << "Start to read data";

   if (SerialPort->bytesAvailable() >= sizeof(double)) {
        QByteArray data = SerialPort->readAll();
         // Чтение всех доступных данных с порта

        // Обработка полученных данных
        if (data.size() >= sizeof(double)) {
            float temperature;
            float humidity;
            float mfield;

            memcpy(&temperature, data.constData(), sizeof(temperature));
            memcpy(&humidity, data.constData() + sizeof(temperature), sizeof(humidity));
            memcpy(&mfield, data.constData() + sizeof(temperature) + sizeof(humidity), sizeof(mfield));

            double time = DateTime();

            FillBuffer1(temperature);
            FillBuffer2(humidity);
            FillBuffer3(mfield);
            FillTimeBuffer(time);

            // DataBuffer.append(temperature);
            // DataBuffer2.append(humidity);
            // DataBuffer3.append(mfield);
            // TimeBuffer.append(time);

            // if (DataBuffer.size() > 512) { // Если сохраненных значений больше 512, удаляем самое старое
            //     DataBuffer.removeFirst(); // Удаляем первое (самое старое) значение
            // }
            // if (DataBuffer2.size() > 512) { // Если сохраненных значений больше 512, удаляем самое старое
            //     DataBuffer2.removeFirst(); // Удаляем первое (самое старое) значение
            // }
            // if (DataBuffer3.size() > 512) { // Если сохраненных значений больше 512, удаляем самое старое
            //     DataBuffer3.removeFirst(); // Удаляем первое (самое старое) значение
            // }
            // if (TimeBuffer.size() > 512) {
            //     TimeBuffer.removeFirst();
            // }
            qDebug() << "Received temperature:" << QString::number(temperature, 'lf', 2);
            qDebug() << "Received humidity:" << QString::number(humidity, 'lf', 2);
            qDebug() << "mfield:" << QString::number(mfield, 'lf', 2);
            qDebug() << "Custom-Time:" << QString::number(time, 'lf', 2);
            // if (onDataReceived) {
                onDataReceived(temperature, humidity, time, mfield);
            // }
        } else {
            qDebug() << "Failed to get data";
        }

    }
}

 UArduinoConnect:: UArduinoConnect(string &PortName, std::function<void(float, float, float, double)> onDataReceived)
    : SerialPort(nullptr), WriteTimer(nullptr), SendTimer(nullptr), com(""), onDataReceived(onDataReceived) {
    InitSerialPort(PortName);
}

 UArduinoConnect::~ UArduinoConnect()
{
 if (WriteTimer) delete WriteTimer;
 if (SendTimer) delete SendTimer;
 if (SerialPort) {
  SerialPort->close();
  delete SerialPort;
 }
}

bool  UArduinoConnect::UploadArduino(const QString &fileName)
{
 // Команда avrdude
 //Пути к avrdude(загружает файл на микроконтроллер ардуины)
 // C:\Users\van15\AppData\Local\Arduino15\packages\arduino\tools\avrdude\6.3.0-arduino17\bin\avrdude
 // C:\Users\van15\AppData\Local\Arduino15\packages\arduino\tools\avrdude\6.3.0-arduino17\etc\avrdude.conf
 //Пути к avr-gcc(компилирует код(если надо будет))
 //C:\Users\van15\AppData\Local\Arduino15\packages\arduino\tools\avr-gcc\7.3.0-atmel3.6.1-arduino7\bin\avr-gcc

 QString avrdudeCommand = QString("avrdude -C avrdude.conf -v -patmega328p -carduino -P %1 -b 9600 -D -U flash:w:%2:i").arg(CurrentPortName).arg(fileName);

 // // Запускаем avrdude через QProcess
 QProcess process;
 process.start(avrdudeCommand);
 process.waitForFinished();

 return process.exitCode() == 0;
}

void UArduinoConnect::CheckWrite() {
    QMutexLocker lockPort(&writeMutex);
    if(!SerialPort || !SerialPort->isOpen()) return;

    if(SerialPort->bytesToWrite() == 0 && !WriteBuffer.isEmpty()) {
        qint64 written = SerialPort->write(WriteBuffer);
        if(written > 0) {
            WriteBuffer = WriteBuffer.mid(written);
        }
        else {
            qDebug() << "Write error:" << SerialPort->errorString();
        }
    }
}

void UArduinoConnect::SetCommand(string& cmd) {
    QMutexLocker lock(&commandMutex);
    com = cmd;
}

void UArduinoConnect::SendData() {
    QMutexLocker lockCmd(&commandMutex);
    QMutexLocker lockWrite(&writeMutex);

    if (com.empty()) return;

    QString qStr = QString::fromStdString(com);
    QByteArray data = qStr.toUtf8().append('\n');

    if (WriteBuffer.size() + data.size() > 4096) {
        qDebug() << "Write buffer overflow!";
        return;
    }

    WriteBuffer.append(data);
    com.clear();

    CheckWrite();
}
}

#endif
