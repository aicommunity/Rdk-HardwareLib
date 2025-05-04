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
}

void UArduinoConnect::FillData(double timestamp, uint8_t paramCount,
                               float temperature, float humidity,
                               float mfield, float servo_speed) {
    QMutexLocker locker(&bufferMutex);
    qDebug() << "Storing data point:"
             << "Params count:" << paramCount
             << "Time:" << timestamp
             << "Temp:" << temperature
             << "Hum:" << humidity
             << "Field:" << mfield
             << "Speed:" << servo_speed;

    if (DataBuffer.size() >= 512) {
        qDebug() << "Buffer overflow - removing oldest pack";
        DataBuffer.removeFirst();
    }

    DataPoint dp;
    dp.data.append(timestamp);
    dp.data.append(static_cast<double>(paramCount));
    dp.data.append(temperature);
    dp.data.append(humidity);
    dp.data.append(mfield);
    dp.data.append(servo_speed);

    DataBuffer.append(dp);
    qDebug() << "Current buffer size:" << DataBuffer.size();
}

QVector<double> UArduinoConnect::GetAndClearAllData() {
    QMutexLocker locker(&bufferMutex);
    QVector<double> result;
    for (const auto& dp : DataBuffer) {
        result += dp.data;
    }
    DataBuffer.clear();
    return result;
}

void UArduinoConnect::OnSerialPortRead() {
    if (!SerialPort->isOpen()) {
        qDebug() << "Port not open, skip reading";
        return;
    }

    QByteArray data = SerialPort->readAll();
    qDebug() << "Received raw data, size:" << data.size() << "bytes";
    qDebug() << "Hex dump:" << data.toHex();

    const char* ptr = data.constData();
    int index = 0;
    int dataSize = data.size();

    while (index < dataSize) {
        if (index >= dataSize) {
            qDebug() << "Reached end of data";
            break;
        }

        uint8_t packetId = static_cast<uint8_t>(ptr[index++]);

        if (packetId == 0x01) {
            qDebug() << "Sensor data packet detected";

            if (index + 1 > dataSize) {
                qDebug() << "Incomplete packet header";
                break;
            }

            uint8_t paramCount = static_cast<uint8_t>(ptr[index++]);
            qDebug() << "Parameters count:" << paramCount;

            int requiredBytes = paramCount * sizeof(float);

            if (index + requiredBytes > dataSize) {
                qDebug() << "Not enough data for parameters";
                break;
            }

            if (paramCount == 4) {
                float temperature, humidity, mfield, speed;
                memcpy(&temperature, ptr + index, sizeof(float)); index += sizeof(float);
                memcpy(&humidity, ptr + index, sizeof(float)); index += sizeof(float);
                memcpy(&mfield, ptr + index, sizeof(float)); index += sizeof(float);
                memcpy(&speed, ptr + index, sizeof(float)); index += sizeof(float);

                qDebug() << "Parsed values:"
                         << "Temp:" << temperature
                         << "Hum:" << humidity
                         << "Field:" << mfield
                         << "Speed:" << speed;

                FillData(DateTime(), paramCount, temperature, humidity, mfield, speed);
            } else {
                qDebug() << "Unexpected parameter count:" << paramCount;
                index += requiredBytes;
            }
        } else if (packetId == 0x02) {
            qDebug() << "Pins info packet detected";

            if (index + 1 > dataSize) {
                qDebug() << "Incomplete pins header";
                break;
            }

            uint8_t pinCount = static_cast<uint8_t>(ptr[index++]);
            qDebug() << "Total pins:" << pinCount;

            if (index + pinCount > dataSize) {
                qDebug() << "Not enough data for pins";
                break;
            }

            QVector<int> newPins;
            qDebug() << "Pins list:";
            for (int i = 0; i < pinCount; ++i) {
                uint8_t pin = static_cast<uint8_t>(ptr[index++]);
                newPins.append(pin);
                qDebug() << "Pin #" << i << ":" << pin;
            }

            {
                QMutexLocker locker(&bufferMutex);
                allPins = newPins;
                qDebug() << "Updated pins list:" << allPins;
            }
        } else {
            qDebug() << "Unknown packet ID:" << packetId;
            break;
        }
    }
}

UArduinoConnect:: UArduinoConnect(string &PortName)
    : SerialPort(nullptr), WriteTimer(nullptr), com("") {
    InitSerialPort(PortName);
}

UArduinoConnect::~UArduinoConnect() {
    if (WriteTimer) {
        WriteTimer->stop();
    }

    if (SerialPort) {
        if (SerialPort->isOpen()) {
            SerialPort->close();
        }
        SerialPort->deleteLater();
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
    if (!SerialPort || !SerialPort->isOpen()) return;

    if (SerialPort->bytesToWrite() == 0 && !WriteBuffer.isEmpty()) {
        qint64 written = SerialPort->write(WriteBuffer);
        if (written > 0) {
            QByteArray sentData = WriteBuffer.left(written);
            qDebug() << "Successfully sent command:" << sentData.trimmed();
            WriteBuffer = WriteBuffer.mid(written);
        } else {
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
    if (com.empty()) return;

    if (!SerialPort || !SerialPort->isOpen()) {
        qDebug() << "SendData: Port is not open.";
        return;
    }

    QByteArray data;
    {
        QString qStr = QString::fromStdString(com);
        data = qStr.toUtf8().append('\n');
        com.clear();
    }

    {
        QMutexLocker lockWrite(&writeMutex);
        if (WriteBuffer.size() + data.size() > 4096) {
            qDebug() << "Write buffer overflow!";
            return;
        }
        WriteBuffer.append(data);
    }
}
}

#endif
