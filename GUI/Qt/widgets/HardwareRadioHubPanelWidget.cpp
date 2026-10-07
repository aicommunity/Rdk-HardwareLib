#include "HardwareRadioHubPanelWidget.h"

#include <QCheckBox>
#include <QFormLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

HardwareRadioHubPanelWidget::HardwareRadioHubPanelWidget(QWidget* parent)
    : QWidget(parent)
{
    HexEdit = new QLineEdit(this);
    HexEdit->setText(QStringLiteral("010203"));
    HexEdit->setPlaceholderText(tr("hex payload"));
    LastRx = new QLabel(tr("—"), this);
    UidLabel = new QLabel(tr("—"), this);
    Esp32ModeCheck = new QCheckBox(tr("ESP32 board mode (Wi‑Fi twin upload stub)"), this);

    auto* form = new QFormLayout();
    HardwareGuiHelpers::applyCompactForm(form);
    form->addRow(tr("RADIO SEND hex:"), HexEdit);
    form->addRow(tr("Last RX len/rssi:"), LastRx);
    form->addRow(tr("UID:"), UidLabel);
    form->addRow(QString(), Esp32ModeCheck);

    auto* sendBtn = new QPushButton(tr("RADIO SEND"), this);
    auto* pingBtn = new QPushButton(tr("PING"), this);
    connect(sendBtn, &QPushButton::clicked, this, [this]() {
        emit commandRequested(QStringLiteral("RADIO SEND %1").arg(HexEdit->text().trimmed()));
    });
    connect(pingBtn, &QPushButton::clicked, this,
            [this]() { emit commandRequested(QStringLiteral("PING")); });
    connect(Esp32ModeCheck, &QCheckBox::toggled, this, &HardwareRadioHubPanelWidget::esp32ModeChanged);

    auto* root = new QVBoxLayout(this);
    HardwareGuiHelpers::applyCompactLayout(root);
    root->addLayout(form);
    root->addWidget(sendBtn);
    root->addWidget(pingBtn);
    root->addStretch(1);
    HardwareGuiHelpers::applyUnicodeFriendlyFont(this);
}

void HardwareRadioHubPanelWidget::setContext(const UComponentGuiContext& context)
{
    Context = context;
    refreshFromModel();
}

void HardwareRadioHubPanelWidget::refreshFromModel()
{
    if (Context.componentLongName.isEmpty())
        return;
    const QString json = HardwareGuiHelpers::getProp(Context, "NamedValuesJson");
    const QJsonObject obj = QJsonDocument::fromJson(json.toUtf8()).object();
    const QString rx = QStringLiteral("len=%1 rssi=%2")
                           .arg(obj.value(QStringLiteral("rx_len")).toDouble())
                           .arg(obj.value(QStringLiteral("rssi")).toDouble());
    LastRx->setText(rx);
    UidLabel->setText(QString::number(obj.value(QStringLiteral("uid")).toDouble(), 'g', 12));
}

bool HardwareRadioHubPanelWidget::esp32BoardMode() const
{
    return Esp32ModeCheck && Esp32ModeCheck->isChecked();
}
