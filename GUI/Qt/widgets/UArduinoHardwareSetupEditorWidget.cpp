#include "UArduinoHardwareSetupEditorWidget.h"

#include "Catalog/UHardwareCatalog.h"
#include "Catalog/UHardwareSetup.h"
#include "HardwareGuiHelpers.h"
#include "UArduinoAssemblyViewWidget.h"

#include <QComboBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

UArduinoHardwareSetupEditorWidget::UArduinoHardwareSetupEditorWidget(QWidget* parent)
    : QWidget(parent)
{
    BoardCombo = new QComboBox(this);
    FirmwareCombo = new QComboBox(this);
    DevicesList = new QListWidget(this);
    DeviceIdEdit = new QLineEdit(this);
    ModuleCombo = new QComboBox(this);
    PortCombo = new QComboBox(this);
    PortCombo->setEditable(true);
    ChannelEdit = new QLineEdit(this);
    RoleCombo = new QComboBox(this);
    RoleCombo->addItem(QStringLiteral("sensor"), QStringLiteral("sensor"));
    RoleCombo->addItem(QStringLiteral("actuator"), QStringLiteral("actuator"));
    IssuesLog = new QPlainTextEdit(this);
    IssuesLog->setReadOnly(true);
    IssuesLog->setMaximumHeight(80);
    AssemblyView = new UArduinoAssemblyViewWidget(this);

    HardwareGuiHelpers::configureExpandingCombo(BoardCombo, 14);
    HardwareGuiHelpers::configureExpandingCombo(FirmwareCombo, 14);
    HardwareGuiHelpers::configureExpandingCombo(ModuleCombo, 16);
    HardwareGuiHelpers::configureExpandingCombo(PortCombo, 6);
    HardwareGuiHelpers::configureExpandingCombo(RoleCombo, 8);

    auto* validateBtn = new QPushButton(tr("Validate"), this);
    auto* applyBtn = new QPushButton(tr("Apply to Board"), this);
    ExportBtn = new QPushButton(tr("Export SVG…"), this);
    AddDeviceBtn = new QPushButton(tr("Add device"), this);
    RemoveDeviceBtn = new QPushButton(tr("Remove"), this);
    ApplyDeviceBtn = new QPushButton(tr("Apply device edits"), this);

    connect(validateBtn, &QPushButton::clicked, this, &UArduinoHardwareSetupEditorWidget::onValidate);
    connect(applyBtn, &QPushButton::clicked, this, &UArduinoHardwareSetupEditorWidget::applyRequested);
    connect(ExportBtn, &QPushButton::clicked, this, &UArduinoHardwareSetupEditorWidget::onExportSvg);
    connect(AddDeviceBtn, &QPushButton::clicked, this, &UArduinoHardwareSetupEditorWidget::onAddDevice);
    connect(RemoveDeviceBtn, &QPushButton::clicked, this, &UArduinoHardwareSetupEditorWidget::onRemoveDevice);
    connect(ApplyDeviceBtn, &QPushButton::clicked, this,
            &UArduinoHardwareSetupEditorWidget::onApplyDeviceEdits);
    connect(BoardCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &UArduinoHardwareSetupEditorWidget::onBoardChanged);
    connect(FirmwareCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &UArduinoHardwareSetupEditorWidget::onFirmwareChanged);
    connect(DevicesList, &QListWidget::currentRowChanged, this,
            &UArduinoHardwareSetupEditorWidget::onDeviceSelectionChanged);

    auto* deviceForm = new QFormLayout;
    HardwareGuiHelpers::applyCompactForm(deviceForm);
    deviceForm->addRow(tr("Id:"), DeviceIdEdit);
    deviceForm->addRow(tr("Module:"), ModuleCombo);
    deviceForm->addRow(tr("Port:"), PortCombo);
    deviceForm->addRow(tr("Channel:"), ChannelEdit);
    deviceForm->addRow(tr("Role:"), RoleCombo);

    auto* deviceBtns = new QHBoxLayout;
    deviceBtns->setContentsMargins(0, 0, 0, 0);
    deviceBtns->setSpacing(4);
    deviceBtns->addWidget(AddDeviceBtn);
    deviceBtns->addWidget(RemoveDeviceBtn);
    deviceBtns->addWidget(ApplyDeviceBtn);

    auto* catalogForm = new QFormLayout;
    HardwareGuiHelpers::applyCompactForm(catalogForm);
    catalogForm->addRow(tr("Board:"), BoardCombo);
    catalogForm->addRow(tr("Firmware:"), FirmwareCombo);

    auto* actionRow = new QHBoxLayout;
    actionRow->setContentsMargins(0, 0, 0, 0);
    actionRow->setSpacing(4);
    actionRow->addWidget(validateBtn);
    actionRow->addWidget(applyBtn);
    actionRow->addWidget(ExportBtn);

    auto* leftWidget = new QWidget(this);
    auto* left = new QVBoxLayout(leftWidget);
    HardwareGuiHelpers::applyCompactLayout(left);
    left->addLayout(catalogForm);
    left->addWidget(DevicesList, 1);
    left->addLayout(deviceForm);
    left->addLayout(deviceBtns);
    left->addWidget(IssuesLog);
    left->addLayout(actionRow);

    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setWidget(leftWidget);

    auto* root = new QHBoxLayout(this);
    HardwareGuiHelpers::applyCompactLayout(root);
    root->addWidget(scroll, 1);
    root->addWidget(AssemblyView, 1);
}

