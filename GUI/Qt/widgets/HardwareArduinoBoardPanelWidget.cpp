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
    PortCombo = new QComboBox(this);
    auto* refreshPortsBtn = new QPushButton(tr("Refresh"), this);
    connect(refreshPortsBtn, &QPushButton::clicked, this, &HardwareArduinoBoardPanelWidget::onRefreshPorts);

    BoardProfileCombo = new QComboBox(this);
    BoardProfileCombo->addItem(tr("Arduino Uno"), 0);
    BoardProfileCombo->addItem(tr("Arduino Mega 2560"), 1);

    BaudSpin = new QSpinBox(this);
    BaudSpin->setRange(9600, 250000);
    BaudSpin->setValue(57600);

    ConnectOnBuildCheck = new QCheckBox(tr("Connect on build"), this);
    AutoReconnectCheck = new QCheckBox(tr("Auto reconnect"), this);
    HeartbeatEnabledCheck = new QCheckBox(tr("Heartbeat enabled"), this);
    ShowDebugCheck = new QCheckBox(tr("Show debug"), this);
    HeartbeatIntervalSpin = new QSpinBox(this);
    HeartbeatIntervalSpin->setRange(500, 60000);
    HeartbeatIntervalSpin->setSuffix(tr(" ms"));
    HeartbeatIntervalSpin->setValue(3000);
    HeartbeatTimeoutSpin = new QSpinBox(this);
    HeartbeatTimeoutSpin->setRange(1000, 120000);
    HeartbeatTimeoutSpin->setSuffix(tr(" ms"));
    HeartbeatTimeoutSpin->setValue(10000);

    BundledFirmwareCombo = new QComboBox(this);
    BundledFirmwareCombo->addItem(tr("Sensor Lab v1"), QStringLiteral("sensor_lab_v1"));
    BundledFirmwareCombo->addItem(tr("Standard Firmata"), QStringLiteral("standard_firmata"));

    FirmwarePathEdit = new QLineEdit(this);
    auto* browseBtn = new QPushButton(tr("Browse…"), this);
    connect(browseBtn, &QPushButton::clicked, this, &HardwareArduinoBoardPanelWidget::onBrowseHex);

    UploadProgress = new QProgressBar(this);
    StatusLog = HardwareGuiHelpers::createStatusLogWidget(this);

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
    portRow->addWidget(PortCombo, 1);
    portRow->addWidget(refreshPortsBtn);
    form->addRow(tr("Port:"), portRow);
    form->addRow(tr("Board:"), BoardProfileCombo);
    form->addRow(tr("Baud rate:"), BaudSpin);
    form->addRow(QString(), ConnectOnBuildCheck);
    form->addRow(QString(), AutoReconnectCheck);
    form->addRow(QString(), HeartbeatEnabledCheck);
    form->addRow(QString(), ShowDebugCheck);
    form->addRow(tr("Heartbeat interval:"), HeartbeatIntervalSpin);
    form->addRow(tr("Heartbeat timeout:"), HeartbeatTimeoutSpin);
    form->addRow(tr("Bundled firmware:"), BundledFirmwareCombo);
    auto* hexRow = new QHBoxLayout();
    hexRow->addWidget(FirmwarePathEdit, 1);
    hexRow->addWidget(browseBtn);
    form->addRow(tr("HEX path:"), hexRow);
    form->addRow(tr("Upload:"), uploadBtn);
    form->addRow(QString(), UploadProgress);
    form->addRow(tr("Status / log:"), StatusLog);

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
    Context = context;
    refreshFromModel();
}

void HardwareArduinoBoardPanelWidget::refreshFromModel()
{
    if (Context.componentLongName.isEmpty())
        return;

    QSignalBlocker b1(PortCombo);
    QSignalBlocker b2(BoardProfileCombo);
    QSignalBlocker b3(BaudSpin);
    QSignalBlocker b4(BundledFirmwareCombo);
    QSignalBlocker b5(FirmwarePathEdit);
    QSignalBlocker b6(ConnectOnBuildCheck);
    QSignalBlocker b7(AutoReconnectCheck);
    QSignalBlocker b8(HeartbeatEnabledCheck);
    QSignalBlocker b9(HeartbeatIntervalSpin);
    QSignalBlocker b10(HeartbeatTimeoutSpin);
    QSignalBlocker b11(ShowDebugCheck);

    const QString port = HardwareGuiHelpers::getProp(Context, "PortName");
    const QString port_path =
        port.isEmpty() ? port : RDK::UArduinoSerialPortUtil::normalizeDevicePath(port);
    HardwareGuiHelpers::selectSerialPortInCombo(PortCombo, port_path);

    const int profile = HardwareGuiHelpers::getPropInt(Context, "BoardProfile", 0);
    BoardProfileCombo->setCurrentIndex(profile == 1 ? 1 : 0);
    BaudSpin->setValue(HardwareGuiHelpers::getPropInt(Context, "BaudRate", 57600));
    ConnectOnBuildCheck->setChecked(HardwareGuiHelpers::getPropBool(Context, "ConnectOnBuild", true));
    AutoReconnectCheck->setChecked(HardwareGuiHelpers::getPropBool(Context, "AutoReconnect", false));
    HeartbeatEnabledCheck->setChecked(
        HardwareGuiHelpers::getPropBool(Context, "HeartbeatEnabled", true));
    ShowDebugCheck->setChecked(HardwareGuiHelpers::getPropBool(Context, "ShowDebug", false));
    HeartbeatIntervalSpin->setValue(HardwareGuiHelpers::getPropInt(Context, "HeartbeatIntervalMs", 3000));
    HeartbeatTimeoutSpin->setValue(HardwareGuiHelpers::getPropInt(Context, "HeartbeatTimeoutMs", 10000));

    const QString bundled = HardwareGuiHelpers::getProp(Context, "BundledFirmwareId");
    const int bundledIdx = BundledFirmwareCombo->findData(bundled);
    BundledFirmwareCombo->setCurrentIndex(bundledIdx >= 0 ? bundledIdx : 0);
    FirmwarePathEdit->setText(HardwareGuiHelpers::getProp(Context, "FirmwarePath"));
    UploadProgress->setValue(HardwareGuiHelpers::getPropInt(Context, "UploadProgress", 0));

    const bool connected = HardwareGuiHelpers::getPropBool(Context, "IsConnected", false);
    const bool hasError = HardwareGuiHelpers::getPropBool(Context, "HasError", false);
    const QString err = HardwareGuiHelpers::getProp(Context, "LastError");
    const QString uploadRes = HardwareGuiHelpers::getProp(Context, "UploadLastResult");
    HardwareGuiHelpers::setStatusLogText(
        StatusLog,
        tr("Connected: %1\nHas error: %2\nLast error: %3\nUpload: %4")
            .arg(connected ? tr("yes") : tr("no"))
            .arg(hasError ? tr("yes") : tr("no"))
            .arg(err, uploadRes));
}

