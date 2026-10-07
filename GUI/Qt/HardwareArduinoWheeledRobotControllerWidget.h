#ifndef HARDWAREARDUINOWHEELEDROBOTCONTROLLERWIDGET_H
#define HARDWAREARDUINOWHEELEDROBOTCONTROLLERWIDGET_H

#include "../../../../Rdk/GUI/Qt/IComponentControllerWidget.h"
#include "../../../../Rdk/GUI/Qt/UComponentGuiContext.h"
#include "../../../../Rdk/GUI/Qt/UVisualControllerWidget.h"

#include <QTabWidget>

class HardwareArduinoBoardPanelWidget;
class HardwareArduinoAssemblyTabHost;
class HardwareWheeledDrivePanelWidget;

class HardwareArduinoWheeledRobotControllerWidget : public UVisualControllerWidget,
                                                    public IComponentControllerWidget {
    Q_OBJECT
public:
    explicit HardwareArduinoWheeledRobotControllerWidget(QWidget* parent = nullptr,
                                                         RDK::UApplication* app = nullptr);
    void setComponentContext(const UComponentGuiContext& context) override;
    void refreshFromModel(bool force) override;
    QString componentGuiId() const override;

private:
    UComponentGuiContext Context;
    QTabWidget* Tabs = nullptr;
    HardwareWheeledDrivePanelWidget* Drive = nullptr;
    HardwareArduinoAssemblyTabHost* AssemblyTab = nullptr;
    HardwareArduinoBoardPanelWidget* BoardPanel = nullptr;
};

#endif
