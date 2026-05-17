#include "HardwareArduinoBoardPanelWidget.h"

#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QSignalBlocker>
#include <QVBoxLayout>

#include "HardwareGuiHelpers.h"

#include "../../../Core/Transport/UArduinoSerialPortUtil.h"

HardwareArduinoBoardPanelWidget::HardwareArduinoBoardPanelWidget(QWidget* parent)
    : QWidget(parent)
{
    m_portCombo = new QComboBox(this);
    auto* refreshPortsBtn = new QPushButton(tr("Refresh"), this);
    connect(refreshPortsBtn, &QPushButton::clicked, this, &HardwareArduinoBoardPanelWidget::onRefreshPorts);

    m_boardProfileCombo = new QComboBox(this);
    m_boardProfileCombo->addItem(tr("Arduino Uno"), 0);
    m_boardProfileCombo->addItem(tr("Arduino Mega 2560"), 1);

    m_baudSpin = new QSpinBox(this);
    m_baudSpin->setRange(9600, 250000);
    m_baudSpin->setValue(57600);

    m_connectOnBuildCheck = new QCheckBox(tr("Connect on build"), this);
    m_autoReconnectCheck = new QCheckBox(tr("Auto reconnect"), this);
    m_heartbeatEnabledCheck = new QCheckBox(tr("Heartbeat enabled"), this);
    m_showDebugCheck = new QCheckBox(tr("Show debug"), this);
    m_heartbeatIntervalSpin = new QSpinBox(this);
    m_heartbeatIntervalSpin->setRange(500, 60000);
    m_heartbeatIntervalSpin->setSuffix(tr(" ms"));
    m_heartbeatIntervalSpin->setValue(3000);
    m_heartbeatTimeoutSpin = new QSpinBox(this);
    m_heartbeatTimeoutSpin->setRange(1000, 120000);
    m_heartbeatTimeoutSpin->setSuffix(tr(" ms"));
    m_heartbeatTimeoutSpin->setValue(10000);

    m_bundledFirmwareCombo = new QComboBox(this);
    m_bundledFirmwareCombo->addItem(tr("Sensor Lab v1"), QStringLiteral("sensor_lab_v1"));
    m_bundledFirmwareCombo->addItem(tr("Standard Firmata"), QStringLiteral("standard_firmata"));

    m_firmwarePathEdit = new QLineEdit(this);
    auto* browseBtn = new QPushButton(tr("Browse…"), this);
    connect(browseBtn, &QPushButton::clicked, this, &HardwareArduinoBoardPanelWidget::onBrowseHex);

    m_uploadProgress = new QProgressBar(this);
    m_statusLog = HardwareGuiHelpers::createStatusLogWidget(this);

    auto* connectBtn = new QPushButton(tr("Connect"), this);
    auto* disconnectBtn = new QPushButton(tr("Disconnect"), this);
    auto* reconnectBtn = new QPushButton(tr("Reconnect"), this);
    auto* healthBtn = new QPushButton(tr("Health check"), this);
    auto* uploadBtn = new QPushButton(tr("Upload firmware"), this);
    connect(connectBtn, &QPushButton::clicked, this, &HardwareArduinoBoardPanelWidget::onConnect);
    connect(disconnectBtn, &QPushButton::clicked, this, &HardwareArduinoBoardPanelWidget::onDisconnect);
    connect(reconnectBtn, &QPushButton::clicked, this, &HardwareArduinoBoardPanelWidget::onReconnect);
    connect(healthBtn, &QPushButton::clicked, this, &HardwareArduinoBoardPanelWidget::onHealthCheck);
    connect(uploadBtn, &QPushButton::clicked, this, &HardwareArduinoBoardPanelWidget::onUpload);

    auto* applyBtn = new QPushButton(tr("Apply parameters"), this);
    auto* resetBtn = new QPushButton(tr("Reset component"), this);
    auto* calcBtn = new QPushButton(tr("Calculate component"), this);
    connect(applyBtn, &QPushButton::clicked, this, &HardwareArduinoBoardPanelWidget::onApply);
    connect(resetBtn, &QPushButton::clicked, this, &HardwareArduinoBoardPanelWidget::onReset);
    connect(calcBtn, &QPushButton::clicked, this, &HardwareArduinoBoardPanelWidget::onCalculate);

    auto* form = new QFormLayout();
    auto* portRow = new QHBoxLayout();
    portRow->addWidget(m_portCombo, 1);
    portRow->addWidget(refreshPortsBtn);
    form->addRow(tr("Port:"), portRow);
    form->addRow(tr("Board:"), m_boardProfileCombo);
    form->addRow(tr("Baud rate:"), m_baudSpin);
    form->addRow(QString(), m_connectOnBuildCheck);
    form->addRow(QString(), m_autoReconnectCheck);
    form->addRow(QString(), m_heartbeatEnabledCheck);
    form->addRow(QString(), m_showDebugCheck);
    form->addRow(tr("Heartbeat interval:"), m_heartbeatIntervalSpin);
    form->addRow(tr("Heartbeat timeout:"), m_heartbeatTimeoutSpin);
    form->addRow(tr("Bundled firmware:"), m_bundledFirmwareCombo);
    auto* hexRow = new QHBoxLayout();
    hexRow->addWidget(m_firmwarePathEdit, 1);
    hexRow->addWidget(browseBtn);
    form->addRow(tr("HEX path:"), hexRow);
    form->addRow(tr("Upload:"), uploadBtn);
    form->addRow(QString(), m_uploadProgress);
    form->addRow(tr("Status / log:"), m_statusLog);

    auto* connRow = new QHBoxLayout();
    connRow->addWidget(connectBtn);
    connRow->addWidget(disconnectBtn);
    connRow->addWidget(reconnectBtn);
    connRow->addWidget(healthBtn);
    form->addRow(tr("Connection:"), connRow);

    auto* btnRow = new QHBoxLayout();
    btnRow->addWidget(applyBtn);
    btnRow->addWidget(resetBtn);
    btnRow->addWidget(calcBtn);
    form->addRow(QString(), btnRow);

    auto* inner = new QWidget(this);
    inner->setLayout(form);
    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setWidget(inner);
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->addWidget(scroll);

    onRefreshPorts();
}

