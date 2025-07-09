#ifndef UARDUINOCONNECT_CPP
#define UARDUINOCONNECT_CPP

#include "UArduinoConnect.h"

//#ifdef Q_OS_WIN
// Аналоговые пины на Arduino UNO: A0=14, A1=15, ..., A5=19
#define A0 14
#define A1 15
#define A2 16
#define A3 17
#define A4 18
#define A5 19
//#endif

namespace RDK {

double UArduinoConnect::DateTime() {
    QDateTime now = QDateTime::currentDateTime();
    //QDate date = now.date();
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

void UArduinoConnect::InitSerialPort(string &PortName)
{
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    CurrentPortName = QString::fromStdString(PortName);

    // Проверяем существование порта
    bool portExists = false;
    for(const auto& port : ports) {
        if(port.portName() == CurrentPortName) {
            portExists = true;
            break;
        }
    }

    if(!portExists) {
        qDebug() << "Port" << CurrentPortName << "not found!";
        return;
    }

    SerialPort = new QSerialPort(CurrentPortName);
    SerialPort->setBaudRate(QSerialPort::Baud9600);

    if (!SerialPort->open(QIODevice::ReadWrite)) {
        qDebug() << "Failed to open port:" << SerialPort->errorString();
        delete SerialPort;
        SerialPort = nullptr;
        return;
    }

    qDebug() << "Arduino port is connected";
    connect(SerialPort, &QSerialPort::readyRead, this, &UArduinoConnect::OnSerialPortRead);

    // // Инициализация таймера только если порт открыт
    // WriteTimer = new QTimer(this);
    // connect(WriteTimer, &QTimer::timeout, this, &UArduinoConnect::CheckWrite);
    // WriteTimer->start(100);
}

void UArduinoConnect::FillData(double timestamp, uint8_t paramCount,
                               float temperature, float humidity,
                               float mfield, float servo_speed) {
    QMutexLocker locker(&bufferMutex);
    if (deb) {
        qDebug() << "Storing data point:"
                 << "Params count:" << paramCount
                 << "Time:" << timestamp
                 << "Temp:" << temperature
                 << "Hum:" << humidity
                 << "Field:" << mfield
                 << "Speed:" << servo_speed;
    }
    if (DataBuffer.size() >= 512) {
        if (deb)
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
    if (deb)
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

QString UArduinoConnect::pinToString(int pin) {
    if (pin >= A0 && pin <= A5) {
        return QString("A%1").arg(pin - A0);
    } else if (pin >= 2 && pin <= 13) {
        return QString("D%1").arg(pin);
    }
    return QString::number(pin);
}

void UArduinoConnect::OnSerialPortRead() {
    if (!SerialPort || !SerialPort->isOpen()) {
        qDebug() << "Port not open, skip reading";
        return;
    }

    QByteArray data = SerialPort->readAll();
    const char* ptr = data.constData();
    int index = 0;
    int dataSize = data.size();

    while (index < dataSize) {
        uint8_t packetId = static_cast<uint8_t>(ptr[index++]);

        if (packetId == 0x01) {
            if (index + 2 > dataSize) {
                qDebug() << "Incomplete sensor header";
                break;
            }

            uint8_t errorFlags = ptr[index++];
            uint8_t paramCount = ptr[index++];
            int requiredBytes = paramCount * sizeof(float);

            if (index + requiredBytes > dataSize) {
                qDebug() << "Incomplete sensor data";
                break;
            }

            float values[4] = {NAN};
            for (int i = 0; i < paramCount; i++) {
                memcpy(&values[i], ptr + index, sizeof(float));
                index += sizeof(float);
            }

            QStringList errors;
            if (errorFlags & 0x01) errors << "Temperature";
            if (errorFlags & 0x02) errors << "Humidity";
            if (errorFlags & 0x04) errors << "Hall Sensor";
            if (!errors.isEmpty()) {
                qDebug() << "Sensor errors:" << errors.join(", ");
            }

            FillData(DateTime(), paramCount, values[0], values[1], values[2], values[3]);
        }

        else if (packetId == 0x02) {
            if (index + 1 > dataSize) {
                qDebug() << "Incomplete pin header";
                break;
            }

            uint8_t pinCount = ptr[index++];
            if (index + pinCount > dataSize) {
                qDebug() << "Incomplete pin data";
                break;
            }

            QVector<int> newPins;
            for (int i = 0; i < pinCount; i++) {
                newPins.append(ptr[index++]);
            }

            {
                QMutexLocker locker(&bufferMutex);
                allPins = newPins;
            }
            if (deb)
                qDebug() << "Updated pin configuration:" << newPins;
        }

        else if (packetId == 0x03) {
            if (index + 1 > dataSize) {
                qDebug() << "Incomplete error packet";
                break;
            }

            uint8_t errorCode = ptr[index++];
            switch(errorCode) {
            case 0x01: qDebug() << "DHT Sensor Failure"; break;
            case 0x02: qDebug() << "Hall Sensor Failure"; break;
            case 0x03: qDebug() << "Analog Sensor Failure"; break;
            default: qDebug() << "Unknown error code:" << errorCode;
            }
        }

        else if (packetId == 0x04) {
            if (index + 1 > dataSize) {
                qDebug() << "[STATUS] Incomplete header";
                break;
            }

            uint8_t analogPinCount = ptr[index++];

            if (index + analogPinCount + 2 > dataSize) {
                qDebug() << "[STATUS] Data corruption. Expected pins:"
                         << analogPinCount << "| Buffer size:" << (dataSize - index);
                break;
            }

            QStringList analogPins;
            for (int i = 0; i < analogPinCount; i++) {
                int pin = static_cast<uint8_t>(ptr[index++]);
                analogPins.append(pinToString(pin));
            }

            int dhtPin = static_cast<uint8_t>(ptr[index++]);
            int servoPin = static_cast<uint8_t>(ptr[index++]);

            if (deb) {
                qDebug().nospace()
                    << "[STATUS] Analog(" << analogPinCount << "): "
                    << analogPins.join(", ")
                    << " | DHT: " << pinToString(dhtPin)
                    << " | Servo: " << pinToString(servoPin);
            }
        }

        else {
            qDebug() << "Unknown packet type:" << packetId;
        }
    }
}

UArduinoConnect:: UArduinoConnect(string &PortName)
    : SerialPort(nullptr), com(""), deb("") {
    InitSerialPort(PortName);
}

UArduinoConnect::~UArduinoConnect() {

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
    if (!SerialPort || !SerialPort->isOpen()) {
        qDebug() << "CheckWrite: Port is not open.";
        return;
    }

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
    CheckWrite();
}
}

#endif
