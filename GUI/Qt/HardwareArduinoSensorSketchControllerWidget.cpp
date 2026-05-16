#include "HardwareArduinoSensorSketchControllerWidget.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QSignalBlocker>
#include <QSplitter>
#include <QVBoxLayout>

#include <QtSerialPort/QSerialPortInfo>

#include "widgets/HardwareGuiHelpers.h"

HardwareArduinoSensorSketchControllerWidget::HardwareArduinoSensorSketchControllerWidget(QWidget* parent,
                                                                                       RDK::UApplication* app)
    : UVisualControllerWidget(parent, app)
{
    m_diagram = new UArduinoBoardDiagramWidget(this);
    m_diagram->setHighlightedPins({QStringLiteral("D2"), QStringLiteral("A2"), QStringLiteral("D9")});

    m_portCombo = new QComboBox(this);
    m_boardProfileCombo = new QComboBox(this);
    m_boardProfileCombo->addItem(tr("Uno"), 0);
    m_boardProfileCombo->addItem(tr("Mega 2560"), 1);

    m_commandEdit = new QLineEdit(this);
    auto* sendBtn = new QPushButton(tr("Send"), this);
    connect(sendBtn, &QPushButton::clicked, this, &HardwareArduinoSensorSketchControllerWidget::onSendCommand);

    m_presetsList = new QListWidget(this);
    m_presetsList->addItems({QStringLiteral("START READING"),
                              QStringLiteral("STOP READING"),
                              QStringLiteral("ROTATE"),
                              QStringLiteral("STOP ROTATE"),
                              QStringLiteral("GET STATUS")});
    connect(m_presetsList, &QListWidget::itemDoubleClicked, this,
            &HardwareArduinoSensorSketchControllerWidget::onPresetCommand);

    m_matrixTable = new QTableWidget(5, 5, this);
    m_matrixTable->horizontalHeader()->setStretchLastSection(true);
    m_getDataCheck = new QCheckBox(tr("Get data from buffers"), this);

    m_applyButton = new QPushButton(tr("Apply"), this);
    m_resetButton = new QPushButton(tr("Reset"), this);
    m_calculateButton = new QPushButton(tr("Calculate"), this);
    connect(m_applyButton, &QPushButton::clicked, this, &HardwareArduinoSensorSketchControllerWidget::onApply);
    connect(m_resetButton, &QPushButton::clicked, this, &HardwareArduinoSensorSketchControllerWidget::onReset);
    connect(m_calculateButton, &QPushButton::clicked, this, &HardwareArduinoSensorSketchControllerWidget::onCalculate);

    auto* form = new QFormLayout();
    form->addRow(tr("Port:"), m_portCombo);
    form->addRow(tr("Board:"), m_boardProfileCombo);
    auto* cmdRow = new QHBoxLayout();
    cmdRow->addWidget(m_commandEdit, 1);
    cmdRow->addWidget(sendBtn);
    form->addRow(tr("Command:"), cmdRow);
    form->addRow(tr("Presets:"), m_presetsList);
    form->addRow(QString(), m_getDataCheck);
    form->addRow(tr("Readings:"), m_matrixTable);
    auto* btnRow = new QHBoxLayout();
    btnRow->addWidget(m_applyButton);
    btnRow->addWidget(m_resetButton);
    btnRow->addWidget(m_calculateButton);
    form->addRow(QString(), btnRow);

    auto* right = new QWidget(this);
    right->setLayout(form);
    auto* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(m_diagram);
    splitter->addWidget(right);

    auto* root = new QVBoxLayout(this);
    root->addWidget(splitter);

    for (const QSerialPortInfo& info : QSerialPortInfo::availablePorts())
        m_portCombo->addItem(info.portName());
}

void HardwareArduinoSensorSketchControllerWidget::setComponentContext(const UComponentGuiContext& context)
{
    m_context = context;
    refreshFromModel(true);
}

QString HardwareArduinoSensorSketchControllerWidget::componentGuiId() const
{
    return QStringLiteral("hw.arduino.sensor_sketch");
}

void HardwareArduinoSensorSketchControllerWidget::applyBoardFields()
{
    HardwareGuiHelpers::setProp(m_context, "PortName", m_portCombo->currentText());
    HardwareGuiHelpers::setProp(m_context, "BoardProfile",
                                QString::number(m_boardProfileCombo->currentData().toInt()));
    HardwareGuiHelpers::setProp(m_context, "GetDataFromBuffers",
                                m_getDataCheck->isChecked() ? QStringLiteral("1") : QStringLiteral("0"));
}

void HardwareArduinoSensorSketchControllerWidget::refreshFromModel(bool force)
{
    Q_UNUSED(force);
    if (m_context.componentLongName.isEmpty())
        return;

    QSignalBlocker b1(m_portCombo);
    QSignalBlocker b2(m_boardProfileCombo);
    QSignalBlocker b3(m_getDataCheck);

    const QString port = HardwareGuiHelpers::getProp(m_context, "PortName");
    if (m_portCombo->findText(port) < 0 && !port.isEmpty())
        m_portCombo->addItem(port);
    m_portCombo->setCurrentText(port);

    const int profile = HardwareGuiHelpers::getPropInt(m_context, "BoardProfile", 0);
    m_boardProfileCombo->setCurrentIndex(profile == 1 ? 1 : 0);
    m_getDataCheck->setChecked(HardwareGuiHelpers::getPropBool(m_context, "GetDataFromBuffers", false));
    m_commandEdit->setText(HardwareGuiHelpers::getProp(m_context, "Command"));

    m_diagram->setBoardProfile(profile);
    m_diagram->setConnectionState(HardwareGuiHelpers::getPropInt(m_context, "ConnectionState", 0));
    refreshMatrixPreview();
}

void HardwareArduinoSensorSketchControllerWidget::refreshMatrixPreview()
{
    m_matrixTable->clear();
    m_matrixTable->setRowCount(0);
    m_matrixTable->setColumnCount(0);
    Q_UNUSED(m_context);
}

void HardwareArduinoSensorSketchControllerWidget::onApply()
{
    applyBoardFields();
    HardwareGuiHelpers::setProp(m_context, "Command", m_commandEdit->text());
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
    applyBoardFields();
    HardwareGuiHelpers::envCalculate(m_context);
    refreshFromModel(true);
}

void HardwareArduinoSensorSketchControllerWidget::onSendCommand()
{
    applyBoardFields();
    HardwareGuiHelpers::setProp(m_context, "Command", m_commandEdit->text());
    HardwareGuiHelpers::setProp(m_context, "SendCommandFlag", QStringLiteral("1"));
    HardwareGuiHelpers::envCalculate(m_context);
    refreshFromModel(true);
}

void HardwareArduinoSensorSketchControllerWidget::onPresetCommand()
{
    if (!m_presetsList->currentItem())
        return;
    m_commandEdit->setText(m_presetsList->currentItem()->text());
    onSendCommand();
}
