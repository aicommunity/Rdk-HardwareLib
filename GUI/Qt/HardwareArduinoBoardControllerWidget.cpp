#include "HardwareArduinoBoardControllerWidget.h"

#include <QSplitter>
#include <QVBoxLayout>

#include "UFirmwareManifest.h"
#include "widgets/HardwareArduinoBoardPanelWidget.h"
#include "widgets/HardwareGuiHelpers.h"

HardwareArduinoBoardControllerWidget::HardwareArduinoBoardControllerWidget(QWidget* parent,
                                                                           RDK::UApplication* app)
    : UVisualControllerWidget(parent, app)
{
    Diagram = new UArduinoBoardDiagramWidget(this);
    BoardPanel = new HardwareArduinoBoardPanelWidget(this);

    auto* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(Diagram);
    splitter->addWidget(BoardPanel);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 2);

    auto* root = new QVBoxLayout(this);
    root->addWidget(splitter);

    setAccessibleName(QStringLiteral("HardwareArduinoBoardControllerWidget"));
    HardwareGuiHelpers::applyUnicodeFriendlyFont(this);
}

void HardwareArduinoBoardControllerWidget::setComponentContext(const UComponentGuiContext& context)
{
    Context = context;
    BoardPanel->setContext(context);
    refreshFromModel(true);
}

QString HardwareArduinoBoardControllerWidget::componentGuiId() const
{
    return QStringLiteral("hw.arduino.board");
}

void HardwareArduinoBoardControllerWidget::refreshFromModel(bool force)
{
    Q_UNUSED(force);
    if (Context.componentLongName.isEmpty())
        return;
    BoardPanel->refreshFromModel();
    updateDiagram();
}

void HardwareArduinoBoardControllerWidget::updateDiagram()
{
    Diagram->setBoardProfile(HardwareGuiHelpers::getPropInt(Context, "BoardProfile", 0));
    Diagram->setConnectionState(HardwareGuiHelpers::getPropInt(Context, "ConnectionState", 0));
    const QString fw_id = HardwareGuiHelpers::getProp(Context, "BundledFirmwareId");
    QStringList pins = RDK::UFirmwareManifest::bundledDefaultPinLabels(fw_id);
    if (pins.isEmpty())
        pins = {QStringLiteral("D2"), QStringLiteral("A2"), QStringLiteral("D9")};
    Diagram->setHighlightedPins(pins);
    Diagram->setPinRoles(RDK::UFirmwareManifest::bundledDefaultPinRoles(fw_id));
}
