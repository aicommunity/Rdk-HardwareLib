#include "UArduinoSerialSession.h"

#include "UArduinoSerialPortUtil.h"

#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

namespace RDK {

UArduinoSerialSession::UArduinoSerialSession(QObject* parent)
    : QObject(parent)
{
}

UArduinoSerialSession::~UArduinoSerialSession()
{
    close();
}

QStringList UArduinoSerialSession::availablePorts()
{
    return UArduinoSerialPortUtil::listAvailableDevicePaths();
}

QString UArduinoSerialSession::lastError() const
{
    return LastErrorText;
}

bool UArduinoSerialSession::tryOpenPort(const QString& device_path, int baud_rate)
{
    if (device_path.isEmpty())
        return false;

    SerialPort = new QSerialPort(device_path, this);
    SerialPort->setBaudRate(baud_rate);
    SerialPort->setDataBits(QSerialPort::Data8);
    SerialPort->setParity(QSerialPort::NoParity);
    SerialPort->setStopBits(QSerialPort::OneStop);
    SerialPort->setFlowControl(QSerialPort::NoFlowControl);

    if (SerialPort->open(QIODevice::ReadWrite)) {
        connect(SerialPort, &QSerialPort::readyRead, this, &UArduinoSerialSession::onReadyRead);
        return true;
    }

    QString err = QStringLiteral("Failed to open %1: %2").arg(device_path, SerialPort->errorString());
    if (SerialPort->error() == QSerialPort::PermissionError) {
        err += QStringLiteral(
            " — check dialout group (sudo usermod -aG dialout $USER) or close Arduino IDE / other apps using the port");
    } else if (SerialPort->error() == QSerialPort::ResourceError) {
        err += QStringLiteral(" — port may be busy or disconnected");
    }

    LastErrorText = err;
    delete SerialPort;
    SerialPort = nullptr;
    return false;
}

bool UArduinoSerialSession::open(const QString& port_name, int baud_rate)
{
    close();
    LastErrorText.clear();

    const QString device_path = UArduinoSerialPortUtil::normalizeDevicePath(port_name);
    if (device_path.isEmpty()) {
        LastErrorText = QStringLiteral("Port name is empty");
        emit errorOccurred(LastErrorText);
        return false;
    }

    QSerialPortInfo info;
    if (!UArduinoSerialPortUtil::portInfoForPath(port_name, &info)) {
        LastErrorText = QStringLiteral("Port not found: %1").arg(device_path);
        emit errorOccurred(LastErrorText);
        return false;
    }

    QStringList candidates;
    const QString preferred = UArduinoSerialPortUtil::preferredOpenName(port_name);
    if (!preferred.isEmpty())
        candidates.append(preferred);
    if (!info.systemLocation().isEmpty() && !candidates.contains(info.systemLocation()))
        candidates.append(info.systemLocation());
    const QString normalized_name = UArduinoSerialPortUtil::normalizeDevicePath(info.portName());
    if (!normalized_name.isEmpty() && !candidates.contains(normalized_name))
        candidates.append(normalized_name);
    if (!candidates.contains(device_path))
        candidates.append(device_path);

    for (const QString& candidate : candidates) {
        if (tryOpenPort(candidate, baud_rate)) {
            LastErrorText.clear();
            if (ShowDebug)
                qDebug() << "UArduinoSerialSession: opened" << candidate << "@" << baud_rate;
            return true;
        }
        emit errorOccurred(LastErrorText);
    }

    return false;
}

void UArduinoSerialSession::close()
{
    if (!SerialPort)
        return;
    if (SerialPort->isOpen())
        SerialPort->close();
    SerialPort->deleteLater();
    SerialPort = nullptr;
    QMutexLocker locker(&RxMutex);
    RxBuffer.clear();
}

bool UArduinoSerialSession::isOpen() const
{
    return SerialPort && SerialPort->isOpen();
}

qint64 UArduinoSerialSession::bytesToWrite() const
{
    return SerialPort && SerialPort->isOpen() ? SerialPort->bytesToWrite() : 0;
}

qint64 UArduinoSerialSession::write(const QByteArray& data)
{
    if (!SerialPort || !SerialPort->isOpen())
        return -1;
    const qint64 n = SerialPort->write(data);
    if (n > 0)
        SerialPort->flush();
    if (ShowDebug && n > 0)
        qDebug() << "UArduinoSerialSession TX:" << data.trimmed();
    return n;
}

QByteArray UArduinoSerialSession::takeReceivedBytes()
{
    QMutexLocker locker(&RxMutex);
    QByteArray out = RxBuffer;
    RxBuffer.clear();
    return out;
}

void UArduinoSerialSession::onReadyRead()
{
    if (!SerialPort)
        return;
    const QByteArray chunk = SerialPort->readAll();
    if (chunk.isEmpty())
        return;
    {
        QMutexLocker locker(&RxMutex);
        RxBuffer.append(chunk);
    }
    if (ShowDebug)
        qDebug() << "UArduinoSerialSession RX:" << chunk.size() << "bytes";
    emit bytesReceived();
}

} // namespace RDK
