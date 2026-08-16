#include "HardwareArduinoBoardPanelWidget.h"

#include <QFileDialog>

#include <QFormLayout>

#include <QHBoxLayout>

#include <QScrollArea>

#include <QSignalBlocker>

#include <QVBoxLayout>

#include "HardwareGuiHelpers.h"

#include "../../../Core/Catalog/UHardwareCatalog.h"

#include "../../../Core/Transport/UArduinoBoardProfile.h"

#include "../../../Core/Transport/UArduinoSerialPortUtil.h"

#include "../../../Core/UFirmwareManifest.h"

HardwareArduinoBoardPanelWidget::HardwareArduinoBoardPanelWidget(QWidget* parent)

    : QWidget(parent)

{

    PortCombo = new QComboBox(this);

    HardwareGuiHelpers::applyUnicodeFriendlyFont(PortCombo);

    auto* refreshPortsBtn = new QPushButton(tr("Refresh"), this);

    connect(refreshPortsBtn, &QPushButton::clicked, this, &HardwareArduinoBoardPanelWidget::onRefreshPorts);

    BoardProfileCombo = new QComboBox(this);

    BoardProfileCombo->addItem(tr("Arduino Uno"), 0);

    BoardProfileCombo->addItem(tr("Arduino Mega 2560"), 1);

    AutoDetectBoardCheck = new QCheckBox(tr("Auto-detect board when port changes"), this);

    AutoDetectBoardCheck->setChecked(true);

    UploadPreviewLabel = new QLabel(this);

    UploadPreviewLabel->setWordWrap(true);

    UploadPreviewLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

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
    populateBundledFirmwareCombo();

    FirmwarePathEdit = new QLineEdit(this);

    auto* browseBtn = new QPushButton(tr("Browse\u2026"), this);

    connect(browseBtn, &QPushButton::clicked, this, &HardwareArduinoBoardPanelWidget::onBrowseHex);

    UploadProgress = new QProgressBar(this);

    StatusLog = HardwareGuiHelpers::createStatusLogWidget(this);

    ConnectBtn = new QPushButton(tr("Connect"), this);

    DisconnectBtn = new QPushButton(tr("Disconnect"), this);

    ReconnectBtn = new QPushButton(tr("Reconnect"), this);

    HealthBtn = new QPushButton(tr("Health check"), this);

    UploadBtn = new QPushButton(tr("Upload firmware"), this);

    CancelUploadBtn = new QPushButton(tr("Cancel upload"), this);

    CancelUploadBtn->setEnabled(false);

    connect(ConnectBtn, &QPushButton::clicked, this, &HardwareArduinoBoardPanelWidget::onConnect);

    connect(DisconnectBtn, &QPushButton::clicked, this, &HardwareArduinoBoardPanelWidget::onDisconnect);

    connect(ReconnectBtn, &QPushButton::clicked, this, &HardwareArduinoBoardPanelWidget::onReconnect);

    connect(HealthBtn, &QPushButton::clicked, this, &HardwareArduinoBoardPanelWidget::onHealthCheck);

    connect(UploadBtn, &QPushButton::clicked, this, &HardwareArduinoBoardPanelWidget::onUpload);

    connect(CancelUploadBtn, &QPushButton::clicked, this, &HardwareArduinoBoardPanelWidget::onCancelUpload);

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

    form->addRow(QString(), AutoDetectBoardCheck);

    form->addRow(tr("Upload preview:"), UploadPreviewLabel);

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

    auto* uploadRow = new QWidget(this);

    auto* uploadRowLayout = new QHBoxLayout(uploadRow);

    uploadRowLayout->setContentsMargins(0, 0, 0, 0);

    uploadRowLayout->addWidget(UploadBtn);

    uploadRowLayout->addWidget(CancelUploadBtn);

    form->addRow(tr("Upload:"), uploadRow);

    form->addRow(QString(), UploadProgress);

    form->addRow(tr("Status / log:"), StatusLog);

    auto* connRow = new QHBoxLayout();

    connRow->addWidget(ConnectBtn);

    connRow->addWidget(DisconnectBtn);

    connRow->addWidget(ReconnectBtn);

    connRow->addWidget(HealthBtn);

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

    UploadPollTimer = new QTimer(this);

    UploadPollTimer->setInterval(200);

    connect(UploadPollTimer, &QTimer::timeout, this, &HardwareArduinoBoardPanelWidget::onUploadPollTick);

    connect(PortCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,

            &HardwareArduinoBoardPanelWidget::onPortOrProfileChanged);

    connect(BoardProfileCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,

            &HardwareArduinoBoardPanelWidget::onPortOrProfileChanged);

    connect(BundledFirmwareCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,

            &HardwareArduinoBoardPanelWidget::onPortOrProfileChanged);

    connect(FirmwarePathEdit, &QLineEdit::textChanged, this,
            &HardwareArduinoBoardPanelWidget::updateUploadPreview);
    connect(AutoDetectBoardCheck, &QCheckBox::toggled, this,
            &HardwareArduinoBoardPanelWidget::onPortOrProfileChanged);

    HardwareGuiHelpers::applyUnicodeFriendlyFont(this);

    onRefreshPorts();

    updateUploadPreview();

}

