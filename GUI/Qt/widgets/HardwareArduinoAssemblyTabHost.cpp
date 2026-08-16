#include "HardwareArduinoAssemblyTabHost.h"

#include "Catalog/UHardwareCatalog.h"
#include "Catalog/UHardwareSetup.h"
#include "UArduinoHardwareSetupEditorWidget.h"
#include "HardwareGuiHelpers.h"

#include <QVBoxLayout>

HardwareArduinoAssemblyTabHost::HardwareArduinoAssemblyTabHost(QWidget* parent)
    : QWidget(parent)
{
    Editor = new UArduinoHardwareSetupEditorWidget(this);
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->addWidget(Editor);
    connect(Editor, &UArduinoHardwareSetupEditorWidget::applyRequested, this,
            &HardwareArduinoAssemblyTabHost::onEditorApply);
    connect(Editor, &UArduinoHardwareSetupEditorWidget::setupChanged, this,
            &HardwareArduinoAssemblyTabHost::onSetupChanged);
}

void HardwareArduinoAssemblyTabHost::setContext(const UComponentGuiContext& context)
{
    Context = context;
    ensureCatalog();
    refreshFromModel();
}

void HardwareArduinoAssemblyTabHost::setReadOnly(bool readOnly)
{
    ReadOnly = readOnly;
    Editor->setReadOnly(readOnly);
}

void HardwareArduinoAssemblyTabHost::ensureCatalog()
{
    RDK::UHardwareCatalog& cat = RDK::UHardwareCatalog::instance();
    if (!cat.isLoaded())
        cat.load(nullptr);
    Editor->setCatalog(&cat);
}

RDK::UHwSetupDocument HardwareArduinoAssemblyTabHost::loadDocumentFromContext() const
{
    RDK::UHwSetupDocument doc;
    const QString path = HardwareGuiHelpers::getProp(Context, "HardwareSetupPath");
    const QString inline_json = HardwareGuiHelpers::getProp(Context, "HardwareSetupJson");
    RDK::UHardwareSetup setup;
    if (!path.isEmpty() && setup.loadFromFile(path, nullptr))
        return setup.document();
    if (!inline_json.isEmpty() && setup.loadFromJson(inline_json.toUtf8(), nullptr))
        return setup.document();

    doc.board = HardwareGuiHelpers::getPropInt(Context, "BoardProfile", 0) == 1
                    ? QStringLiteral("mega2560")
                    : QStringLiteral("uno");
    doc.firmwareId = HardwareGuiHelpers::getProp(Context, "BundledFirmwareId");
    if (doc.firmwareId.isEmpty())
        doc.firmwareId = QStringLiteral("sensor_lab_v1");
    const RDK::UHwFirmwareInfo* fw = RDK::UHardwareCatalog::instance().firmware(doc.firmwareId);
    if (fw)
        doc = RDK::UHardwareSetup::fromFirmwareDefaults(*fw, doc.board);
    return doc;
}

void HardwareArduinoAssemblyTabHost::refreshFromModel()
{
    if (Context.componentLongName.isEmpty())
        return;
    ensureCatalog();
    Editor->setDocument(loadDocumentFromContext());
}

void HardwareArduinoAssemblyTabHost::applyToModel()
{
    if (ReadOnly || Context.componentLongName.isEmpty())
        return;
    const RDK::UHwSetupDocument doc = Editor->document();
    RDK::UHardwareSetup setup;
    setup.setDocument(doc);
    QVector<RDK::UHwIssue> issues;
    const bool ok = RDK::UHardwareCatalog::instance().isLoaded()
                    && setup.validate(RDK::UHardwareCatalog::instance(), &issues);
    QString issuesText;
    for (const RDK::UHwIssue& issue : issues) {
        if (!issuesText.isEmpty())
            issuesText.append('\n');
        issuesText.append(issue.code + QStringLiteral(": ") + issue.message);
    }
    HardwareGuiHelpers::setProp(Context, "HardwareSetupValid", ok ? QStringLiteral("1")
                                                                  : QStringLiteral("0"));
    HardwareGuiHelpers::setProp(Context, "HardwareSetupIssues", issuesText);
    HardwareGuiHelpers::setProp(Context, "BoardProfile",
                                doc.board == QStringLiteral("mega2560") ? QStringLiteral("1")
                                                                       : QStringLiteral("0"));
    if (!doc.firmwareId.isEmpty())
        HardwareGuiHelpers::setProp(Context, "BundledFirmwareId", doc.firmwareId);
}

void HardwareArduinoAssemblyTabHost::onEditorApply()
{
    applyToModel();
    emit applyHardwareSetupRequested();
}

void HardwareArduinoAssemblyTabHost::onSetupChanged()
{
    if (!ReadOnly)
        applyToModel();
}
