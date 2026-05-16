#include "HardwareArduinoSensorSketchControllerWidget.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QSignalBlocker>
#include <QSplitter>
#include <QVBoxLayout>

#include <QtSerialPort/QSerialPortInfo>

#include "widgets/HardwareGuiHelpers.h"

#include "../../../Core/Transport/UArduinoSerialPortUtil.h"

namespace {

QMap<QString, QString> defaultSensorPinRoles()
{
    return {{QStringLiteral("D2"), QStringLiteral("DHT")},
            {QStringLiteral("A2"), QStringLiteral("Hall")},
            {QStringLiteral("D9"), QStringLiteral("Servo")}};
}

} // namespace

HardwareArduinoSensorSketchControllerWidget::HardwareArduinoSensorSketchControllerWidget(QWidget* parent,
                                                                                       RDK::UApplication* app)
    : UVisualControllerWidget(parent, app)
{
    m_diagram = new UArduinoBoardDiagramWidget(this);
    m_diagram->setPinRoles(defaultSensorPinRoles());
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
                              QStringLiteral("GET STATUS"),
                              QStringLiteral("GET PINS INFO")});
    connect(m_presetsList, &QListWidget::itemDoubleClicked, this,
            &HardwareArduinoSensorSketchControllerWidget::onPresetCommand);

    m_matrixTable = new QTableWidget(0, 0, this);
    m_matrixTable->horizontalHeader()->setStretchLastSection(true);
    m_getDataCheck = new QCheckBox(tr("Get data from buffers"), this);
    m_getPinsInfoCheck = new QCheckBox(tr("Get pins info"), this);

    m_applyButton = new QPushButton(tr("Apply"), this);
    m_resetButton = new QPushButton(tr("Reset"), this);
    m_calculateButton = new QPushButton(tr("Calculate"), this);
    connect(m_applyButton, &QPushButton::clicked, this, &HardwareArduinoSensorSketchControllerWidget::onApply);
    connect(m_resetButton, &QPushButton::clicked, this, &HardwareArduinoSensorSketchControllerWidget::onReset);
    connect(m_calculateButton, &QPushButton::clicked, this, &HardwareArduinoSensorSketchControllerWidget::onCalculate);
    connect(m_getPinsInfoCheck, &QCheckBox::toggled, this,
            &HardwareArduinoSensorSketchControllerWidget::onGetPinsInfoToggled);

    auto* form = new QFormLayout();
    form->addRow(tr("Port:"), m_portCombo);
    form->addRow(tr("Board:"), m_boardProfileCombo);
    auto* cmdRow = new QHBoxLayout();
    cmdRow->addWidget(m_commandEdit, 1);
    cmdRow->addWidget(sendBtn);
    form->addRow(tr("Command:"), cmdRow);
    form->addRow(tr("Presets:"), m_presetsList);
    form->addRow(QString(), m_getDataCheck);
    form->addRow(QString(), m_getPinsInfoCheck);
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

    for (const QString& path : HardwareGuiHelpers::listSerialPortDevicePaths())
        m_portCombo->addItem(path);
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
    HardwareGuiHelpers::setProp(m_context, "GetPinsInfo",
                                m_getPinsInfoCheck->isChecked() ? QStringLiteral("1") : QStringLiteral("0"));
}

void HardwareArduinoSensorSketchControllerWidget::refreshFromModel(bool force)
{
    Q_UNUSED(force);
    if (m_context.componentLongName.isEmpty())
        return;

    QSignalBlocker b1(m_portCombo);
    QSignalBlocker b2(m_boardProfileCombo);
    QSignalBlocker b3(m_getDataCheck);
    QSignalBlocker b4(m_getPinsInfoCheck);

    const QString port = HardwareGuiHelpers::getProp(m_context, "PortName");
    const QString portPath = port.isEmpty() ? port : RDK::UArduinoSerialPortUtil::normalizeDevicePath(port);
    if (m_portCombo->findText(portPath) < 0 && !portPath.isEmpty())
        m_portCombo->addItem(portPath);
    m_portCombo->setCurrentText(portPath);

    const int profile = HardwareGuiHelpers::getPropInt(m_context, "BoardProfile", 0);
    m_boardProfileCombo->setCurrentIndex(profile == 1 ? 1 : 0);
    m_getDataCheck->setChecked(HardwareGuiHelpers::getPropBool(m_context, "GetDataFromBuffers", false));
    m_getPinsInfoCheck->setChecked(HardwareGuiHelpers::getPropBool(m_context, "GetPinsInfo", false));
    m_commandEdit->setText(HardwareGuiHelpers::getProp(m_context, "Command"));

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

void HardwareArduinoSensorSketchControllerWidget::onGetPinsInfoToggled(bool checked)
{
    applyBoardFields();
    HardwareGuiHelpers::setProp(m_context, "GetPinsInfo", checked ? QStringLiteral("1") : QStringLiteral("0"));
    if (checked) {
        HardwareGuiHelpers::envCalculate(m_context);
        refreshFromModel(true);
    }
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
    const QString text = m_presetsList->currentItem()->text();
    if (text == QStringLiteral("GET PINS INFO")) {
        m_getPinsInfoCheck->setChecked(true);
        onGetPinsInfoToggled(true);
        return;
    }
    m_commandEdit->setText(text);
    onSendCommand();
}
