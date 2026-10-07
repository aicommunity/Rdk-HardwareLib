#include "HardwareUartDeviceHubPanelWidget.h"

#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

HardwareUartDeviceHubPanelWidget::HardwareUartDeviceHubPanelWidget(QWidget* parent)
    : QWidget(parent)
{
    Warning = new QLabel(
        tr("Device UART: Serial1 on Mega/ESP32, else SoftSerial D10/D11. USB Serial = hub."), this);
    Warning->setWordWrap(true);
    TxEdit = new QPlainTextEdit(this);
    TxEdit->setPlaceholderText(tr("TX line / HMI payload…"));
    TxEdit->setMaximumHeight(80);
    LastRx = new QLabel(tr("Last RX: —"), this);
    LastRx->setWordWrap(true);

    auto* hmiBtn = new QPushButton(tr("HMI TX"), this);
    auto* atBtn = new QPushButton(tr("AT"), this);
    auto* gpsBtn = new QPushButton(tr("AT+GPSRD"), this);
    auto* bridgeOn = new QPushButton(tr("BRIDGE ON"), this);
    auto* bridgeOff = new QPushButton(tr("BRIDGE OFF"), this);
    auto* pingBtn = new QPushButton(tr("PING"), this);

    connect(hmiBtn, &QPushButton::clicked, this, [this]() {
        emit commandRequested(QStringLiteral("HMI TX %1").arg(TxEdit->toPlainText().trimmed()));
    });
    connect(atBtn, &QPushButton::clicked, this, [this]() { emit commandRequested(QStringLiteral("AT")); });
    connect(gpsBtn, &QPushButton::clicked, this,
            [this]() { emit commandRequested(QStringLiteral("AT+GPSRD")); });
    connect(bridgeOn, &QPushButton::clicked, this,
            [this]() { emit commandRequested(QStringLiteral("BRIDGE ON")); });
    connect(bridgeOff, &QPushButton::clicked, this,
            [this]() { emit commandRequested(QStringLiteral("BRIDGE OFF")); });
    connect(pingBtn, &QPushButton::clicked, this,
            [this]() { emit commandRequested(QStringLiteral("PING")); });

    auto* row = new QHBoxLayout();
    row->addWidget(hmiBtn);
    row->addWidget(atBtn);
    row->addWidget(gpsBtn);
    row->addWidget(bridgeOn);
    row->addWidget(bridgeOff);
    row->addWidget(pingBtn);

    auto* root = new QVBoxLayout(this);
    HardwareGuiHelpers::applyCompactLayout(root);
    root->addWidget(Warning);
    root->addWidget(TxEdit);
    root->addLayout(row);
    root->addWidget(LastRx);
    root->addStretch(1);
    HardwareGuiHelpers::applyUnicodeFriendlyFont(this);
}

void HardwareUartDeviceHubPanelWidget::setContext(const UComponentGuiContext& context)
{
    Context = context;
    refreshFromModel();
}

void HardwareUartDeviceHubPanelWidget::refreshFromModel()
{
    if (Context.componentLongName.isEmpty())
        return;
    const QString json = HardwareGuiHelpers::getProp(Context, "NamedValuesJson");
    const QJsonObject obj = QJsonDocument::fromJson(json.toUtf8()).object();
    const QString line = obj.value(QStringLiteral("last_line")).toString();
    if (!line.isEmpty())
        LastRx->setText(tr("Last RX: %1").arg(line));
    else
        LastRx->setText(tr("Last RX len: %1")
                            .arg(obj.value(QStringLiteral("last_line_len")).toDouble()));
}
