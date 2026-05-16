#ifndef HARDWAREARDUINOBOARDCONTROLLERWIDGET_H
#define HARDWAREARDUINOBOARDCONTROLLERWIDGET_H

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QSpinBox>

#include "../../../../Rdk/GUI/Qt/IComponentControllerWidget.h"
#include "../../../../Rdk/GUI/Qt/UComponentGuiContext.h"
#include <functional>
#include "../../../../Rdk/GUI/Qt/UVisualControllerWidget.h"
#include "widgets/UArduinoBoardDiagramWidget.h"

class HardwareArduinoBoardControllerWidget : public UVisualControllerWidget, public IComponentControllerWidget {
    Q_OBJECT
public:
    explicit HardwareArduinoBoardControllerWidget(QWidget* parent = nullptr, RDK::UApplication* app = nullptr);

    void setComponentContext(const UComponentGuiContext& context) override;
    void refreshFromModel(bool force) override;
    QString componentGuiId() const override;

private slots:
    void onRefreshPorts();
    void onApply();
    void onReset();
    void onCalculate();
    void onConnect();
    void onDisconnect();
    void onHealthCheck();
    void onUpload();
    void onBrowseHex();

private:
    void applyToModel();
    void updateDiagram();

    UComponentGuiContext m_context;
    UArduinoBoardDiagramWidget* m_diagram = nullptr;
    QComboBox* m_portCombo = nullptr;
    QComboBox* m_boardProfileCombo = nullptr;
    QCheckBox* m_connectOnBuildCheck = nullptr;
    QCheckBox* m_autoReconnectCheck = nullptr;
    QCheckBox* m_heartbeatEnabledCheck = nullptr;
    QSpinBox* m_heartbeatIntervalSpin = nullptr;
    QSpinBox* m_heartbeatTimeoutSpin = nullptr;
    QSpinBox* m_baudSpin = nullptr;
    QComboBox* m_bundledFirmwareCombo = nullptr;
    QLineEdit* m_firmwarePathEdit = nullptr;
    QProgressBar* m_uploadProgress = nullptr;
    QPlainTextEdit* m_statusLog = nullptr;
    QPushButton* m_applyButton = nullptr;
    QPushButton* m_resetButton = nullptr;
    QPushButton* m_calculateButton = nullptr;
};

#endif
