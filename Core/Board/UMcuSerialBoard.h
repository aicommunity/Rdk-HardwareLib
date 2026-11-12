#ifndef UMCUSERIALBOARD_H
#define UMCUSERIALBOARD_H

#include "rdk.h"

#include <memory>

#include <QString>

class QThread;

namespace RDK {

class UArduinoSerialSession;
struct UArduinoUploadJobState;

enum UArduinoConnectionState {
    ArduinoDisconnected = 0,
    ArduinoOpening = 1,
    ArduinoConnected = 2,
    ArduinoError = 3
};

/** Shared serial MCU board: port/baud, connect lifecycle, upload job shell, heartbeat. */
class RDK_LIB_TYPE UMcuSerialBoard : public UNet {
public:
    UProperty<string, UMcuSerialBoard, ptPubParameter> PortName;
    UProperty<int, UMcuSerialBoard, ptPubParameter> BaudRate;
    UProperty<bool, UMcuSerialBoard, ptPubParameter> AutoReconnect;
    UProperty<bool, UMcuSerialBoard, ptPubParameter> ConnectOnBuild;

    UProperty<bool, UMcuSerialBoard, ptPubParameter | ptInput> Connect;
    UProperty<bool, UMcuSerialBoard, ptPubParameter | ptInput> Disconnect;
    UProperty<bool, UMcuSerialBoard, ptPubParameter | ptInput> Reconnect;
    UProperty<bool, UMcuSerialBoard, ptPubParameter | ptInput> UploadFirmware;
    UProperty<bool, UMcuSerialBoard, ptPubParameter | ptInput> CancelUpload;
    UProperty<bool, UMcuSerialBoard, ptPubParameter | ptInput> ClearLastError;

    UProperty<int, UMcuSerialBoard, ptPubState> ConnectionState;
    UProperty<string, UMcuSerialBoard, ptPubState> LastError;
    UProperty<double, UMcuSerialBoard, ptPubState> LastActivityMs;

    UProperty<bool, UMcuSerialBoard, ptPubState> IsConnected;
    UProperty<bool, UMcuSerialBoard, ptPubState> IsOpening;
    UProperty<bool, UMcuSerialBoard, ptPubState> HasError;
    UProperty<bool, UMcuSerialBoard, ptPubState> IsDisconnected;
    UProperty<bool, UMcuSerialBoard, ptPubState> IsUploading;
    UProperty<bool, UMcuSerialBoard, ptPubState> UploadComplete;

    UProperty<bool, UMcuSerialBoard, ptPubParameter> HeartbeatEnabled;
    UProperty<int, UMcuSerialBoard, ptPubParameter> HeartbeatIntervalMs;
    UProperty<int, UMcuSerialBoard, ptPubParameter> HeartbeatTimeoutMs;
    UProperty<int, UMcuSerialBoard, ptPubState> MissedHeartbeats;
    UProperty<bool, UMcuSerialBoard, ptPubState> RequestHealthCheck;

    UProperty<string, UMcuSerialBoard, ptPubParameter> FirmwarePath;
    UProperty<string, UMcuSerialBoard, ptPubParameter> BundledFirmwareId;
    UProperty<bool, UMcuSerialBoard, ptPubState> UploadFirmwareFlag;
    UProperty<int, UMcuSerialBoard, ptPubState> UploadProgress;
    UProperty<string, UMcuSerialBoard, ptPubState> UploadLastResult;

    UProperty<string, UMcuSerialBoard, ptPubParameter> HardwareSetupPath;
    UProperty<string, UMcuSerialBoard, ptPubParameter> HardwareSetupJson;
    UProperty<bool, UMcuSerialBoard, ptPubState> HardwareSetupValid;
    UProperty<string, UMcuSerialBoard, ptPubState> HardwareSetupIssues;

    UProperty<bool, UMcuSerialBoard, ptPubState> ShowDebug;

    UMcuSerialBoard();
    virtual ~UMcuSerialBoard();

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

    virtual void ConfigureSerialSession(UArduinoSerialSession& session);
    virtual QString ResolveFirmwarePath() const;
    virtual int DefaultBaudRate() const;
    virtual void RunUploadBlocking();
    virtual void startUploadAsync();

    virtual bool EnsureConnected();
    virtual void CloseConnection();
    void RunUpload();
    virtual void requestCancelUpload();
    void PollUploadJob();
    void finishUploadThread();
    void HeartbeatTick();
    void TouchActivity();
    UArduinoSerialSession* session();

    void SyncDerivedStates();
    void ProcessBoardEdges();
    void RefreshHardwareSetup();
    static void ResetEdge(bool& flag);
    template<typename OwnerT, unsigned int PropType>
    static void ResetEdge(UProperty<bool, OwnerT, PropType>& edge)
    {
        if (edge)
            edge = false;
    }

    std::unique_ptr<UArduinoUploadJobState> UploadJob;
    QThread* UploadThread = nullptr;

    UArduinoSerialSession* Session = nullptr;
    bool PortChanged = false;
    qint64 LastHeartbeatSentMs = 0;
    int ReconnectBackoffMs = 1000;
    qint64 LastHealthResponseMs = 0;
    qint64 LastReconnectAttemptMs = 0;
};

} // namespace RDK

#endif
