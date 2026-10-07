#ifndef HARDWAREUARTDEVICEHUBPANELWIDGET_H
#define HARDWAREUARTDEVICEHUBPANELWIDGET_H

#include "HardwareGuiHelpers.h"

#include <QWidget>

class QLabel;
class QPlainTextEdit;

class HardwareUartDeviceHubPanelWidget : public QWidget {
    Q_OBJECT
public:
    explicit HardwareUartDeviceHubPanelWidget(QWidget* parent = nullptr);
    void setContext(const UComponentGuiContext& context);
    void refreshFromModel();

signals:
    void commandRequested(const QString& line);

private:
    UComponentGuiContext Context;
    QPlainTextEdit* TxEdit = nullptr;
    QLabel* LastRx = nullptr;
    QLabel* Warning = nullptr;
};

#endif
