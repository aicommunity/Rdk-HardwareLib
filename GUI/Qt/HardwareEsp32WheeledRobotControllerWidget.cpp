#include "HardwareEsp32WheeledRobotControllerWidget.h"

#include <QVBoxLayout>

#include "widgets/HardwareArduinoAssemblyTabHost.h"
#include "widgets/HardwareArduinoBoardPanelWidget.h"
#include "widgets/HardwareGuiHelpers.h"
#include "widgets/HardwareWheeledDrivePanelWidget.h"
#include "widgets/HardwareWheeledSensorsPanelWidget.h"

HardwareEsp32WheeledRobotControllerWidget::HardwareEsp32WheeledRobotControllerWidget(
    QWidget* parent, RDK::UApplication* app)
    : UVisualControllerWidget(parent, app)
{
    Drive = new HardwareWheeledDrivePanelWidget(this);
    Sensors = new HardwareWheeledSensorsPanelWidget(this);
    AssemblyTab = new HardwareArduinoAssemblyTabHost(this);
    BoardPanel = new HardwareArduinoBoardPanelWidget(this);
    BoardPanel->setEsp32Mode(true);
    Tabs = new QTabWidget(this);
    Tabs->setDocumentMode(true);
    Tabs->addTab(Drive, tr("Drive"));
    Tabs->addTab(Sensors, tr("Sensors"));
    Tabs->addTab(AssemblyTab, tr("Assembly"));
    Tabs->addTab(BoardPanel, tr("Board"));
    auto* root = new QVBoxLayout(this);
    HardwareGuiHelpers::applyCompactLayout(root);
    root->addWidget(Tabs);
    HardwareGuiHelpers::applyUnicodeFriendlyFont(this);
}

void HardwareEsp32WheeledRobotControllerWidget::setComponentContext(const UComponentGuiContext& context)
{
    Context = context;
    Drive->setContext(context);
    Sensors->setContext(context);
    AssemblyTab->setContext(context);
    BoardPanel->setContext(context);
    refreshFromModel(true);
}

QString HardwareEsp32WheeledRobotControllerWidget::componentGuiId() const
{
    return QStringLiteral("hw.esp32.wheeled_robot");
}

void HardwareEsp32WheeledRobotControllerWidget::refreshFromModel(bool force)
{
    Q_UNUSED(force);
    if (Context.componentLongName.isEmpty())
        return;
    Drive->refreshFromModel();
    Sensors->refreshFromModel();
    AssemblyTab->refreshFromModel();
    BoardPanel->refreshFromModel();
}
