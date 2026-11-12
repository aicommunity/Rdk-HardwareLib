#include "HardwareWheeledDrivePanelWidget.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

#include "HardwareGuiHelpers.h"

HardwareWheeledDrivePanelWidget::HardwareWheeledDrivePanelWidget(QWidget* parent)
    : QWidget(parent)
{
    LeftPwm = new QSlider(Qt::Horizontal, this);
    LeftPwm->setRange(0, 255);
    RightPwm = new QSlider(Qt::Horizontal, this);
    RightPwm->setRange(0, 255);
    LeftDir = new QCheckBox(tr("Left forward"), this);
    LeftDir->setChecked(true);
    RightDir = new QCheckBox(tr("Right forward"), this);
    RightDir->setChecked(true);
    WatchdogMs = new QSpinBox(this);
    WatchdogMs->setRange(0, 60000);
    WatchdogMs->setValue(2000);
    MotorDriver = new QComboBox(this);
    MotorDriver->setEditable(true);
    MotorDriver->addItems({QStringLiteral("motor_shield_r3"), QStringLiteral("wire_l298n"),
                           QStringLiteral("tb6612fng_motor_driver")});
    FbLabel = new QLabel(tr("FB: —"), this);

    auto* applyBtn = new QPushButton(tr("Apply drive"), this);
    auto* stopBtn = new QPushButton(tr("Stop"), this);
    auto* mdBtn = new QPushButton(tr("Apply motor driver"), this);
    connect(applyBtn, &QPushButton::clicked, this, &HardwareWheeledDrivePanelWidget::onApply);
    connect(stopBtn, &QPushButton::clicked, this, &HardwareWheeledDrivePanelWidget::onStop);
    connect(mdBtn, &QPushButton::clicked, this, &HardwareWheeledDrivePanelWidget::onApplyMotorDriver);

    auto* form = new QFormLayout(this);
    HardwareGuiHelpers::applyCompactForm(form);
    form->addRow(tr("Left PWM"), LeftPwm);
    form->addRow(tr("Right PWM"), RightPwm);
    form->addRow(QString(), LeftDir);
    form->addRow(QString(), RightDir);
    form->addRow(tr("Watchdog ms"), WatchdogMs);
    MotorDriverRow = new QWidget(this);
    auto* mdLay = new QHBoxLayout(MotorDriverRow);
    mdLay->setContentsMargins(0, 0, 0, 0);
    mdLay->addWidget(MotorDriver, 1);
    mdLay->addWidget(mdBtn);
    form->addRow(tr("MotorDriverId"), MotorDriverRow);
    auto* btnRow = new QHBoxLayout();
    btnRow->addWidget(applyBtn);
    btnRow->addWidget(stopBtn);
    form->addRow(QString(), btnRow);
    form->addRow(QString(), FbLabel);
}

void HardwareWheeledDrivePanelWidget::setContext(const UComponentGuiContext& context)
{
    Context = context;
    refreshFromModel();
}

void HardwareWheeledDrivePanelWidget::setMotorDriverVisible(bool visible)
{
    if (MotorDriverRow)
        MotorDriverRow->setVisible(visible);
}

void HardwareWheeledDrivePanelWidget::refreshFromModel()
{
    if (Context.componentLongName.isEmpty())
        return;
    LeftPwm->setValue(HardwareGuiHelpers::getPropInt(Context, "LeftPwm", 0));
    RightPwm->setValue(HardwareGuiHelpers::getPropInt(Context, "RightPwm", 0));
    LeftDir->setChecked(HardwareGuiHelpers::getPropInt(Context, "LeftDir", 1) != 0);
    RightDir->setChecked(HardwareGuiHelpers::getPropInt(Context, "RightDir", 1) != 0);
    WatchdogMs->setValue(HardwareGuiHelpers::getPropInt(Context, "WatchdogMs", 2000));
    const QString md = HardwareGuiHelpers::getProp(Context, "MotorDriverId");
    if (!md.isEmpty())
        MotorDriver->setCurrentText(md);
    const int lfb = HardwareGuiHelpers::getPropInt(Context, "LeftPwmFb", 0);
    const int rfb = HardwareGuiHelpers::getPropInt(Context, "RightPwmFb", 0);
    FbLabel->setText(tr("FB L/R PWM: %1 / %2").arg(lfb).arg(rfb));
}

void HardwareWheeledDrivePanelWidget::onApply()
{
    HardwareGuiHelpers::setProp(Context, "LeftPwm", QString::number(LeftPwm->value()));
    HardwareGuiHelpers::setProp(Context, "RightPwm", QString::number(RightPwm->value()));
    HardwareGuiHelpers::setProp(Context, "LeftDir", LeftDir->isChecked() ? QStringLiteral("1") : QStringLiteral("0"));
    HardwareGuiHelpers::setProp(Context, "RightDir", RightDir->isChecked() ? QStringLiteral("1") : QStringLiteral("0"));
    HardwareGuiHelpers::setProp(Context, "WatchdogMs", QString::number(WatchdogMs->value()));
    HardwareGuiHelpers::pulseEdge(Context, "ApplyDrive");
}

void HardwareWheeledDrivePanelWidget::onStop()
{
    HardwareGuiHelpers::pulseEdge(Context, "Stop");
}

void HardwareWheeledDrivePanelWidget::onApplyMotorDriver()
{
    HardwareGuiHelpers::setProp(Context, "MotorDriverId", MotorDriver->currentText());
    HardwareGuiHelpers::pulseEdge(Context, "ApplyMotorDriver");
}
