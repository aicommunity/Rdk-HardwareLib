#ifndef HARDWAREESP32WHEELEDROBOTCONTROLLERWIDGET_H
#define HARDWAREESP32WHEELEDROBOTCONTROLLERWIDGET_H

#include "../../../../Rdk/GUI/Qt/IComponentControllerWidget.h"
#include "../../../../Rdk/GUI/Qt/UComponentGuiContext.h"
#include "../../../../Rdk/GUI/Qt/UVisualControllerWidget.h"

#include <QTabWidget>

class HardwareArduinoBoardPanelWidget;
class HardwareArduinoAssemblyTabHost;
class HardwareWheeledDrivePanelWidget;
class HardwareWheeledSensorsPanelWidget;

class HardwareEsp32WheeledRobotControllerWidget : public UVisualControllerWidget,
                                                  public IComponentControllerWidget {
    Q_OBJECT
public:
    explicit HardwareEsp32WheeledRobotControllerWidget(QWidget* parent = nullptr,
                                                       RDK::UApplication* app = nullptr);
    void setComponentContext(const UComponentGuiContext& context) override;
    void refreshFromModel(bool force) override;
    QString componentGuiId() const override;

private:
    UComponentGuiContext Context;
    QTabWidget* Tabs = nullptr;
    HardwareWheeledDrivePanelWidget* Drive = nullptr;
    HardwareWheeledSensorsPanelWidget* Sensors = nullptr;
    HardwareArduinoAssemblyTabHost* AssemblyTab = nullptr;
    HardwareArduinoBoardPanelWidget* BoardPanel = nullptr;
};

#endif
