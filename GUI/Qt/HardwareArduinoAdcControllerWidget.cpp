#include "HardwareArduinoAdcControllerWidget.h"

#include <QFormLayout>
#include <QPushButton>
#include <QVBoxLayout>

#include "widgets/HardwareArduinoAssemblyTabHost.h"
#include "widgets/HardwareGuiHelpers.h"

#include <Transport/UArduinoPinMap.h>

HardwareArduinoAdcControllerWidget::HardwareArduinoAdcControllerWidget(QWidget* parent,
                                                                       RDK::UApplication* app)
    : UVisualControllerWidget(parent, app)
{
    auto* adcPage = new QWidget(this);
    LinkedEdit = new QLineEdit(adcPage);
    PinCombo = new QComboBox(adcPage);
    UseLinkedSamples = new QCheckBox(tr("Use linked AnalogSamples"), adcPage);
    UseLinkedSamples->setChecked(true);
    ValueLabel = new QLabel(tr("Adc value: \u2014"), adcPage);

    auto* readBtn = new QPushButton(tr("Read ADC"), adcPage);
    connect(readBtn, &QPushButton::clicked, this, &HardwareArduinoAdcControllerWidget::onReadAdc);

    auto* form = new QFormLayout(adcPage);
    HardwareGuiHelpers::applyCompactForm(form);
    form->addRow(tr("Linked Firmata:"), LinkedEdit);
    form->addRow(tr("Analog pin:"), PinCombo);
    form->addRow(QString(), UseLinkedSamples);
    form->addRow(QString(), readBtn);
    form->addRow(QString(), ValueLabel);
    HardwareGuiHelpers::configureExpandingCombo(PinCombo, 6);

    AssemblyTab = new HardwareArduinoAssemblyTabHost(this);
    AssemblyTab->setReadOnly(true);
    Tabs = new QTabWidget(this);
    Tabs->setDocumentMode(true);
    Tabs->setUsesScrollButtons(true);
    Tabs->addTab(adcPage, tr("ADC"));
    Tabs->addTab(AssemblyTab, tr("Assembly"));

    auto* root = new QVBoxLayout(this);
    HardwareGuiHelpers::applyCompactLayout(root);
    root->addWidget(Tabs);
    HardwareGuiHelpers::applyUnicodeFriendlyFont(this);
}

void HardwareArduinoAdcControllerWidget::setComponentContext(const UComponentGuiContext& context)
{
    Context = context;
    AssemblyTab->setContext(context);
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

    AssemblyTab->refreshFromModel();

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
