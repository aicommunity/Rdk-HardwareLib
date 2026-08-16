#ifndef HARDWAREARDUINOCUSTOMFIRMWARECONTROLLERWIDGET_H
#define HARDWAREARDUINOCUSTOMFIRMWARECONTROLLERWIDGET_H

#include <QLineEdit>
#include <QLabel>
#include <QPlainTextEdit>
#include <QTabWidget>

#include "../../../../Rdk/GUI/Qt/IComponentControllerWidget.h"
#include "../../../../Rdk/GUI/Qt/UComponentGuiContext.h"
#include "../../../../Rdk/GUI/Qt/UVisualControllerWidget.h"

class HardwareArduinoAssemblyTabHost;
class HardwareArduinoBoardPanelWidget;

class HardwareArduinoCustomFirmwareControllerWidget : public UVisualControllerWidget,
                                                      public IComponentControllerWidget {
    Q_OBJECT
public:
    explicit HardwareArduinoCustomFirmwareControllerWidget(QWidget* parent = nullptr,
                                                           RDK::UApplication* app = nullptr);

    void setComponentContext(const UComponentGuiContext& context) override;
    void refreshFromModel(bool force) override;
    QString componentGuiId() const override;

private slots:
    void onSend();
    void onClearLog();

private:
    UComponentGuiContext Context;
    QTabWidget* Tabs = nullptr;
    HardwareArduinoBoardPanelWidget* BoardPanel = nullptr;
    HardwareArduinoAssemblyTabHost* AssemblyTab = nullptr;
    QLineEdit* PluginEdit = nullptr;
    QLineEdit* CommandEdit = nullptr;
    QLabel* PluginStatus = nullptr;
    QPlainTextEdit* FrameLogView = nullptr;
};

#endif
