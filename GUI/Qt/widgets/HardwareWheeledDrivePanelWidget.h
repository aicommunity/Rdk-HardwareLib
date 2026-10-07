#ifndef HARDWAREWHEELEDDRIVEPANELWIDGET_H
#define HARDWAREWHEELEDDRIVEPANELWIDGET_H

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QWidget>

#include "../../../../../Rdk/GUI/Qt/UComponentGuiContext.h"

class HardwareWheeledDrivePanelWidget : public QWidget {
    Q_OBJECT
public:
    explicit HardwareWheeledDrivePanelWidget(QWidget* parent = nullptr);

    void setContext(const UComponentGuiContext& context);
    void setMotorDriverVisible(bool visible);
    void refreshFromModel();

private slots:
    void onApply();
    void onStop();
    void onApplyMotorDriver();

private:
    UComponentGuiContext Context;
    QSlider* LeftPwm = nullptr;
    QSlider* RightPwm = nullptr;
    QCheckBox* LeftDir = nullptr;
    QCheckBox* RightDir = nullptr;
    QSpinBox* WatchdogMs = nullptr;
    QComboBox* MotorDriver = nullptr;
    QLabel* FbLabel = nullptr;
    QWidget* MotorDriverRow = nullptr;
};

#endif
