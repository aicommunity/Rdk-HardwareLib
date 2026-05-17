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
    m_commandEdit = new QLineEdit(dcPage);
    auto* sendBtn = new QPushButton(tr("Send"), dcPage);
    connect(sendBtn, &QPushButton::clicked, this, &HardwareArduinoDcDemoControllerWidget::onSendCommand);

    auto* speedBtn = new QPushButton(tr("Read speed"), dcPage);
    connect(speedBtn, &QPushButton::clicked, this, &HardwareArduinoDcDemoControllerWidget::onGetSpeed);

    m_speedLabel = new QLabel(tr("Speed: —"), dcPage);

    auto* presets = new QListWidget(dcPage);
    presets->addItems({QStringLiteral("SET SPEED 50"), QStringLiteral("SET SPEED 0")});
    connect(presets, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
        if (item)
            onPreset(item->text());
    });

    auto* form = new QFormLayout();
    auto* cmdRow = new QHBoxLayout();
    cmdRow->addWidget(m_commandEdit, 1);
    cmdRow->addWidget(sendBtn);
    form->addRow(tr("Command:"), cmdRow);
    form->addRow(tr("Presets:"), presets);
    form->addRow(QString(), speedBtn);
    form->addRow(QString(), m_speedLabel);
    dcPage->setLayout(form);

    m_boardPanel = new HardwareArduinoBoardPanelWidget(this);
    m_tabs = new QTabWidget(this);
    m_tabs->addTab(dcPage, tr("DC"));
    m_tabs->addTab(m_boardPanel, tr("Board"));

    auto* root = new QVBoxLayout(this);
    root->addWidget(m_tabs);
}

void HardwareArduinoDcDemoControllerWidget::setComponentContext(const UComponentGuiContext& context)
{
    m_context = context;
    m_boardPanel->setContext(context);
    refreshFromModel(true);
}

QString HardwareArduinoDcDemoControllerWidget::componentGuiId() const
{
    return QStringLiteral("hw.arduino.dc_demo");
}

void HardwareArduinoDcDemoControllerWidget::refreshFromModel(bool force)
{
    Q_UNUSED(force);
    if (m_context.componentLongName.isEmpty())
        return;
    m_boardPanel->refreshFromModel();
    const float speed = HardwareGuiHelpers::getProp(m_context, "Speed").toFloat();
    const float accel = HardwareGuiHelpers::getProp(m_context, "Acceleration").toFloat();
    m_speedLabel->setText(tr("Speed: %1  Acceleration: %2").arg(speed).arg(accel));
}

void HardwareArduinoDcDemoControllerWidget::onSendCommand()
{
    m_boardPanel->applyToModel();
    HardwareGuiHelpers::setProp(m_context, "Command", m_commandEdit->text());
    HardwareGuiHelpers::pulseEdge(m_context, "SendCommand");
    refreshFromModel(true);
}

void HardwareArduinoDcDemoControllerWidget::onGetSpeed()
{
    m_boardPanel->applyToModel();
    HardwareGuiHelpers::pulseEdge(m_context, "GetSpeed");
    refreshFromModel(true);
}

void HardwareArduinoDcDemoControllerWidget::onPreset(const QString& command)
{
    m_commandEdit->setText(command);
    onSendCommand();
}
