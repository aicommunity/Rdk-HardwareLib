#ifndef UARDUINOHARDWARESETUPEDITORWIDGET_H
#define UARDUINOHARDWARESETUPEDITORWIDGET_H

#include "Catalog/UHardwareCatalogTypes.h"

#include <QWidget>

class QComboBox;
class QLineEdit;
class QListWidget;
class QPlainTextEdit;
class QPushButton;
class UArduinoAssemblyViewWidget;

namespace RDK {
class UHardwareCatalog;
}

class UArduinoHardwareSetupEditorWidget : public QWidget {
    Q_OBJECT
public:
    explicit UArduinoHardwareSetupEditorWidget(QWidget* parent = nullptr);

    void setCatalog(RDK::UHardwareCatalog* catalog);
    void setDocument(const RDK::UHwSetupDocument& doc);
    RDK::UHwSetupDocument document() const { return Doc; }
    void setReadOnly(bool readOnly);
    void refreshUi();

signals:
    void setupChanged();
    void applyRequested();
    void exportSvgRequested();

private slots:
    void onValidate();
    void onBoardChanged();
    void onFirmwareChanged();
    void onExportSvg();
    void onDeviceSelectionChanged();
    void onAddDevice();
    void onRemoveDevice();
    void onApplyDeviceEdits();

private:
    void syncDeviceEditorFromSelection();
    void setDeviceEditorEnabled(bool enabled);
    QStringList suggestedPorts() const;

    RDK::UHardwareCatalog* Catalog = nullptr;
    RDK::UHwSetupDocument Doc;
    QComboBox* BoardCombo = nullptr;
    QComboBox* FirmwareCombo = nullptr;
    QListWidget* DevicesList = nullptr;
    QLineEdit* DeviceIdEdit = nullptr;
    QComboBox* ModuleCombo = nullptr;
    QComboBox* PortCombo = nullptr;
    QLineEdit* ChannelEdit = nullptr;
    QComboBox* RoleCombo = nullptr;
    QPlainTextEdit* IssuesLog = nullptr;
    UArduinoAssemblyViewWidget* AssemblyView = nullptr;
    QPushButton* ExportBtn = nullptr;
    QPushButton* AddDeviceBtn = nullptr;
    QPushButton* RemoveDeviceBtn = nullptr;
    QPushButton* ApplyDeviceBtn = nullptr;
    bool ReadOnly = false;
    bool SuppressDeviceSelect = false;
};

#endif
