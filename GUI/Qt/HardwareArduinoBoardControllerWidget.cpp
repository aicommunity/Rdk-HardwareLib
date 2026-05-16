#include "HardwareArduinoBoardControllerWidget.h"

#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QSignalBlocker>
#include <QSplitter>
#include <QVBoxLayout>

#include <QtSerialPort/QSerialPortInfo>

#include "widgets/HardwareGuiHelpers.h"

HardwareArduinoBoardControllerWidget::HardwareArduinoBoardControllerWidget(QWidget* parent,
                                                                           RDK::UApplication* app)
    : UVisualControllerWidget(parent, app)
{
    m_diagram = new UArduinoBoardDiagramWidget(this);

    m_portCombo = new QComboBox(this);
    auto* refreshPortsBtn = new QPushButton(tr("Refresh"), this);
    connect(refreshPortsBtn, &QPushButton::clicked, this, &HardwareArduinoBoardControllerWidget::onRefreshPorts);

    m_boardProfileCombo = new QComboBox(this);
    m_boardProfileCombo->addItem(tr("Arduino Uno"), 0);
    m_boardProfileCombo->addItem(tr("Arduino Mega 2560"), 1);

    m_baudSpin = new QSpinBox(this);
    m_baudSpin->setRange(9600, 250000);
    m_baudSpin->setValue(57600);

    m_bundledFirmwareCombo = new QComboBox(this);
    m_bundledFirmwareCombo->addItem(tr("Sensor Lab v1"), QStringLiteral("sensor_lab_v1"));
    m_bundledFirmwareCombo->addItem(tr("Standard Firmata"), QStringLiteral("standard_firmata"));

    m_firmwarePathEdit = new QLineEdit(this);
    auto* browseBtn = new QPushButton(tr("Browse…"), this);
    connect(browseBtn, &QPushButton::clicked, this, &HardwareArduinoBoardControllerWidget::onBrowseHex);

    m_uploadProgress = new QProgressBar(this);
    m_statusLabel = new QLabel(this);
    m_statusLabel->setWordWrap(true);

    auto* connectBtn = new QPushButton(tr("Connect"), this);
    auto* disconnectBtn = new QPushButton(tr("Disconnect"), this);
    auto* healthBtn = new QPushButton(tr("Health check"), this);
    auto* uploadBtn = new QPushButton(tr("Upload firmware"), this);
    connect(connectBtn, &QPushButton::clicked, this, &HardwareArduinoBoardControllerWidget::onConnect);
    connect(disconnectBtn, &QPushButton::clicked, this, &HardwareArduinoBoardControllerWidget::onDisconnect);
    connect(healthBtn, &QPushButton::clicked, this, &HardwareArduinoBoardControllerWidget::onHealthCheck);
    connect(uploadBtn, &QPushButton::clicked, this, &HardwareArduinoBoardControllerWidget::onUpload);

    m_applyButton = new QPushButton(tr("Apply parameters"), this);
    m_resetButton = new QPushButton(tr("Reset component"), this);
    m_calculateButton = new QPushButton(tr("Calculate component"), this);
    connect(m_applyButton, &QPushButton::clicked, this, &HardwareArduinoBoardControllerWidget::onApply);
    connect(m_resetButton, &QPushButton::clicked, this, &HardwareArduinoBoardControllerWidget::onReset);
    connect(m_calculateButton, &QPushButton::clicked, this, &HardwareArduinoBoardControllerWidget::onCalculate);

    auto* form = new QFormLayout();
    auto* portRow = new QHBoxLayout();
    portRow->addWidget(m_portCombo, 1);
    portRow->addWidget(refreshPortsBtn);
    form->addRow(tr("Port:"), portRow);
    form->addRow(tr("Board:"), m_boardProfileCombo);
    form->addRow(tr("Baud rate:"), m_baudSpin);
    form->addRow(tr("Bundled firmware:"), m_bundledFirmwareCombo);
    auto* hexRow = new QHBoxLayout();
    hexRow->addWidget(m_firmwarePathEdit, 1);
    hexRow->addWidget(browseBtn);
    form->addRow(tr("HEX path:"), hexRow);
    form->addRow(tr("Upload:"), uploadBtn);
    form->addRow(QString(), m_uploadProgress);
    form->addRow(QString(), m_statusLabel);

    auto* connRow = new QHBoxLayout();
    connRow->addWidget(connectBtn);
    connRow->addWidget(disconnectBtn);
    connRow->addWidget(healthBtn);
    form->addRow(tr("Connection:"), connRow);

    auto* btnRow = new QHBoxLayout();
    btnRow->addWidget(m_applyButton);
    btnRow->addWidget(m_resetButton);
    btnRow->addWidget(m_calculateButton);
    form->addRow(QString(), btnRow);

    auto* right = new QWidget(this);
    right->setLayout(form);
    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setWidget(right);

    auto* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(m_diagram);
    splitter->addWidget(scroll);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 2);

    auto* root = new QVBoxLayout(this);
    root->addWidget(splitter);

    onRefreshPorts();
    setAccessibleName(QStringLiteral("HardwareArduinoBoardControllerWidget"));
}

void HardwareArduinoBoardControllerWidget::setComponentContext(const UComponentGuiContext& context)
{
    m_context = context;
    refreshFromModel(true);
}

QString HardwareArduinoBoardControllerWidget::componentGuiId() const
{
    return QStringLiteral("hw.arduino.board");
}

