#ifndef HARDWAREARDUINOFIRMATACONTROLLERWIDGET_H
#define HARDWAREARDUINOFIRMATACONTROLLERWIDGET_H

#include <QComboBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>

#include "../../../../Rdk/GUI/Qt/IComponentControllerWidget.h"
#include "../../../../Rdk/GUI/Qt/UComponentGuiContext.h"
#include "../../../../Rdk/GUI/Qt/UVisualControllerWidget.h"
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
    void onSetPinMode();
    void onWriteDigital();
    void onReadAnalog();
    void onRestartFirmata();
    void onDiagramPinClicked(const QString& pinId);

private:
    QString firmataPinToLabel(int pin, int board_profile) const;

    UComponentGuiContext Context;
    UArduinoBoardDiagramWidget* Diagram = nullptr;
    QTabWidget* Tabs = nullptr;
    HardwareArduinoBoardPanelWidget* BoardPanel = nullptr;
    QSpinBox* PinSpin = nullptr;
    QComboBox* ModeCombo = nullptr;
    QSpinBox* DigitalValueSpin = nullptr;
    QPlainTextEdit* StatusLog = nullptr;
};

#endif
