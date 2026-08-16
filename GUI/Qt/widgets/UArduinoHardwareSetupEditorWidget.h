#ifndef UARDUINOHARDWARESETUPEDITORWIDGET_H
#define UARDUINOHARDWARESETUPEDITORWIDGET_H

#include "Catalog/UHardwareCatalogTypes.h"

#include <QWidget>

class QComboBox;
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

private:
    RDK::UHardwareCatalog* Catalog = nullptr;
    RDK::UHwSetupDocument Doc;
    QComboBox* BoardCombo = nullptr;
    QComboBox* FirmwareCombo = nullptr;
    QListWidget* DevicesList = nullptr;
    QPlainTextEdit* IssuesLog = nullptr;
    UArduinoAssemblyViewWidget* AssemblyView = nullptr;
    QPushButton* ExportBtn = nullptr;
    bool ReadOnly = false;
};

#endif
