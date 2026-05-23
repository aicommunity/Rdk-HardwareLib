#ifndef HARDWAREARDUINOFIRMATACONTROLLERWIDGET_H
#define HARDWAREARDUINOFIRMATACONTROLLERWIDGET_H

#include <QPlainTextEdit>
#include <QPushButton>
#include <QTabWidget>

#include "../../../../Rdk/GUI/Qt/IComponentControllerWidget.h"
#include "../../../../Rdk/GUI/Qt/UComponentGuiContext.h"
#include "../../../../Rdk/GUI/Qt/UVisualControllerWidget.h"
#include "widgets/HardwareArduinoPinConsoleWidget.h"
#include "widgets/UArduinoBoardDiagramWidget.h"

class HardwareArduinoBoardPanelWidget;

class HardwareArduinoFirmataControllerWidget : public UVisualControllerWidget, public IComponentControllerWidget {
    Q_OBJECT
public:
    explicit HardwareArduinoFirmataControllerWidget(QWidget* parent = nullptr, RDK::UApplication* app = nullptr);

    void setComponentContext(const UComponentGuiContext& context) override;
    void refreshFromModel(bool force) override;
    QString componentGuiId() const override;

private slots:
    void onApply();
    void onCalculate();
    void onRestartFirmata();
    void onDiagramPinClicked(const QString& pinId);
    void onConsoleCalculate();
    void onQueryPinState();
    void onWritePwm();
    void onI2cWrite();
    void onI2cRead();

private:
    UComponentGuiContext Context;
    UArduinoBoardDiagramWidget* Diagram = nullptr;
    QTabWidget* Tabs = nullptr;
    HardwareArduinoBoardPanelWidget* BoardPanel = nullptr;
    HardwareArduinoPinConsoleWidget* PinConsole = nullptr;
    QWidget* MonitorPage = nullptr;
    QPlainTextEdit* AnalogPreview = nullptr;
    QPlainTextEdit* StreamLogView = nullptr;
    QPlainTextEdit* StatusLog = nullptr;
    QWidget* I2cPage = nullptr;
};

#endif
