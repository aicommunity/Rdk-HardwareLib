#ifndef UARDUINOCONNECT_H
#define UARDUINOCONNECT_H

#include <QObject>
#include <QThread>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QByteArray>
#include <QProcess>
#include <QTimer>
#include <QString>
#include <QDebug>
#include <QMutex>
#include <QDateTime>
#include <QTimeZone>
#include <QQueue>
#include <thread>
#include <mutex>
#include <string>

#include "../../../Rdk/Deploy/Include/rdk.h"
#include "../../../Rdk/Core/Engine/ModernSmartPointers.h"
#include "../../../Rdk/Core/Engine/ModernContainers.h"
#include "../../../Rdk/Core/System/ModernChrono.h"
#include "../../../Rdk/Core/System/ModernMutex.h"
#include <memory>
#include <memory>
#include <mutex>
#include <chrono>
#include <thread>
#include <atomic>
// #include "UArduinoControl.h"

namespace RDK{

// class UArduinoControl;

struct DataPoint {
    QVector<double> data; // ������: [timestamp, temperature, humidity, mfield]
};

class UArduinoConnect: public QThread
{
    Q_OBJECT

public:
    void InitSerialPort(string &PortName);
    bool UploadArduino(const QString &FileName);

    QSerialPort *SerialPort;
    QString CurrentPortName;
    // QTimer *WriteTimer;
    QMutex bufferMutex;
    QMutex writeMutex;
    QMutex commandMutex;
    QByteArray WriteBuffer;

    UArduinoConnect(string &PortName);
    virtual ~ UArduinoConnect();
    string com;
    bool deb;

signals:
    void UploadFinished(bool success);
    void DataReceived(float temperature, float humidity, double time, float mfield);
    void SerialPortConnected(bool connected);

public:
    void OnSerialPortRead();
    QString pinToString(int pin);
    void CheckWrite();
    void SetCommand(string& cmd);
    void SendData();
    double DateTime();

    QVector<DataPoint> DataBuffer;
    void FillData(double timestamp, uint8_t paramCount, float temperature, float humidity, float mfield, float servo_speed);
    QVector<double> GetAndClearAllData();

    QVector<int> allPins;

    // --------------------------
    // Modern C++20 methods
    // --------------------------
    // Move semantics
    UArduinoConnect(const UArduinoConnect&) = default;
    UArduinoConnect(UArduinoConnect&&) noexcept = default;
    UArduinoConnect& operator=(const UArduinoConnect&) = default;
    UArduinoConnect& operator=(UArduinoConnect&&) noexcept = default;

    // Modern Arduino operations
    void OptimizeArduinoPerformance();
    bool IsArduinoConnectionValid() const;
    void ReserveArduinoMemory(size_t data_points_count);
    TimePoint GetLastArduinoUpdateTime() const;
    void SetLastArduinoUpdateTime(TimePoint time);

    // Modern serial communication operations
    template<typename T>
    void ReserveSerialBuffer(std::vector<T>& buffer, size_t buffer_size);

    template<typename T>
    void MoveSerialBuffer(std::vector<T>& target, std::vector<T>&& source);

    // Advanced Arduino algorithms
    void UpdateArduinoParameters(const std::vector<double>& parameters);
    void CalculateArduinoMetrics(const std::vector<double>& sensor_data);
    void OptimizeArduinoConfiguration(double performance_target);
    void ProcessArduinoDataStream(const std::vector<std::vector<double>>& data_stream);
    // --------------------------
};
}

#endif // UARDUINOCONNECT_H
