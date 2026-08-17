#include "HardwareArduinoCustomFirmwareControllerWidget.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

#include "widgets/HardwareArduinoAssemblyTabHost.h"
#include "widgets/HardwareArduinoBoardPanelWidget.h"
#include "widgets/HardwareGuiHelpers.h"

HardwareArduinoCustomFirmwareControllerWidget::HardwareArduinoCustomFirmwareControllerWidget(
    QWidget* parent, RDK::UApplication* app)
    : UVisualControllerWidget(parent, app)
{
    auto* page = new QWidget(this);
    PluginEdit = new QLineEdit(page);
    CommandEdit = new QLineEdit(page);
    PluginStatus = new QLabel(page);
    FrameLogView = new QPlainTextEdit(page);
    FrameLogView->setReadOnly(true);
    auto* sendBtn = new QPushButton(tr("Send"), page);
    auto* clearBtn = new QPushButton(tr("Clear log"), page);
    connect(sendBtn, &QPushButton::clicked, this, &HardwareArduinoCustomFirmwareControllerWidget::onSend);
    connect(clearBtn, &QPushButton::clicked, this,
            &HardwareArduinoCustomFirmwareControllerWidget::onClearLog);

    auto* form = new QFormLayout(page);
    HardwareGuiHelpers::applyCompactForm(form);
    form->addRow(tr("HostPluginId:"), PluginEdit);
    form->addRow(tr("Command:"), CommandEdit);
    auto* actionRow = new QHBoxLayout();
    actionRow->setContentsMargins(0, 0, 0, 0);
    actionRow->setSpacing(4);
    actionRow->addWidget(sendBtn);
    actionRow->addWidget(clearBtn);
    form->addRow(QString(), actionRow);
    form->addRow(QString(), PluginStatus);
    form->addRow(tr("Frame log:"), FrameLogView);

    BoardPanel = new HardwareArduinoBoardPanelWidget(this);
    AssemblyTab = new HardwareArduinoAssemblyTabHost(this);
    connect(AssemblyTab, &HardwareArduinoAssemblyTabHost::applyHardwareSetupRequested, this, [this]() {
        BoardPanel->applyToModel();
        refreshFromModel(true);
    });

    Tabs = new QTabWidget(this);
    Tabs->setDocumentMode(true);
    Tabs->setUsesScrollButtons(true);
    Tabs->addTab(page, tr("Custom"));
    Tabs->addTab(AssemblyTab, tr("Assembly"));
    Tabs->addTab(BoardPanel, tr("Board"));

    auto* root = new QVBoxLayout(this);
    HardwareGuiHelpers::applyCompactLayout(root);
    root->addWidget(Tabs);
    HardwareGuiHelpers::applyUnicodeFriendlyFont(this);
}

void HardwareArduinoCustomFirmwareControllerWidget::setComponentContext(
    const UComponentGuiContext& context)
{
    Context = context;
    BoardPanel->setContext(context);
    AssemblyTab->setContext(context);
    refreshFromModel(true);
}

QString HardwareArduinoCustomFirmwareControllerWidget::componentGuiId() const
{
    return QStringLiteral("hw.arduino.custom_firmware");
}

void HardwareArduinoCustomFirmwareControllerWidget::refreshFromModel(bool force)
{
    Q_UNUSED(force);
    if (Context.componentLongName.isEmpty())
        return;
    BoardPanel->refreshFromModel();
    AssemblyTab->refreshFromModel();
    PluginEdit->setText(HardwareGuiHelpers::getProp(Context, "HostPluginId"));
    CommandEdit->setText(HardwareGuiHelpers::getProp(Context, "Command"));
    const bool bound = HardwareGuiHelpers::getPropBool(Context, "PluginBound", false);
    PluginStatus->setText(bound ? tr("Plugin bound")
                                : tr("Upload-only (plugin not found)"));
    FrameLogView->setPlainText(HardwareGuiHelpers::getProp(Context, "FrameLog"));
}

void HardwareArduinoCustomFirmwareControllerWidget::onSend()
{
    BoardPanel->applyToModel();
    HardwareGuiHelpers::setProp(Context, "HostPluginId", PluginEdit->text());
    HardwareGuiHelpers::setProp(Context, "Command", CommandEdit->text());
    HardwareGuiHelpers::pulseEdge(Context, "SendCommand");
    refreshFromModel(true);
}

void HardwareArduinoCustomFirmwareControllerWidget::onClearLog()
{
    HardwareGuiHelpers::pulseEdge(Context, "ClearFrameLog");
    refreshFromModel(true);
}
