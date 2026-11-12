#include "HardwarePixelHubPanelWidget.h"

#include "HardwareGuiHelpers.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

HardwarePixelHubPanelWidget::HardwarePixelHubPanelWidget(QWidget* parent)
    : QWidget(parent)
{
    LedIndex = new QSpinBox(this);
    LedIndex->setRange(0, 255);
    RSpin = new QSpinBox(this);
    RSpin->setRange(0, 255);
    RSpin->setValue(32);
    GSpin = new QSpinBox(this);
    GSpin->setRange(0, 255);
    BSpin = new QSpinBox(this);
    BSpin->setRange(0, 255);
    Color565 = new QSpinBox(this);
    Color565->setRange(0, 65535);
    Color565->setValue(0xF800);
    TextEdit = new QLineEdit(this);
    TextEdit->setText(QStringLiteral("A"));

    auto* form = new QFormLayout();
    HardwareGuiHelpers::applyCompactForm(form);
    form->addRow(tr("LED index:"), LedIndex);
    form->addRow(tr("R:"), RSpin);
    form->addRow(tr("G:"), GSpin);
    form->addRow(tr("B:"), BSpin);
    form->addRow(tr("TFT rgb565:"), Color565);
    form->addRow(tr("Text:"), TextEdit);

    auto* root = new QVBoxLayout(this);
    HardwareGuiHelpers::applyCompactLayout(root);
    root->addLayout(form);

    auto addBtn = [this, root](const QString& label, auto fn) {
        auto* btn = new QPushButton(label, this);
        connect(btn, &QPushButton::clicked, this, fn);
        root->addWidget(btn);
    };
    addBtn(tr("LED SET"), [this]() {
        emit commandRequested(QStringLiteral("LED SET %1 %2 %3 %4")
                                  .arg(LedIndex->value())
                                  .arg(RSpin->value())
                                  .arg(GSpin->value())
                                  .arg(BSpin->value()));
    });
    addBtn(tr("LED FILL"), [this]() { emit commandRequested(QStringLiteral("LED FILL")); });
    addBtn(tr("LED SHOW"), [this]() { emit commandRequested(QStringLiteral("LED SHOW")); });
    addBtn(tr("MATRIX CLEAR"), [this]() { emit commandRequested(QStringLiteral("MATRIX CLEAR")); });
    addBtn(tr("MATRIX TEXT"), [this]() {
        emit commandRequested(QStringLiteral("MATRIX TEXT %1").arg(TextEdit->text()));
    });
    addBtn(tr("TFT FILL"), [this]() {
        emit commandRequested(QStringLiteral("TFT FILL %1").arg(Color565->value()));
    });
    addBtn(tr("TFT TEXT"), [this]() {
        emit commandRequested(QStringLiteral("TFT TEXT %1").arg(TextEdit->text()));
    });
    addBtn(tr("EPD CLEAR"), [this]() { emit commandRequested(QStringLiteral("EPD CLEAR")); });
    addBtn(tr("PING"), [this]() { emit commandRequested(QStringLiteral("PING")); });
    root->addStretch(1);
    HardwareGuiHelpers::applyUnicodeFriendlyFont(this);
}
