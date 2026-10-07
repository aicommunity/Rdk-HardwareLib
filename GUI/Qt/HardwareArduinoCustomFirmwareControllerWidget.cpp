#include "HardwareArduinoCustomFirmwareControllerWidget.h"

#include <utility>

#include <QFormLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

#include "Catalog/UHardwareCatalog.h"
#include "Protocol/UArduinoProtocolPluginRegistry.h"
#include "widgets/HardwareArduinoAssemblyTabHost.h"
#include "widgets/HardwareArduinoBoardPanelWidget.h"
#include "widgets/HardwareGuiHelpers.h"
#include "widgets/HardwareHubTelemetryPanelWidget.h"

namespace {

QPushButton* makePresetButton(QWidget* parent, const QString& label, const QString& command)
{
    auto* btn = new QPushButton(label, parent);
    btn->setProperty("hubCommand", command);
    return btn;
}

} // namespace

HardwareArduinoCustomFirmwareControllerWidget::HardwareArduinoCustomFirmwareControllerWidget(
    QWidget* parent, RDK::UApplication* app)
    : UVisualControllerWidget(parent, app)
{
    RDK::registerBuiltinArduinoProtocolPlugins();

    auto* hubPage = new QWidget(this);
    PluginCombo = new QComboBox(hubPage);
    HardwareGuiHelpers::configureExpandingCombo(PluginCombo, 24);
    FirmwareHint = new QLabel(hubPage);
    FirmwareHint->setWordWrap(true);
    PluginStatus = new QLabel(hubPage);
    auto* hubForm = new QFormLayout(hubPage);
    HardwareGuiHelpers::applyCompactForm(hubForm);
    hubForm->addRow(tr("HostPluginId:"), PluginCombo);
    hubForm->addRow(tr("Firmware:"), FirmwareHint);
    hubForm->addRow(QString(), PluginStatus);
    connect(PluginCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &HardwareArduinoCustomFirmwareControllerWidget::onPluginChanged);

    auto* cmdPage = new QWidget(this);
    CommandEdit = new QLineEdit(cmdPage);
    auto* sendBtn = new QPushButton(tr("Send"), cmdPage);
    connect(sendBtn, &QPushButton::clicked, this, &HardwareArduinoCustomFirmwareControllerWidget::onSend);
    PresetStack = new QStackedWidget(cmdPage);
    PresetStack->addWidget(buildGenericPresets(cmdPage));  // 0
    PresetStack->addWidget(buildI2cPresets(cmdPage));      // 1
    PresetStack->addWidget(buildDisplayPresets(cmdPage));  // 2
    PresetStack->addWidget(buildPixelPresets(cmdPage));    // 3
    PresetStack->addWidget(buildRadioPresets(cmdPage));    // 4
    PresetStack->addWidget(buildUartPresets(cmdPage));     // 5
    auto* cmdRoot = new QVBoxLayout(cmdPage);
    HardwareGuiHelpers::applyCompactLayout(cmdRoot);
    auto* cmdRow = new QHBoxLayout();
    cmdRow->addWidget(CommandEdit, 1);
    cmdRow->addWidget(sendBtn);
    cmdRoot->addLayout(cmdRow);
    cmdRoot->addWidget(new QLabel(tr("Presets"), cmdPage));
    cmdRoot->addWidget(PresetStack, 1);

    Telemetry = new HardwareHubTelemetryPanelWidget(this);

    auto* logPage = new QWidget(this);
    FrameLogView = new QPlainTextEdit(logPage);
    FrameLogView->setReadOnly(true);
    auto* clearBtn = new QPushButton(tr("Clear log"), logPage);
    connect(clearBtn, &QPushButton::clicked, this,
            &HardwareArduinoCustomFirmwareControllerWidget::onClearLog);
    auto* logRoot = new QVBoxLayout(logPage);
    HardwareGuiHelpers::applyCompactLayout(logRoot);
    logRoot->addWidget(FrameLogView, 1);
    logRoot->addWidget(clearBtn);

    BoardPanel = new HardwareArduinoBoardPanelWidget(this);
    AssemblyTab = new HardwareArduinoAssemblyTabHost(this);
    connect(AssemblyTab, &HardwareArduinoAssemblyTabHost::applyHardwareSetupRequested, this, [this]() {
        BoardPanel->applyToModel();
        refreshFromModel(true);
    });

    Tabs = new QTabWidget(this);
    Tabs->setDocumentMode(true);
    Tabs->setUsesScrollButtons(true);
    Tabs->addTab(hubPage, tr("Hub"));
    Tabs->addTab(cmdPage, tr("Commands"));
    Tabs->addTab(Telemetry, tr("Telemetry"));
    Tabs->addTab(logPage, tr("Log"));
    Tabs->addTab(AssemblyTab, tr("Assembly"));
    Tabs->addTab(BoardPanel, tr("Board"));

    auto* root = new QVBoxLayout(this);
    HardwareGuiHelpers::applyCompactLayout(root);
    root->addWidget(Tabs);
    HardwareGuiHelpers::applyUnicodeFriendlyFont(this);

    rebuildPluginCombo(QString());
}

