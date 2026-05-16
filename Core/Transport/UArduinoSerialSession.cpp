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
    return m_lastError;
}

bool UArduinoSerialSession::tryOpenPort(const QString& devicePath, int baudRate)
{
    if (devicePath.isEmpty())
        return false;

    m_port = new QSerialPort(devicePath, this);
    m_port->setBaudRate(baudRate);
    m_port->setDataBits(QSerialPort::Data8);
    m_port->setParity(QSerialPort::NoParity);
    m_port->setStopBits(QSerialPort::OneStop);
    m_port->setFlowControl(QSerialPort::NoFlowControl);

    if (m_port->open(QIODevice::ReadWrite)) {
        connect(m_port, &QSerialPort::readyRead, this, &UArduinoSerialSession::onReadyRead);
        return true;
    }

    QString err = QStringLiteral("Failed to open %1: %2").arg(devicePath, m_port->errorString());
    if (m_port->error() == QSerialPort::PermissionError) {
        err += QStringLiteral(
            " — check dialout group (sudo usermod -aG dialout $USER) or close Arduino IDE / other apps using the port");
    } else if (m_port->error() == QSerialPort::ResourceError) {
        err += QStringLiteral(" — port may be busy or disconnected");
    }

    m_lastError = err;
    delete m_port;
    m_port = nullptr;
    return false;
}

bool UArduinoSerialSession::open(const QString& portName, int baudRate)
{
    close();
    m_lastError.clear();

    const QString devicePath = UArduinoSerialPortUtil::normalizeDevicePath(portName);
    if (devicePath.isEmpty()) {
        m_lastError = QStringLiteral("Port name is empty");
        emit errorOccurred(m_lastError);
        return false;
    }

    QSerialPortInfo info;
    if (!UArduinoSerialPortUtil::portInfoForPath(portName, &info)) {
        m_lastError = QStringLiteral("Port not found: %1").arg(devicePath);
        emit errorOccurred(m_lastError);
        return false;
    }

    QStringList candidates;
    const QString preferred = UArduinoSerialPortUtil::preferredOpenName(portName);
    if (!preferred.isEmpty())
        candidates.append(preferred);
    if (!info.systemLocation().isEmpty() && !candidates.contains(info.systemLocation()))
        candidates.append(info.systemLocation());
    const QString normalizedName = UArduinoSerialPortUtil::normalizeDevicePath(info.portName());
    if (!normalizedName.isEmpty() && !candidates.contains(normalizedName))
        candidates.append(normalizedName);
    if (!candidates.contains(devicePath))
        candidates.append(devicePath);

    for (const QString& candidate : candidates) {
        if (tryOpenPort(candidate, baudRate)) {
            m_lastError.clear();
            if (showDebug)
                qDebug() << "UArduinoSerialSession: opened" << candidate << "@" << baudRate;
            return true;
        }
        emit errorOccurred(m_lastError);
    }

    return false;
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

qint64 UArduinoSerialSession::bytesToWrite() const
{
    return m_port && m_port->isOpen() ? m_port->bytesToWrite() : 0;
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
