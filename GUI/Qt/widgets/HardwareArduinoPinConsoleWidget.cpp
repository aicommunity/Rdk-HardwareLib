#include "HardwareArduinoPinConsoleWidget.h"

#include "HardwareGuiHelpers.h"

#include <Transport/UArduinoPinMap.h>

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPushButton>
#include <QVBoxLayout>

HardwareArduinoPinConsoleWidget::HardwareArduinoPinConsoleWidget(QWidget* parent)
    : QWidget(parent)
{
    Table = new QTableWidget(this);
    Table->setColumnCount(6);
    Table->setHorizontalHeaderLabels(
        {tr("Label"), tr("Firmata #"), tr("Mode"), tr("Digital"), tr("Analog"), tr("PWM")});
    Table->horizontalHeader()->setStretchLastSection(true);
    Table->setSelectionBehavior(QAbstractItemView::SelectRows);
    Table->setSelectionMode(QAbstractItemView::SingleSelection);

    PresetCombo = new QComboBox(this);
    PresetCombo->addItem(tr("(none)"), QString());
    PresetCombo->addItem(tr("Uno D13 blink"), QStringLiteral("uno_d13_blink"));
    PresetCombo->addItem(tr("Uno A0 monitor"), QStringLiteral("uno_a0_monitor"));
    PresetCombo->addItem(tr("Mega D13 blink"), QStringLiteral("mega_d13_blink"));
    PresetCombo->addItem(tr("Mega A0 monitor"), QStringLiteral("mega_a0_monitor"));
    connect(PresetCombo, QOverload<int>::of(&QComboBox::activated), this, [this](int) {
        if (Context.componentLongName.isEmpty())
            return;
        const QString id = PresetCombo->currentData().toString();
        if (id.isEmpty())
            return;
        HardwareGuiHelpers::setProp(Context, "PinConfigPreset", id);
        HardwareGuiHelpers::pulseEdge(Context, "LoadPreset");
        emit calculateRequested();
    });

    DigitalSpin = new QSpinBox(this);
    DigitalSpin->setRange(0, 1);
    PwmSpin = new QSpinBox(this);
    PwmSpin->setRange(0, 255);
    ServoAngleSpin = new QSpinBox(this);
    ServoAngleSpin->setRange(0, 180);
    ServoAngleSpin->setValue(90);

    auto* applyBtn = new QPushButton(tr("Apply selected"), this);
    auto* modeBtn = new QPushButton(tr("Set mode"), this);
    auto* writeBtn = new QPushButton(tr("Write"), this);
    auto* readBtn = new QPushButton(tr("Read analog"), this);
    auto* pwmBtn = new QPushButton(tr("Write PWM"), this);
    auto* servoBtn = new QPushButton(tr("Write servo"), this);
    auto* monitorBtn = new QPushButton(tr("Monitor all"), this);
    connect(applyBtn, &QPushButton::clicked, this, &HardwareArduinoPinConsoleWidget::onApplySelected);
    connect(modeBtn, &QPushButton::clicked, this, &HardwareArduinoPinConsoleWidget::onSetPinMode);
    connect(writeBtn, &QPushButton::clicked, this, &HardwareArduinoPinConsoleWidget::onWriteDigital);
    connect(readBtn, &QPushButton::clicked, this, &HardwareArduinoPinConsoleWidget::onReadAnalog);
    connect(pwmBtn, &QPushButton::clicked, this, &HardwareArduinoPinConsoleWidget::onWritePwm);
    connect(servoBtn, &QPushButton::clicked, this, [this]() {
        applyToModel();
        HardwareGuiHelpers::setProp(Context, "ServoPin",
                                    QString::number(HardwareGuiHelpers::getPropInt(Context, "SelectedPin", 9)));
        HardwareGuiHelpers::setProp(Context, "ServoAngle", QString::number(ServoAngleSpin->value()));
        HardwareGuiHelpers::pulseEdge(Context, "WriteServo");
        emit calculateRequested();
    });
    connect(monitorBtn, &QPushButton::clicked, this, &HardwareArduinoPinConsoleWidget::onMonitorAll);
    connect(Table, &QTableWidget::itemSelectionChanged, this,
            &HardwareArduinoPinConsoleWidget::onRowSelectionChanged);

    auto* valueRow = new QHBoxLayout();
    valueRow->addWidget(new QLabel(tr("Digital:"), this));
    valueRow->addWidget(DigitalSpin);
    valueRow->addWidget(new QLabel(tr("PWM:"), this));
    valueRow->addWidget(PwmSpin);
    valueRow->addWidget(new QLabel(tr("Servo °:"), this));
    valueRow->addWidget(ServoAngleSpin);
    valueRow->addStretch();

    auto* toolbar = new QHBoxLayout();
    toolbar->addWidget(new QLabel(tr("Preset:"), this));
    toolbar->addWidget(PresetCombo, 1);
    toolbar->addWidget(applyBtn);
    toolbar->addWidget(modeBtn);
    toolbar->addWidget(writeBtn);
    toolbar->addWidget(readBtn);
    toolbar->addWidget(pwmBtn);
    toolbar->addWidget(servoBtn);
    toolbar->addWidget(monitorBtn);

    auto* root = new QVBoxLayout(this);
    root->addLayout(toolbar);
    root->addLayout(valueRow);
    root->addWidget(Table, 1);
    HardwareGuiHelpers::applyUnicodeFriendlyFont(this);
}

