#include "HardwareArduinoFirmataControllerWidget.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QSplitter>
#include <QVBoxLayout>

#include "widgets/HardwareArduinoAssemblyTabHost.h"
#include "widgets/HardwareArduinoBoardPanelWidget.h"
#include "widgets/HardwareGuiHelpers.h"

#include <Transport/UArduinoPinMap.h>

HardwareArduinoFirmataControllerWidget::HardwareArduinoFirmataControllerWidget(QWidget* parent,
                                                                               RDK::UApplication* app)
    : UVisualControllerWidget(parent, app)
{
    Diagram = new UArduinoBoardDiagramWidget(this);
    Diagram->setInteractive(true);
    connect(Diagram, &UArduinoBoardDiagramWidget::pinClicked, this,
            &HardwareArduinoFirmataControllerWidget::onDiagramPinClicked);

    auto* firmataPage = new QWidget(this);
    PinConsole = new HardwareArduinoPinConsoleWidget(firmataPage);
    connect(PinConsole, &HardwareArduinoPinConsoleWidget::calculateRequested, this,
            &HardwareArduinoFirmataControllerWidget::onConsoleCalculate);
    connect(PinConsole, &HardwareArduinoPinConsoleWidget::selectedPinChanged, this,
            [this](int pin, int profile) {
                Diagram->setSelectedPinId(RDK::UArduinoPinMap::labelForFirmataPin(pin, profile));
            });

    StatusLog = HardwareGuiHelpers::createStatusLogWidget(firmataPage);
    StatusLog->setMaximumHeight(120);

    auto* restartBtn = new QPushButton(tr("Restart Firmata"), firmataPage);
    auto* applyBtn = new QPushButton(tr("Apply"), firmataPage);
    auto* calcBtn = new QPushButton(tr("Calculate"), firmataPage);
    auto* queryBtn = new QPushButton(tr("Query pin"), firmataPage);
    auto* pwmBtn = new QPushButton(tr("Write PWM"), firmataPage);
    connect(restartBtn, &QPushButton::clicked, this, &HardwareArduinoFirmataControllerWidget::onRestartFirmata);
    connect(applyBtn, &QPushButton::clicked, this, &HardwareArduinoFirmataControllerWidget::onApply);
    connect(calcBtn, &QPushButton::clicked, this, &HardwareArduinoFirmataControllerWidget::onCalculate);
    connect(queryBtn, &QPushButton::clicked, this, &HardwareArduinoFirmataControllerWidget::onQueryPinState);
    connect(pwmBtn, &QPushButton::clicked, this, &HardwareArduinoFirmataControllerWidget::onWritePwm);

  auto* bottom = new QHBoxLayout();
    bottom->addWidget(applyBtn);
    bottom->addWidget(calcBtn);
    bottom->addWidget(restartBtn);
    bottom->addWidget(queryBtn);
    bottom->addWidget(pwmBtn);

    auto* firmataLayout = new QVBoxLayout(firmataPage);
    firmataLayout->addWidget(PinConsole, 1);
    firmataLayout->addLayout(bottom);
    firmataLayout->addWidget(new QLabel(tr("Status:"), firmataPage));
    firmataLayout->addWidget(StatusLog);

    MonitorPage = new QWidget(this);
    AnalogPreview = HardwareGuiHelpers::createStatusLogWidget(MonitorPage);
    StreamLogView = HardwareGuiHelpers::createStatusLogWidget(MonitorPage);
    auto* monLayout = new QVBoxLayout(MonitorPage);
    monLayout->addWidget(new QLabel(tr("AnalogSamples (last rows):"), MonitorPage));
    monLayout->addWidget(AnalogPreview, 1);
    monLayout->addWidget(new QLabel(tr("Stream log:"), MonitorPage));
    monLayout->addWidget(StreamLogView, 1);

    I2cPage = new QWidget(this);
    auto* i2cAddr = new QSpinBox(I2cPage);
    i2cAddr->setObjectName(QStringLiteral("i2cAddress"));
    i2cAddr->setRange(0, 127);
    auto* i2cHex = new QLineEdit(I2cPage);
    i2cHex->setObjectName(QStringLiteral("i2cWriteHex"));
    i2cHex->setPlaceholderText(tr("01 02"));
    auto* i2cForm = new QFormLayout(I2cPage);
    i2cForm->addRow(tr("Address (7-bit):"), i2cAddr);
    i2cForm->addRow(tr("Write data (hex):"), i2cHex);
    auto* i2cWriteBtn = new QPushButton(tr("I2C write"), I2cPage);
    auto* i2cReadBtn = new QPushButton(tr("I2C read"), I2cPage);
    connect(i2cWriteBtn, &QPushButton::clicked, this, &HardwareArduinoFirmataControllerWidget::onI2cWrite);
    connect(i2cReadBtn, &QPushButton::clicked, this, &HardwareArduinoFirmataControllerWidget::onI2cRead);
    i2cForm->addRow(QString(), i2cWriteBtn);
    i2cForm->addRow(QString(), i2cReadBtn);

    BoardPanel = new HardwareArduinoBoardPanelWidget(this);
    AssemblyTab = new HardwareArduinoAssemblyTabHost(this);
    Tabs = new QTabWidget(this);
    Tabs->addTab(firmataPage, tr("Pins"));
    Tabs->addTab(MonitorPage, tr("Monitor"));
    Tabs->addTab(I2cPage, tr("I2C"));
    Tabs->addTab(AssemblyTab, tr("Assembly"));
    Tabs->addTab(BoardPanel, tr("Board"));

    auto* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(Diagram);
    splitter->addWidget(Tabs);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 2);

    auto* root = new QVBoxLayout(this);
    root->addWidget(splitter);
    HardwareGuiHelpers::applyUnicodeFriendlyFont(this);
}

