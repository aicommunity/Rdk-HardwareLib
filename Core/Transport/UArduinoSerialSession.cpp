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

bool UArduinoSerialSession::open(const QString& portName, int baudRate)
{
    close();

    const QString devicePath = UArduinoSerialPortUtil::normalizeDevicePath(portName);
    if (devicePath.isEmpty()) {
        emit errorOccurred(QStringLiteral("Port name is empty"));
        return false;
    }

    bool portExists = false;
    for (const QSerialPortInfo& info : QSerialPortInfo::availablePorts()) {
        if (info.systemLocation() == devicePath || info.portName() == portName
            || info.portName() == devicePath) {
            portExists = true;
            break;
        }
    }
    if (!portExists) {
        emit errorOccurred(QStringLiteral("Port not found: %1").arg(devicePath));
        return false;
    }

    m_port = new QSerialPort(devicePath, this);
    m_port->setBaudRate(baudRate);
    m_port->setDataBits(QSerialPort::Data8);
    m_port->setParity(QSerialPort::NoParity);
    m_port->setStopBits(QSerialPort::OneStop);
    m_port->setFlowControl(QSerialPort::NoFlowControl);

    if (!m_port->open(QIODevice::ReadWrite)) {
        const QString err = m_port->errorString();
        delete m_port;
        m_port = nullptr;
        emit errorOccurred(QStringLiteral("Failed to open port: %1").arg(err));
        return false;
    }

    connect(m_port, &QSerialPort::readyRead, this, &UArduinoSerialSession::onReadyRead);
    if (showDebug)
        qDebug() << "UArduinoSerialSession: opened" << portName << "@" << baudRate;
    return true;
}

void UArduinoSerialSession::close()
{
    if (!m_port)
        return;
    if (m_port->isOpen())
        m_port->close();
    m_port->deleteLater();
    m_port = nullptr;
    QMutexLocker locker(&m_mutex);
    m_rxBuffer.clear();
}

bool UArduinoSerialSession::isOpen() const
{
    return m_port && m_port->isOpen();
}

qint64 UArduinoSerialSession::write(const QByteArray& data)
{
    if (!m_port || !m_port->isOpen())
        return -1;
    const qint64 n = m_port->write(data);
    if (n > 0)
        m_port->flush();
    if (showDebug && n > 0)
        qDebug() << "UArduinoSerialSession TX:" << data.trimmed();
    return n;
}

QByteArray UArduinoSerialSession::takeReceivedBytes()
{
    QMutexLocker locker(&m_mutex);
    QByteArray out = m_rxBuffer;
    m_rxBuffer.clear();
    return out;
}

void UArduinoSerialSession::onReadyRead()
{
    if (!m_port)
        return;
    const QByteArray chunk = m_port->readAll();
    if (chunk.isEmpty())
        return;
    {
        QMutexLocker locker(&m_mutex);
        m_rxBuffer.append(chunk);
    }
    if (showDebug)
        qDebug() << "UArduinoSerialSession RX:" << chunk.size() << "bytes";
    emit bytesReceived();
}

} // namespace RDK
