#ifndef HARDWAREARDUINOASSEMBLYTABHOST_H
#define HARDWAREARDUINOASSEMBLYTABHOST_H

#include "../../../../Rdk/GUI/Qt/UComponentGuiContext.h"

#include <QWidget>

class UArduinoHardwareSetupEditorWidget;

class HardwareArduinoAssemblyTabHost : public QWidget {
    Q_OBJECT
public:
    explicit HardwareArduinoAssemblyTabHost(QWidget* parent = nullptr);

    void setContext(const UComponentGuiContext& context);
    void setReadOnly(bool readOnly);
    void refreshFromModel();
    void applyToModel();

signals:
    void applyHardwareSetupRequested();

private slots:
    void onEditorApply();
    void onSetupChanged();

private:
    void ensureCatalog();
    RDK::UHwSetupDocument loadDocumentFromContext() const;

    UComponentGuiContext Context;
    UArduinoHardwareSetupEditorWidget* Editor = nullptr;
    bool ReadOnly = false;
};

#endif