void HardwareArduinoFirmataControllerWidget::setComponentContext(const UComponentGuiContext& context)
{
    Context = context;
    BoardPanel->setContext(context);
    AssemblyTab->setContext(context);
    PinConsole->setContext(context);
    refreshFromModel(true);
}

QString HardwareArduinoFirmataControllerWidget::componentGuiId() const
{
    return QStringLiteral("hw.arduino.firmata");
}

void HardwareArduinoFirmataControllerWidget::refreshFromModel(bool force)
{
    Q_UNUSED(force);
    if (Context.componentLongName.isEmpty())
        return;

    BoardPanel->refreshFromModel();
    AssemblyTab->refreshFromModel();
    PinConsole->refreshFromModel();

    const int profile = HardwareGuiHelpers::getPropInt(Context, "BoardProfile", 0);
    const int pin = HardwareGuiHelpers::getPropInt(Context, "SelectedPin", 13);

    Diagram->setBoardProfile(profile);
    Diagram->setConnectionState(HardwareGuiHelpers::getPropInt(Context, "ConnectionState", 0));
    Diagram->applyPinStatusJson(HardwareGuiHelpers::getProp(Context, "PinStatusJson"));
    Diagram->setSelectedPinId(RDK::UArduinoPinMap::labelForFirmataPin(pin, profile));

    const bool firmata_ready = HardwareGuiHelpers::getPropBool(Context, "IsFirmataReady", false);
    const bool link_ready = HardwareGuiHelpers::getPropBool(Context, "IsLinkReady", false);
    const QString ver = HardwareGuiHelpers::getProp(Context, "FirmataFirmwareVersion");
    const int analog = HardwareGuiHelpers::getPropInt(Context, "AnalogPinValue", 0);
    const int stage = HardwareGuiHelpers::getPropInt(Context, "HandshakeStage", 0);
    HardwareGuiHelpers::setStatusLogText(
        StatusLog,
        tr("Firmata ready: %1\nLink ready: %2\nHandshake stage: %3\nVersion: %4\nAnalog: %5")
            .arg(firmata_ready ? tr("yes") : tr("no"))
            .arg(link_ready ? tr("yes") : tr("no"))
            .arg(stage)
            .arg(ver)
            .arg(analog));

    QVector<QVector<double>> sample_rows;
    QString matrix_preview;
    if (HardwareGuiHelpers::getMatrixPreview(Context, "AnalogSamples", 32, 4, &sample_rows)) {
        for (const QVector<double>& row : sample_rows) {
            QStringList cols;
            for (double v : row)
                cols.append(QString::number(v, 'f', 1));
            matrix_preview += cols.join(QStringLiteral("\t")) + QLatin1Char('\n');
        }
    } else {
        matrix_preview = tr("(no samples)");
    }
    HardwareGuiHelpers::setStatusLogText(AnalogPreview, matrix_preview);
    HardwareGuiHelpers::setStatusLogText(
        StreamLogView, HardwareGuiHelpers::getProp(Context, "StreamLog"));

    if (I2cPage) {
        if (auto* addr = I2cPage->findChild<QSpinBox*>(QStringLiteral("i2cAddress")))
            addr->setValue(HardwareGuiHelpers::getPropInt(Context, "I2cAddress", 72));
        if (auto* hex = I2cPage->findChild<QLineEdit*>(QStringLiteral("i2cWriteHex")))
            hex->setText(HardwareGuiHelpers::getProp(Context, "I2cWriteData"));
    }
}

