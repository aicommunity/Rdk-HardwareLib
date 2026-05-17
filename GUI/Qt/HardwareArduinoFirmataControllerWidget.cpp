#include "HardwareArduinoFirmataControllerWidget.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSplitter>
#include <QVBoxLayout>

#include "widgets/HardwareArduinoBoardPanelWidget.h"
#include "widgets/HardwareGuiHelpers.h"

HardwareArduinoFirmataControllerWidget::HardwareArduinoFirmataControllerWidget(QWidget* parent,
                                                                               RDK::UApplication* app)
    : UVisualControllerWidget(parent, app)
{
    m_diagram = new UArduinoBoardDiagramWidget(this);
    m_diagram->setInteractive(true);
    connect(m_diagram, &UArduinoBoardDiagramWidget::pinClicked, this,
            &HardwareArduinoFirmataControllerWidget::onDiagramPinClicked);

    auto* firmataPage = new QWidget(this);
    m_pinSpin = new QSpinBox(firmataPage);
    m_pinSpin->setRange(0, 69);
    m_pinSpin->setValue(13);

    m_modeCombo = new QComboBox(firmataPage);
    m_modeCombo->addItem(tr("Input"), 0);
    m_modeCombo->addItem(tr("Output"), 1);
    m_modeCombo->addItem(tr("Analog"), 2);
    m_modeCombo->addItem(tr("PWM"), 3);

    m_digitalValueSpin = new QSpinBox(firmataPage);
    m_digitalValueSpin->setRange(0, 1);

    m_statusLog = HardwareGuiHelpers::createStatusLogWidget(firmataPage);

    auto* setModeBtn = new QPushButton(tr("Set pin mode"), firmataPage);
    auto* writeBtn = new QPushButton(tr("Write digital"), firmataPage);
    auto* readBtn = new QPushButton(tr("Read analog"), firmataPage);
    auto* restartBtn = new QPushButton(tr("Restart Firmata"), firmataPage);
    connect(setModeBtn, &QPushButton::clicked, this, &HardwareArduinoFirmataControllerWidget::onSetPinMode);
    connect(writeBtn, &QPushButton::clicked, this, &HardwareArduinoFirmataControllerWidget::onWriteDigital);
    connect(readBtn, &QPushButton::clicked, this, &HardwareArduinoFirmataControllerWidget::onReadAnalog);
    connect(restartBtn, &QPushButton::clicked, this, &HardwareArduinoFirmataControllerWidget::onRestartFirmata);

    auto* applyBtn = new QPushButton(tr("Apply"), firmataPage);
    auto* calcBtn = new QPushButton(tr("Calculate"), firmataPage);
    connect(applyBtn, &QPushButton::clicked, this, &HardwareArduinoFirmataControllerWidget::onApply);
    connect(calcBtn, &QPushButton::clicked, this, &HardwareArduinoFirmataControllerWidget::onCalculate);

    auto* form = new QFormLayout();
    form->addRow(tr("Pin (Firmata #):"), m_pinSpin);
    form->addRow(tr("Mode:"), m_modeCombo);
    form->addRow(tr("Digital value:"), m_digitalValueSpin);
    auto* row = new QHBoxLayout();
    row->addWidget(setModeBtn);
    row->addWidget(writeBtn);
    row->addWidget(readBtn);
    form->addRow(tr("Actions:"), row);
    form->addRow(QString(), restartBtn);
    form->addRow(QString(), applyBtn);
    form->addRow(QString(), calcBtn);
    form->addRow(tr("Status:"), m_statusLog);
    form->addRow(QString(), new QLabel(tr("Click a pin on the diagram to select it."), firmataPage));
    firmataPage->setLayout(form);

    m_boardPanel = new HardwareArduinoBoardPanelWidget(this);
    m_tabs = new QTabWidget(this);
    m_tabs->addTab(firmataPage, tr("Firmata"));
    m_tabs->addTab(m_boardPanel, tr("Board"));

    auto* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(m_diagram);
    splitter->addWidget(m_tabs);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 2);

    auto* root = new QVBoxLayout(this);
    root->addWidget(splitter);
}

void HardwareArduinoFirmataControllerWidget::setComponentContext(const UComponentGuiContext& context)
{
    m_context = context;
    m_boardPanel->setContext(context);
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

    m_boardPanel->refreshFromModel();

    const int profile = HardwareGuiHelpers::getPropInt(m_context, "BoardProfile", 0);
    const int pin = HardwareGuiHelpers::getPropInt(m_context, "SelectedPin", 13);
    m_pinSpin->setValue(pin);
    m_digitalValueSpin->setValue(HardwareGuiHelpers::getPropInt(m_context, "DigitalPinValue", 0));

    m_diagram->setBoardProfile(profile);
    m_diagram->setConnectionState(HardwareGuiHelpers::getPropInt(m_context, "ConnectionState", 0));
    m_diagram->setSelectedPinId(firmataPinToLabel(pin, profile));

    const bool ready = HardwareGuiHelpers::getPropBool(m_context, "IsFirmataReady", false);
    const bool linkReady = HardwareGuiHelpers::getPropBool(m_context, "IsLinkReady", false);
    const QString ver = HardwareGuiHelpers::getProp(m_context, "FirmataFirmwareVersion");
    const int analog = HardwareGuiHelpers::getPropInt(m_context, "AnalogPinValue", 0);
    HardwareGuiHelpers::setStatusLogText(
        m_statusLog,
        tr("Firmata ready: %1\nLink ready: %2\nVersion: %3\nAnalog: %4")
            .arg(ready ? tr("yes") : tr("no"))
            .arg(linkReady ? tr("yes") : tr("no"))
            .arg(ver)
            .arg(analog));
}

QString HardwareArduinoFirmataControllerWidget::firmataPinToLabel(int pin, int boardProfile) const
{
    Q_UNUSED(boardProfile);
    if (pin >= 14)
        return QStringLiteral("A%1").arg(pin - 14);
    return QStringLiteral("D%1").arg(pin);
}

void HardwareArduinoFirmataControllerWidget::onDiagramPinClicked(const QString& pinId)
{
    const int profile = HardwareGuiHelpers::getPropInt(m_context, "BoardProfile", 0);
    const int pin = UArduinoBoardDiagramWidget::firmataPinFromLabel(pinId, profile);
    if (pin < 0)
        return;
    m_pinSpin->setValue(pin);
    m_diagram->setSelectedPinId(pinId);
    onApply();
}

void HardwareArduinoFirmataControllerWidget::onApply()
{
    m_boardPanel->applyToModel();
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

void HardwareArduinoFirmataControllerWidget::onRestartFirmata()
{
    m_boardPanel->applyToModel();
    HardwareGuiHelpers::pulseEdge(m_context, "RestartFirmata");
    refreshFromModel(true);
}