void UArduinoHardwareSetupEditorWidget::setCatalog(RDK::UHardwareCatalog* catalog)
{
    Catalog = catalog;
    BoardCombo->clear();
    FirmwareCombo->clear();
    ModuleCombo->clear();
    if (!Catalog)
        return;
    for (const QString& id : Catalog->boardIds()) {
        const RDK::UHwBoardInfo* info = Catalog->board(id);
        BoardCombo->addItem(info && !info->title.isEmpty() ? info->title : id, id);
    }
    for (const QString& id : Catalog->firmwareIds(true)) {
        const RDK::UHwFirmwareInfo* info = Catalog->firmware(id);
        FirmwareCombo->addItem(info && !info->title.isEmpty() ? info->title : id, id);
    }
    for (const QString& id : Catalog->moduleIds()) {
        const RDK::UHwModuleInfo* info = Catalog->module(id);
        ModuleCombo->addItem(info && !info->title.isEmpty() ? info->title : id, id);
    }
    AssemblyView->setCatalog(Catalog);
}

void UArduinoHardwareSetupEditorWidget::setDocument(const RDK::UHwSetupDocument& doc)
{
    Doc = doc;
    refreshUi();
}

void UArduinoHardwareSetupEditorWidget::setReadOnly(bool readOnly)
{
    ReadOnly = readOnly;
    BoardCombo->setEnabled(!readOnly);
    FirmwareCombo->setEnabled(!readOnly);
    setDeviceEditorEnabled(!readOnly);
}

void UArduinoHardwareSetupEditorWidget::setDeviceEditorEnabled(bool enabled)
{
    DevicesList->setEnabled(true);
    DeviceIdEdit->setEnabled(enabled);
    ModuleCombo->setEnabled(enabled);
    PortCombo->setEnabled(enabled);
    ChannelEdit->setEnabled(enabled);
    RoleCombo->setEnabled(enabled);
    AddDeviceBtn->setEnabled(enabled);
    RemoveDeviceBtn->setEnabled(enabled);
    ApplyDeviceBtn->setEnabled(enabled);
}

QStringList UArduinoHardwareSetupEditorWidget::suggestedPorts() const
{
    QStringList ports;
    for (int i = 0; i <= 13; ++i)
        ports << QStringLiteral("D%1").arg(i);
    for (int i = 0; i < 6; ++i)
        ports << QStringLiteral("A%1").arg(i);
    return ports;
}

void UArduinoHardwareSetupEditorWidget::refreshUi()
{
    if (!Catalog)
        return;
    SuppressDeviceSelect = true;
    const int bi = BoardCombo->findData(Doc.board);
    if (bi >= 0)
        BoardCombo->setCurrentIndex(bi);
    const int fi = FirmwareCombo->findData(Doc.firmwareId);
    if (fi >= 0)
        FirmwareCombo->setCurrentIndex(fi);

    PortCombo->clear();
    PortCombo->addItems(suggestedPorts());

    const int previous = DevicesList->currentRow();
    DevicesList->clear();
    for (const RDK::UHwSetupDevice& d : Doc.devices) {
        DevicesList->addItem(QStringLiteral("%1 | %2 @ %3%4")
                                 .arg(d.id, d.module, d.port,
                                      d.channel.isEmpty() ? QString()
                                                         : (QStringLiteral("/") + d.channel)));
    }
    SuppressDeviceSelect = false;
    if (!Doc.devices.isEmpty()) {
        const int row = (previous >= 0 && previous < Doc.devices.size()) ? previous : 0;
        DevicesList->setCurrentRow(row);
    } else {
        syncDeviceEditorFromSelection();
    }
    onValidate();
}

