#ifndef HARDWAREARDUINOFIRMATACONTROLLERWIDGET_H
#define HARDWAREARDUINOFIRMATACONTROLLERWIDGET_H

#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>

#include "../../../../Rdk/GUI/Qt/IComponentControllerWidget.h"
#include "../../../../Rdk/GUI/Qt/UComponentGuiContext.h"
#include "../../../../Rdk/GUI/Qt/UVisualControllerWidget.h"
#include "widgets/UArduinoBoardDiagramWidget.h"

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

private:
    UComponentGuiContext m_context;
    UArduinoBoardDiagramWidget* m_diagram = nullptr;
    QComboBox* m_portCombo = nullptr;
    QSpinBox* m_pinSpin = nullptr;
    QComboBox* m_modeCombo = nullptr;
    QSpinBox* m_digitalValueSpin = nullptr;
    QLabel* m_statusLabel = nullptr;
};

#endif
