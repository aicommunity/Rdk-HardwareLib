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
    UComponentGuiContext m_context;
    QComboBox* m_portCombo = nullptr;
    QComboBox* m_boardProfileCombo = nullptr;
    QCheckBox* m_connectOnBuildCheck = nullptr;
    QCheckBox* m_autoReconnectCheck = nullptr;
    QCheckBox* m_heartbeatEnabledCheck = nullptr;
    QCheckBox* m_showDebugCheck = nullptr;
    QSpinBox* m_heartbeatIntervalSpin = nullptr;
    QSpinBox* m_heartbeatTimeoutSpin = nullptr;
    QSpinBox* m_baudSpin = nullptr;
    QComboBox* m_bundledFirmwareCombo = nullptr;
    QLineEdit* m_firmwarePathEdit = nullptr;
    QProgressBar* m_uploadProgress = nullptr;
    QPlainTextEdit* m_statusLog = nullptr;
};

#endif
