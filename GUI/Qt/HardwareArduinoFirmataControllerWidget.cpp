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
    Diagram = new UArduinoBoardDiagramWidget(this);
    Diagram->setInteractive(true);
    connect(Diagram, &UArduinoBoardDiagramWidget::pinClicked, this,
            &HardwareArduinoFirmataControllerWidget::onDiagramPinClicked);

    auto* firmataPage = new QWidget(this);
    PinSpin = new QSpinBox(firmataPage);
    PinSpin->setRange(0, 69);
    PinSpin->setValue(13);

    ModeCombo = new QComboBox(firmataPage);
    ModeCombo->addItem(tr("Input"), 0);
    ModeCombo->addItem(tr("Output"), 1);
    ModeCombo->addItem(tr("Analog"), 2);
    ModeCombo->addItem(tr("PWM"), 3);

    DigitalValueSpin = new QSpinBox(firmataPage);
    DigitalValueSpin->setRange(0, 1);

    StatusLog = HardwareGuiHelpers::createStatusLogWidget(firmataPage);

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
    form->addRow(tr("Pin (Firmata #):"), PinSpin);
    form->addRow(tr("Mode:"), ModeCombo);
    form->addRow(tr("Digital value:"), DigitalValueSpin);
    auto* row = new QHBoxLayout();
    row->addWidget(setModeBtn);
    row->addWidget(writeBtn);
    row->addWidget(readBtn);
    form->addRow(tr("Actions:"), row);
    form->addRow(QString(), restartBtn);
    form->addRow(QString(), applyBtn);
    form->addRow(QString(), calcBtn);
    form->addRow(tr("Status:"), StatusLog);
    form->addRow(QString(), new QLabel(tr("Click a pin on the diagram to select it."), firmataPage));
    firmataPage->setLayout(form);

    BoardPanel = new HardwareArduinoBoardPanelWidget(this);
    Tabs = new QTabWidget(this);
    Tabs->addTab(firmataPage, tr("Firmata"));
    Tabs->addTab(BoardPanel, tr("Board"));

    auto* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(Diagram);
    splitter->addWidget(Tabs);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 2);

    auto* root = new QVBoxLayout(this);
    root->addWidget(splitter);
}

void HardwareArduinoFirmataControllerWidget::setComponentContext(const UComponentGuiContext& context)
{
    Context = context;
    BoardPanel->setContext(context);
    refreshFromModel(true);
}

QString HardwareArduinoFirmataControllerWidget::componentGuiId() const
{
    return QStringLiteral("hw.arduino.firmata");
}

void HardwareArduinoFirmataControllerWidget::refreshFromModel(bool force)
{
    Q_UNUSED(force);
    if (Context.componentLongName.isEmpty())
        return;

    BoardPanel->refreshFromModel();

    const int profile = HardwareGuiHelpers::getPropInt(Context, "BoardProfile", 0);
    const int pin = HardwareGuiHelpers::getPropInt(Context, "SelectedPin", 13);
    PinSpin->setValue(pin);
    DigitalValueSpin->setValue(HardwareGuiHelpers::getPropInt(Context, "DigitalPinValue", 0));

    Diagram->setBoardProfile(profile);
    Diagram->setConnectionState(HardwareGuiHelpers::getPropInt(Context, "ConnectionState", 0));
    Diagram->setSelectedPinId(firmataPinToLabel(pin, profile));

    const bool firmata_ready = HardwareGuiHelpers::getPropBool(Context, "IsFirmataReady", false);
    const bool link_ready = HardwareGuiHelpers::getPropBool(Context, "IsLinkReady", false);
    const QString ver = HardwareGuiHelpers::getProp(Context, "FirmataFirmwareVersion");
    const int analog = HardwareGuiHelpers::getPropInt(Context, "AnalogPinValue", 0);
    HardwareGuiHelpers::setStatusLogText(
        StatusLog,
        tr("Firmata ready: %1\nLink ready: %2\nVersion: %3\nAnalog: %4")
            .arg(firmata_ready ? tr("yes") : tr("no"))
            .arg(link_ready ? tr("yes") : tr("no"))
            .arg(ver)
            .arg(analog));
}

QString HardwareArduinoFirmataControllerWidget::firmataPinToLabel(int pin, int board_profile) const
{
    Q_UNUSED(board_profile);
    if (pin >= 14)
        return QStringLiteral("A%1").arg(pin - 14);
    return QStringLiteral("D%1").arg(pin);
}

void HardwareArduinoFirmataControllerWidget::onDiagramPinClicked(const QString& pinId)
{
    const int profile = HardwareGuiHelpers::getPropInt(Context, "BoardProfile", 0);
    const int pin = UArduinoBoardDiagramWidget::firmataPinFromLabel(pinId, profile);
    if (pin < 0)
        return;
    PinSpin->setValue(pin);
    Diagram->setSelectedPinId(pinId);
    onApply();
}

void HardwareArduinoFirmataControllerWidget::onApply()
{
    BoardPanel->applyToModel();
    HardwareGuiHelpers::setProp(Context, "SelectedPin", QString::number(PinSpin->value()));
    HardwareGuiHelpers::setProp(Context, "SelectedPinMode",
                                QString::number(ModeCombo->currentData().toInt()));
    HardwareGuiHelpers::setProp(Context, "DigitalPinValue", QString::number(DigitalValueSpin->value()));
}

void HardwareArduinoFirmataControllerWidget::onCalculate()
{
    onApply();
    HardwareGuiHelpers::envCalculate(Context);
    refreshFromModel(true);
}

void HardwareArduinoFirmataControllerWidget::onSetPinMode()
{
    onApply();
    HardwareGuiHelpers::setProp(Context, "SetPinModeFlag", QStringLiteral("1"));
    onCalculate();
}

void HardwareArduinoFirmataControllerWidget::onWriteDigital()
{
    onApply();
    HardwareGuiHelpers::setProp(Context, "WriteDigitalFlag", QStringLiteral("1"));
    onCalculate();
}

void HardwareArduinoFirmataControllerWidget::onReadAnalog()
{
    onApply();
    HardwareGuiHelpers::setProp(Context, "ReadAnalogFlag", QStringLiteral("1"));
    onCalculate();
}

void HardwareArduinoFirmataControllerWidget::onRestartFirmata()
{
    BoardPanel->applyToModel();
    HardwareGuiHelpers::pulseEdge(Context, "RestartFirmata");
    refreshFromModel(true);
}