void HardwareArduinoBoardPanelWidget::setContext(const UComponentGuiContext& context)
{
    m_context = context;
    refreshFromModel();
}

void HardwareArduinoBoardPanelWidget::refreshFromModel()
{
    if (m_context.componentLongName.isEmpty())
        return;

    QSignalBlocker b1(m_portCombo);
    QSignalBlocker b2(m_boardProfileCombo);
    QSignalBlocker b3(m_baudSpin);
    QSignalBlocker b4(m_bundledFirmwareCombo);
    QSignalBlocker b5(m_firmwarePathEdit);
    QSignalBlocker b6(m_connectOnBuildCheck);
    QSignalBlocker b7(m_autoReconnectCheck);
    QSignalBlocker b8(m_heartbeatEnabledCheck);
    QSignalBlocker b9(m_heartbeatIntervalSpin);
    QSignalBlocker b10(m_heartbeatTimeoutSpin);
    QSignalBlocker b11(m_showDebugCheck);

    const QString port = HardwareGuiHelpers::getProp(m_context, "PortName");
    const QString portPath =
        port.isEmpty() ? port : RDK::UArduinoSerialPortUtil::normalizeDevicePath(port);
    HardwareGuiHelpers::selectSerialPortInCombo(m_portCombo, portPath);

    const int profile = HardwareGuiHelpers::getPropInt(m_context, "BoardProfile", 0);
    m_boardProfileCombo->setCurrentIndex(profile == 1 ? 1 : 0);
    m_baudSpin->setValue(HardwareGuiHelpers::getPropInt(m_context, "BaudRate", 57600));
    m_connectOnBuildCheck->setChecked(HardwareGuiHelpers::getPropBool(m_context, "ConnectOnBuild", true));
    m_autoReconnectCheck->setChecked(HardwareGuiHelpers::getPropBool(m_context, "AutoReconnect", false));
    m_heartbeatEnabledCheck->setChecked(
        HardwareGuiHelpers::getPropBool(m_context, "HeartbeatEnabled", true));
    m_showDebugCheck->setChecked(HardwareGuiHelpers::getPropBool(m_context, "ShowDebug", false));
    m_heartbeatIntervalSpin->setValue(HardwareGuiHelpers::getPropInt(m_context, "HeartbeatIntervalMs", 3000));
    m_heartbeatTimeoutSpin->setValue(HardwareGuiHelpers::getPropInt(m_context, "HeartbeatTimeoutMs", 10000));

    const QString bundled = HardwareGuiHelpers::getProp(m_context, "BundledFirmwareId");
    const int bundledIdx = m_bundledFirmwareCombo->findData(bundled);
    m_bundledFirmwareCombo->setCurrentIndex(bundledIdx >= 0 ? bundledIdx : 0);
    m_firmwarePathEdit->setText(HardwareGuiHelpers::getProp(m_context, "FirmwarePath"));
    m_uploadProgress->setValue(HardwareGuiHelpers::getPropInt(m_context, "UploadProgress", 0));

    const bool connected = HardwareGuiHelpers::getPropBool(m_context, "IsConnected", false);
    const bool hasError = HardwareGuiHelpers::getPropBool(m_context, "HasError", false);
    const QString err = HardwareGuiHelpers::getProp(m_context, "LastError");
    const QString uploadRes = HardwareGuiHelpers::getProp(m_context, "UploadLastResult");
    HardwareGuiHelpers::setStatusLogText(
        m_statusLog,
        tr("Connected: %1\nHas error: %2\nLast error: %3\nUpload: %4")
            .arg(connected ? tr("yes") : tr("no"))
            .arg(hasError ? tr("yes") : tr("no"))
            .arg(err, uploadRes));
}

