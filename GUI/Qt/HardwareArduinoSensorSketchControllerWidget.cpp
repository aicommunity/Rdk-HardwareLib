#include "HardwareArduinoSensorSketchControllerWidget.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QSplitter>
#include <QVBoxLayout>

#include "widgets/HardwareArduinoBoardPanelWidget.h"
#include "widgets/HardwareGuiHelpers.h"

namespace {

QMap<QString, QString> defaultSensorPinRoles()
{
    return {{QStringLiteral("D2"), QStringLiteral("DHT")},
            {QStringLiteral("A2"), QStringLiteral("Hall")},
            {QStringLiteral("D9"), QStringLiteral("Servo")}};
}

} // namespace

HardwareArduinoSensorSketchControllerWidget::HardwareArduinoSensorSketchControllerWidget(
    QWidget* parent,
    RDK::UApplication* app)
    : UVisualControllerWidget(parent, app)
{
    Diagram = new UArduinoBoardDiagramWidget(this);
    Diagram->setPinRoles(defaultSensorPinRoles());
    Diagram->setHighlightedPins({QStringLiteral("D2"), QStringLiteral("A2"), QStringLiteral("D9")});

    auto* sensorPage = new QWidget(this);
    CommandEdit = new QLineEdit(sensorPage);
    auto* sendBtn = new QPushButton(tr("Send"), sensorPage);
    connect(sendBtn, &QPushButton::clicked, this, &HardwareArduinoSensorSketchControllerWidget::onSendCommand);

    PresetsList = new QListWidget(sensorPage);
    PresetsList->addItems({QStringLiteral("START READING"),
                              QStringLiteral("STOP READING"),
                              QStringLiteral("ROTATE"),
                              QStringLiteral("STOP ROTATE"),
                              QStringLiteral("GET STATUS"),
                              QStringLiteral("GET PINS INFO")});
    connect(PresetsList, &QListWidget::itemDoubleClicked, this,
            &HardwareArduinoSensorSketchControllerWidget::onPresetCommand);

    MatrixTable = new QTableWidget(0, 0, sensorPage);
    MatrixTable->horizontalHeader()->setStretchLastSection(true);

    auto* getDataBtn = new QPushButton(tr("Get data from buffers"), sensorPage);
    auto* getPinsBtn = new QPushButton(tr("Get pins info"), sensorPage);
    connect(getDataBtn, &QPushButton::clicked, this, &HardwareArduinoSensorSketchControllerWidget::onGetData);
    connect(getPinsBtn, &QPushButton::clicked, this, &HardwareArduinoSensorSketchControllerWidget::onGetPinsInfo);

    auto* applyBtn = new QPushButton(tr("Apply"), sensorPage);
    auto* resetBtn = new QPushButton(tr("Reset"), sensorPage);
    auto* calcBtn = new QPushButton(tr("Calculate"), sensorPage);
    connect(applyBtn, &QPushButton::clicked, this, &HardwareArduinoSensorSketchControllerWidget::onApply);
    connect(resetBtn, &QPushButton::clicked, this, &HardwareArduinoSensorSketchControllerWidget::onReset);
    connect(calcBtn, &QPushButton::clicked, this, &HardwareArduinoSensorSketchControllerWidget::onCalculate);

    auto* form = new QFormLayout();
    auto* cmdRow = new QHBoxLayout();
    cmdRow->addWidget(CommandEdit, 1);
    cmdRow->addWidget(sendBtn);
    form->addRow(tr("Command:"), cmdRow);
    form->addRow(tr("Presets:"), PresetsList);
    auto* edgeRow = new QHBoxLayout();
    edgeRow->addWidget(getDataBtn);
    edgeRow->addWidget(getPinsBtn);
    form->addRow(tr("Actions:"), edgeRow);
    form->addRow(tr("Readings:"), MatrixTable);
    auto* btnRow = new QHBoxLayout();
    btnRow->addWidget(applyBtn);
    btnRow->addWidget(resetBtn);
    btnRow->addWidget(calcBtn);
    form->addRow(QString(), btnRow);
    sensorPage->setLayout(form);

    BoardPanel = new HardwareArduinoBoardPanelWidget(this);
    Tabs = new QTabWidget(this);
    Tabs->addTab(sensorPage, tr("Sensor"));
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

void HardwareArduinoSensorSketchControllerWidget::setComponentContext(const UComponentGuiContext& context)
{
    Context = context;
    BoardPanel->setContext(context);
    refreshFromModel(true);
}

QString HardwareArduinoSensorSketchControllerWidget::componentGuiId() const
{
    return QStringLiteral("hw.arduino.sensor_sketch");
}

void HardwareArduinoSensorSketchControllerWidget::refreshFromModel(bool force)
{
    Q_UNUSED(force);
    if (Context.componentLongName.isEmpty())
        return;

    BoardPanel->refreshFromModel();
    CommandEdit->setText(HardwareGuiHelpers::getProp(Context, "Command"));

    const int profile = HardwareGuiHelpers::getPropInt(Context, "BoardProfile", 0);
    Diagram->setBoardProfile(profile);
    Diagram->setConnectionState(HardwareGuiHelpers::getPropInt(Context, "ConnectionState", 0));
    Diagram->setPinRoles(defaultSensorPinRoles());
    Diagram->setHighlightedPins({QStringLiteral("D2"), QStringLiteral("A2"), QStringLiteral("D9")});
    refreshMatrixPreview();
}

void HardwareArduinoSensorSketchControllerWidget::refreshMatrixPreview()
{
    QVector<QVector<double>> rows;
    if (!HardwareGuiHelpers::getMatrixPreview(Context, "DoubleMatrixReadings", 16, 12, &rows)) {
        MatrixTable->clear();
        MatrixTable->setRowCount(0);
        MatrixTable->setColumnCount(0);
        return;
    }

    const int col_count = rows.isEmpty() ? 0 : rows.first().size();
    MatrixTable->setRowCount(rows.size());
    MatrixTable->setColumnCount(col_count);
    for (int r = 0; r < rows.size(); ++r) {
        for (int c = 0; c < rows[r].size(); ++c)
            MatrixTable->setItem(r, c, new QTableWidgetItem(QString::number(rows[r][c], 'g', 4)));
    }
}

void HardwareArduinoSensorSketchControllerWidget::onApply()
{
    HardwareGuiHelpers::setProp(Context, "Command", CommandEdit->text());
    BoardPanel->applyToModel();
    HardwareGuiHelpers::envReset(Context);
    refreshFromModel(true);
}

void HardwareArduinoSensorSketchControllerWidget::onReset()
{
    HardwareGuiHelpers::envReset(Context);
    refreshFromModel(true);
}

void HardwareArduinoSensorSketchControllerWidget::onCalculate()
{
    HardwareGuiHelpers::setProp(Context, "Command", CommandEdit->text());
    BoardPanel->applyToModel();
    HardwareGuiHelpers::envCalculate(Context);
    refreshFromModel(true);
}

void HardwareArduinoSensorSketchControllerWidget::onSendCommand()
{
    HardwareGuiHelpers::setProp(Context, "Command", CommandEdit->text());
    BoardPanel->applyToModel();
    HardwareGuiHelpers::pulseEdge(Context, "SendCommand");
    refreshFromModel(true);
}

void HardwareArduinoSensorSketchControllerWidget::onGetData()
{
    BoardPanel->applyToModel();
    HardwareGuiHelpers::pulseEdge(Context, "GetDataFromBuffers");
    refreshFromModel(true);
}

void HardwareArduinoSensorSketchControllerWidget::onGetPinsInfo()
{
    BoardPanel->applyToModel();
    HardwareGuiHelpers::pulseEdge(Context, "GetPinsInfo");
    refreshFromModel(true);
}

void HardwareArduinoSensorSketchControllerWidget::onPresetCommand()
{
    if (!PresetsList->currentItem())
        return;
    const QString text = PresetsList->currentItem()->text();
    if (text == QStringLiteral("GET PINS INFO")) {
        onGetPinsInfo();
        return;
    }
    CommandEdit->setText(text);
    onSendCommand();
}
