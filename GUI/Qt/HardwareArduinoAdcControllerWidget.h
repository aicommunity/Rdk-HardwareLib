#ifndef HARDWAREARDUINOADCCONTROLLERWIDGET_H
#define HARDWAREARDUINOADCCONTROLLERWIDGET_H

#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>

#include "../../../../Rdk/GUI/Qt/IComponentControllerWidget.h"
#include "../../../../Rdk/GUI/Qt/UComponentGuiContext.h"
#include "../../../../Rdk/GUI/Qt/UVisualControllerWidget.h"

class HardwareArduinoAdcControllerWidget : public UVisualControllerWidget, public IComponentControllerWidget {
    Q_OBJECT
public:
    explicit HardwareArduinoAdcControllerWidget(QWidget* parent = nullptr, RDK::UApplication* app = nullptr);

    void setComponentContext(const UComponentGuiContext& context) override;
    void refreshFromModel(bool force) override;
    QString componentGuiId() const override;

private slots:
    void onReadAdc();

private:
    UComponentGuiContext Context;
    QLineEdit* LinkedEdit = nullptr;
    QSpinBox* PinSpin = nullptr;
    QLabel* ValueLabel = nullptr;
};

#endif
