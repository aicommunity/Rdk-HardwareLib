#ifndef UARDUINOSERIALSESSION_H
#define UARDUINOSERIALSESSION_H

#include <QByteArray>
#include <QMutex>
#include <QObject>
#include <QString>
#include <QStringList>

class QSerialPort;

namespace RDK {

class UArduinoSerialSession : public QObject {
    Q_OBJECT
public:
    explicit UArduinoSerialSession(QObject* parent = nullptr);
    ~UArduinoSerialSession() override;

    bool open(const QString& portName, int baudRate);
    void close();
    bool isOpen() const;
    qint64 bytesToWrite() const;
    QString lastError() const;
    qint64 write(const QByteArray& data);
    QByteArray takeReceivedBytes();
    static QStringList availablePorts();

    bool showDebug = false;

signals:
    void bytesReceived();
    void errorOccurred(const QString& message);

private slots:
    void onReadyRead();

private:
    bool tryOpenPort(const QString& devicePath, int baudRate);

    QSerialPort* m_port = nullptr;
    QByteArray m_rxBuffer;
    QString m_lastError;
    mutable QMutex m_mutex;
};

} // namespace RDK

#endif
