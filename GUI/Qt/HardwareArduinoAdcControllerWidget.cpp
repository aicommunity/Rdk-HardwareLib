#include "HardwareArduinoAdcControllerWidget.h"

#include <QFormLayout>
#include <QVBoxLayout>

#include "widgets/HardwareGuiHelpers.h"

HardwareArduinoAdcControllerWidget::HardwareArduinoAdcControllerWidget(QWidget* parent,
                                                                       RDK::UApplication* app)
    : UVisualControllerWidget(parent, app)
{
    LinkedEdit = new QLineEdit(this);
    PinSpin = new QSpinBox(this);
    PinSpin->setRange(0, 15);
    ValueLabel = new QLabel(tr("Adc value: —"), this);

    auto* readBtn = new QPushButton(tr("Read ADC"), this);
    connect(readBtn, &QPushButton::clicked, this, &HardwareArduinoAdcControllerWidget::onReadAdc);

    auto* form = new QFormLayout();
    form->addRow(tr("Linked Firmata:"), LinkedEdit);
    form->addRow(tr("Analog pin:"), PinSpin);
    form->addRow(QString(), readBtn);
    form->addRow(QString(), ValueLabel);

    auto* root = new QVBoxLayout(this);
    root->addLayout(form);
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
    LinkedEdit->setText(HardwareGuiHelpers::getProp(Context, "LinkedFirmataName"));
    PinSpin->setValue(HardwareGuiHelpers::getPropInt(Context, "AnalogPin", 0));
    ValueLabel->setText(
        tr("Adc value: %1").arg(HardwareGuiHelpers::getPropInt(Context, "AdcValue", 0)));
}

void HardwareArduinoAdcControllerWidget::onReadAdc()
{
    HardwareGuiHelpers::setProp(Context, "LinkedFirmataName", LinkedEdit->text());
    HardwareGuiHelpers::setProp(Context, "AnalogPin", QString::number(PinSpin->value()));
    HardwareGuiHelpers::pulseEdge(Context, "ReadAdcFlag");
    refreshFromModel(true);
}
