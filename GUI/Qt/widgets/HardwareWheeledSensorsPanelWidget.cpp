#include "HardwareWheeledSensorsPanelWidget.h"

#include "HardwareHubTelemetryPanelWidget.h"

#include <QVBoxLayout>

HardwareWheeledSensorsPanelWidget::HardwareWheeledSensorsPanelWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* root = new QVBoxLayout(this);
    HardwareGuiHelpers::applyCompactLayout(root);
    Telemetry = new HardwareHubTelemetryPanelWidget(this);
    Telemetry->setFallbackJsonProp("LastFeedbackJson");
    root->addWidget(Telemetry, 1);
    HardwareGuiHelpers::applyUnicodeFriendlyFont(this);
}

void HardwareWheeledSensorsPanelWidget::setContext(const UComponentGuiContext& context)
{
    Telemetry->setContext(context);
}

void HardwareWheeledSensorsPanelWidget::refreshFromModel()
{
    Telemetry->refreshFromModel();
}
