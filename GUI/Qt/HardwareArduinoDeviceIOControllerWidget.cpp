#include "HardwareArduinoDeviceIOControllerWidget.h"

#include <QFormLayout>
#include <QPushButton>
#include <QVBoxLayout>

#include "Catalog/UHardwareCatalog.h"
#include "widgets/HardwareGuiHelpers.h"

HardwareArduinoDeviceIOControllerWidget::HardwareArduinoDeviceIOControllerWidget(
    QWidget* parent, RDK::UApplication* app)
    : UVisualControllerWidget(parent, app)
{
    LinkedEdit = new QLineEdit(this);
    ModuleCombo = new QComboBox(this);
    PortEdit = new QLineEdit(this);
    ChannelEdit = new QLineEdit(this);
    RoleCombo = new QComboBox(this);
    RoleCombo->addItem(tr("Sensor"), 0);
    RoleCombo->addItem(tr("Actuator"), 1);
    RoleCombo->addItem(tr("Auto"), 2);
    ValueInSpin = new QDoubleSpinBox(this);
    ValueInSpin->setRange(-1.0, 1.0);
    ValueInSpin->setSingleStep(0.05);
    ContinuousCheck = new QCheckBox(tr("Continuous sensor"), this);
    ValueLabel = new QLabel(tr("Value: \u2014"), this);
    StatusLabel = new QLabel(this);

    auto* applyBtn = new QPushButton(tr("Apply config"), this);
    auto* writeBtn = new QPushButton(tr("Write output"), this);
    auto* readBtn = new QPushButton(tr("Read input"), this);
    connect(applyBtn, &QPushButton::clicked, this, &HardwareArduinoDeviceIOControllerWidget::onApplyConfig);
    connect(writeBtn, &QPushButton::clicked, this, &HardwareArduinoDeviceIOControllerWidget::onWriteOutput);
    connect(readBtn, &QPushButton::clicked, this, &HardwareArduinoDeviceIOControllerWidget::onReadInput);

    auto* form = new QFormLayout;
    form->addRow(tr("Linked Firmata:"), LinkedEdit);
    form->addRow(tr("Module:"), ModuleCombo);
    form->addRow(tr("Port:"), PortEdit);
    form->addRow(tr("Channel:"), ChannelEdit);
    form->addRow(tr("Role:"), RoleCombo);
    form->addRow(tr("ValueIn:"), ValueInSpin);
    form->addRow(QString(), ContinuousCheck);
    form->addRow(QString(), applyBtn);
    form->addRow(QString(), writeBtn);
    form->addRow(QString(), readBtn);
    form->addRow(QString(), ValueLabel);
    form->addRow(QString(), StatusLabel);

    auto* root = new QVBoxLayout(this);
    root->addLayout(form);
    HardwareGuiHelpers::applyUnicodeFriendlyFont(this);

    RDK::UHardwareCatalog& cat = RDK::UHardwareCatalog::instance();
    if (!cat.isLoaded())
        cat.load(nullptr);
    for (const QString& id : cat.moduleIds())
        ModuleCombo->addItem(id, id);
}

void HardwareArduinoDeviceIOControllerWidget::setComponentContext(const UComponentGuiContext& context)
{
    Context = context;
    refreshFromModel(true);
}

QString HardwareArduinoDeviceIOControllerWidget::componentGuiId() const
{
    return QStringLiteral("hw.arduino.device_io");
}

void HardwareArduinoDeviceIOControllerWidget::refreshFromModel(bool force)
{
    Q_UNUSED(force);
    if (Context.componentLongName.isEmpty())
        return;
    LinkedEdit->setText(HardwareGuiHelpers::getProp(Context, "LinkedFirmataName"));
    const QString module = HardwareGuiHelpers::getProp(Context, "ModuleId");
    const int mi = ModuleCombo->findData(module);
    if (mi >= 0)
        ModuleCombo->setCurrentIndex(mi);
    PortEdit->setText(HardwareGuiHelpers::getProp(Context, "Port"));
    ChannelEdit->setText(HardwareGuiHelpers::getProp(Context, "Channel"));
    const int role = HardwareGuiHelpers::getPropInt(Context, "Role", 2);
    const int ri = RoleCombo->findData(role);
    if (ri >= 0)
        RoleCombo->setCurrentIndex(ri);
    ValueInSpin->setValue(HardwareGuiHelpers::getProp(Context, "ValueIn").toDouble());
    ContinuousCheck->setChecked(HardwareGuiHelpers::getPropBool(Context, "Continuous", true));
    ValueLabel->setText(tr("Value: %1 (raw %2)")
                            .arg(HardwareGuiHelpers::getProp(Context, "Value"))
                            .arg(HardwareGuiHelpers::getPropInt(Context, "ValueRaw", 0)));
    StatusLabel->setText(
        tr("Online: %1  Error: %2")
            .arg(HardwareGuiHelpers::getPropBool(Context, "IsOnline", false) ? tr("yes") : tr("no"))
            .arg(HardwareGuiHelpers::getProp(Context, "LastError")));
}

void HardwareArduinoDeviceIOControllerWidget::applyFieldsToModel()
{
    HardwareGuiHelpers::setProp(Context, "LinkedFirmataName", LinkedEdit->text());
    HardwareGuiHelpers::setProp(Context, "ModuleId", ModuleCombo->currentData().toString());
    HardwareGuiHelpers::setProp(Context, "Port", PortEdit->text());
    HardwareGuiHelpers::setProp(Context, "Channel", ChannelEdit->text());
    HardwareGuiHelpers::setProp(Context, "Role", QString::number(RoleCombo->currentData().toInt()));
    HardwareGuiHelpers::setProp(Context, "ValueIn", QString::number(ValueInSpin->value()));
    HardwareGuiHelpers::setProp(Context, "Continuous",
                                ContinuousCheck->isChecked() ? QStringLiteral("1")
                                                             : QStringLiteral("0"));
}

void HardwareArduinoDeviceIOControllerWidget::onApplyConfig()
{
    applyFieldsToModel();
    HardwareGuiHelpers::pulseEdge(Context, "ApplyConfig");
    refreshFromModel(true);
}

void HardwareArduinoDeviceIOControllerWidget::onWriteOutput()
{
    applyFieldsToModel();
    HardwareGuiHelpers::pulseEdge(Context, "WriteOutput");
    refreshFromModel(true);
}

void HardwareArduinoDeviceIOControllerWidget::onReadInput()
{
    applyFieldsToModel();
    HardwareGuiHelpers::pulseEdge(Context, "ReadInput");
    refreshFromModel(true);
}
