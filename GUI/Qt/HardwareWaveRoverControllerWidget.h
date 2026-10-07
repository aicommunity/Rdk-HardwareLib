#ifndef HARDWAREWAVEROVERCONTROLLERWIDGET_H
#define HARDWAREWAVEROVERCONTROLLERWIDGET_H

#include "../../../../Rdk/GUI/Qt/IComponentControllerWidget.h"
#include "../../../../Rdk/GUI/Qt/UComponentGuiContext.h"
#include "../../../../Rdk/GUI/Qt/UVisualControllerWidget.h"

#include <QLabel>
#include <QTabWidget>

class HardwareArduinoBoardPanelWidget;
class HardwareWheeledDrivePanelWidget;

class HardwareWaveRoverControllerWidget : public UVisualControllerWidget,
                                           public IComponentControllerWidget {
    Q_OBJECT
public:
    explicit HardwareWaveRoverControllerWidget(QWidget* parent = nullptr,
                                               RDK::UApplication* app = nullptr);
    void setComponentContext(const UComponentGuiContext& context) override;
    void refreshFromModel(bool force) override;
    QString componentGuiId() const override;

private:
    UComponentGuiContext Context;
    QTabWidget* Tabs = nullptr;
    HardwareWheeledDrivePanelWidget* Drive = nullptr;
    QLabel* FeedbackLabel = nullptr;
    HardwareArduinoBoardPanelWidget* BoardPanel = nullptr;
};

#endif
