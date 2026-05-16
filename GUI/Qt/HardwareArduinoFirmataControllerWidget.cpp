#include "HardwareArduinoFirmataControllerWidget.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QVBoxLayout>

#include <QtSerialPort/QSerialPortInfo>

#include "widgets/HardwareGuiHelpers.h"

HardwareArduinoFirmataControllerWidget::HardwareArduinoFirmataControllerWidget(QWidget* parent,
                                                                               RDK::UApplication* app)
    : UVisualControllerWidget(parent, app)
{
    m_diagram = new UArduinoBoardDiagramWidget(this);

    m_portCombo = new QComboBox(this);
    for (const QSerialPortInfo& info : QSerialPortInfo::availablePorts())
        m_portCombo->addItem(info.portName());

    m_pinSpin = new QSpinBox(this);
    m_pinSpin->setRange(0, 69);
    m_pinSpin->setValue(13);

    m_modeCombo = new QComboBox(this);
    m_modeCombo->addItem(tr("Input"), 0);
    m_modeCombo->addItem(tr("Output"), 1);
    m_modeCombo->addItem(tr("Analog"), 2);
    m_modeCombo->addItem(tr("PWM"), 3);

    m_digitalValueSpin = new QSpinBox(this);
    m_digitalValueSpin->setRange(0, 1);

    m_statusLabel = new QLabel(this);
    m_statusLabel->setWordWrap(true);

    auto* setModeBtn = new QPushButton(tr("Set pin mode"), this);
    auto* writeBtn = new QPushButton(tr("Write digital"), this);
    auto* readBtn = new QPushButton(tr("Read analog"), this);
    connect(setModeBtn, &QPushButton::clicked, this, &HardwareArduinoFirmataControllerWidget::onSetPinMode);
    connect(writeBtn, &QPushButton::clicked, this, &HardwareArduinoFirmataControllerWidget::onWriteDigital);
    connect(readBtn, &QPushButton::clicked, this, &HardwareArduinoFirmataControllerWidget::onReadAnalog);

    auto* applyBtn = new QPushButton(tr("Apply"), this);
    auto* calcBtn = new QPushButton(tr("Calculate"), this);
    connect(applyBtn, &QPushButton::clicked, this, &HardwareArduinoFirmataControllerWidget::onApply);
    connect(calcBtn, &QPushButton::clicked, this, &HardwareArduinoFirmataControllerWidget::onCalculate);

    auto* form = new QFormLayout();
    form->addRow(tr("Port:"), m_portCombo);
    form->addRow(tr("Pin:"), m_pinSpin);
    form->addRow(tr("Mode:"), m_modeCombo);
    form->addRow(tr("Digital value:"), m_digitalValueSpin);
    auto* row = new QHBoxLayout();
    row->addWidget(setModeBtn);
    row->addWidget(writeBtn);
    row->addWidget(readBtn);
    form->addRow(tr("Actions:"), row);
    form->addRow(QString(), applyBtn);
    form->addRow(QString(), calcBtn);
    form->addRow(tr("Status:"), m_statusLabel);

    auto* right = new QWidget(this);
    right->setLayout(form);
    auto* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(m_diagram);
    splitter->addWidget(right);

    auto* root = new QVBoxLayout(this);
    root->addWidget(splitter);
}

void HardwareArduinoFirmataControllerWidget::setComponentContext(const UComponentGuiContext& context)
{
    m_context = context;
    refreshFromModel(true);
}

QString HardwareArduinoFirmataControllerWidget::componentGuiId() const
{
    return QStringLiteral("hw.arduino.firmata");
}

void HardwareArduinoFirmataControllerWidget::refreshFromModel(bool force)
{
    Q_UNUSED(force);
    if (m_context.componentLongName.isEmpty())
        return;

    m_portCombo->setCurrentText(HardwareGuiHelpers::getProp(m_context, "PortName"));
    m_pinSpin->setValue(HardwareGuiHelpers::getPropInt(m_context, "SelectedPin", 13));
    m_digitalValueSpin->setValue(HardwareGuiHelpers::getPropInt(m_context, "DigitalPinValue", 0));

    m_diagram->setBoardProfile(HardwareGuiHelpers::getPropInt(m_context, "BoardProfile", 0));
    m_diagram->setConnectionState(HardwareGuiHelpers::getPropInt(m_context, "ConnectionState", 0));

    const bool ready = HardwareGuiHelpers::getPropBool(m_context, "FirmataReady", false);
    const QString ver = HardwareGuiHelpers::getProp(m_context, "FirmataFirmwareVersion");
    const int analog = HardwareGuiHelpers::getPropInt(m_context, "AnalogPinValue", 0);
    m_statusLabel->setText(
        tr("Firmata ready: %1\nVersion: %2\nAnalog: %3").arg(ready ? tr("yes") : tr("no"), ver).arg(analog));
}

void HardwareArduinoFirmataControllerWidget::onApply()
{
    HardwareGuiHelpers::setProp(m_context, "PortName", m_portCombo->currentText());
    HardwareGuiHelpers::setProp(m_context, "SelectedPin", QString::number(m_pinSpin->value()));
    HardwareGuiHelpers::setProp(m_context, "SelectedPinMode",
                                QString::number(m_modeCombo->currentData().toInt()));
    HardwareGuiHelpers::setProp(m_context, "DigitalPinValue", QString::number(m_digitalValueSpin->value()));
}

void HardwareArduinoFirmataControllerWidget::onCalculate()
{
    onApply();
    HardwareGuiHelpers::envCalculate(m_context);
    refreshFromModel(true);
}

void HardwareArduinoFirmataControllerWidget::onSetPinMode()
{
    onApply();
    HardwareGuiHelpers::setProp(m_context, "SetPinModeFlag", QStringLiteral("1"));
    onCalculate();
}

void HardwareArduinoFirmataControllerWidget::onWriteDigital()
{
    onApply();
    HardwareGuiHelpers::setProp(m_context, "WriteDigitalFlag", QStringLiteral("1"));
    onCalculate();
}

void HardwareArduinoFirmataControllerWidget::onReadAnalog()
{
    onApply();
    HardwareGuiHelpers::setProp(m_context, "ReadAnalogFlag", QStringLiteral("1"));
    onCalculate();
}