void HardwareArduinoBoardPanelWidget::applyToModel()
{
    HardwareGuiHelpers::setProp(Context,
                                "PortName",
                                HardwareGuiHelpers::selectedSerialPortPath(PortCombo));
    HardwareGuiHelpers::setProp(Context, "BoardProfile",
                                QString::number(BoardProfileCombo->currentData().toInt()));
    HardwareGuiHelpers::setProp(Context, "BaudRate", QString::number(BaudSpin->value()));
    HardwareGuiHelpers::setProp(Context, "ConnectOnBuild",
                                ConnectOnBuildCheck->isChecked() ? QStringLiteral("1")
                                                                   : QStringLiteral("0"));
    HardwareGuiHelpers::setProp(Context, "AutoReconnect",
                                AutoReconnectCheck->isChecked() ? QStringLiteral("1")
                                                                  : QStringLiteral("0"));
    HardwareGuiHelpers::setProp(Context, "HeartbeatEnabled",
                                HeartbeatEnabledCheck->isChecked() ? QStringLiteral("1")
                                                                     : QStringLiteral("0"));
    HardwareGuiHelpers::setProp(Context, "ShowDebug",
                                ShowDebugCheck->isChecked() ? QStringLiteral("1")
                                                              : QStringLiteral("0"));
    HardwareGuiHelpers::setProp(Context, "HeartbeatIntervalMs",
                                QString::number(HeartbeatIntervalSpin->value()));
    HardwareGuiHelpers::setProp(Context, "HeartbeatTimeoutMs",
                                QString::number(HeartbeatTimeoutSpin->value()));
    HardwareGuiHelpers::setProp(Context, "BundledFirmwareId",
                                BundledFirmwareCombo->currentData().toString());
    HardwareGuiHelpers::setProp(Context, "FirmwarePath", FirmwarePathEdit->text());
}

void HardwareArduinoBoardPanelWidget::onRefreshPorts()
{
    const QString current = HardwareGuiHelpers::selectedSerialPortPath(PortCombo);
    HardwareGuiHelpers::populateSerialPortCombo(PortCombo, current);
}

void HardwareArduinoBoardPanelWidget::onApply()
{
    applyToModel();
    HardwareGuiHelpers::envReset(Context);
    refreshFromModel();
}

void HardwareArduinoBoardPanelWidget::onReset()
{
    HardwareGuiHelpers::envReset(Context);
    refreshFromModel();
}

void HardwareArduinoBoardPanelWidget::onCalculate()
{
    applyToModel();
    HardwareGuiHelpers::envCalculate(Context);
    refreshFromModel();
}

void HardwareArduinoBoardPanelWidget::onConnect()
{
    applyToModel();
    HardwareGuiHelpers::pulseEdge(Context, "Connect");
    refreshFromModel();
}

void HardwareArduinoBoardPanelWidget::onDisconnect()
{
    applyToModel();
    HardwareGuiHelpers::pulseEdge(Context, "Disconnect");
    refreshFromModel();
}

void HardwareArduinoBoardPanelWidget::onReconnect()
{
    applyToModel();
    HardwareGuiHelpers::pulseEdge(Context, "Reconnect");
    refreshFromModel();
}

void HardwareArduinoBoardPanelWidget::onHealthCheck()
{
    applyToModel();
    HardwareGuiHelpers::pulseEdge(Context, "RequestHealthCheck");
    refreshFromModel();
}

void HardwareArduinoBoardPanelWidget::onUpload()
{
    applyToModel();
    HardwareGuiHelpers::pulseEdge(Context, "UploadFirmware");
    refreshFromModel();
}

void HardwareArduinoBoardPanelWidget::onBrowseHex()
{
    const QString path = QFileDialog::getOpenFileName(this, tr("Select HEX file"), QString(),
                                                      tr("Intel HEX (*.hex);;All (*)"));
    if (!path.isEmpty())
        FirmwarePathEdit->setText(path);
}
