#ifndef HARDWAREWHEELEDSENSORSPANELWIDGET_H
#define HARDWAREWHEELEDSENSORSPANELWIDGET_H

#include "HardwareGuiHelpers.h"

#include <QWidget>

class HardwareHubTelemetryPanelWidget;

/** Sensors tab: NamedValuesJson / matrix via shared hub telemetry panel. */
class HardwareWheeledSensorsPanelWidget : public QWidget {
    Q_OBJECT
public:
    explicit HardwareWheeledSensorsPanelWidget(QWidget* parent = nullptr);
    void setContext(const UComponentGuiContext& context);
    void refreshFromModel();

private:
    HardwareHubTelemetryPanelWidget* Telemetry = nullptr;
};

#endif
