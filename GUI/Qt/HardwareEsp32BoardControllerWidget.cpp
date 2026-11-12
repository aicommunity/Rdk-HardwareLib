#include "HardwareEsp32BoardControllerWidget.h"

#include <QSplitter>
#include <QVBoxLayout>

#include "widgets/HardwareArduinoAssemblyTabHost.h"
#include "widgets/HardwareArduinoBoardPanelWidget.h"
#include "widgets/HardwareGuiHelpers.h"

HardwareEsp32BoardControllerWidget::HardwareEsp32BoardControllerWidget(QWidget* parent,
                                                                       RDK::UApplication* app)
    : UVisualControllerWidget(parent, app)
{
    AssemblyTab = new HardwareArduinoAssemblyTabHost(this);
    BoardPanel = new HardwareArduinoBoardPanelWidget(this);
    BoardPanel->setEsp32Mode(true);

    Tabs = new QTabWidget(this);
    Tabs->setDocumentMode(true);
    Tabs->addTab(BoardPanel, tr("Board"));
    Tabs->addTab(AssemblyTab, tr("Assembly"));

    auto* root = new QVBoxLayout(this);
    HardwareGuiHelpers::applyCompactLayout(root);
    root->addWidget(Tabs);

    setAccessibleName(QStringLiteral("HardwareEsp32BoardControllerWidget"));
    HardwareGuiHelpers::applyUnicodeFriendlyFont(this);
}

void HardwareEsp32BoardControllerWidget::setComponentContext(const UComponentGuiContext& context)
{
    Context = context;
    BoardPanel->setContext(context);
    AssemblyTab->setContext(context);
    refreshFromModel(true);
}

QString HardwareEsp32BoardControllerWidget::componentGuiId() const
{
    return QStringLiteral("hw.esp32.board");
}

void HardwareEsp32BoardControllerWidget::refreshFromModel(bool force)
{
    Q_UNUSED(force);
    if (Context.componentLongName.isEmpty())
        return;
    BoardPanel->refreshFromModel();
    AssemblyTab->refreshFromModel();
}
