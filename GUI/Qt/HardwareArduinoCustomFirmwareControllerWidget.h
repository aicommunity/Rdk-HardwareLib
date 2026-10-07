#ifndef HARDWAREARDUINOCUSTOMFIRMWARECONTROLLERWIDGET_H
#define HARDWAREARDUINOCUSTOMFIRMWARECONTROLLERWIDGET_H

#include <initializer_list>
#include <utility>

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QStackedWidget>
#include <QTabWidget>

#include "../../../../Rdk/GUI/Qt/IComponentControllerWidget.h"
#include "../../../../Rdk/GUI/Qt/UComponentGuiContext.h"
#include "../../../../Rdk/GUI/Qt/UVisualControllerWidget.h"

class HardwareArduinoAssemblyTabHost;
class HardwareArduinoBoardPanelWidget;
class HardwareDisplayHubPanelWidget;
class HardwareHubTelemetryPanelWidget;
class HardwarePixelHubPanelWidget;
class HardwareRadioHubPanelWidget;
class HardwareUartDeviceHubPanelWidget;
class QPushButton;

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
    void onPluginChanged(int index);
    void onPresetClicked();
    void onHubCommand(const QString& line);
    void onRadioEsp32Mode(bool enabled);

private:
    void rebuildPluginCombo(const QString& selectId);
    void updatePresetStack(const QString& pluginId);
    void sendCommandLine(const QString& line);
    QWidget* buildI2cPresets(QWidget* parent);
    QWidget* buildGenericPresets(QWidget* parent);
    QWidget* buildPresetPage(QWidget* parent,
                             const std::initializer_list<std::pair<const char*, const char*>>& presets);

    UComponentGuiContext Context;
    QTabWidget* Tabs = nullptr;
    HardwareArduinoBoardPanelWidget* BoardPanel = nullptr;
    HardwareArduinoAssemblyTabHost* AssemblyTab = nullptr;
    HardwareHubTelemetryPanelWidget* Telemetry = nullptr;
    HardwareDisplayHubPanelWidget* DisplayPanel = nullptr;
    HardwarePixelHubPanelWidget* PixelPanel = nullptr;
    HardwareRadioHubPanelWidget* RadioPanel = nullptr;
    HardwareUartDeviceHubPanelWidget* UartPanel = nullptr;

    QComboBox* PluginCombo = nullptr;
    QLabel* FirmwareHint = nullptr;
    QLabel* PluginStatus = nullptr;
    QLineEdit* CommandEdit = nullptr;
    QStackedWidget* PresetStack = nullptr;
    QPlainTextEdit* FrameLogView = nullptr;
    bool SuppressPluginSignal = false;
};

#endif
