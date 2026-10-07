#ifndef HARDWAREWHEELEDSENSORSPANELWIDGET_H
#define HARDWAREWHEELEDSENSORSPANELWIDGET_H

#include "HardwareGuiHelpers.h"

#include <QWidget>

class QPlainTextEdit;

/** Stub Sensors tab: shows NamedValuesJson / matrix preview props (P1). */
class HardwareWheeledSensorsPanelWidget : public QWidget {
    Q_OBJECT
public:
    explicit HardwareWheeledSensorsPanelWidget(QWidget* parent = nullptr);
    void setContext(const UComponentGuiContext& context);
    void refreshFromModel();

private:
    UComponentGuiContext Context;
    QPlainTextEdit* Preview = nullptr;
};

#endif
