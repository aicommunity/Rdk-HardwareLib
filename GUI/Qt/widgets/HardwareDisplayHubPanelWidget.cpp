#include "HardwareDisplayHubPanelWidget.h"

#include "HardwareGuiHelpers.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

HardwareDisplayHubPanelWidget::HardwareDisplayHubPanelWidget(QWidget* parent)
    : QWidget(parent)
{
    RowSpin = new QSpinBox(this);
    RowSpin->setRange(0, 7);
    ColSpin = new QSpinBox(this);
    ColSpin->setRange(0, 31);
    TextEdit = new QLineEdit(this);
    TextEdit->setText(QStringLiteral("hello"));

    auto* form = new QFormLayout();
    HardwareGuiHelpers::applyCompactForm(form);
    form->addRow(tr("Row:"), RowSpin);
    form->addRow(tr("Col:"), ColSpin);
    form->addRow(tr("Text:"), TextEdit);

    auto* printBtn = new QPushButton(tr("PRINT"), this);
    auto* oledPrintBtn = new QPushButton(tr("OLED PRINT"), this);
    auto* clearBtn = new QPushButton(tr("CLEAR"), this);
    auto* oledClearBtn = new QPushButton(tr("OLED CLEAR"), this);
    auto* pingBtn = new QPushButton(tr("PING"), this);

    connect(printBtn, &QPushButton::clicked, this, [this]() {
        emit commandRequested(QStringLiteral("PRINT %1 %2 %3")
                                  .arg(RowSpin->value())
                                  .arg(ColSpin->value())
                                  .arg(TextEdit->text()));
    });
    connect(oledPrintBtn, &QPushButton::clicked, this, [this]() {
        emit commandRequested(QStringLiteral("OLED PRINT %1").arg(TextEdit->text()));
    });
    connect(clearBtn, &QPushButton::clicked, this,
            [this]() { emit commandRequested(QStringLiteral("CLEAR")); });
    connect(oledClearBtn, &QPushButton::clicked, this,
            [this]() { emit commandRequested(QStringLiteral("OLED CLEAR")); });
    connect(pingBtn, &QPushButton::clicked, this,
            [this]() { emit commandRequested(QStringLiteral("PING")); });

    auto* btns = new QHBoxLayout();
    btns->addWidget(printBtn);
    btns->addWidget(oledPrintBtn);
    btns->addWidget(clearBtn);
    btns->addWidget(oledClearBtn);
    btns->addWidget(pingBtn);

    auto* root = new QVBoxLayout(this);
    HardwareGuiHelpers::applyCompactLayout(root);
    root->addLayout(form);
    root->addLayout(btns);
    root->addStretch(1);
    HardwareGuiHelpers::applyUnicodeFriendlyFont(this);
}
