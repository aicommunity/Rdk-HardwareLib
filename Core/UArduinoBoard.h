#ifndef UARDUINOBOARD_H
#define UARDUINOBOARD_H

#include "../../../Rdk/Deploy/Include/rdk.h"

#include <QObject>
#include <QString>

namespace RDK {

class UArduinoSerialSession;
class UArduinoFlasher;

enum UArduinoConnectionState {
    ArduinoDisconnected = 0,
    ArduinoOpening = 1,
    ArduinoConnected = 2,
    ArduinoError = 3
};

class RDK_LIB_TYPE UArduinoBoard : public UNet {
public:
    UProperty<string, UArduinoBoard, ptPubParameter> PortName;
    UProperty<int, UArduinoBoard, ptPubParameter> BaudRate;
    UProperty<int, UArduinoBoard, ptPubParameter> BoardProfile;
    UProperty<bool, UArduinoBoard, ptPubParameter> AutoReconnect;
    UProperty<bool, UArduinoBoard, ptPubParameter> ConnectOnBuild;

    UProperty<int, UArduinoBoard, ptPubState> ConnectionState;
    UProperty<string, UArduinoBoard, ptPubState> LastError;
    UProperty<double, UArduinoBoard, ptPubState> LastActivityMs;

    UProperty<bool, UArduinoBoard, ptPubParameter> HeartbeatEnabled;
    UProperty<int, UArduinoBoard, ptPubParameter> HeartbeatIntervalMs;
    UProperty<int, UArduinoBoard, ptPubParameter> HeartbeatTimeoutMs;
    UProperty<int, UArduinoBoard, ptPubState> MissedHeartbeats;
    UProperty<bool, UArduinoBoard, ptPubState> RequestHealthCheck;

    UProperty<string, UArduinoBoard, ptPubParameter> FirmwarePath;
    UProperty<string, UArduinoBoard, ptPubParameter> BundledFirmwareId;
    UProperty<bool, UArduinoBoard, ptPubState> UploadFirmwareFlag;
    UProperty<int, UArduinoBoard, ptPubState> UploadProgress;
    UProperty<string, UArduinoBoard, ptPubState> UploadLastResult;

    UProperty<bool, UArduinoBoard, ptPubState> ShowDebug;

    UArduinoBoard();
    virtual ~UArduinoBoard();

    virtual UArduinoBoard* New();

protected:
    bool SetPortName(const string& value);

    virtual bool ADefault() override;
    virtual bool ABuild() override;
    virtual bool AReset() override;
    virtual bool ACalculate() override;
    virtual void AInit() override;
    virtual void AUnInit() override;

    virtual void OnBoardCalculate();
    virtual void OnHealthCheck();

    virtual bool EnsureConnected();
    void Disconnect();
    void RunUpload();
    void HeartbeatTick();
    void TouchActivity();
    UArduinoSerialSession* session();
    QString resolveHexPath() const;

    UArduinoSerialSession* Session = nullptr;
    UArduinoFlasher* Flasher = nullptr;
    bool PortChanged = false;
    qint64 LastHeartbeatSentMs = 0;
    int ReconnectBackoffMs = 1000;
    qint64 LastHealthResponseMs = 0;
    qint64 LastReconnectAttemptMs = 0;
};

} // namespace RDK

#endif