void HardwareArduinoBoardControllerWidget::refreshFromModel(bool force)
{
    Q_UNUSED(force);
    if (m_context.componentLongName.isEmpty())
        return;

    QSignalBlocker b1(m_portCombo);
    QSignalBlocker b2(m_boardProfileCombo);
    QSignalBlocker b3(m_baudSpin);
    QSignalBlocker b4(m_bundledFirmwareCombo);
    QSignalBlocker b5(m_firmwarePathEdit);

    const QString port = HardwareGuiHelpers::getProp(m_context, "PortName");
    int portIdx = m_portCombo->findText(port);
    if (portIdx < 0 && !port.isEmpty()) {
        m_portCombo->addItem(port);
        portIdx = m_portCombo->findText(port);
    }
    m_portCombo->setCurrentIndex(portIdx >= 0 ? portIdx : 0);

    const int profile = HardwareGuiHelpers::getPropInt(m_context, "BoardProfile", 0);
    m_boardProfileCombo->setCurrentIndex(profile == 1 ? 1 : 0);
    m_baudSpin->setValue(HardwareGuiHelpers::getPropInt(m_context, "BaudRate", 57600));

    const QString bundled = HardwareGuiHelpers::getProp(m_context, "BundledFirmwareId");
    const int bundledIdx = m_bundledFirmwareCombo->findData(bundled);
    m_bundledFirmwareCombo->setCurrentIndex(bundledIdx >= 0 ? bundledIdx : 0);
    m_firmwarePathEdit->setText(HardwareGuiHelpers::getProp(m_context, "FirmwarePath"));

    const int progress = HardwareGuiHelpers::getPropInt(m_context, "UploadProgress", 0);
    m_uploadProgress->setValue(progress);

    const int state = HardwareGuiHelpers::getPropInt(m_context, "ConnectionState", 0);
    const QString err = HardwareGuiHelpers::getProp(m_context, "LastError");
    const QString uploadRes = HardwareGuiHelpers::getProp(m_context, "UploadLastResult");
    m_statusLabel->setText(
        tr("State: %1\nLast error: %2\nUpload: %3").arg(state).arg(err, uploadRes));

    updateDiagram();
}

void HardwareArduinoBoardControllerWidget::applyToModel()
{
    HardwareGuiHelpers::setProp(m_context, "PortName", m_portCombo->currentText());
    HardwareGuiHelpers::setProp(m_context, "BoardProfile",
                                QString::number(m_boardProfileCombo->currentData().toInt()));
    HardwareGuiHelpers::setProp(m_context, "BaudRate", QString::number(m_baudSpin->value()));
    HardwareGuiHelpers::setProp(m_context, "BundledFirmwareId",
                                m_bundledFirmwareCombo->currentData().toString());
    HardwareGuiHelpers::setProp(m_context, "FirmwarePath", m_firmwarePathEdit->text());
}

void HardwareArduinoBoardControllerWidget::updateDiagram()
{
    m_diagram->setBoardProfile(HardwareGuiHelpers::getPropInt(m_context, "BoardProfile", 0));
    m_diagram->setConnectionState(HardwareGuiHelpers::getPropInt(m_context, "ConnectionState", 0));
    m_diagram->setHighlightedPins({QStringLiteral("D2"), QStringLiteral("A2"), QStringLiteral("D9")});
}

void HardwareArduinoBoardControllerWidget::onRefreshPorts()
{
    const QString current = m_portCombo->currentText();
    m_portCombo->clear();
    for (const QSerialPortInfo& info : QSerialPortInfo::availablePorts())
        m_portCombo->addItem(info.portName());
    const int idx = m_portCombo->findText(current);
    if (idx >= 0)
        m_portCombo->setCurrentIndex(idx);
}

void HardwareArduinoBoardControllerWidget::onApply()
{
    applyToModel();
    HardwareGuiHelpers::envReset(m_context);
    refreshFromModel(true);
}

void HardwareArduinoBoardControllerWidget::onReset()
{
    HardwareGuiHelpers::envReset(m_context);
    refreshFromModel(true);
}

void HardwareArduinoBoardControllerWidget::onCalculate()
{
    applyToModel();
    HardwareGuiHelpers::envCalculate(m_context);
    refreshFromModel(true);
}

void HardwareArduinoBoardControllerWidget::onConnect()
{
    applyToModel();
    HardwareGuiHelpers::setProp(m_context, "ConnectOnBuild", QStringLiteral("1"));
    HardwareGuiHelpers::envCalculate(m_context);
    refreshFromModel(true);
}

void HardwareArduinoBoardControllerWidget::onDisconnect()
{
    applyToModel();
    HardwareGuiHelpers::setProp(m_context, "ConnectOnBuild", QStringLiteral("0"));
    HardwareGuiHelpers::setProp(m_context, "PortName", QString());
    HardwareGuiHelpers::envCalculate(m_context);
    refreshFromModel(true);
}

void HardwareArduinoBoardControllerWidget::onHealthCheck()
{
    applyToModel();
    HardwareGuiHelpers::setProp(m_context, "RequestHealthCheck", QStringLiteral("1"));
    HardwareGuiHelpers::envCalculate(m_context);
    refreshFromModel(true);
}

void HardwareArduinoBoardControllerWidget::onUpload()
{
    applyToModel();
    HardwareGuiHelpers::setProp(m_context, "UploadFirmwareFlag", QStringLiteral("1"));
    HardwareGuiHelpers::envCalculate(m_context);
    refreshFromModel(true);
}

void HardwareArduinoBoardControllerWidget::onBrowseHex()
{
    const QString path = QFileDialog::getOpenFileName(this, tr("Select HEX file"), QString(),
                                                      tr("Intel HEX (*.hex);;All (*)"));
    if (!path.isEmpty())
        m_firmwarePathEdit->setText(path);
}
