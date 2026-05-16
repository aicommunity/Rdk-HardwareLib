#ifndef HARDWAREARDUINOSENSORSKETCHCONTROLLERWIDGET_H
#define HARDWAREARDUINOSENSORSKETCHCONTROLLERWIDGET_H

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QTableWidget>

#include "../../../../Rdk/GUI/Qt/IComponentControllerWidget.h"
#include "../../../../Rdk/GUI/Qt/UComponentGuiContext.h"
#include "../../../../Rdk/GUI/Qt/UVisualControllerWidget.h"
#include "widgets/UArduinoBoardDiagramWidget.h"

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
    void onGetPinsInfoToggled(bool checked);

private:
    void applyBoardFields();
    void refreshMatrixPreview();

    UComponentGuiContext m_context;
    UArduinoBoardDiagramWidget* m_diagram = nullptr;
    QComboBox* m_portCombo = nullptr;
    QComboBox* m_boardProfileCombo = nullptr;
    QLineEdit* m_commandEdit = nullptr;
    QListWidget* m_presetsList = nullptr;
    QTableWidget* m_matrixTable = nullptr;
    QCheckBox* m_getDataCheck = nullptr;
    QCheckBox* m_getPinsInfoCheck = nullptr;
    QPushButton* m_applyButton = nullptr;
    QPushButton* m_resetButton = nullptr;
    QPushButton* m_calculateButton = nullptr;
};

#endif
