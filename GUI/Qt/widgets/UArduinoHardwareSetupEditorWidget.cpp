#include "UArduinoHardwareSetupEditorWidget.h"

#include "Catalog/UHardwareCatalog.h"
#include "Catalog/UHardwareSetup.h"
#include "UArduinoAssemblyViewWidget.h"

#include <QComboBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QListWidget>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

UArduinoHardwareSetupEditorWidget::UArduinoHardwareSetupEditorWidget(QWidget* parent)
    : QWidget(parent)
{
    BoardCombo = new QComboBox(this);
    FirmwareCombo = new QComboBox(this);
    DevicesList = new QListWidget(this);
    IssuesLog = new QPlainTextEdit(this);
    IssuesLog->setReadOnly(true);
    AssemblyView = new UArduinoAssemblyViewWidget(this);
    auto* validateBtn = new QPushButton(tr("Validate"), this);
    auto* applyBtn = new QPushButton(tr("Apply to Board"), this);
    ExportBtn = new QPushButton(tr("Export SVG…"), this);
    connect(validateBtn, &QPushButton::clicked, this, &UArduinoHardwareSetupEditorWidget::onValidate);
    connect(applyBtn, &QPushButton::clicked, this, &UArduinoHardwareSetupEditorWidget::applyRequested);
    connect(ExportBtn, &QPushButton::clicked, this, &UArduinoHardwareSetupEditorWidget::onExportSvg);
    connect(BoardCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &UArduinoHardwareSetupEditorWidget::onBoardChanged);
    connect(FirmwareCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &UArduinoHardwareSetupEditorWidget::onFirmwareChanged);

    auto* left = new QVBoxLayout;
    left->addWidget(BoardCombo);
    left->addWidget(FirmwareCombo);
    left->addWidget(DevicesList, 1);
    left->addWidget(IssuesLog, 1);
    left->addWidget(validateBtn);
    left->addWidget(applyBtn);
    left->addWidget(ExportBtn);

    auto* root = new QHBoxLayout(this);
    root->addLayout(left, 1);
    root->addWidget(AssemblyView, 2);
}

void UArduinoHardwareSetupEditorWidget::setCatalog(RDK::UHardwareCatalog* catalog)
{
    Catalog = catalog;
    BoardCombo->clear();
    FirmwareCombo->clear();
    if (!Catalog)
        return;
    for (const QString& id : Catalog->boardIds())
        BoardCombo->addItem(id, id);
    for (const QString& id : Catalog->firmwareIds(true))
        FirmwareCombo->addItem(id, id);
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
}

void UArduinoHardwareSetupEditorWidget::refreshUi()
{
    if (!Catalog)
        return;
    const int bi = BoardCombo->findData(Doc.board);
    if (bi >= 0)
        BoardCombo->setCurrentIndex(bi);
    const int fi = FirmwareCombo->findData(Doc.firmwareId);
    if (fi >= 0)
        FirmwareCombo->setCurrentIndex(fi);
    DevicesList->clear();
    for (const RDK::UHwSetupDevice& d : Doc.devices) {
        DevicesList->addItem(QStringLiteral("%1 | %2 @ %3%4")
                                 .arg(d.id, d.module, d.port,
                                      d.channel.isEmpty() ? QString()
                                                         : (QStringLiteral("/") + d.channel)));
    }
    onValidate();
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