void HardwareArduinoFirmataControllerWidget::onDiagramPinClicked(const QString& pinId)
{
    const int profile = HardwareGuiHelpers::getPropInt(Context, "BoardProfile", 0);
    const int pin = UArduinoBoardDiagramWidget::firmataPinFromLabel(pinId, profile);
    if (pin < 0)
        return;
    HardwareGuiHelpers::setProp(Context, "SelectedPin", QString::number(pin));
    Diagram->setSelectedPinId(pinId);
    PinConsole->refreshFromModel();
}

void HardwareArduinoFirmataControllerWidget::onApply()
{
    BoardPanel->applyToModel();
    PinConsole->applyToModel();
}

void HardwareArduinoFirmataControllerWidget::onCalculate()
{
    onApply();
    HardwareGuiHelpers::envCalculate(Context);
    refreshFromModel(true);
}

void HardwareArduinoFirmataControllerWidget::onConsoleCalculate()
{
    onCalculate();
}

void HardwareArduinoFirmataControllerWidget::onRestartFirmata()
{
    BoardPanel->applyToModel();
    HardwareGuiHelpers::pulseEdge(Context, "RestartFirmata");
    refreshFromModel(true);
}

void HardwareArduinoFirmataControllerWidget::onQueryPinState()
{
    onApply();
    const int pin = HardwareGuiHelpers::getPropInt(Context, "SelectedPin", 13);
    HardwareGuiHelpers::setProp(Context, "QueryPin", QString::number(pin));
    HardwareGuiHelpers::pulseEdge(Context, "QueryPinState");
    onCalculate();
}

void HardwareArduinoFirmataControllerWidget::onWritePwm()
{
    onApply();
    HardwareGuiHelpers::pulseEdge(Context, "WritePwm");
    onCalculate();
}

void HardwareArduinoFirmataControllerWidget::onI2cWrite()
{
    onApply();
    if (auto* addr = I2cPage->findChild<QSpinBox*>(QStringLiteral("i2cAddress")))
        HardwareGuiHelpers::setProp(Context, "I2cAddress", QString::number(addr->value()));
    if (auto* hex = I2cPage->findChild<QLineEdit*>(QStringLiteral("i2cWriteHex")))
        HardwareGuiHelpers::setProp(Context, "I2cWriteData", hex->text());
    HardwareGuiHelpers::pulseEdge(Context, "I2cWrite");
    onCalculate();
}

void HardwareArduinoFirmataControllerWidget::onI2cRead()
{
    onApply();
    if (auto* addr = I2cPage->findChild<QSpinBox*>(QStringLiteral("i2cAddress")))
        HardwareGuiHelpers::setProp(Context, "I2cAddress", QString::number(addr->value()));
    HardwareGuiHelpers::pulseEdge(Context, "I2cRead");
    onCalculate();
}
