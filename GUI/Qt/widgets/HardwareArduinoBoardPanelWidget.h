#ifndef HARDWAREARDUINOBOARDPANELWIDGET_H
#define HARDWAREARDUINOBOARDPANELWIDGET_H

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QSpinBox>
#include <QTimer>
#include <QWidget>

#include "../../../../../Rdk/GUI/Qt/UComponentGuiContext.h"

class HardwareArduinoBoardPanelWidget : public QWidget {
    Q_OBJECT
public:
    explicit HardwareArduinoBoardPanelWidget(QWidget* parent = nullptr);

    void setContext(const UComponentGuiContext& context);
    void refreshFromModel();
    void applyToModel();

private slots:
    void onRefreshPorts();
    void onApply();
    void onReset();
    void onCalculate();
    void onConnect();
    void onDisconnect();
    void onReconnect();
    void onHealthCheck();
    void onUpload();
    void onBrowseHex();
    void onPortOrProfileChanged();
    void onUploadPollTick();

private:
    void updateUploadPreview();
    void updateUploadControlsEnabled();
    void startUploadUiPoll();
    void stopUploadUiPoll();

    UComponentGuiContext Context;
    QComboBox* PortCombo = nullptr;
    QComboBox* BoardProfileCombo = nullptr;
    QCheckBox* AutoDetectBoardCheck = nullptr;
    QLabel* UploadPreviewLabel = nullptr;
    QCheckBox* ConnectOnBuildCheck = nullptr;
    QCheckBox* AutoReconnectCheck = nullptr;
    QCheckBox* HeartbeatEnabledCheck = nullptr;
    QCheckBox* ShowDebugCheck = nullptr;
    QSpinBox* HeartbeatIntervalSpin = nullptr;
    QSpinBox* HeartbeatTimeoutSpin = nullptr;
    QSpinBox* BaudSpin = nullptr;
    QComboBox* BundledFirmwareCombo = nullptr;
    QLineEdit* FirmwarePathEdit = nullptr;
    QProgressBar* UploadProgress = nullptr;
    QPlainTextEdit* StatusLog = nullptr;
    QPushButton* UploadBtn = nullptr;
    QPushButton* ConnectBtn = nullptr;
    QPushButton* DisconnectBtn = nullptr;
    QPushButton* ReconnectBtn = nullptr;
    QPushButton* HealthBtn = nullptr;
    QTimer* UploadPollTimer = nullptr;
};

#endif