void HardwareArduinoBoardPanelWidget::applyToModel()
{
    HardwareGuiHelpers::setProp(m_context,
                                "PortName",
                                HardwareGuiHelpers::selectedSerialPortPath(m_portCombo));
    HardwareGuiHelpers::setProp(m_context, "BoardProfile",
                                QString::number(m_boardProfileCombo->currentData().toInt()));
    HardwareGuiHelpers::setProp(m_context, "BaudRate", QString::number(m_baudSpin->value()));
    HardwareGuiHelpers::setProp(m_context, "ConnectOnBuild",
                                m_connectOnBuildCheck->isChecked() ? QStringLiteral("1")
                                                                   : QStringLiteral("0"));
    HardwareGuiHelpers::setProp(m_context, "AutoReconnect",
                                m_autoReconnectCheck->isChecked() ? QStringLiteral("1")
                                                                  : QStringLiteral("0"));
    HardwareGuiHelpers::setProp(m_context, "HeartbeatEnabled",
                                m_heartbeatEnabledCheck->isChecked() ? QStringLiteral("1")
                                                                     : QStringLiteral("0"));
    HardwareGuiHelpers::setProp(m_context, "ShowDebug",
                                m_showDebugCheck->isChecked() ? QStringLiteral("1")
                                                              : QStringLiteral("0"));
    HardwareGuiHelpers::setProp(m_context, "HeartbeatIntervalMs",
                                QString::number(m_heartbeatIntervalSpin->value()));
    HardwareGuiHelpers::setProp(m_context, "HeartbeatTimeoutMs",
                                QString::number(m_heartbeatTimeoutSpin->value()));
    HardwareGuiHelpers::setProp(m_context, "BundledFirmwareId",
                                m_bundledFirmwareCombo->currentData().toString());
    HardwareGuiHelpers::setProp(m_context, "FirmwarePath", m_firmwarePathEdit->text());
}

void HardwareArduinoBoardPanelWidget::onRefreshPorts()
{
    const QString current = HardwareGuiHelpers::selectedSerialPortPath(m_portCombo);
    HardwareGuiHelpers::populateSerialPortCombo(m_portCombo, current);
}

void HardwareArduinoBoardPanelWidget::onApply()
{
    applyToModel();
    HardwareGuiHelpers::envReset(m_context);
    refreshFromModel();
}

void HardwareArduinoBoardPanelWidget::onReset()
{
    HardwareGuiHelpers::envReset(m_context);
    refreshFromModel();
}

void HardwareArduinoBoardPanelWidget::onCalculate()
{
    applyToModel();
    HardwareGuiHelpers::envCalculate(m_context);
    refreshFromModel();
}

void HardwareArduinoBoardPanelWidget::onConnect()
{
    applyToModel();
    HardwareGuiHelpers::pulseEdge(m_context, "Connect");
    refreshFromModel();
}

void HardwareArduinoBoardPanelWidget::onDisconnect()
{
    applyToModel();
    HardwareGuiHelpers::pulseEdge(m_context, "Disconnect");
    refreshFromModel();
}

void HardwareArduinoBoardPanelWidget::onReconnect()
{
    applyToModel();
    HardwareGuiHelpers::pulseEdge(m_context, "Reconnect");
    refreshFromModel();
}

void HardwareArduinoBoardPanelWidget::onHealthCheck()
{
    applyToModel();
    HardwareGuiHelpers::pulseEdge(m_context, "RequestHealthCheck");
    refreshFromModel();
}

void HardwareArduinoBoardPanelWidget::onUpload()
{
    applyToModel();
    HardwareGuiHelpers::pulseEdge(m_context, "UploadFirmware");
    refreshFromModel();
}

void HardwareArduinoBoardPanelWidget::onBrowseHex()
{
    const QString path = QFileDialog::getOpenFileName(this, tr("Select HEX file"), QString(),
                                                      tr("Intel HEX (*.hex);;All (*)"));
    if (!path.isEmpty())
        m_firmwarePathEdit->setText(path);
}
