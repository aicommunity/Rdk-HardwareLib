#ifndef HARDWAREESP32BOARDCONTROLLERWIDGET_H
#define HARDWAREESP32BOARDCONTROLLERWIDGET_H

#include "../../../../Rdk/GUI/Qt/IComponentControllerWidget.h"
#include "../../../../Rdk/GUI/Qt/UComponentGuiContext.h"
#include "../../../../Rdk/GUI/Qt/UVisualControllerWidget.h"

#include <QTabWidget>

class HardwareArduinoBoardPanelWidget;
class HardwareArduinoAssemblyTabHost;

class HardwareEsp32BoardControllerWidget : public UVisualControllerWidget, public IComponentControllerWidget {
    Q_OBJECT
public:
    explicit HardwareEsp32BoardControllerWidget(QWidget* parent = nullptr, RDK::UApplication* app = nullptr);

    void setComponentContext(const UComponentGuiContext& context) override;
    void refreshFromModel(bool force) override;
    QString componentGuiId() const override;

private:
    UComponentGuiContext Context;
    QTabWidget* Tabs = nullptr;
    HardwareArduinoBoardPanelWidget* BoardPanel = nullptr;
    HardwareArduinoAssemblyTabHost* AssemblyTab = nullptr;
};

#endif