QWidget* HardwareArduinoCustomFirmwareControllerWidget::buildPresetPage(
    QWidget* parent, const std::initializer_list<std::pair<const char*, const char*>>& presets)
{
    auto* page = new QWidget(parent);
    auto* lay = new QVBoxLayout(page);
    HardwareGuiHelpers::applyCompactLayout(lay);
    for (const auto& p : presets) {
        auto* btn = makePresetButton(page, tr(p.first), QString::fromUtf8(p.second));
        connect(btn, &QPushButton::clicked, this,
                &HardwareArduinoCustomFirmwareControllerWidget::onPresetClicked);
        lay->addWidget(btn);
    }
    lay->addStretch(1);
    return page;
}

QWidget* HardwareArduinoCustomFirmwareControllerWidget::buildGenericPresets(QWidget* parent)
{
    return buildPresetPage(parent, {{"PING", "PING"}, {"PROTO 2", "PROTO 2"}});
}

QWidget* HardwareArduinoCustomFirmwareControllerWidget::buildI2cPresets(QWidget* parent)
{
    return buildPresetPage(parent,
                           {{"START READING", "START READING"},
                            {"STOP READING", "STOP READING"},
                            {"PING", "PING"},
                            {"SET DELAY 500", "SET DELAY 500"},
                            {"SET PWM 0 2048", "SET PWM 0 2048"}});
}

QWidget* HardwareArduinoCustomFirmwareControllerWidget::buildDisplayPresets(QWidget* parent)
{
    return buildPresetPage(parent,
                           {{"CLEAR", "CLEAR"},
                            {"PRINT 0 0 hello", "PRINT 0 0 hello"},
                            {"OLED CLEAR", "OLED CLEAR"},
                            {"OLED PRINT hello", "OLED PRINT hello"},
                            {"PING", "PING"}});
}

QWidget* HardwareArduinoCustomFirmwareControllerWidget::buildPixelPresets(QWidget* parent)
{
    return buildPresetPage(parent,
                           {{"LED FILL", "LED FILL"},
                            {"LED SHOW", "LED SHOW"},
                            {"MATRIX CLEAR", "MATRIX CLEAR"},
                            {"MATRIX TEXT A", "MATRIX TEXT A"},
                            {"TFT FILL 0", "TFT FILL 0"},
                            {"PING", "PING"}});
}

QWidget* HardwareArduinoCustomFirmwareControllerWidget::buildRadioPresets(QWidget* parent)
{
    return buildPresetPage(parent,
                           {{"RADIO SEND 010203", "RADIO SEND 010203"},
                            {"PING", "PING"},
                            {"PROTO 2", "PROTO 2"}});
}

QWidget* HardwareArduinoCustomFirmwareControllerWidget::buildUartPresets(QWidget* parent)
{
    return buildPresetPage(parent,
                           {{"AT", "AT"},
                            {"HMI TX hello", "HMI TX hello"},
                            {"BRIDGE ON", "BRIDGE ON"},
                            {"BRIDGE OFF", "BRIDGE OFF"},
                            {"PING", "PING"}});
}