void HardwareArduinoPinConsoleWidget::setContext(const UComponentGuiContext& context)
{
    Context = context;
    refreshFromModel();
}

void HardwareArduinoPinConsoleWidget::rebuildTable(int profile)
{
    const int max_pin = RDK::UArduinoPinMap::maxFirmataPin(profile);
    Table->setRowCount(max_pin + 1);
    for (int pin = 0; pin <= max_pin; ++pin) {
        const QString label = RDK::UArduinoPinMap::labelForFirmataPin(pin, profile);
        Table->setItem(pin, 0, new QTableWidgetItem(label));
        Table->item(pin, 0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        Table->setItem(pin, 1, new QTableWidgetItem(QString::number(pin)));
        Table->item(pin, 1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

        auto* mode = new QComboBox(Table);
        mode->addItem(tr("Input"), 0);
        mode->addItem(tr("Output"), 1);
        mode->addItem(tr("Analog"), 2);
        mode->addItem(tr("PWM"), 3);
        Table->setCellWidget(pin, 2, mode);

        Table->setItem(pin, 3, new QTableWidgetItem(QStringLiteral("—")));
        Table->setItem(pin, 4, new QTableWidgetItem(QStringLiteral("—")));
        Table->setItem(pin, 5, new QTableWidgetItem(QStringLiteral("—")));
    }
    LastProfile = profile;
}

void HardwareArduinoPinConsoleWidget::emitSelectedPin()
{
    const int profile = HardwareGuiHelpers::getPropInt(Context, "BoardProfile", 0);
    const QList<QTableWidgetItem*> selected = Table->selectedItems();
    if (selected.isEmpty())
        return;
    emit selectedPinChanged(selected.first()->row(), profile);
}

void HardwareArduinoPinConsoleWidget::refreshFromModel()
{
    if (Context.componentLongName.isEmpty())
        return;

    const int profile = HardwareGuiHelpers::getPropInt(Context, "BoardProfile", 0);
    if (profile != LastProfile)
        rebuildTable(profile);

    const int selected = HardwareGuiHelpers::getPropInt(Context, "SelectedPin", 13);
    const int mode = HardwareGuiHelpers::getPropInt(Context, "SelectedPinMode", 1);
    const int digital = HardwareGuiHelpers::getPropInt(Context, "DigitalPinValue", 0);
    const int pwm = HardwareGuiHelpers::getPropInt(Context, "PwmPinValue", 0);

    QMap<int, QJsonObject> pin_map;
    const QString json = HardwareGuiHelpers::getProp(Context, "PinStatusJson");
    const QJsonArray pins = QJsonDocument::fromJson(json.toUtf8()).object().value(QStringLiteral("pins")).toArray();
    for (const QJsonValue& v : pins) {
        const QJsonObject o = v.toObject();
        pin_map.insert(o.value(QStringLiteral("firmataPin")).toInt(), o);
    }

    for (int pin = 0; pin < Table->rowCount(); ++pin) {
        const QJsonObject o = pin_map.value(pin);
        if (!o.isEmpty()) {
            const int dig = o.value(QStringLiteral("digital")).toInt(-1);
            const int ana = o.value(QStringLiteral("analog")).toInt(-1);
            Table->item(pin, 3)->setText(dig >= 0 ? QString::number(dig) : QStringLiteral("—"));
            Table->item(pin, 4)->setText(ana >= 0 ? QString::number(ana) : QStringLiteral("—"));
            Table->item(pin, 5)->setText(
                pwm > 0 && pin == selected ? QString::number(pwm) : QStringLiteral("—"));
        }
        if (pin == selected) {
            if (auto* mode_combo = qobject_cast<QComboBox*>(Table->cellWidget(pin, 2))) {
                const int idx = mode_combo->findData(mode);
                if (idx >= 0)
                    mode_combo->setCurrentIndex(idx);
            }
            Table->selectRow(pin);
        }
    }

    DigitalSpin->setValue(digital);
    PwmSpin->setValue(pwm);
    ServoAngleSpin->setValue(HardwareGuiHelpers::getPropInt(Context, "ServoAngle", 90));
}

void HardwareArduinoPinConsoleWidget::syncSelectedRowToProps()
{
    const QList<QTableWidgetItem*> selected = Table->selectedItems();
    if (selected.isEmpty())
        return;
    const int row = selected.first()->row();
    HardwareGuiHelpers::setProp(Context, "SelectedPin", QString::number(row));
    if (auto* mode_combo = qobject_cast<QComboBox*>(Table->cellWidget(row, 2)))
        HardwareGuiHelpers::setProp(Context, "SelectedPinMode",
                                    QString::number(mode_combo->currentData().toInt()));
    HardwareGuiHelpers::setProp(Context, "DigitalPinValue", QString::number(DigitalSpin->value()));
    HardwareGuiHelpers::setProp(Context, "PwmPinValue", QString::number(PwmSpin->value()));
    emitSelectedPin();
}

void HardwareArduinoPinConsoleWidget::applyToModel()
{
    syncSelectedRowToProps();
}

void HardwareArduinoPinConsoleWidget::onRowSelectionChanged()
{
    syncSelectedRowToProps();
}

void HardwareArduinoPinConsoleWidget::onApplySelected()
{
    applyToModel();
    emit calculateRequested();
}

void HardwareArduinoPinConsoleWidget::onSetPinMode()
{
    applyToModel();
    HardwareGuiHelpers::pulseEdge(Context, "SetPinMode");
    emit calculateRequested();
}

void HardwareArduinoPinConsoleWidget::onWriteDigital()
{
    applyToModel();
    HardwareGuiHelpers::pulseEdge(Context, "WriteDigital");
    emit calculateRequested();
}

void HardwareArduinoPinConsoleWidget::onReadAnalog()
{
    applyToModel();
    HardwareGuiHelpers::pulseEdge(Context, "ReadAnalog");
    emit calculateRequested();
}

void HardwareArduinoPinConsoleWidget::onWritePwm()
{
    applyToModel();
    HardwareGuiHelpers::pulseEdge(Context, "WritePwm");
    emit calculateRequested();
}

void HardwareArduinoPinConsoleWidget::onMonitorAll()
{
    applyToModel();
    HardwareGuiHelpers::setProp(Context, "AutoRefreshPins", QStringLiteral("1"));
    emit calculateRequested();
}
