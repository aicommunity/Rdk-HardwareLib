#include "HardwareArduinoBoardControllerWidget.h"

#include <QSplitter>
#include <QVBoxLayout>

#include "widgets/HardwareArduinoBoardPanelWidget.h"
#include "widgets/HardwareGuiHelpers.h"

HardwareArduinoBoardControllerWidget::HardwareArduinoBoardControllerWidget(QWidget* parent,
                                                                           RDK::UApplication* app)
    : UVisualControllerWidget(parent, app)
{
    m_diagram = new UArduinoBoardDiagramWidget(this);
    m_boardPanel = new HardwareArduinoBoardPanelWidget(this);

    auto* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(m_diagram);
    splitter->addWidget(m_boardPanel);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 2);

    auto* root = new QVBoxLayout(this);
    root->addWidget(splitter);

    setAccessibleName(QStringLiteral("HardwareArduinoBoardControllerWidget"));
}

void HardwareArduinoBoardControllerWidget::setComponentContext(const UComponentGuiContext& context)
{
    m_context = context;
    m_boardPanel->setContext(context);
    refreshFromModel(true);
}

QString HardwareArduinoBoardControllerWidget::componentGuiId() const
{
    return QStringLiteral("hw.arduino.board");
}

void HardwareArduinoBoardControllerWidget::refreshFromModel(bool force)
{
    Q_UNUSED(force);
    if (m_context.componentLongName.isEmpty())
        return;
    m_boardPanel->refreshFromModel();
    updateDiagram();
}

void HardwareArduinoBoardControllerWidget::updateDiagram()
{
    m_diagram->setBoardProfile(HardwareGuiHelpers::getPropInt(m_context, "BoardProfile", 0));
    m_diagram->setConnectionState(HardwareGuiHelpers::getPropInt(m_context, "ConnectionState", 0));
    m_diagram->setHighlightedPins({QStringLiteral("D2"), QStringLiteral("A2"), QStringLiteral("D9")});
}
