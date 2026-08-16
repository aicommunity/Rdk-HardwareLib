#ifndef HARDWAREARDUINODEVICEIOCONTROLLERWIDGET_H
#define HARDWAREARDUINODEVICEIOCONTROLLERWIDGET_H

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>

#include "../../../../Rdk/GUI/Qt/IComponentControllerWidget.h"
#include "../../../../Rdk/GUI/Qt/UComponentGuiContext.h"
#include "../../../../Rdk/GUI/Qt/UVisualControllerWidget.h"

class HardwareArduinoDeviceIOControllerWidget : public UVisualControllerWidget,
                                                public IComponentControllerWidget {
    Q_OBJECT
public:
    explicit HardwareArduinoDeviceIOControllerWidget(QWidget* parent = nullptr,
                                                     RDK::UApplication* app = nullptr);

    void setComponentContext(const UComponentGuiContext& context) override;
    void refreshFromModel(bool force) override;
    QString componentGuiId() const override;

private slots:
    void onApplyConfig();
    void onWriteOutput();
    void onReadInput();

private:
    void applyFieldsToModel();

    UComponentGuiContext Context;
    QLineEdit* LinkedEdit = nullptr;
    QComboBox* ModuleCombo = nullptr;
    QLineEdit* PortEdit = nullptr;
    QLineEdit* ChannelEdit = nullptr;
    QComboBox* RoleCombo = nullptr;
    QDoubleSpinBox* ValueInSpin = nullptr;
    QCheckBox* ContinuousCheck = nullptr;
    QLabel* ValueLabel = nullptr;
    QLabel* StatusLabel = nullptr;
};

#endif
