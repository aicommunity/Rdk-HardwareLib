#ifndef HARDWAREARDUINOSENSORSKETCHCONTROLLERWIDGET_H
#define HARDWAREARDUINOSENSORSKETCHCONTROLLERWIDGET_H

#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QTabWidget>

#include "../../../../Rdk/GUI/Qt/IComponentControllerWidget.h"
#include "../../../../Rdk/GUI/Qt/UComponentGuiContext.h"
#include "../../../../Rdk/GUI/Qt/UVisualControllerWidget.h"
#include "widgets/UArduinoBoardDiagramWidget.h"

class HardwareArduinoBoardPanelWidget;
class HardwareArduinoAssemblyTabHost;

class HardwareArduinoSensorSketchControllerWidget : public UVisualControllerWidget,
                                                    public IComponentControllerWidget {
    Q_OBJECT
public:
    explicit HardwareArduinoSensorSketchControllerWidget(QWidget* parent = nullptr,
                                                         RDK::UApplication* app = nullptr);

    void setComponentContext(const UComponentGuiContext& context) override;
    void refreshFromModel(bool force) override;
    QString componentGuiId() const override;

private slots:
    void onApply();
    void onReset();
    void onCalculate();
    void onSendCommand();
    void onPresetCommand();
    void onGetData();
    void onGetPinsInfo();

private:
    void refreshMatrixPreview();

    UComponentGuiContext Context;
    UArduinoBoardDiagramWidget* Diagram = nullptr;
    QTabWidget* Tabs = nullptr;
    HardwareArduinoBoardPanelWidget* BoardPanel = nullptr;
    HardwareArduinoAssemblyTabHost* AssemblyTab = nullptr;
    QLineEdit* CommandEdit = nullptr;
    QListWidget* PresetsList = nullptr;
    QTableWidget* MatrixTable = nullptr;
};

#endif
