#include "HardwareArduinoAdcControllerWidget.h"

#include <QFormLayout>
#include <QVBoxLayout>

#include "widgets/HardwareGuiHelpers.h"

HardwareArduinoAdcControllerWidget::HardwareArduinoAdcControllerWidget(QWidget* parent,
                                                                       RDK::UApplication* app)
    : UVisualControllerWidget(parent, app)
{
    m_linkedEdit = new QLineEdit(this);
    m_pinSpin = new QSpinBox(this);
    m_pinSpin->setRange(0, 15);
    m_valueLabel = new QLabel(tr("Adc value: —"), this);

    auto* readBtn = new QPushButton(tr("Read ADC"), this);
    connect(readBtn, &QPushButton::clicked, this, &HardwareArduinoAdcControllerWidget::onReadAdc);

    auto* form = new QFormLayout();
    form->addRow(tr("Linked Firmata:"), m_linkedEdit);
    form->addRow(tr("Analog pin:"), m_pinSpin);
    form->addRow(QString(), readBtn);
    form->addRow(QString(), m_valueLabel);

    auto* root = new QVBoxLayout(this);
    root->addLayout(form);
}

void HardwareArduinoAdcControllerWidget::setComponentContext(const UComponentGuiContext& context)
{
    m_context = context;
    refreshFromModel(true);
}

QString HardwareArduinoAdcControllerWidget::componentGuiId() const
{
    return QStringLiteral("hw.arduino.adc");
}

void HardwareArduinoAdcControllerWidget::refreshFromModel(bool force)
{
    Q_UNUSED(force);
    if (m_context.componentLongName.isEmpty())
        return;
    m_linkedEdit->setText(HardwareGuiHelpers::getProp(m_context, "LinkedFirmataName"));
    m_pinSpin->setValue(HardwareGuiHelpers::getPropInt(m_context, "AnalogPin", 0));
    m_valueLabel->setText(
        tr("Adc value: %1").arg(HardwareGuiHelpers::getPropInt(m_context, "AdcValue", 0)));
}

void HardwareArduinoAdcControllerWidget::onReadAdc()
{
    HardwareGuiHelpers::setProp(m_context, "LinkedFirmataName", m_linkedEdit->text());
    HardwareGuiHelpers::setProp(m_context, "AnalogPin", QString::number(m_pinSpin->value()));
    HardwareGuiHelpers::pulseEdge(m_context, "ReadAdcFlag");
    refreshFromModel(true);
}