void UArduinoHardwareSetupEditorWidget::syncDeviceEditorFromSelection()
{
    const int row = DevicesList->currentRow();
    if (row < 0 || row >= Doc.devices.size()) {
        DeviceIdEdit->clear();
        ChannelEdit->clear();
        return;
    }
    const RDK::UHwSetupDevice& d = Doc.devices.at(row);
    DeviceIdEdit->setText(d.id);
    const int mi = ModuleCombo->findData(d.module);
    if (mi >= 0)
        ModuleCombo->setCurrentIndex(mi);
    PortCombo->setCurrentText(d.port);
    ChannelEdit->setText(d.channel);
    const int ri = RoleCombo->findData(d.role.isEmpty() ? QStringLiteral("sensor") : d.role);
    if (ri >= 0)
        RoleCombo->setCurrentIndex(ri);
}

void UArduinoHardwareSetupEditorWidget::onDeviceSelectionChanged()
{
    if (SuppressDeviceSelect)
        return;
    syncDeviceEditorFromSelection();
}

void UArduinoHardwareSetupEditorWidget::onAddDevice()
{
    if (ReadOnly || !Catalog)
        return;
    RDK::UHwSetupDevice d;
    d.id = QStringLiteral("dev_%1").arg(Doc.devices.size() + 1);
    const QStringList modules = Catalog->moduleIds();
    d.module = modules.isEmpty() ? QStringLiteral("potentiometer") : modules.first();
    d.port = QStringLiteral("A0");
    d.role = QStringLiteral("sensor");
    Doc.devices.append(d);
    emit setupChanged();
    refreshUi();
    DevicesList->setCurrentRow(Doc.devices.size() - 1);
}

void UArduinoHardwareSetupEditorWidget::onRemoveDevice()
{
    if (ReadOnly)
        return;
    const int row = DevicesList->currentRow();
    if (row < 0 || row >= Doc.devices.size())
        return;
    Doc.devices.removeAt(row);
    emit setupChanged();
    refreshUi();
}

void UArduinoHardwareSetupEditorWidget::onApplyDeviceEdits()
{
    if (ReadOnly)
        return;
    const int row = DevicesList->currentRow();
    if (row < 0 || row >= Doc.devices.size())
        return;
    RDK::UHwSetupDevice& d = Doc.devices[row];
    d.id = DeviceIdEdit->text().trimmed();
    d.module = ModuleCombo->currentData().toString();
    d.port = PortCombo->currentText().trimmed();
    d.channel = ChannelEdit->text().trimmed();
    d.role = RoleCombo->currentData().toString();
    if (d.id.isEmpty())
        d.id = QStringLiteral("dev_%1").arg(row + 1);
    emit setupChanged();
    refreshUi();
    DevicesList->setCurrentRow(row);
}

void UArduinoHardwareSetupEditorWidget::onBoardChanged()
{
    if (ReadOnly)
        return;
    Doc.board = BoardCombo->currentData().toString();
    emit setupChanged();
    onValidate();
}

void UArduinoHardwareSetupEditorWidget::onFirmwareChanged()
{
    if (ReadOnly)
        return;
    Doc.firmwareId = FirmwareCombo->currentData().toString();
    emit setupChanged();
    onValidate();
}

void UArduinoHardwareSetupEditorWidget::onValidate()
{
    if (!Catalog)
        return;
    RDK::UHardwareSetup setup;
    setup.setDocument(Doc);
    QVector<RDK::UHwIssue> issues;
    setup.validate(*Catalog, &issues);
    IssuesLog->clear();
    for (const RDK::UHwIssue& issue : issues) {
        IssuesLog->appendPlainText(issue.code + QStringLiteral(": ") + issue.message);
    }
    if (issues.isEmpty())
        IssuesLog->appendPlainText(tr("OK"));
    AssemblyView->setSetup(Doc);
    AssemblyView->setIssues(issues);
    AssemblyView->rebuild();
}

void UArduinoHardwareSetupEditorWidget::onExportSvg()
{
    const QString path = QFileDialog::getSaveFileName(this, tr("Export assembly SVG"),
                                                      QStringLiteral("assembly.svg"),
                                                      tr("SVG (*.svg)"));
    if (path.isEmpty())
        return;
    QString err;
    if (!AssemblyView->exportSvg(path, &err))
        QMessageBox::warning(this, tr("Export SVG"), err);
    else
        emit exportSvgRequested();
}