void HardwareArduinoBoardPanelWidget::setContext(const UComponentGuiContext& context)

{

    Context = context;

    refreshFromModel();

}

void HardwareArduinoBoardPanelWidget::updateUploadPreview()

{

    const int profile = BoardProfileCombo->currentData().toInt();

    const RDK::UArduinoBoardProfile prof =

        RDK::UArduinoBoardProfileUtil::profileForKind(profile);

    const QString bundled = BundledFirmwareCombo->currentData().toString();

    const QString custom_hex = FirmwarePathEdit->text().trimmed();

    QString hex_display;

    QString hex_for_validate;

    if (!custom_hex.isEmpty()) {

        hex_display = custom_hex;

        hex_for_validate = RDK::UFirmwareManifest::resolveFromApplicationDir(custom_hex);

    } else {

        hex_display = RDK::UFirmwareManifest::bundledHexRelativePath(bundled, profile);

        hex_for_validate = RDK::UFirmwareManifest::resolveBundledHex(bundled, profile);

    }

    const QString validation =

        RDK::UArduinoBoardProfileUtil::validateUploadTargets(profile, hex_for_validate);

    QString text = tr("MCU: %1, protocol: %2\nHEX: %3")

                       .arg(QString::fromUtf8(prof.mcu),

                            QString::fromUtf8(prof.protocol),

                            hex_display);

    if (!validation.isEmpty()) {

        text += QStringLiteral("\n\n") + tr("Warning: %1").arg(validation);

        UploadPreviewLabel->setStyleSheet(QStringLiteral("color: #c06000;"));

    } else {

        UploadPreviewLabel->setStyleSheet(QString());

    }

    UploadPreviewLabel->setText(text);

}

void HardwareArduinoBoardPanelWidget::updateUploadControlsEnabled()

{

    if (Context.componentLongName.isEmpty())

        return;

    const bool uploading = HardwareGuiHelpers::getPropBool(Context, "IsUploading", false);

    UploadBtn->setEnabled(!uploading);

    CancelUploadBtn->setEnabled(uploading);

    ConnectBtn->setEnabled(!uploading);

    DisconnectBtn->setEnabled(!uploading);

    ReconnectBtn->setEnabled(!uploading);

    HealthBtn->setEnabled(!uploading);

}

void HardwareArduinoBoardPanelWidget::startUploadUiPoll()

{

    UploadPollTimer->start();

    onUploadPollTick();

}

void HardwareArduinoBoardPanelWidget::stopUploadUiPoll()

{

    UploadPollTimer->stop();

}

void HardwareArduinoBoardPanelWidget::onUploadPollTick()

{

    if (Context.componentLongName.isEmpty())

        return;

    HardwareGuiHelpers::envCalculate(Context);

    refreshFromModel();

    updateUploadControlsEnabled();

    const bool uploading = HardwareGuiHelpers::getPropBool(Context, "IsUploading", false);

    if (!uploading)

        stopUploadUiPoll();

}

QString HardwareArduinoBoardPanelWidget::formatBoardDetectLine(const QString& port_path,
                                                               int detected) const
{
    if (port_path.isEmpty())
        return tr("Detected: (no port selected)");
    if (detected == 0)
        return tr("Detected: Arduino Uno");
    if (detected == 1)
        return tr("Detected: Arduino Mega 2560");
    return tr("Detected: unknown (set board manually; CH340 clones often need manual profile)");
}

