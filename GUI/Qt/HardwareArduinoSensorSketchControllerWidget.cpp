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
    m_diagram = new UArduinoBoardDiagramWidget(this);
    m_diagram->setPinRoles(defaultSensorPinRoles());
    m_diagram->setHighlightedPins({QStringLiteral("D2"), QStringLiteral("A2"), QStringLiteral("D9")});

    auto* sensorPage = new QWidget(this);
    m_commandEdit = new QLineEdit(sensorPage);
    auto* sendBtn = new QPushButton(tr("Send"), sensorPage);
    connect(sendBtn, &QPushButton::clicked, this, &HardwareArduinoSensorSketchControllerWidget::onSendCommand);

    m_presetsList = new QListWidget(sensorPage);
    m_presetsList->addItems({QStringLiteral("START READING"),
                              QStringLiteral("STOP READING"),
                              QStringLiteral("ROTATE"),
                              QStringLiteral("STOP ROTATE"),
                              QStringLiteral("GET STATUS"),
                              QStringLiteral("GET PINS INFO")});
    connect(m_presetsList, &QListWidget::itemDoubleClicked, this,
            &HardwareArduinoSensorSketchControllerWidget::onPresetCommand);

    m_matrixTable = new QTableWidget(0, 0, sensorPage);
    m_matrixTable->horizontalHeader()->setStretchLastSection(true);

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
    cmdRow->addWidget(m_commandEdit, 1);
    cmdRow->addWidget(sendBtn);
    form->addRow(tr("Command:"), cmdRow);
    form->addRow(tr("Presets:"), m_presetsList);
    auto* edgeRow = new QHBoxLayout();
    edgeRow->addWidget(getDataBtn);
    edgeRow->addWidget(getPinsBtn);
    form->addRow(tr("Actions:"), edgeRow);
    form->addRow(tr("Readings:"), m_matrixTable);
    auto* btnRow = new QHBoxLayout();
    btnRow->addWidget(applyBtn);
    btnRow->addWidget(resetBtn);
    btnRow->addWidget(calcBtn);
    form->addRow(QString(), btnRow);
    sensorPage->setLayout(form);

    m_boardPanel = new HardwareArduinoBoardPanelWidget(this);
    m_tabs = new QTabWidget(this);
    m_tabs->addTab(sensorPage, tr("Sensor"));
    m_tabs->addTab(m_boardPanel, tr("Board"));

    auto* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(m_diagram);
    splitter->addWidget(m_tabs);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 2);

    auto* root = new QVBoxLayout(this);
    root->addWidget(splitter);
}

void HardwareArduinoSensorSketchControllerWidget::setComponentContext(const UComponentGuiContext& context)
{
    m_context = context;
    m_boardPanel->setContext(context);
    refreshFromModel(true);
}

QString HardwareArduinoSensorSketchControllerWidget::componentGuiId() const
{
    return QStringLiteral("hw.arduino.sensor_sketch");
}

void HardwareArduinoSensorSketchControllerWidget::refreshFromModel(bool force)
{
    Q_UNUSED(force);
    if (m_context.componentLongName.isEmpty())
        return;

    m_boardPanel->refreshFromModel();
    m_commandEdit->setText(HardwareGuiHelpers::getProp(m_context, "Command"));

    const int profile = HardwareGuiHelpers::getPropInt(m_context, "BoardProfile", 0);
    m_diagram->setBoardProfile(profile);
    m_diagram->setConnectionState(HardwareGuiHelpers::getPropInt(m_context, "ConnectionState", 0));
    m_diagram->setPinRoles(defaultSensorPinRoles());
    m_diagram->setHighlightedPins({QStringLiteral("D2"), QStringLiteral("A2"), QStringLiteral("D9")});
    refreshMatrixPreview();
}

void HardwareArduinoSensorSketchControllerWidget::refreshMatrixPreview()
{
    QVector<QVector<double>> rows;
    if (!HardwareGuiHelpers::getMatrixPreview(m_context, "DoubleMatrixReadings", 16, 12, &rows)) {
        m_matrixTable->clear();
        m_matrixTable->setRowCount(0);
        m_matrixTable->setColumnCount(0);
        return;
    }

    const int colCount = rows.isEmpty() ? 0 : rows.first().size();
    m_matrixTable->setRowCount(rows.size());
    m_matrixTable->setColumnCount(colCount);
    for (int r = 0; r < rows.size(); ++r) {
        for (int c = 0; c < rows[r].size(); ++c)
            m_matrixTable->setItem(r, c, new QTableWidgetItem(QString::number(rows[r][c], 'g', 4)));
    }
}

void HardwareArduinoSensorSketchControllerWidget::onApply()
{
    HardwareGuiHelpers::setProp(m_context, "Command", m_commandEdit->text());
    m_boardPanel->applyToModel();
    HardwareGuiHelpers::envReset(m_context);
    refreshFromModel(true);
}

void HardwareArduinoSensorSketchControllerWidget::onReset()
{
    HardwareGuiHelpers::envReset(m_context);
    refreshFromModel(true);
}

void HardwareArduinoSensorSketchControllerWidget::onCalculate()
{
    HardwareGuiHelpers::setProp(m_context, "Command", m_commandEdit->text());
    m_boardPanel->applyToModel();
    HardwareGuiHelpers::envCalculate(m_context);
    refreshFromModel(true);
}

void HardwareArduinoSensorSketchControllerWidget::onSendCommand()
{
    HardwareGuiHelpers::setProp(m_context, "Command", m_commandEdit->text());
    m_boardPanel->applyToModel();
    HardwareGuiHelpers::pulseEdge(m_context, "SendCommand");
    refreshFromModel(true);
}

void HardwareArduinoSensorSketchControllerWidget::onGetData()
{
    m_boardPanel->applyToModel();
    HardwareGuiHelpers::pulseEdge(m_context, "GetDataFromBuffers");
    refreshFromModel(true);
}

void HardwareArduinoSensorSketchControllerWidget::onGetPinsInfo()
{
    m_boardPanel->applyToModel();
    HardwareGuiHelpers::pulseEdge(m_context, "GetPinsInfo");
    refreshFromModel(true);
}

void HardwareArduinoSensorSketchControllerWidget::onPresetCommand()
{
    if (!m_presetsList->currentItem())
        return;
    const QString text = m_presetsList->currentItem()->text();
    if (text == QStringLiteral("GET PINS INFO")) {
        onGetPinsInfo();
        return;
    }
    m_commandEdit->setText(text);
    onSendCommand();
}
