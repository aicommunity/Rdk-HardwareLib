#ifndef HARDWAREARDUINOBOARDPANELWIDGET_H
#define HARDWAREARDUINOBOARDPANELWIDGET_H

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QSpinBox>
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

private:
    UComponentGuiContext Context;
    QComboBox* PortCombo = nullptr;
    QComboBox* BoardProfileCombo = nullptr;
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
};

#endif
