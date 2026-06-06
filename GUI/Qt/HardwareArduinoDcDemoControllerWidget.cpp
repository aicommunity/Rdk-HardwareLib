#include "HardwareArduinoDcDemoControllerWidget.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QVBoxLayout>

#include "widgets/HardwareArduinoBoardPanelWidget.h"
#include "widgets/HardwareGuiHelpers.h"

HardwareArduinoDcDemoControllerWidget::HardwareArduinoDcDemoControllerWidget(QWidget* parent,
                                                                             RDK::UApplication* app)
    : UVisualControllerWidget(parent, app)
{
    auto* dcPage = new QWidget(this);
    CommandEdit = new QLineEdit(dcPage);
    auto* sendBtn = new QPushButton(tr("Send"), dcPage);
    connect(sendBtn, &QPushButton::clicked, this, &HardwareArduinoDcDemoControllerWidget::onSendCommand);

    auto* speedBtn = new QPushButton(tr("Read speed"), dcPage);
    connect(speedBtn, &QPushButton::clicked, this, &HardwareArduinoDcDemoControllerWidget::onGetSpeed);

    SpeedLabel = new QLabel(tr("Speed: \u2014"), dcPage);

    auto* presets = new QListWidget(dcPage);
    presets->addItems({QStringLiteral("SET SPEED 50"), QStringLiteral("SET SPEED 0")});
    connect(presets, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
        if (item)
            onPreset(item->text());
    });

    auto* form = new QFormLayout();
    auto* cmdRow = new QHBoxLayout();
    cmdRow->addWidget(CommandEdit, 1);
    cmdRow->addWidget(sendBtn);
    form->addRow(tr("Command:"), cmdRow);
    form->addRow(tr("Presets:"), presets);
    form->addRow(QString(), speedBtn);
    form->addRow(QString(), SpeedLabel);
    dcPage->setLayout(form);

    BoardPanel = new HardwareArduinoBoardPanelWidget(this);
    Tabs = new QTabWidget(this);
    Tabs->addTab(dcPage, tr("DC"));
    Tabs->addTab(BoardPanel, tr("Board"));

    auto* root = new QVBoxLayout(this);
    root->addWidget(Tabs);
    HardwareGuiHelpers::applyUnicodeFriendlyFont(this);
}

void HardwareArduinoDcDemoControllerWidget::setComponentContext(const UComponentGuiContext& context)
{
    Context = context;
    BoardPanel->setContext(context);
    refreshFromModel(true);
}

QString HardwareArduinoDcDemoControllerWidget::componentGuiId() const
{
    return QStringLiteral("hw.arduino.dc_demo");
}

void HardwareArduinoDcDemoControllerWidget::refreshFromModel(bool force)
{
    Q_UNUSED(force);
    if (Context.componentLongName.isEmpty())
        return;
    BoardPanel->refreshFromModel();
    const float speed = HardwareGuiHelpers::getProp(Context, "Speed").toFloat();
    const float accel = HardwareGuiHelpers::getProp(Context, "Acceleration").toFloat();
    SpeedLabel->setText(tr("Speed: %1  Acceleration: %2").arg(speed).arg(accel));
}

void HardwareArduinoDcDemoControllerWidget::onSendCommand()
{
    BoardPanel->applyToModel();
    HardwareGuiHelpers::setProp(Context, "Command", CommandEdit->text());
    HardwareGuiHelpers::pulseEdge(Context, "SendCommand");
    refreshFromModel(true);
}

void HardwareArduinoDcDemoControllerWidget::onGetSpeed()
{
    BoardPanel->applyToModel();
    HardwareGuiHelpers::pulseEdge(Context, "GetSpeed");
    refreshFromModel(true);
}

void HardwareArduinoDcDemoControllerWidget::onPreset(const QString& command)
{
    CommandEdit->setText(command);
    onSendCommand();
}