void HardwareArduinoBoardPanelWidget::populateBundledFirmwareCombo()
{
    BundledFirmwareCombo->clear();
    QString err;
    if (RDK::UHardwareCatalog::instance().load(&err)) {
        const QStringList ids = RDK::UHardwareCatalog::instance().firmwareIds(true);
        for (const QString& id : ids) {
            const RDK::UHwFirmwareInfo* fw = RDK::UHardwareCatalog::instance().firmware(id);
            const QString title = fw ? fw->title : id;
            BundledFirmwareCombo->addItem(title, id);
        }
    }
    if (BundledFirmwareCombo->count() == 0) {
        BundledFirmwareCombo->addItem(tr("Sensor Lab v1"), QStringLiteral("sensor_lab_v1"));
        BundledFirmwareCombo->addItem(tr("Standard Firmata"), QStringLiteral("standard_firmata"));
        if (!err.isEmpty() && StatusLog)
            StatusLog->appendPlainText(tr("Catalog load failed, using built-in firmware list: %1").arg(err));
    }
}

void HardwareArduinoBoardPanelWidget::updateBoardDetectHint()
{
    const QString port_path = HardwareGuiHelpers::selectedSerialPortPath(PortCombo);
    const int detected =
        port_path.isEmpty() ? -1
                            : RDK::UArduinoSerialPortUtil::inferBoardProfileForDevicePath(port_path);
    if (AutoDetectBoardCheck->isChecked())
        BoardDetectHint.clear();
    else
        BoardDetectHint = formatBoardDetectLine(port_path, detected);
}

void HardwareArduinoBoardPanelWidget::onPortOrProfileChanged()

{

    const QString port_path = HardwareGuiHelpers::selectedSerialPortPath(PortCombo);

    const int detected =

        port_path.isEmpty() ? -1

                            : RDK::UArduinoSerialPortUtil::inferBoardProfileForDevicePath(port_path);

    if (AutoDetectBoardCheck->isChecked() && (detected == 0 || detected == 1)) {
        QSignalBlocker b(BoardProfileCombo);
        BoardProfileCombo->setCurrentIndex(detected);
        BoardDetectHint.clear();
        BoardProfileCombo->setToolTip(QString());
    } else if (!port_path.isEmpty() && detected < 0) {
        // AU-10: CH340 / unknown VID-PID — force attention to manual BoardProfile.
        BoardDetectHint = formatBoardDetectLine(port_path, detected);
        BoardProfileCombo->setToolTip(
            tr("USB board type unknown (common with CH340 clones). Select Uno or Mega manually before Upload."));
    } else {
        updateBoardDetectHint();
        BoardProfileCombo->setToolTip(QString());
    }

    updateUploadPreview();
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

    const bool uploading = HardwareGuiHelpers::getPropBool(Context, "IsUploading", false);

    const int progress = HardwareGuiHelpers::getPropInt(Context, "UploadProgress", 0);

    if (uploading && progress < 15)

        UploadProgress->setRange(0, 0);

    else {

        UploadProgress->setRange(0, 100);

        UploadProgress->setValue(progress);

    }

    const bool connected = HardwareGuiHelpers::getPropBool(Context, "IsConnected", false);

    const bool hasError = HardwareGuiHelpers::getPropBool(Context, "HasError", false);

    const QString err = HardwareGuiHelpers::getProp(Context, "LastError");

    const QString uploadRes = HardwareGuiHelpers::getProp(Context, "UploadLastResult");

    updateBoardDetectHint();

    QString status = tr("Connected: %1\nHas error: %2\nLast error: %3\nUpload: %4")
                         .arg(connected ? tr("yes") : tr("no"))
                         .arg(hasError ? tr("yes") : tr("no"))
                         .arg(err, uploadRes);
    if (!BoardDetectHint.isEmpty())
        status += QLatin1Char('\n') + BoardDetectHint;
    HardwareGuiHelpers::setStatusLogText(StatusLog, status);

    updateUploadPreview();

    updateUploadControlsEnabled();

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

    onPortOrProfileChanged();

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

    startUploadUiPoll();

}

void HardwareArduinoBoardPanelWidget::onCancelUpload()

{

    if (Context.componentLongName.isEmpty())

        return;

    HardwareGuiHelpers::pulseEdge(Context, "CancelUpload");

    startUploadUiPoll();

}



void HardwareArduinoBoardPanelWidget::onBrowseHex()

{

    const QString path = QFileDialog::getOpenFileName(this, tr("Select HEX file"), QString(),

                                                      tr("Intel HEX (*.hex);;All (*)"));

    if (!path.isEmpty()) {

        FirmwarePathEdit->setText(path);

        updateUploadPreview();

    }

}
