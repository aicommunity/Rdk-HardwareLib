#include "HardwareArduinoAdcControllerWidget.h"

#include <QFormLayout>
#include <QPushButton>
#include <QVBoxLayout>

#include "widgets/HardwareGuiHelpers.h"

#include <Transport/UArduinoPinMap.h>

HardwareArduinoAdcControllerWidget::HardwareArduinoAdcControllerWidget(QWidget* parent,
                                                                       RDK::UApplication* app)
    : UVisualControllerWidget(parent, app)
{
    LinkedEdit = new QLineEdit(this);
    PinCombo = new QComboBox(this);
    UseLinkedSamples = new QCheckBox(tr("Use linked AnalogSamples"), this);
    UseLinkedSamples->setChecked(true);
    ValueLabel = new QLabel(tr("Adc value: \u2014"), this);

    auto* readBtn = new QPushButton(tr("Read ADC"), this);
    connect(readBtn, &QPushButton::clicked, this, &HardwareArduinoAdcControllerWidget::onReadAdc);

    auto* form = new QFormLayout();
    form->addRow(tr("Linked Firmata:"), LinkedEdit);
    form->addRow(tr("Analog pin:"), PinCombo);
    form->addRow(QString(), UseLinkedSamples);
    form->addRow(QString(), readBtn);
    form->addRow(QString(), ValueLabel);

    auto* root = new QVBoxLayout(this);
    root->addLayout(form);
    HardwareGuiHelpers::applyUnicodeFriendlyFont(this);
}

void HardwareArduinoAdcControllerWidget::setComponentContext(const UComponentGuiContext& context)
{
    Context = context;
    refreshFromModel(true);
}

QString HardwareArduinoAdcControllerWidget::componentGuiId() const
{
    return QStringLiteral("hw.arduino.adc");
}

void HardwareArduinoAdcControllerWidget::refreshFromModel(bool force)
{
    Q_UNUSED(force);
    if (Context.componentLongName.isEmpty())
        return;

    const int profile = HardwareGuiHelpers::getPropInt(Context, "BoardProfile", 0);
    PinCombo->clear();
  const int max_pin = RDK::UArduinoPinMap::maxFirmataPin(profile);
    const int analog_base = RDK::UArduinoPinMap::analogBase(profile);

    for (int pin = analog_base; pin <= max_pin; ++pin)
        PinCombo->addItem(RDK::UArduinoPinMap::labelForFirmataPin(pin, profile), pin);

    LinkedEdit->setText(HardwareGuiHelpers::getProp(Context, "LinkedFirmataName"));
    const int analog_pin = HardwareGuiHelpers::getPropInt(Context, "AnalogPin", analog_base);
    const int idx = PinCombo->findData(analog_pin);
    if (idx >= 0)
        PinCombo->setCurrentIndex(idx);
    UseLinkedSamples->setChecked(
        HardwareGuiHelpers::getPropBool(Context, "UseLinkedAnalogSamples", true));
    ValueLabel->setText(
        tr("Adc value: %1").arg(HardwareGuiHelpers::getPropInt(Context, "AdcValue", 0)));
}

void HardwareArduinoAdcControllerWidget::onReadAdc()
{
    HardwareGuiHelpers::setProp(Context, "LinkedFirmataName", LinkedEdit->text());
    HardwareGuiHelpers::setProp(Context, "AnalogPin",
                                QString::number(PinCombo->currentData().toInt()));
    HardwareGuiHelpers::setProp(Context, "UseLinkedAnalogSamples",
                                UseLinkedSamples->isChecked() ? QStringLiteral("1")
                                                              : QStringLiteral("0"));
    HardwareGuiHelpers::pulseEdge(Context, "ReadAdcFlag");
    refreshFromModel(true);
}
