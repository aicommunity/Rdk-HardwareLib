#include "HardwareWheeledSensorsPanelWidget.h"

#include <QLabel>
#include <QPlainTextEdit>
#include <QVBoxLayout>

HardwareWheeledSensorsPanelWidget::HardwareWheeledSensorsPanelWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* root = new QVBoxLayout(this);
    HardwareGuiHelpers::applyCompactLayout(root);
    root->addWidget(new QLabel(tr("Named values / sensor preview (hub plugin telemetry)"), this));
    Preview = new QPlainTextEdit(this);
    Preview->setReadOnly(true);
    Preview->setPlaceholderText(tr("NamedValuesJson or LastFeedbackJson…"));
    root->addWidget(Preview, 1);
    HardwareGuiHelpers::applyUnicodeFriendlyFont(this);
}

void HardwareWheeledSensorsPanelWidget::setContext(const UComponentGuiContext& context)
{
    Context = context;
    refreshFromModel();
}

void HardwareWheeledSensorsPanelWidget::refreshFromModel()
{
    if (Context.componentLongName.isEmpty())
        return;
    QString text = HardwareGuiHelpers::getProp(Context, "NamedValuesJson");
    if (text.isEmpty())
        text = HardwareGuiHelpers::getProp(Context, "LastFeedbackJson");
    Preview->setPlainText(text);
}
