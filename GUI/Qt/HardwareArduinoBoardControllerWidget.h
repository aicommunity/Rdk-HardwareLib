#ifndef HARDWAREARDUINOBOARDCONTROLLERWIDGET_H
#define HARDWAREARDUINOBOARDCONTROLLERWIDGET_H

#include "../../../../Rdk/GUI/Qt/IComponentControllerWidget.h"
#include "../../../../Rdk/GUI/Qt/UComponentGuiContext.h"
#include "../../../../Rdk/GUI/Qt/UVisualControllerWidget.h"
#include "widgets/UArduinoBoardDiagramWidget.h"

#include <QTabWidget>

class HardwareArduinoBoardPanelWidget;
class HardwareArduinoAssemblyTabHost;

class HardwareArduinoBoardControllerWidget : public UVisualControllerWidget, public IComponentControllerWidget {
    Q_OBJECT
public:
    explicit HardwareArduinoBoardControllerWidget(QWidget* parent = nullptr, RDK::UApplication* app = nullptr);

    void setComponentContext(const UComponentGuiContext& context) override;
    void refreshFromModel(bool force) override;
    QString componentGuiId() const override;

private:
    void updateDiagram();

    UComponentGuiContext Context;
    QTabWidget* LeftTabs = nullptr;
    UArduinoBoardDiagramWidget* Diagram = nullptr;
    HardwareArduinoBoardPanelWidget* BoardPanel = nullptr;
    HardwareArduinoAssemblyTabHost* AssemblyTab = nullptr;
};

#endif
