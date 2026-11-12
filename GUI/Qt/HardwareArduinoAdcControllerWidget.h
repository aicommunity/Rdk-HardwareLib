#ifndef HARDWAREARDUINOADCCONTROLLERWIDGET_H
#define HARDWAREARDUINOADCCONTROLLERWIDGET_H

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QTabWidget>

#include "../../../../Rdk/GUI/Qt/IComponentControllerWidget.h"
#include "../../../../Rdk/GUI/Qt/UComponentGuiContext.h"
#include "../../../../Rdk/GUI/Qt/UVisualControllerWidget.h"

class HardwareArduinoAssemblyTabHost;

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
    QTabWidget* Tabs = nullptr;
    HardwareArduinoAssemblyTabHost* AssemblyTab = nullptr;
    QLineEdit* LinkedEdit = nullptr;
    QComboBox* PinCombo = nullptr;
    QCheckBox* UseLinkedSamples = nullptr;
    QLabel* ValueLabel = nullptr;
};

#endif
