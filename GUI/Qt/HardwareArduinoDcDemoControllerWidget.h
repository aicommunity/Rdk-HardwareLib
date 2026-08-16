#ifndef HARDWAREARDUINODCDEMOCONTROLLERWIDGET_H
#define HARDWAREARDUINODCDEMOCONTROLLERWIDGET_H

#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>

#include "../../../../Rdk/GUI/Qt/IComponentControllerWidget.h"
#include "../../../../Rdk/GUI/Qt/UComponentGuiContext.h"
#include "../../../../Rdk/GUI/Qt/UVisualControllerWidget.h"

class HardwareArduinoBoardPanelWidget;
class HardwareArduinoAssemblyTabHost;

class HardwareArduinoDcDemoControllerWidget : public UVisualControllerWidget,
                                              public IComponentControllerWidget {
    Q_OBJECT
public:
    explicit HardwareArduinoDcDemoControllerWidget(QWidget* parent = nullptr,
                                                   RDK::UApplication* app = nullptr);

    void setComponentContext(const UComponentGuiContext& context) override;
    void refreshFromModel(bool force) override;
    QString componentGuiId() const override;

private slots:
    void onSendCommand();
    void onGetSpeed();
    void onPreset(const QString& command);

private:
    UComponentGuiContext Context;
    QTabWidget* Tabs = nullptr;
    HardwareArduinoBoardPanelWidget* BoardPanel = nullptr;
    HardwareArduinoAssemblyTabHost* AssemblyTab = nullptr;
    QLineEdit* CommandEdit = nullptr;
    QLabel* SpeedLabel = nullptr;
};

#endif