void HardwareArduinoCustomFirmwareControllerWidget::rebuildPluginCombo(const QString& selectId)
{
    SuppressPluginSignal = true;
    PluginCombo->clear();
    PluginCombo->addItem(tr("(none)"), QString());
    const QStringList ids = RDK::registeredArduinoProtocolPluginIds();
    for (const QString& id : ids)
        PluginCombo->addItem(id, id);
    int idx = PluginCombo->findData(selectId);
    if (idx < 0)
        idx = 0;
    PluginCombo->setCurrentIndex(idx);
    SuppressPluginSignal = false;
    updatePresetStack(selectId);
}

void HardwareArduinoCustomFirmwareControllerWidget::updatePresetStack(const QString& pluginId)
{
    if (pluginId.contains(QStringLiteral("i2c_hub")))
        PresetStack->setCurrentIndex(1);
    else if (pluginId.contains(QStringLiteral("display_hub")))
        PresetStack->setCurrentIndex(2);
    else if (pluginId.contains(QStringLiteral("pixel_hub")))
        PresetStack->setCurrentIndex(3);
    else if (pluginId.contains(QStringLiteral("radio_hub")))
        PresetStack->setCurrentIndex(4);
    else if (pluginId.contains(QStringLiteral("uart_device_hub")))
        PresetStack->setCurrentIndex(5);
    else
        PresetStack->setCurrentIndex(0);

    QString hint = tr("Select a HostPluginId; Upload matching hub firmware from Board tab.");
    if (!RDK::UHardwareCatalog::instance().isLoaded())
        RDK::UHardwareCatalog::instance().load(nullptr);
    if (RDK::UHardwareCatalog::instance().isLoaded()) {
        for (const QString& fid : RDK::UHardwareCatalog::instance().firmwareIds(false)) {
            const RDK::UHwFirmwareInfo* fw = RDK::UHardwareCatalog::instance().firmware(fid);
            if (fw && fw->hostPlugin == pluginId) {
                hint = tr("BundledFirmware hint: %1 (%2)").arg(fw->id, fw->title);
                break;
            }
        }
    }
    FirmwareHint->setText(hint);
}

void HardwareArduinoCustomFirmwareControllerWidget::setComponentContext(
    const UComponentGuiContext& context)
{
    Context = context;
    BoardPanel->setContext(context);
    AssemblyTab->setContext(context);
    Telemetry->setContext(context);
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
    Telemetry->refreshFromModel();
    const QString pluginId = HardwareGuiHelpers::getProp(Context, "HostPluginId");
    rebuildPluginCombo(pluginId);
    CommandEdit->setText(HardwareGuiHelpers::getProp(Context, "Command"));
    const bool bound = HardwareGuiHelpers::getPropBool(Context, "PluginBound", false);
    PluginStatus->setText(bound ? tr("Plugin bound")
                                : tr("Upload-only (plugin not found)"));
    FrameLogView->setPlainText(HardwareGuiHelpers::getProp(Context, "FrameLog"));
}

void HardwareArduinoCustomFirmwareControllerWidget::onPluginChanged(int index)
{
    if (SuppressPluginSignal || Context.componentLongName.isEmpty())
        return;
    const QString id = PluginCombo->itemData(index).toString();
    HardwareGuiHelpers::setProp(Context, "HostPluginId", id);
    updatePresetStack(id);
    refreshFromModel(true);
}

void HardwareArduinoCustomFirmwareControllerWidget::sendCommandLine(const QString& line)
{
    BoardPanel->applyToModel();
    const QString pluginId = PluginCombo->currentData().toString();
    HardwareGuiHelpers::setProp(Context, "HostPluginId", pluginId);
    HardwareGuiHelpers::setProp(Context, "Command", line);
    HardwareGuiHelpers::pulseEdge(Context, "SendCommand");
    refreshFromModel(true);
}

void HardwareArduinoCustomFirmwareControllerWidget::onSend()
{
    sendCommandLine(CommandEdit->text());
}

void HardwareArduinoCustomFirmwareControllerWidget::onPresetClicked()
{
    auto* btn = qobject_cast<QPushButton*>(sender());
    if (!btn)
        return;
    const QString cmd = btn->property("hubCommand").toString();
    CommandEdit->setText(cmd);
    sendCommandLine(cmd);
}

void HardwareArduinoCustomFirmwareControllerWidget::onClearLog()
{
    HardwareGuiHelpers::pulseEdge(Context, "ClearFrameLog");
    refreshFromModel(true);
}
